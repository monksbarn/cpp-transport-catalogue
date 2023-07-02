#include "stat_reader.h"

int UniqueStopsCount(std::vector<t_catalogue::Stop*> stops) {
    std::sort(stops.begin(), stops.end(), [](const t_catalogue::Stop* left, const t_catalogue::Stop* right) {
        return left->name > right->name;
        });
    return std::distance(stops.begin(), std::unique(stops.begin(), stops.end()));
}

std::string PrintBusInfo(const TransportCatalogue& catalogue, const std::string& bus) {
    std::string out;
    t_catalogue::Bus bus_info = std::move(catalogue.SearchRoute(bus));
    if (!bus_info.name.empty()) {
        std::stringstream ss;;
        ss << std::setprecision(6) << bus_info.real_length / bus_info.geo_length;
        int stops_count = (bus_info.ring) ? bus_info.stops.size() : bus_info.stops.size() * 2 - 1;
        out = ("Bus " + bus + ": " + std::to_string(stops_count) + " stops on route, " + std::to_string(UniqueStopsCount(bus_info.stops)) + " unique stops, " + std::to_string(bus_info.real_length) + " route length, " + ss.str() + " curvature");
    }
    else {
        out = ("Bus " + bus + ": not found");
    }
    return out;
}

std::string PrintStopInfo(const TransportCatalogue& catalogue, const std::string& stop) {
    std::string out;
    t_catalogue::Stop stop_info = std::move(catalogue.SearchStop(stop));
    out += ("Stop " + stop + ":");
    if (!stop_info.name.empty()) {
        if (stop_info.buses.empty()) {
            out += " no buses";
        }
        else {
            out += " buses";
            for (const auto& bus : stop_info.buses) {
                out += (" " + bus->name);
            }
        }
    }
    else {
        out += " not found";
    }
    return out;
}

void SortBusInfo(std::vector<t_catalogue::Bus*>& buses) {
    std::sort(buses.begin(), buses.end(), [](const t_catalogue::Bus* lhs, const t_catalogue::Bus* rhs) {
        return std::lexicographical_compare(lhs->name.begin(), lhs->name.end(), rhs->name.begin(), rhs->name.end());
        });
}
