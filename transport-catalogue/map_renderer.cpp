#include "map_renderer.h"

bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

void Sorting(std::deque<t_catalogue::Bus>& buses) {
    std::sort(buses.begin(), buses.end(), [](const auto& lhs, const auto& rhs) {
        return std::lexicographical_compare(lhs.name.begin(), lhs.name.end(), rhs.name.begin(), rhs.name.end());
        });
}
void Sorting(std::deque<t_catalogue::Stop>& buses) {
    std::sort(buses.begin(), buses.end(), [](const auto& lhs, const auto& rhs) {
        return std::lexicographical_compare(lhs.name.begin(), lhs.name.end(), rhs.name.begin(), rhs.name.end());
        });
}
//________________________________________________________________________________________________________________


//================================================================================================================HANDLE_RENDER_SETTINGS
RenderSettings HandleRenderSettings(const json::Node& render) {
    using namespace std::literals;
    RenderSettings ren_set;
    for (const auto& [key, value] : render.AsMap()) {
        if (key == "width"s) {
            ren_set.width = value.AsDouble();
        }
        else if (key == "height"s) {
            ren_set.height = value.AsDouble();
        }
        else if (key == "padding"s) {
            ren_set.padding = value.AsDouble();
        }
        else if (key == "line_width"s) {
            ren_set.line_width = value.AsDouble();
        }
        else if (key == "stop_radius"s) {
            ren_set.stop_radius = value.AsDouble();
        }
        else if (key == "bus_label_font_size"s) {
            ren_set.bus_label_font_size = value.AsInt();
        }
        else if (key == "stop_label_font_size"s) {
            ren_set.stop_label_font_size = value.AsInt();
        }
        else if (key == "bus_label_offset"s) {
            for (const auto& item : value.AsArray()) {
                ren_set.bus_label_offset.push_back(item.AsDouble());
            }
        }
        else if (key == "stop_label_font_size"s) {
            ren_set.stop_label_font_size = value.AsDouble();
        }
        else if (key == "stop_label_offset") {
            for (const auto& item : value.AsArray()) {
                ren_set.stop_label_offset.push_back(item.AsDouble());
            }
        }
        else if (key == "underlayer_color"s) {
            if (value.IsString()) {
                ren_set.underlayer_color = value.AsString();
            }
            else {
                if (value.AsArray().size() == 3) {
                    ren_set.underlayer_color = (svg::Rgb{
                         static_cast<uint8_t>(value.AsArray().at(0).AsInt()),
                         static_cast<uint8_t>(value.AsArray().at(1).AsInt()),
                         static_cast<uint8_t>(value.AsArray().at(2).AsInt()) });
                }
                else {
                    ren_set.underlayer_color = (svg::Rgba{
                         static_cast<uint8_t>(value.AsArray().at(0).AsInt()),
                         static_cast<uint8_t>(value.AsArray().at(1).AsInt()),
                         static_cast<uint8_t>(value.AsArray().at(2).AsInt()),
                         value.AsArray().at(3).AsDouble() });
                }
            }
        }
        else if (key == "underlayer_width"s) {
            ren_set.underlayer_width = value.AsDouble();
        }
        else if (key == "color_palette"s) {
            for (const auto& item : value.AsArray()) {
                if (item.IsString()) {
                    ren_set.color_palette.push_back(item.AsString());
                }
                else {
                    if (item.AsArray().size() == 3) {
                        ren_set.color_palette.push_back(svg::Rgb{
                        static_cast<uint8_t>(item.AsArray().at(0).AsInt()),
                        static_cast<uint8_t>(item.AsArray().at(1).AsInt()),
                        static_cast<uint8_t>(item.AsArray().at(2).AsInt()) });
                    }
                    else {
                        ren_set.color_palette.push_back(svg::Rgba{
                        static_cast<uint8_t>(item.AsArray().at(0).AsInt()),
                        static_cast<uint8_t>(item.AsArray().at(1).AsInt()),
                        static_cast<uint8_t>(item.AsArray().at(2).AsInt()),
                        item.AsArray().at(3).AsDouble() });
                    }
                }
            }
        }
        else {
            throw std::logic_error("HandleRenderSettings() error: the key \""s + key + "\" dosn't exists for this context"s);
        }
    }
    return ren_set;
}
//________________________________________________________________________________________________________________


