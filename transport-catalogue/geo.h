#pragma once

#define EARTH_RADIUS (6371000)

namespace geo {

   struct Coordinates {
      double lat; // Широта
      double lng; // Долгота
      bool operator==(const Coordinates& other) const;
      bool operator!=(const Coordinates& other) const;
   };

   double ComputeDistance(Coordinates from, Coordinates to);

}  // namespace geo