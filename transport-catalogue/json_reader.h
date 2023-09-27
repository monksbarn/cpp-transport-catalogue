#pragma once

#include <sstream>
#include <iostream>

#include "geo.h"
#include "json.h"
#include "domain.h"
#include "request_handler.h"
#include "transport_catalogue.h"

//1) данные о маршрутах и остановках
//2) запросы к каталогу для поиска маршрутов и остановок
//3) данные о настройках прописовки маршрутов
struct RequestsDataBase {
  json::Node base_requests;
  json::Node stat_requests;
  json::Node render_settings;
};

//Преобразование входного потока в JSON
RequestsDataBase ReadJSON(std::istream& input);

//Преобразование JSON "base_requests" в очереди bus and stop
domain::Queue CreateQueue(const json::Node& requests);

//Добавление в каталог данных
void FillCatalogue(TransportCatalogue& catalogue, const json::Node& base_requests);

//Обработка JSON "stat_requests"
json::Node HandleStatRequests(const RequestHandler& catalogue, const json::Node& stat_requests);

//Преобразование данных об остановках в JSON
json::Node StopAsJSON(const t_catalogue::Stop* stop_info, const int id);

//Преобразование данных об автобусах в JSON
json::Node BusAsJSON(const t_catalogue::Bus* bus_info, const int id);

json::Node MapAsJSON(std::string&& value, const int id);

//Ответ в формате JSON в случае отсутсвия автобуса или остановки
json::Node NotFound(const int id);
