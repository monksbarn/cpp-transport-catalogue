#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "transport_catalogue.h"

int UniqueStopsCount(std::vector<t_catalogue::Stop*> stops);

void SortBusInfo(std::vector<t_catalogue::Bus*>& buses);

void PrintBusInfo(const TransportCatalogue& catalogue, const std::string& bus);

void PrintStopInfo(const TransportCatalogue& catalogue, const std::string& stop);
