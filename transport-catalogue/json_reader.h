#pragma once

#include <sstream>
#include <iostream>

#include "geo.h"
#include "json.h"
#include "graph.h"
#include "domain.h"
#include "json_builder.h"
#include "request_handler.h"
#include "transport_catalogue.h"

//1) данные о маршрутах и остановках
//2) запросы к каталогу для поиска маршрутов и остановок
//3) данные о настройках прописовки маршрутов
struct RequestsDataBase {
   json::Node base_requests;
   json::Node stat_requests;
   json::Node render_settings;
   json::Node routing_settings;
};

class JSONReader {

public:
   //Преобразование входного потока в JSON-формат запросов к каталогу
   JSONReader(std::istream& input) : db_(std::move(ReadJSON(input))) {}

   RequestsDataBase ReadJSON(std::istream& input);

   //Добавление в каталог данных
   void FillCatalogue(TransportCatalogue& catalogue) const;

   graph::DirectedWeightedGraph<double> CreateGraph(const TransportCatalogue& catalogue);

   const json::Node GetBaseRequests() const;

   const json::Node GetStatRequests() const;

   const json::Node GetRenderSettings() const;

   const json::Node GetRoutingSettings() const;

   //Обработка JSON "stat_requests"
   json::Node HandleStatRequests(const RequestHandler& handler) const;


private:
   RequestsDataBase db_;

   //Преобразование JSON "base_requests" в очереди bus and stop
   domain::Queue CreateQueue(const json::Node& requests) const;

   //Возвращает данные об остановках в формате JSON
   json::Node StopAsJSON(const t_catalogue::Stop* stop_info, int id) const;

   //Возвращает данные об атвобусах в формате в JSON
   json::Node BusAsJSON(const t_catalogue::Bus* bus_info, int id) const;

   //Возвращает карту маршрутов в формате JSON
   json::Node MapAsJSON(std::string&& value, int id) const;

   //Возвращает описание кратчайшего маршрута в формате JSON
   json::Node RouteAsJSON(const typename graph::Router<double>::RouteInfo& route_info, const RequestHandler& handler, int id) const;

   //возвращает ответ в формате JSON в случае отсутсвия автобуса или остановки
   json::Node NotFound(const int id) const;

};
// //Преобразование входного потока в JSON
// RequestsDataBase ReadJSON(std::istream& input);

// //Преобразование JSON "base_requests" в очереди bus and stop
// domain::Queue CreateQueue(const json::Node& requests);

// //Добавление в каталог данных
// void FillCatalogue(TransportCatalogue& catalogue, const json::Node& base_requests);

// //Обработка JSON "stat_requests"
// json::Node HandleStatRequests(const RequestHandler& catalogue, const json::Node& stat_requests);

// //Преобразование данных об остановках в JSON
// json::Node StopAsJSON(const t_catalogue::Stop* stop_info, const int id);

// //Преобразование данных об автобусах в JSON
// json::Node BusAsJSON(const t_catalogue::Bus* bus_info, const int id);

// json::Node MapAsJSON(std::string&& value, const int id);

// //Ответ в формате JSON в случае отсутсвия автобуса или остановки
// json::Node NotFound(const int id);
