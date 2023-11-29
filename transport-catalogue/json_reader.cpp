
#include "json_reader.h"

#include <exception>
#include <vector>
#include <string>
#include <deque>
#include <map>



//================================================================================================================READ_JSON
RequestsDataBase JSONReader::ReadJSON(std::istream& input) {
   using namespace std::literals;
   RequestsDataBase db;
   json::Node tmp = json::LoadNode(input).AsDict();
   for (const auto& [key, value] : tmp.AsDict()) {
      if (key == "base_requests"s) {
         db.base_requests = value.AsArray();
      }
      else if (key == "stat_requests"s) {
         db.stat_requests = value.AsArray();
      }
      else if (key == "render_settings"s) {
         db.render_settings = value.AsDict();
      }
      else if (key == "routing_settings"s) {
         db.routing_settings = value.AsDict();
      }
      else {
         throw std::logic_error("ReadJSON() error: the key \""s + key + "\" dosn't exists for this context"s);
      }
   }
   return db;
}



//================================================================================================================CREATE_QUEUE
domain::Queue JSONReader::CreateQueue(const json::Node& base_requests) const {
   using namespace std::literals;
   domain::Queue queue;

   for (const auto& request : base_requests.AsArray()) {
      std::string type, name;
      double lat = 0., lng = 0.;
      bool is_roundtrip = false;
      std::vector<std::string> stops;
      std::map<std::string, double> dist_to_other;
      for (const auto& [key, value] : request.AsDict()) {
         if (key == "type"s) {
            type = value.AsString();
         }
         else if (key == "name"s) {
            name = value.AsString();
         }
         else if (key == "latitude"s) {
            lat = value.AsDouble();
         }
         else if (key == "longitude"s) {
            lng = value.AsDouble();
         }
         else if (key == "road_distances"s) {
            for (const auto& [stop_name, dist] : value.AsDict()) {
               dist_to_other.insert({ stop_name,dist.AsDouble() });
            }
         }
         else if (key == "stops"s) {
            for (const auto& stop : value.AsArray()) {
               stops.push_back(stop.AsString());
            }
         }
         else if (key == "is_roundtrip"s) {
            is_roundtrip = value.AsBool();
         }
         else {
            throw std::logic_error("CreateQueue() error: the key \""s + key + "\" dosn't exists for this context"s);
         }
      }
      if (type == "Stop"s) {
         queue.stops.push_back({ name,lat,lng,dist_to_other });
      }
      else if (type == "Bus"s) {
         queue.buses.push_back({ name,stops,is_roundtrip });
      }
      else {
         throw std::logic_error("CreateQueue() error: the type \""s + type + "\" dosn't exists for this context"s);
      }
   }

   return queue;
}



//================================================================================================================FILL_CATALOGUE
void JSONReader::FillCatalogue(TransportCatalogue& catalogue) const {
   using namespace std::literals;

   int bus_wait_time{};
   double bus_velocity{};
   for (const auto& [key, value] : db_.routing_settings.AsDict()) {
      if (key == "bus_wait_time"s) {
         bus_wait_time = value.AsInt();
      }
      else {
         bus_velocity = value.AsDouble();
      }
   }

   domain::Queue queue = std::move(CreateQueue(db_.base_requests));
   std::unordered_map<std::string, t_catalogue::Stop> stopname_to_stop;
   std::unordered_map<std::pair<std::string, std::string>, double, Hasher> stoppair_to_distance;
   for (const auto& stop : queue.stops) {
      stopname_to_stop[stop.name] = { stop.name, stop.lat,stop.lng,{} };
      for (const auto& [other_name, distance] : stop.distance_to_other) {
         stoppair_to_distance.insert({ {stop.name,other_name},distance });
      }
   }
   for (const auto& bus : queue.buses) {
      t_catalogue::Bus bus_info = { bus.name,{},bus.is_roundtrip,bus_velocity, bus_wait_time };
      for (auto stop = bus.stops.begin(); stop != bus.stops.end();++stop) {
         auto next_stop = std::next(stop, 1);
         auto stop_info = stopname_to_stop.at(*stop);
         bus_info.stops.push_back(*stop);
         if (stop_info.buses.empty() || stop_info.buses.back() != bus.name) {
            stopname_to_stop.at(*stop).buses.push_back(bus_info.name);
         }
         if (next_stop != bus.stops.end()) {
            auto next_stop_info = stopname_to_stop.at(*next_stop);
            double geo_length = geo::ComputeDistance(geo::Coordinates{ stop_info.lat,stop_info.lng },
               geo::Coordinates{ next_stop_info.lat,next_stop_info.lng });
            auto res = stoppair_to_distance.find(std::make_pair(*stop, *next_stop));
            int length_to = (res != stoppair_to_distance.end()) ? res->second : 0;

            res = stoppair_to_distance.find(std::make_pair(*next_stop, *stop));
            int length_from = (res != stoppair_to_distance.end()) ? res->second : 0;

            bus_info.geo_length += bus_info.is_roundtrip ? geo_length : geo_length * 2;

            if (length_to && length_from) {
               bus_info.real_length += (bus_info.is_roundtrip) ? length_to : length_to + length_from;
               catalogue.SetDistanceBetweenStops(bus_info.name, *stop, *next_stop, length_to, length_from);
            }
            else if (length_to) {
               bus_info.real_length += (bus_info.is_roundtrip) ? length_to : length_to * 2;
               catalogue.SetDistanceBetweenStops(bus_info.name, *stop, *next_stop, length_to);
            }
            else if (length_from) {
               bus_info.real_length += (bus_info.is_roundtrip) ? length_from : length_from * 2;
               catalogue.SetDistanceBetweenStops(bus_info.name, *stop, *next_stop, length_from);
            }
            else {
               bus_info.real_length += (bus_info.is_roundtrip) ? geo_length : geo_length * 2;
               catalogue.SetDistanceBetweenStops(bus_info.name, *stop, *next_stop, geo_length);
            }

         }
      }
      catalogue.AddBus(bus_info);
   }
   for (const auto& [key, value] : stopname_to_stop) {
      catalogue.AddStop(value);
   }
}

