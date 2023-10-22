#include "transport_catalogue.h"
#include "domain.h"

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

