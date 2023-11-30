#pragma once

#include "json.h"
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

#include <map>
#include <string>
#include <optional>
#include <string_view>

#define KMHOUR_METRMIN (1000. / 60.)

struct RouteFeatures {
   double velocity{};
   int wait_time{};
   RouteFeatures() = default;
   RouteFeatures(const RouteFeatures& other) {
      velocity = other.velocity;
      wait_time = other.wait_time;
   }
   RouteFeatures(RouteFeatures&& other)
   {
      velocity = std::move(other.velocity);
      wait_time = std::move(other.wait_time);
   }
};

RouteFeatures HandleRouteSettings(const json::Node& route_features);

//Данные для восстановления информации о ребре графа
struct Buffer {
   std::string_view bus_name;
   std::string_view from;
   int wait_time{};
   double weight{};
   int span_count{};
};

class TransportRouter {
public:
   TransportRouter(const TransportCatalogue& catalogue, RouteFeatures features);

   const Buffer& GetEdgeInfoFromId(size_t id) const;

   std::optional<size_t> GetIdFromStop(const std::string_view name) const;

   std::optional<typename graph::Router<double>::RouteInfo> BuildRoute(graph::VertexId from, graph::VertexId to) const;

private:
   graph::DirectedWeightedGraph<double> graph_;
   RouteFeatures route_features_;
   std::map<std::string_view, size_t> stop_to_id_;
   std::map<size_t, Buffer> buffer_;

   void SetStopID(const std::string_view name) {
      if (!stop_to_id_.count(name)) {
         size_t id = stop_to_id_.size();
         stop_to_id_[name] = id;
      }
   }
};



