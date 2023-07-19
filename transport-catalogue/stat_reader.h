#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include "transport_catalogue.h"
#include "input_reader.h"

int UniqueStopsCount(std::vector<std::string> stops);

void SortBusInfo(std::vector<std::string>& buses);

std::string GetBusInfo(const TransportCatalogue& catalogue, const std::string& bus);

std::string GetStopInfo(const TransportCatalogue& catalogue, const std::string& stop);

std::string PrintRoutInfo(const TransportCatalogue& catalogue, std::istream& input);
