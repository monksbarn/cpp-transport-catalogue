#pragma once

#include <string>
#include <string_view>
#include <vector>


#include "transport_catalogue.h"
#include "map_renderer.h"


/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

 // Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
 // с другими подсистемами приложения.
 // См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)

class RequestHandler {
public:
   RequestHandler(const TransportCatalogue& db, MapRenderer renderer) : db_(db), renderer_(std::move(renderer)) {}

   //Возвращает информацию о маршруте 
   const t_catalogue::Bus* GetBusInfo(std::string_view bus_name) const;

   //Возвращает информацию об остановке
   const t_catalogue::Stop* GetStopInfo(std::string_view stop_name) const;

   //Возвращает маршруты, проходящие через остановку
   const std::vector<std::string>* GetBusesByStop(std::string_view stop_name) const;

   //Возвращает контейнкр svg
   const svg::Document RenderMap() const;

   virtual ~RequestHandler() = default;

private:
   // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
   const TransportCatalogue& db_;
   const MapRenderer renderer_;
};



