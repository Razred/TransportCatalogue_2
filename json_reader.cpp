#include "json_reader.h"

const json::Node& JsonReader::GetBaseRequest() const {
    if (input_.GetRoot().AsMap().count("base_requests") == 0)
        return nullptr_return;
    return input_.GetRoot().AsMap().at("base_requests");
}

const json::Node& JsonReader::GetStatRequest() const {
    if (input_.GetRoot().AsMap().count("stat_requests") == 0)
        return nullptr_return;
    return input_.GetRoot().AsMap().at("stat_requests");
}

const json::Node &JsonReader::GetRenderRequest() const {
    if (input_.GetRoot().AsMap().count("render_settings") == 0)
        return nullptr_return;
    return input_.GetRoot().AsMap().at("render_settings");
}


void JsonReader::FillingCatalogue(transport_catalogue::TransportCatalogue &catalogue) {
    const json::Array& arr = GetBaseRequest().AsArray();
    for (const auto& req : arr) {
        const auto& full_request = req.AsMap();
        const auto& type_request = full_request.at("type").AsString();
       if (type_request == "Stop") {
           auto [name_stop, coord, distance_stop] = FillingStop(full_request);
           catalogue.addStop(name_stop, coord);
           for (const auto& [name, dist] : distance_stop) {
               catalogue.SetDistance(name_stop, std::move(name), dist);
           }
       }
       if (type_request == "Bus") {
           auto [name_bus, stops, is_circle] = FillingBus(full_request);
           catalogue.addBus(std::move(name_bus), std::move(stops), is_circle);
       }
    }
}

std::tuple<std::string, geo::Coordinates, std::map<std::string, int>> JsonReader::FillingStop(const json::Dict& full_request) const {
    std::string name_stop = full_request.at("name").AsString();
    geo::Coordinates coord = {full_request.at("latitude").AsDouble(), full_request.at("longitude").AsDouble()};
    std::map<std::string, int> distance_stops;
    for (auto& [stop_name, dist] : full_request.at("road_distances").AsMap()) {
        distance_stops.emplace(stop_name, dist.AsInt());
    }
    return std::make_tuple(std::move(name_stop), coord, distance_stops);
}

std::tuple<std::string, std::vector<std::string>, bool> JsonReader::FillingBus(const json::Dict& full_request) const {
    std::string name_bus = full_request.at("name").AsString();
    std::vector<std::string> stops;
    for (auto& val : full_request.at("stops").AsArray()) {
        stops.push_back(std::move(val.AsString()));
    }
    bool is_circle = full_request.at("is_roundtrip").AsBool();
    return std::make_tuple(std::move(name_bus), std::move(stops), is_circle);
}

const render::MapRender JsonReader::FillingRenderSettings() {
    const json::Dict& dict = GetRenderRequest().AsMap();
    render::RenderSettings settings;
    settings.width = dict.at("width").AsDouble();
    settings.height = dict.at("height").AsDouble();
    settings.padiing = dict.at("padding").AsDouble();
    settings.line_width = dict.at("line_width").AsDouble();
    settings.stop_radius = dict.at("stop_radius").AsDouble();
    settings.bus_label_font_size = dict.at("bus_label_font_size").AsInt();
    settings.bus_label_offset = svg::Point{(dict.at("bus_label_offset").AsArray())[0].AsDouble(), (dict.at("bus_label_offset").AsArray())[1].AsDouble()};
    settings.stop_label_font_size = dict.at("stop_label_font_size").AsInt();
    settings.stop_label_offset = svg::Point{(dict.at("stop_label_offset").AsArray())[0].AsDouble(), (dict.at("stop_label_offset").AsArray())[1].AsDouble()};
    settings.underlayer_width = dict.at("underlayer_width").AsDouble();

    auto tmp = dict.at("underlayer_color");

    if (tmp.IsString())
        settings.underlayer_color = tmp.AsString();
   else {
       if (tmp.AsArray().size() == 3)
           settings.underlayer_color = svg::Rgb((tmp.AsArray())[0].AsInt(), (tmp.AsArray())[1].AsInt(), (tmp.AsArray())[2].AsInt());
       else
           settings.underlayer_color = svg::Rgba((tmp.AsArray())[0].AsInt(), (tmp.AsArray())[1].AsInt(), (tmp.AsArray())[2].AsInt(), (tmp.AsArray())[3].AsDouble());
   }

   tmp = dict.at("color_palette");
   std::vector<svg::Color> tmpvec;
   for (auto& val : tmp.AsArray()) {
       if (val.IsString())
           tmpvec.push_back(val.AsString());
       else {
           if (val.AsArray().size() == 3)
               tmpvec.push_back(svg::Rgb((val.AsArray())[0].AsInt(), (val.AsArray())[1].AsInt(), (val.AsArray())[2].AsInt()));
           else
               tmpvec.push_back(svg::Rgba((val.AsArray())[0].AsInt(), (val.AsArray())[1].AsInt(), (val.AsArray())[2].AsInt(), (val.AsArray())[3].AsDouble()));
       }
   }
   settings.color_palette = tmpvec;

   return settings;
}


