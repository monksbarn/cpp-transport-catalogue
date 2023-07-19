#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <deque>
#include <vector>
#include <unordered_map>


namespace t_catalogue {

    struct Bus {
        std::string name;
        std::vector<std::string> stops;
        bool ring = false;
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

    //добавление расстояния между остановками
    void AddStopsDistance(const std::string& first, const std::string& second, const double distance);

    // поиск маршрута по имени
    const t_catalogue::Bus SearchRoute(const std::string& name) const;

    // поиск остановки по имени
    const t_catalogue::Stop SearchStop(const std::string& name) const;

    //получение информации о маршруте
    std::string GetRoutInfo(std::istream& input) const;

    // запрос известного расстояние между остановками
    double SearchPairStops(t_catalogue::Stop& first, t_catalogue::Stop& second) const;

private:

    std::deque<t_catalogue::Stop> stops_;
    std::deque<t_catalogue::Bus> buses_;

    std::unordered_map<std::string_view, t_catalogue::Stop*, std::hash<std::string_view>> stopname_to_stop_;
    std::unordered_map<std::string_view, t_catalogue::Bus*, std::hash<std::string_view>> busname_to_bus_;

};