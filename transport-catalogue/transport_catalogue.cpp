#include "transport_catalogue.h"
#include "stat_reader.h"
#include "geo.h"

#include <algorithm>

TransportCatalogue::TransportCatalogue(std::istream& input) {
    AddInfo(std::move(CreateQueue(input)));
}

void TransportCatalogue::AddInfo(tc_input_reader::Queue&& queue) {
    for (auto& stop : queue.stops) {
        AddStop(stop);
    }
    for (auto& stop : queue.stops) {
        AddStopsDistance(std::move(stop));
    }
    for (auto& bus : queue.buses) {
        AddBus(std::move(bus));
    }
    for (auto& stop : stops_) {
        SortBusInfo(stop.buses);
    }
}

//добавление маршрута в базу
void TransportCatalogue::AddRoute(std::istream& input) {
    AddInfo(CreateQueue(input));
}

// добавление остановки в базу
void TransportCatalogue::AddStop(std::istream& input) {
    AddInfo(CreateQueue(input));
}

// поиск маршрута по имени
const t_catalogue::Bus TransportCatalogue::SearchRoute(const std::string& name) const {
    t_catalogue::Bus bus;
    auto it = busname_to_bus_.find(name);
    if (it != busname_to_bus_.end()) {
        bus = *it->second;
    }
    return bus;
}

// поиск остановки по имени
const t_catalogue::Stop TransportCatalogue::SearchStop(const std::string& name) const {
    t_catalogue::Stop stop;
    auto it = stopname_to_stop_.find(name);
    if (it != stopname_to_stop_.end()) {
        stop = *it->second;
    }
    return stop;
}

double TransportCatalogue::SearchPairStops(t_catalogue::Stop* first, t_catalogue::Stop* second) const {
    auto it = stoppair_to_distance_.find({ first,second });
    return it == stoppair_to_distance_.end() ? 0. : it->second;
}

// получение информации о маршруте.
std::string TransportCatalogue::GetRoutInfo(std::istream& input) const {
    std::string out;
    auto data = std::move(ParseQuery(input));
    for (const auto& [type, key] : data) {
        out += (type == "Bus") ? PrintBusInfo(*this, key) + "\n" : PrintStopInfo(*this, key) + "\n";
    }
    return out;
}

void TransportCatalogue::AddBus(tc_input_reader::Bus&& bus) {
    buses_.push_back({ bus.name,{},bus.ring });
    for (auto stop = bus.stops.begin(); stop != bus.stops.end();++stop) {
        auto next_stop = std::next(stop, 1);
        auto stop_ptr = stopname_to_stop_.at(*stop);
        buses_.back().stops.push_back(stop_ptr);
        if (stop_ptr->buses.empty() || stop_ptr->buses.back()->name != bus.name) {
            stop_ptr->buses.push_back(&buses_.back());
        }
        if (next_stop != bus.stops.end()) {
            auto next_stop_ptr = stopname_to_stop_.at(*next_stop);
            double geo_length = ComputeDistance(Coordinates{ stop_ptr->lat,stop_ptr->lng }, Coordinates{ next_stop_ptr->lat,next_stop_ptr->lng });
            int length_to = SearchPairStops(stop_ptr, next_stop_ptr);
            int length_from = SearchPairStops(next_stop_ptr, stop_ptr);
            buses_.back().geo_length += buses_.back().ring ? geo_length : geo_length * 2;
            if (length_to && length_from) {
                buses_.back().real_length += (buses_.back().ring) ? length_to : length_to + length_from;
            }
            else if (length_to) {
                buses_.back().real_length += (buses_.back().ring) ? length_to : length_to * 2;
            }
            else if (length_from) {
                buses_.back().real_length += (buses_.back().ring) ? length_from : length_from * 2;
            }
            else {
                buses_.back().real_length += (buses_.back().ring) ? geo_length : geo_length * 2;
            }

        }
    }
    busname_to_bus_.insert({ buses_.back().name,&buses_.back() });
}

void TransportCatalogue::AddStop(const tc_input_reader::Stop& stop) {
    stops_.push_back({ stop.name,stop.lat,stop.lng,{} });
    stopname_to_stop_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddStopsDistance(tc_input_reader::Stop&& stop) {
    for (auto& [other_name, dist] : stop.distance_to_other) {
        stoppair_to_distance_[{stopname_to_stop_.at(stop.name), stopname_to_stop_.at(other_name)}] = dist;
    }
}