//================================================================================================================FILL_GRAPH

void JSONReader::StopNameToID(std::string_view name) {
   if (!stop_name_to_id_.count(name)) {
      size_t id = stop_name_to_id_.size();
      stop_name_to_id_[name] = id;
   }
}


graph::DirectedWeightedGraph<double> JSONReader::CreateGraph(const TransportCatalogue& catalogue) {
   using namespace t_catalogue;
   using namespace std::literals;
   graph::DirectedWeightedGraph<double> graph(catalogue.GetUniqueStopsCount());
   for (const auto& bus : catalogue.GetBusInfo()) {
      StopNameToID(bus.stops.front());
      for (size_t from = 0; from < bus.stops.size(); ++from) {
         double weight_from = bus.wait_time;
         double weight_to = bus.wait_time;
         int span_count_from = 0;
         int span_count_to = 0;
         for (size_t to = from + 1; to < bus.stops.size(); ++to) {
            StopNameToID(bus.stops.at(to));
            weight_from += catalogue.GetDistanceBetweenStops(bus.name, bus.stops.at(to - 1), bus.stops.at(to)) / (bus.velocity * 1000 / 60);
            size_t id = graph.AddEdge({ stop_name_to_id_.at(bus.stops.at(from)),stop_name_to_id_.at(bus.stops.at(to)), weight_from });
            ++span_count_from;
            buffer_[id] = Buffer{ bus.name, bus.stops.at(from),bus.wait_time,weight_from ,span_count_from };
            if (!bus.is_roundtrip) {
               ++span_count_to;
               weight_to += catalogue.GetDistanceBetweenStops(bus.name, bus.stops.at(to), bus.stops.at(to - 1)) / (bus.velocity * 1000 / 60);
               size_t id = graph.AddEdge({ stop_name_to_id_.at(bus.stops.at(to)),stop_name_to_id_.at(bus.stops.at(from)), weight_to });
               buffer_[id] = Buffer{ bus.name, bus.stops.at(to),bus.wait_time,weight_to,span_count_to };
            }
         }
      }
   }
   return graph;
}

