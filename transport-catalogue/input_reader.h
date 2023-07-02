#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <deque>

#include "geo.h"

namespace tc_input_reader {
    namespace detail {
        struct DistanceToOther {
            std::string other_name;
            double distance;
        };
    }
    struct Bus {
        std::string name;
        std::vector<std::string> stops;
        bool ring = false;
    };
    struct Stop {
        std::string name;
        double lat;
        double lng;
        std::vector<detail::DistanceToOther> distance_to_other;
    };
    struct Queue {
        std::deque<Stop> stops;
        std::vector<Bus> buses;
    };
}

tc_input_reader::Queue CreateQueue(std::istream& input);

std::vector<std::pair<std::string, std::string>> ParseQuery(std::istream& input);

tc_input_reader::Bus ParseBus(std::istream& input);

tc_input_reader::Stop ParseStop(std::istream& input);

void RemoveSpaces(std::string& line);