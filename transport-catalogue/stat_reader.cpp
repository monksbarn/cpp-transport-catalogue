#include "stat_reader.h"
#include "geo.h"



int UniqueStopsCount(std::vector<t_catalogue::Stop*> stops) {
    std::sort(stops.begin(), stops.end(), [](const t_catalogue::Stop* left, const t_catalogue::Stop* right) {
        return left->name > right->name;
        });
    return std::distance(stops.begin(), std::unique(stops.begin(), stops.end()));
}

void PrintBusInfo(const TransportCatalogue& catalogue, const std::string& bus) {
    t_catalogue::Bus bus_info = std::move(catalogue.SearchRoute(bus));
    if (!bus_info.name.empty()) {
        double geo_length = 0.;
        double real_length = 0;
        double real_length_reverce = 0;
        int stops_count = bus_info.stops.size();
        for (auto begin = bus_info.stops.begin(); begin != bus_info.stops.end(); ++begin) {
            auto nearest = std::next(begin, 1);
            if (nearest != bus_info.stops.end()) {
                geo_length += ComputeDistance(Coordinates{ (*begin)->lat,(*begin)->lng }, Coordinates{ (*nearest)->lat,(*nearest)->lng });
                double length_to = catalogue.SearchPairStops(*begin, *nearest);
                double length_from = catalogue.SearchPairStops(*nearest, *begin);
                if (length_to && length_from) {
                    real_length += length_to;
                    real_length_reverce += length_from;
                }
                else if (length_to) {
                    real_length += length_to;
                    real_length_reverce += length_to;
                }
                else if (length_from) {
                    real_length += length_from;
                    real_length_reverce += length_from;
                }
                else {
                    real_length = geo_length;
                    real_length_reverce = geo_length;
                }
            }
        }
        if (!bus_info.ring) {
            geo_length *= 2;
            real_length += real_length_reverce;
            stops_count = stops_count * 2 - 1;

        }
        std::cout << "Bus " << bus << ": " << stops_count << " stops on route, " << UniqueStopsCount(bus_info.stops) << " unique stops, " << real_length << " route length, " << real_length / geo_length << " curvature" << std::endl;
    }
    else {
        std::cout << "Bus " << bus << ": not found" << std::endl;
    }
}

void PrintStopInfo(const TransportCatalogue& catalogue, const std::string& stop) {
    t_catalogue::Stop stop_info = std::move(catalogue.SearchStop(stop));
    std::cout << "Stop " << stop << ":";
    if (!stop_info.name.empty()) {
        if (stop_info.buses.empty()) {
            std::cout << " no buses";
        }
        else {
            std::cout << " buses";
            for (const auto& bus : stop_info.buses) {
                std::cout << " " << bus->name;
            }
        }
    }
    else {
        std::cout << " not found";
    }
    std::cout << std::endl;
}

void SortBusInfo(std::vector<t_catalogue::Bus*>& buses) {
    std::sort(buses.begin(), buses.end(), [](const t_catalogue::Bus* lhs, const t_catalogue::Bus* rhs) {
        return std::lexicographical_compare(lhs->name.begin(), lhs->name.end(), rhs->name.begin(), rhs->name.end());
        });
}