//================================================================================================================HANDLE_STAT_REQUESTS
json::Node JSONReader::HandleStatRequests(const RequestHandler& handler) const {
   using namespace std::literals;
   json::Array result;
   for (const auto& request : db_.stat_requests.AsArray()) {
      std::string type, name, from, to;
      int id = 0;
      for (const auto& [key, value] : request.AsDict()) {
         if (key == "id"s) {
            id = value.AsInt();
         }
         else if (key == "name"s) {
            name = value.AsString();
         }
         else if (key == "type"s) {
            type = value.AsString();
         }
         else if (key == "from"s) {
            from = value.AsString();
         }
         else if (key == "to"s) {
            to = value.AsString();
         }
         else {
            throw std::logic_error("HandleStatRequest() error: the key \""s + key + "\" dosn't exist for this context"s);
         }
      }
      if (type == "Bus"s) {
         const auto bus = handler.GetBusInfo(name);
         result.push_back(bus ? std::move(BusAsJSON(bus, id)) : std::move(NotFound(id)));
      }
      else if (type == "Stop"s) {
         const auto stop = handler.GetStopInfo(name);
         result.push_back(stop ? std::move(StopAsJSON(stop, id)) : std::move(NotFound(id)));
      }
      else if (type == "Map"s) {
         std::ostringstream oss;
         handler.RenderMap().Render(oss);
         result.push_back(std::move(MapAsJSON(oss.str(), id)));
      }
      else if (type == "Route"s) {
         auto f = stop_name_to_id_.find(from);
         auto t = stop_name_to_id_.find(to);
         if (f != stop_name_to_id_.end() && t != stop_name_to_id_.end()) {
            const auto data = handler.BuildRoute(f->second, t->second);
            result.push_back((data.has_value()) ? std::move(RouteAsJSON(*data, id)) : std::move(NotFound(id)));
         }
         else {
            result.push_back(std::move(NotFound(id)));
         }
      }
      else {
         throw std::logic_error("HandleStatRequest() error: the type \""s + type + "\" dosn't exist for this context"s);
      }
   }
   return json::Node(result);
}


const json::Node JSONReader::GetBaseRequests() const {
   return db_.base_requests;
}

const json::Node JSONReader::GetStatRequests() const {
   return db_.stat_requests;
}

const json::Node JSONReader::GetRenderSettings() const {
   return db_.render_settings;
}

const json::Node JSONReader::GetRoutingSettings() const {
   return db_.routing_settings;
}



//================================================================================================================BUS_AS_JSON
json::Node JSONReader::BusAsJSON(const t_catalogue::Bus* bus_info, const int id) const {
   using namespace std::literals;
   int stops_count = (bus_info->is_roundtrip) ? bus_info->stops.size() : bus_info->stops.size() * 2 - 1;
   return json::Builder{}.StartDict()
      .Key("request_id"s).Value(id)
      .Key("curvature"s).Value(bus_info->real_length / bus_info->geo_length)
      .Key("route_length"s).Value(bus_info->real_length)//static_cast для вывода в экспоненциальной форме
      .Key("stop_count"s).Value(stops_count)
      .Key("unique_stop_count"s).Value(domain::UniqueStopsCount(bus_info->stops))
      .EndDict().Build();
}


//================================================================================================================STOP_AS_JSON
json::Node JSONReader::StopAsJSON(const t_catalogue::Stop* stop_info, const int id) const {
   using namespace std::literals;
   json::Array arr;
   for (auto it = stop_info->buses.begin(); it != stop_info->buses.end(); ++it) {
      arr.push_back(json::Node{ *it });
   }
   return json::Builder{}.StartDict().Key("request_id"s).Value(id).Key("buses"s).Value(arr).EndDict().Build();
}



//================================================================================================================MAP_AS_JSON
json::Node JSONReader::MapAsJSON(std::string&& value, const int id) const {
   using namespace std::literals;
   return json::Builder{}.StartDict().Key("map"s).Value(std::move(value)).Key("request_id"s).Value(id).EndDict().Build();
}

json::Node JSONReader::RouteAsJSON(const typename graph::Router<double>::RouteInfo& route_info, int id) const {
   using namespace std::literals;
   json::Builder builder;
   builder.StartDict()
      .Key("items"s).StartArray();
   for (auto edge_id = route_info.edges.begin(); edge_id != route_info.edges.end(); ++edge_id) {
      if (route_info.weight != 0) {
         builder.StartDict()
            .Key("type"s).Value("Wait"s)
            .Key("stop_name"s).Value(static_cast<std::string>(buffer_.at(*edge_id).from))
            .Key("time"s).Value(buffer_.at(*edge_id).wait_time)
            .EndDict()
            .StartDict()
            .Key("type"s).Value("Bus"s)
            .Key("bus"s).Value(static_cast<std::string>(buffer_.at(*edge_id).bus_name))
            .Key("span_count"s).Value(buffer_.at(*edge_id).span_count)
            .Key("time"s).Value(buffer_.at(*edge_id).weight - buffer_.at(*edge_id).wait_time)
            .EndDict();
      }
   }
   builder.EndArray()
      .Key("request_id"s).Value(id)
      .Key("total_time"s).Value(route_info.weight)
      .EndDict();
   return builder.Build();
}

//================================================================================================================NOT_FOUND
json::Node JSONReader::NotFound(const int id) const {
   using namespace std::literals;
   return json::Builder{}.StartDict().Key("request_id"s).Value(id).Key("error_message"s).Value("not found"s).EndDict().Build();
}
