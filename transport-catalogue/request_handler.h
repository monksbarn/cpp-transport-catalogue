#pragma once


#include <string_view>
#include <vector>
#include <string>


#include "transport_catalogue.h"
#include "transport_router.h"
#include "map_renderer.h"
#include "domain.h"



class RequestHandler {
public:
   RequestHandler(const TransportCatalogue& catalogue, MapRenderer&& renderer, TransportRouter&& router) :
      catalogue_(catalogue),
      renderer_(std::move(renderer)),
      router_(std::move(router)) {
   }

   //Возвращает информацию о маршруте 
   const t_catalogue::Bus* GetBusInfo(std::string_view bus_name) const;

   //Возвращает информацию об остановке
   const t_catalogue::Stop* GetStopInfo(std::string_view stop_name) const;

   //Возвращает маршруты, проходящие через остановку
   const std::vector<std::string>* GetBusesByStop(std::string_view stop_name) const;

   //Возвращает контейнкр svg
   const svg::Document RenderMap() const;

   //Возвращает информацию о ребре по его ID
   const Buffer& GetEdgeInfoFromId(size_t id) const;

   //Возвращает ID остановки по названию
   std::optional<size_t> GetIdFromStop(std::string_view name) const;

   //Построить маршрут
   std::optional<typename graph::Router<double>::RouteInfo>  BuildRoute(graph::VertexId from, graph::VertexId to) const {
      return router_.BuildRoute(from, to);
   }

private:

   const TransportCatalogue& catalogue_;
   const MapRenderer renderer_;
   const TransportRouter router_;
};


