
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