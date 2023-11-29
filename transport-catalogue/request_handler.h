#pragma once


#include <string_view>
#include <vector>
#include <string>


#include "transport_catalogue.h"
#include "map_renderer.h"
#include "domain.h"
#include "router.h"
#include "graph.h"



class RequestHandler {
public:
   RequestHandler(const TransportCatalogue& db, MapRenderer renderer) :
      catalogue_(db),
      renderer_(std::move(renderer)) {
   }
   RequestHandler(const TransportCatalogue& db, graph::DirectedWeightedGraph<double>&& graph) :
      catalogue_(db),
      graph_(std::move(graph)),
      router_(graph_) {
   }

   RequestHandler(const TransportCatalogue& db, MapRenderer renderer, graph::DirectedWeightedGraph<double>&& graph) :
      catalogue_(db),
      renderer_(renderer),
      graph_(std::move(graph)),
      router_(graph_) {
   }

   //Возвращает информацию о маршруте 
   const t_catalogue::Bus* GetBusInfo(std::string_view bus_name) const;

   //Возвращает информацию об остановке
   const t_catalogue::Stop* GetStopInfo(std::string_view stop_name) const;

   //Возвращает маршруты, проходящие через остановку
   const std::vector<std::string>* GetBusesByStop(std::string_view stop_name) const;

   //Возвращает контейнкр svg
   const svg::Document RenderMap() const;

   std::optional<typename graph::Router<double>::RouteInfo> BuildRoute(graph::VertexId from, graph::VertexId to) const;

   const graph::Edge<double>& GetEdge(graph::EdgeId edge_id) const;

   virtual ~RequestHandler() = default;

private:
   // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
   const TransportCatalogue& catalogue_;
   const MapRenderer renderer_;
   const graph::DirectedWeightedGraph<double> graph_;
   const graph::Router<double> router_;
};




