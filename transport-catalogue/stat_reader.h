#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include "transport_catalogue.h"

int UniqueStopsCount(std::vector<t_catalogue::Stop*> stops);

void SortBusInfo(std::vector<t_catalogue::Bus*>& buses);

std::string PrintBusInfo(const TransportCatalogue& catalogue, const std::string& bus);

std::string PrintStopInfo(const TransportCatalogue& catalogue, const std::string& stop);
