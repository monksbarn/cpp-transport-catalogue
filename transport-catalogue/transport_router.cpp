#include "transport_router.h"

RouteFeatures HandleRouteSettings(const json::Node& route_features) {
   using namespace std::literals;
   RouteFeatures rf;
   for (const auto& [key, value] : route_features.AsDict())
   {
      key == "bus_wait_time"s ? rf.wait_time = value.AsInt() : rf.velocity = value.AsDouble();
   }
   return rf;
}


TransportRouter::TransportRouter(const TransportCatalogue& catalogue, RouteFeatures features) :
   graph_(catalogue.GetUniqueStopsCount()),
   route_features_(std::move(features)) {

   using namespace t_catalogue;

   for (const auto& bus : catalogue.GetBusInfo())
   {
      SetStopID(bus.stops.front());
      for (size_t from = 0; from < bus.stops.size(); ++from)
      {
         double weight_forward = route_features_.wait_time;
         double weight_backward = route_features_.wait_time;
         int span_count_forward = 0;
         int span_count_backward = 0;
         for (size_t to = from + 1; to < bus.stops.size(); ++to)
         {
            SetStopID(bus.stops.at(to));
            weight_forward += catalogue.GetDistanceBetweenStops(bus.name, bus.stops.at(to - 1), bus.stops.at(to)) / (route_features_.velocity * KMHOUR_METRMIN);
            size_t id = graph_.AddEdge({ stop_to_id_[bus.stops[from]], stop_to_id_[bus.stops[to]], weight_forward });
            buffer_[id] = Buffer{ bus.name, bus.stops[from], route_features_.wait_time, weight_forward, ++span_count_forward };
            if (!bus.is_roundtrip)
            {
               weight_backward += catalogue.GetDistanceBetweenStops(bus.name, bus.stops.at(to), bus.stops.at(to - 1)) / (route_features_.velocity * KMHOUR_METRMIN);
               size_t id = graph_.AddEdge({ stop_to_id_[bus.stops[to]], stop_to_id_[bus.stops[from]], weight_backward });
               buffer_[id] = Buffer{ bus.name, bus.stops[to], route_features_.wait_time, weight_backward, ++span_count_backward };
            }
         }
      }
   }
}

const Buffer& TransportRouter::GetEdgeInfoFromId(size_t id) const {
   return buffer_.at(id);
}


std::optional<size_t> TransportRouter::GetIdFromStop(const std::string_view name) const {
   auto search = stop_to_id_.find(name);
   return search != stop_to_id_.end() ? std::optional<size_t>(stop_to_id_.at(name)) : std::nullopt;
}


std::optional<typename graph::Router<double>::RouteInfo>  TransportRouter::BuildRoute(graph::VertexId from, graph::VertexId to) const {
   graph::Router router(graph_);
   return router.BuildRoute(from, to);
}