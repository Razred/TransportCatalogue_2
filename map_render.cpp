#include "map_render.h"


bool render::IsZero(double value) {
    return std::abs(value) < EPSILON;
}

std::vector<svg::Polyline> render::MapRender::GetRoutes(const std::map<std::string_view, const domain::Bus *> & buses, const std::map<std::string_view, const domain::Stop*>& stops,
                                                            const SphereProjector & sp) const {
    std::vector<svg::Polyline> res;
    size_t color_counter = 0;
    for (const auto& [name_bus, bus_info] : buses) {
        if (bus_info->stops.empty())
            continue;
        svg::Polyline line;
        for (const auto& stop: bus_info->stops) {
            line.AddPoint(sp(stops.at(stop)->coordinates));
        }
        line.SetStrokeColor(settings.color_palette[color_counter++]);
        line.SetFillColor(svg::NoneColor);
        line.SetStrokeWidth(settings.line_width);
        line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        res.push_back(line);

        if (color_counter == settings.color_palette.size()) {
            color_counter = 0;
        }
    }
    return res;
}

svg::Document render::MapRender::GetSvg(const std::map<std::string_view, const domain::Bus *>& buses,
                                        const std::map<std::string_view, const domain::Stop *>& stops) const {
    svg::Document res;
    std::vector<geo::Coordinates> coords;
    for (const auto& [name_bus, bus_info] : buses) {
        for (const auto& stop : bus_info->stops) {
           coords.push_back(stops.at(stop)->coordinates);
        }
    }
    SphereProjector sp(coords.begin(), coords.end(), settings.width, settings.height, settings.padiing);
    for (const auto& pol : GetRoutes(buses, stops, sp)) {
        res.Add(pol);
    }
    for (const auto& text : GetRouteNames(buses, stops, sp)) {
        res.Add(text);
    }
    auto pair_circ_text = GetStop(buses, stops, sp);
    for (const auto& circ : pair_circ_text.first) {
        res.Add(circ);
    }
    for (const auto& text : pair_circ_text.second) {
        res.Add(text);
    }
    return res;
}

std::vector<svg::Text> render::MapRender::GetRouteNames(const std::map<std::string_view, const domain::Bus *> &buses,
                                                   const std::map<std::string_view, const domain::Stop *> &stops,
                                                   const render::SphereProjector &sp) const {
   std::vector<svg::Text> res;
   int color_counter = 0;
   for (const auto& [bus_name, bus_info] : buses) {
       if (bus_info->stops.empty())
           continue;
        int i = 0;
        int num_stop = 0;
        while (i != 2) {
            svg::Text text;
            svg::Text background;
            if (bus_info->is_Circle)
                i++;
            auto stop_info = stops.at(bus_info->stops[num_stop]);
            text.SetPosition(sp(stop_info->coordinates));
            background.SetPosition(sp(stop_info->coordinates));
            text.SetOffset(settings.bus_label_offset);
            background.SetOffset(settings.bus_label_offset);
            text.SetFontSize(settings.bus_label_font_size);
            background.SetFontSize(settings.bus_label_font_size);
            text.SetFontFamily("Verdana");
            background.SetFontFamily("Verdana");
            text.SetFontWeight("bold");
            background.SetFontWeight("bold");
            text.SetData(std::string(bus_name));
            background.SetData(std::string(bus_name));
            background.SetFillColor(settings.underlayer_color);
            background.SetStrokeColor(settings.underlayer_color);
            background.SetStrokeWidth(settings.underlayer_width);
            background.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            background.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            text.SetFillColor(settings.color_palette[color_counter]);
            res.push_back(background);
            res.push_back(text);
            i++;
            num_stop = (bus_info->stops.size() - 1);
        }
       if (++color_counter == settings.color_palette.size()) {
           color_counter = 0;
       }
   }
   return res;
}

std::pair<std::vector<svg::Circle>, std::vector<svg::Text>> render::MapRender::GetStop(const std::map<std::string_view, const domain::Bus *> & buses,
                                                          const std::map<std::string_view, const domain::Stop *> &stops,
                                                          const render::SphereProjector &sp) const {
   std::vector<svg::Circle> res_cir;
   std::vector<svg::Text> res_text;
   for (const auto& [bus_name, bus_info] : buses) {
       if (bus_info->stops.empty())
           continue;
       for (const auto& stop : bus_info->stops) {
           svg::Circle circle;
           svg::Text text;
           svg::Text background;
           circle.SetCenter(sp(stops.at(stop)->coordinates));
           circle.SetRadius(settings.stop_radius);
           circle.SetFillColor("white");

           text.SetPosition(sp(stops.at(stop)->coordinates));
           background.SetPosition(sp(stops.at(stop)->coordinates));
           text.SetOffset(settings.stop_label_offset);
           background.SetOffset((settings.stop_label_offset));
           text.SetFontSize(settings.stop_label_font_size);
           background.SetFontSize(settings.stop_label_font_size);
           text.SetFontFamily("Verdana");
           background.SetFontFamily("Verdana");
           text.SetData(stop);
           background.SetData(stop);

           background.SetFillColor(settings.underlayer_color);
           background.SetStrokeColor(settings.underlayer_color);
           background.SetStrokeWidth(settings.underlayer_width);
           background.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
           background.SetStrokeLineCap(svg::StrokeLineCap::ROUND);

           text.SetFillColor("black");

           res_text.push_back(background);
           res_text.push_back(text);
           res_cir.push_back(circle);
       }
   }
   return std::make_pair(res_cir, res_text);
}


