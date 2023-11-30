#include "domain.h"

int domain::UniqueStopsCount(std::vector<std::string> stops) {
   std::sort(stops.begin(), stops.end(), [](const auto left, const auto right) {
      return left > right;
      });
   return std::distance(stops.begin(), std::unique(stops.begin(), stops.end()));
}


void domain::SortBuses(std::vector<std::string>& buses) {
   std::sort(buses.begin(), buses.end(), [](const auto& lhs, const auto& rhs) {
      return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
      });
}