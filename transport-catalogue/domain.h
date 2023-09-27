#pragma once

#include <vector>
#include <string>
#include <deque>
#include <map>
#include <algorithm>

#include "json.h"

struct Hasher {
    size_t operator()(const std::pair<std::string, std::string>& stop_pair) const {
        size_t one = s_hasher(stop_pair.first);
        size_t two = s_hasher(stop_pair.second);
        return one + two * 43;
    }
private:
    std::hash<std::string> s_hasher;
};

namespace domain {
    struct Bus {
        std::string name;
        std::vector<std::string> stops;
        bool is_roundtrip = false;
    };

    struct Stop {
        std::string name;
        double lat;
        double lng;
        std::map<std::string, double> distance_to_other;
    };

    struct Queue {
        std::deque<Stop> stops;
        std::deque<Bus> buses;
    };

    int UniqueStopsCount(std::vector<std::string> stops);

    void SortBuses(std::vector<std::string>& buses);

}



