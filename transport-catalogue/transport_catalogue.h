#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <deque>
#include <vector>
#include <unordered_map>

#include "input_reader.h"

namespace t_catalogue {
    struct Bus;
    struct Stop;

    struct Bus {
        std::string name;
        std::vector<Stop*> stops;
        bool ring = false;
    };

    struct Stop {
        std::string name;
        double lat;
        double lng;
        std::vector<Bus*> buses;
    };
}

struct Hasher {
    size_t operator()(const std::pair<t_catalogue::Stop*, t_catalogue::Stop*>& stop_pair) const {
        size_t lat_one = d_hasher(stop_pair.first->lat);
        size_t lng_one = d_hasher(stop_pair.first->lng);
        size_t lat_two = d_hasher(stop_pair.second->lat);
        size_t lng_two = d_hasher(stop_pair.second->lng);
        return lat_one + lng_one * 7 + lat_two * 49 + lng_two * (7 * 7 * 7);
    }
private:
    std::hash<double> d_hasher;
};

class TransportCatalogue {
public:
    explicit TransportCatalogue(std::istream& input);

    // добавление маршрута в базу
    void AddRoute(std::istream& input);

    // добавление остановки в базу
    void AddStop(std::istream& input);

    // поиск маршрута по имени
    const t_catalogue::Bus SearchRoute(const std::string& name) const;

    // поиск остановки по имени
    const t_catalogue::Stop SearchStop(const std::string& name) const;

    //получение информации о маршруте
    void RouteInfo(std::istream& input) const;

    // запрос известного расстояние между остановками
    double SearchPairStops(t_catalogue::Stop* first, t_catalogue::Stop* second) const;

private:
    std::unordered_map<std::pair<t_catalogue::Stop*, t_catalogue::Stop*>, double, Hasher> stop_pair_to_distance_;

    std::deque<t_catalogue::Stop> stops_;
    std::deque<t_catalogue::Bus> buses_;

    std::unordered_map<std::string_view, t_catalogue::Stop*, std::hash<std::string_view>> stopname_to_stop_;
    std::unordered_map<std::string_view, t_catalogue::Bus*, std::hash<std::string_view>> busname_to_bus_;
};