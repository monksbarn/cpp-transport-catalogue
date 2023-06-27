#include "transport_catalogue.h"
#include "stat_reader.h"
#include "geo.h"

#include <algorithm>

TransportCatalogue::TransportCatalogue(std::istream& input) {
    tc_input_reader::Queue queue = CreateQueue(input);
    for (auto& stop : queue.stops) {
        stops_.push_back({ stop.name,stop.lat,stop.lng,{} });
        stopname_to_stop_[stops_.back().name] = &stops_.back();
    }
    for (auto& stop : queue.stops) {
        for (auto& [other_name, dist] : stop.distance_to_other) {
            stop_pair_to_distance_[{stopname_to_stop_.at(stop.name), stopname_to_stop_.at(other_name)}] = dist;
        }
    }
    for (const auto& [name, stops, ring] : queue.buses) {
        buses_.push_back({ name,{},ring });
        for (const auto& stop : stops) {
            buses_.back().stops.push_back(stopname_to_stop_.at(stop));
            if (stopname_to_stop_.at(stop)->buses.empty() || stopname_to_stop_.at(stop)->buses.back()->name != name) {
                stopname_to_stop_.at(stop)->buses.push_back(&buses_.back());
            }
        }
        busname_to_bus_.insert({ buses_.back().name,&buses_.back() });
    }
    for (auto& stop : stops_) {
        SortBusInfo(stop.buses);
    }
}

//добавление маршрута в базу
void TransportCatalogue::AddRoute(std::istream& input) {
    tc_input_reader::Bus bus_bufer = std::move(ParseBus(input));
    buses_.push_back({ bus_bufer.name,{},bus_bufer.ring });
    for (const auto& stop : bus_bufer.stops) {
        buses_.back().stops.push_back(stopname_to_stop_.at(stop));
        if (stopname_to_stop_.at(stop)->buses.empty() || stopname_to_stop_.at(stop)->buses.back()->name != bus_bufer.name) {
            stopname_to_stop_.at(stop)->buses.push_back(&buses_.back());
        }
    }
    busname_to_bus_.insert({ buses_.back().name,&buses_.back() });
}

// добавление остановки в базу
void TransportCatalogue::AddStop(std::istream& input) {
    tc_input_reader::Stop stop = std::move(ParseStop(input));
    stops_.push_back({ stop.name,stop.lat,stop.lng,{} });
    stopname_to_stop_[stops_.back().name] = &stops_.back();
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
    auto it = stop_pair_to_distance_.find({ first,second });
    return it == stop_pair_to_distance_.end() ? 0. : it->second;
}

// получение информации о маршруте.
void TransportCatalogue::RouteInfo(std::istream& input) const {
    auto data = std::move(ParseQuery(input));
    for (const auto& [key, value] : data) {
        key == "Bus" ? PrintBusInfo(*this, value) : PrintStopInfo(*this, value);
    }
}