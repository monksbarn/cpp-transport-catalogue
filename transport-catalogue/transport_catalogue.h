#pragma once

#include <string>
#include <string_view>
#include <deque>
#include <vector>
#include <unordered_map>

namespace t_catalogue {

    struct Bus {
        std::string name;
        std::vector<std::string> stops;
        bool is_roundtrip = false;
        int real_length = 0;
        double geo_length = 0;
    };

    struct Stop {
        std::string name;
        double lat;
        double lng;
        std::vector<std::string> buses;
    };
}


class TransportCatalogue {
public:

    // добавление маршрута в базу
    void AddBus(const t_catalogue::Bus& bus);

    // добавление остановки в базу
    void AddStop(const t_catalogue::Stop& stop);

    // поиск маршрута по имени
    const t_catalogue::Bus* SearchRoute(std::string_view name) const;

    // поиск остановки по имени
    const t_catalogue::Stop* SearchStop(std::string_view name) const;

    // получение информации об остановках
    const std::deque<t_catalogue::Stop>& GetStopsInfo() const;

    // получение информации об автобусах
    const std::deque<t_catalogue::Bus>& GetBusInfo() const;

private:

    std::deque<t_catalogue::Stop> stops_;
    std::deque<t_catalogue::Bus> buses_;

    std::unordered_map<std::string_view, t_catalogue::Stop*, std::hash<std::string_view>> stopname_to_stop_;
    std::unordered_map<std::string_view, t_catalogue::Bus*, std::hash<std::string_view>> busname_to_bus_;

};