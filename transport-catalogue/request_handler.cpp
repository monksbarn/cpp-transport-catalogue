#include "request_handler.h"

const t_catalogue::Bus* RequestHandler::GetBusInfo(std::string_view bus_name) const {
   return db_.SearchRoute(bus_name);
}

const t_catalogue::Stop* RequestHandler::GetStopInfo(std::string_view stop_name) const {
   return db_.SearchStop(stop_name);
}

const std::vector<std::string>* RequestHandler::GetBusesByStop(std::string_view stop_name) const {
   const t_catalogue::Stop* stop = db_.SearchStop(stop_name);
   return stop ? &stop->buses : nullptr;
}

const svg::Document RequestHandler::RenderMap() const {
   return renderer_.MakeSVG(db_);
}
