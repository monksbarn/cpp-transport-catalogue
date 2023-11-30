#include "transport_catalogue.h"


//добавление маршрута в базу
void TransportCatalogue::AddBus(const t_catalogue::Bus& bus) {
   buses_.push_back(bus);
   busname_to_bus_.insert({ buses_.back().name,&buses_.back() });
}

// добавление остановки в базу
void TransportCatalogue::AddStop(const t_catalogue::Stop& stop) {
   stops_.push_back(stop);
   stopname_to_stop_[stops_.back().name] = &stops_.back();
   domain::SortBuses(stops_.back().buses);
}

// поиск маршрута по имени
const t_catalogue::Bus* TransportCatalogue::SearchRoute(std::string_view name) const {
   auto it = busname_to_bus_.find(name);
   return it != busname_to_bus_.end() ? it->second : nullptr;
}

// поиск остановки по имени
const t_catalogue::Stop* TransportCatalogue::SearchStop(std::string_view name) const {
   auto it = stopname_to_stop_.find(name);
   return it != stopname_to_stop_.end() ? it->second : nullptr;
}


const std::deque<t_catalogue::Stop>& TransportCatalogue::GetStopsInfo() const {
   return stops_;
}

const std::deque<t_catalogue::Bus>& TransportCatalogue::GetBusInfo() const {
   return buses_;
}

size_t TransportCatalogue::GetUniqueStopsCount() const {
   return stops_.size();
}

double TransportCatalogue::GetDistanceBetweenStops(const std::string& bus, const std::string& from, const std::string& to) const {
   const auto result = stops_distances_.at(bus).find(std::make_pair(from, to));
   return result != stops_distances_.at(bus).end() ? result->second : 0;
}

void TransportCatalogue::SetDistanceBetweenStops(const std::string& bus, const std::string& from, const std::string& to, double length_to, double length_from) {
   stops_distances_[bus][std::make_pair(from, to)] = length_to;
   stops_distances_[bus][std::make_pair(to, from)] = length_from;
}

void TransportCatalogue::SetDistanceBetweenStops(const std::string& bus, const std::string& from, const std::string& to, double length) {
   stops_distances_[bus][std::make_pair(from, to)] = length;
   stops_distances_[bus][std::make_pair(to, from)] = length;
}


