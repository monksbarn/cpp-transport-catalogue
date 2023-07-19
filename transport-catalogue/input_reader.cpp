
#include "input_reader.h"

#include <string_view>
#include <iomanip>
#include <unordered_map>

tc_input_reader::Queue CreateQueue(std::istream& input) {
    int size;
    input >> size;
    tc_input_reader::Queue queue;
    while (size--) {
        std::string  key;
        input >> key;
        key == "Stop" ? queue.stops.push_back(std::move(ParseStop(input))) : queue.buses.push_back(std::move(ParseBus(input)));
    }
    return queue;
}

tc_input_reader::Bus ParseBus(std::istream& input) {
    tc_input_reader::Bus bus;
    std::string key;
    std::getline(input, key, ':');
    RemoveSpaces(key);
    bus.name = std::move(key);
    while (true) {
        char ch = input.get();
        if (ch == '\n') {
            RemoveSpaces(key);
            bus.stops.push_back(std::move(key));
            break;
        }
        else if (ch == '>') {
            RemoveSpaces(key);
            bus.ring = true;
            bus.stops.push_back(std::move(key));
        }
        else if (ch == '-') {
            RemoveSpaces(key);
            bus.stops.push_back(std::move(key));
        }
        else {
            key += ch;
        }
    }
    return bus;
}


tc_input_reader::Stop ParseStop(std::istream& input) {
    tc_input_reader::Stop stop;
    char ch;
    std::getline(input, stop.name, ':');
    RemoveSpaces(stop.name);
    input >> stop.lat >> ch >> stop.lng;
    ch = input.get();
    if (ch == ',') {
        while (ch != '\n') {
            double dist;
            std::string other_stop;
            std::string trash;
            input >> dist;
            std::getline(input, trash, 'o');
            while (true) {
                ch = input.get();
                if (ch != ',' && ch != '\n') {
                    other_stop += ch;
                }
                else {
                    RemoveSpaces(other_stop);
                    stop.distance_to_other.push_back({ other_stop,dist });
                    break;
                }
            }
        }
    }
    else {
        input.putback(ch);
    }
    return stop;
}

std::vector<std::pair<std::string, std::string>> ParseQuery(std::istream& input)
{
    int count;
    input >> count;
    std::vector<std::pair<std::string, std::string>> content(count);
    for (int i = 0; i < count; ++i) {
        input >> content[i].first;
        std::getline(input, content[i].second);
        RemoveSpaces(content[i].second);
    }
    return content;
}

void RemoveSpaces(std::string& line) {
    line.erase(0, line.find_first_not_of(" "));
    line.erase(line.find_last_not_of(" ") + 1);
}

void FillTransportCatalogue(TransportCatalogue& catalogue, std::istream& input) {
    tc_input_reader::Queue queue = std::move(CreateQueue(input));
    std::unordered_map<std::string, t_catalogue::Stop> stopname_to_stop;
    std::unordered_map<std::pair<std::string, std::string>, double, Hasher> stoppair_to_distance;
    for (const auto& stop : queue.stops) {
        stopname_to_stop[stop.name] = { stop.name, stop.lat,stop.lng,{} };
        for (const auto& [other_name, distance] : stop.distance_to_other) {
            stoppair_to_distance.insert({ {stop.name,other_name},distance });
        }
    }
    for (const auto& bus : queue.buses) {
        t_catalogue::Bus bus_info = { bus.name,{},bus.ring };
        for (auto stop = bus.stops.begin(); stop != bus.stops.end();++stop) {
            auto next_stop = std::next(stop, 1);
            auto stop_info = stopname_to_stop.at(*stop);
            bus_info.stops.push_back(*stop);
            if (stop_info.buses.empty() || stop_info.buses.back() != bus.name) {
                stopname_to_stop.at(*stop).buses.push_back(bus_info.name);
            }
            if (next_stop != bus.stops.end()) {
                auto next_stop_info = stopname_to_stop.at(*next_stop);
                double geo_length = ComputeDistance(Coordinates{ stop_info.lat,stop_info.lng }, Coordinates{ next_stop_info.lat,next_stop_info.lng });
                auto res = stoppair_to_distance.find(std::make_pair(*stop, *next_stop));
                int length_to = (res != stoppair_to_distance.end()) ? res->second : 0;
                res = stoppair_to_distance.find(std::make_pair(*next_stop, *stop));
                int length_from = (res != stoppair_to_distance.end()) ? res->second : 0;
                bus_info.geo_length += bus_info.ring ? geo_length : geo_length * 2;
                if (length_to && length_from) {
                    bus_info.real_length += (bus_info.ring) ? length_to : length_to + length_from;
                }
                else if (length_to) {
                    bus_info.real_length += (bus_info.ring) ? length_to : length_to * 2;
                }
                else if (length_from) {
                    bus_info.real_length += (bus_info.ring) ? length_from : length_from * 2;
                }
                else {
                    bus_info.real_length += (bus_info.ring) ? geo_length : geo_length * 2;
                }

            }
        }
        catalogue.AddBus(bus_info);
    }
    for (const auto& [key, value] : stopname_to_stop) {
        catalogue.AddStop(value);
    }
}
