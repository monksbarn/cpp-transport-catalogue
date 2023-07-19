#include "transport_catalogue.h"
#include "stat_reader.h"
#include "geo.h"

#include <algorithm>

//добавление маршрута в базу
void TransportCatalogue::AddBus(const t_catalogue::Bus& bus) {
    buses_.push_back(bus);
    busname_to_bus_.insert({ buses_.back().name,&buses_.back() });
}

// добавление остановки в базу
void TransportCatalogue::AddStop(const t_catalogue::Stop& stop) {
    stops_.push_back(stop);
    stopname_to_stop_[stops_.back().name] = &stops_.back();
    SortBusInfo(stops_.back().buses);
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

// получение информации о маршруте.
std::string TransportCatalogue::GetRoutInfo(std::istream& input) const {
    return PrintRoutInfo(*this, input);
}