//================================================================================================================TEXT_RENDER_GENERAL_SETTINGS
void GeneralTextRenderSettings(svg::Text& text, const svg::Point& point, const std::vector<double>& offset, const double font_size) {
    using namespace std::literals;
    text.SetPosition(point).
        SetOffset({ offset.front(),offset.back() }).
        SetFontSize(font_size).
        SetFontFamily("Verdana"s);
}


//================================================================================================================MAKE_SVG
const svg::Document MapRenderer::MakeSVG(const TransportCatalogue& catalogue) const {
    using namespace std::literals;
    std::vector<geo::Coordinates> coordinates;
    for (const auto& bus : catalogue.GetBusInfo()) {
        for (const auto& stop : bus.stops) {
            const t_catalogue::Stop* s = catalogue.SearchStop(stop);
            coordinates.push_back({ s->lat,s->lng });
        }
    }
    SphereProjector projector(coordinates.begin(), coordinates.end(), settings_.width, settings_.height, settings_.padding);
    svg::Document doc;
    size_t color_palette_index = 0;
    std::deque<t_catalogue::Bus> buses = catalogue.GetBusInfo();
    Sorting(buses);
    std::vector<svg::Text> text;
    for (const auto& bus : buses) {
        svg::Polyline route_line;
        for (auto stop = bus.stops.begin(); stop != bus.stops.end(); ++stop) {
            const t_catalogue::Stop* stop_info = catalogue.SearchStop(*stop);
            if (stop_info) {
                svg::Point point = projector({ stop_info->lat,stop_info->lng });
                route_line.AddPoint(point);
                if (stop == bus.stops.begin() || (stop == std::next(bus.stops.end(), -1)/*  && !bus.is_roundtrip */ && bus.stops.front() != bus.stops.back())) {
                    text.push_back(svg::Text{});
                    GeneralTextRenderSettings(text.back(), point, settings_.bus_label_offset, settings_.bus_label_font_size);
                    text.back().SetData(bus.name).
                        SetFillColor(settings_.underlayer_color).
                        SetStrokeColor(settings_.underlayer_color).
                        SetStrokeWidth(settings_.underlayer_width).
                        SetStrokeLineCap(svg::StrokeLineCap::ROUND).
                        SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).
                        SetFontWeight("bold"s);
                    text.push_back(svg::Text{});
                    GeneralTextRenderSettings(text.back(), point, settings_.bus_label_offset, settings_.bus_label_font_size);
                    text.back().SetData(bus.name).
                        SetFillColor(settings_.color_palette.at(color_palette_index % settings_.color_palette.size())).
                        SetFontWeight("bold"s);
                }
            }
        }
        if (!bus.is_roundtrip) {
            for (auto it = bus.stops.rbegin() + 1; it != bus.stops.rend(); ++it) {
                if (it != bus.stops.rend()) {
                    const t_catalogue::Stop* stop_info = catalogue.SearchStop(*it);
                    if (stop_info) {
                        route_line.AddPoint(projector({ stop_info->lat,stop_info->lng }));
                    }
                }
            }
        }
        route_line.SetStrokeWidth(settings_.line_width)
            .SetStrokeColor(settings_.color_palette.at(color_palette_index % settings_.color_palette.size()))
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
            .SetFillColor(svg::NoneColor);
        doc.Add(route_line);
        ++color_palette_index;
    }
    for (const auto& t : text) {
        doc.Add(t);
    }
    std::deque<t_catalogue::Stop> stops = catalogue.GetStopsInfo();
    Sorting(stops);
    text.clear();
    for (const auto& stop : stops) {
        using namespace std::literals;
        if (!stop.buses.empty()) {
            svg::Circle circle;
            svg::Point point = projector({ stop.lat, stop.lng });
            circle.SetCenter(point).SetRadius(settings_.stop_radius).SetFillColor("white"s);
            doc.Add(circle);
            text.push_back(svg::Text{});
            GeneralTextRenderSettings(text.back(), point, settings_.stop_label_offset, settings_.stop_label_font_size);
            text.back().SetData(stop.name).
                SetFillColor(settings_.underlayer_color).
                SetStrokeColor(settings_.underlayer_color).
                SetStrokeWidth(settings_.underlayer_width).
                SetStrokeLineCap(svg::StrokeLineCap::ROUND).
                SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            text.push_back(svg::Text{});
            GeneralTextRenderSettings(text.back(), point, settings_.stop_label_offset, settings_.stop_label_font_size);
            text.back().SetData(stop.name).SetFillColor("black"s);
        }
    }
    for (const auto& t : text) {
        doc.Add(t);
    }
    return doc;
}

//OT9Bcs7tyVD4B
