#include "request_handler.h"
void RequestHandler::RequestProccesing() {
    json::Array res;
    auto& req = request.GetStatRequest().AsArray();
    for (const auto& val : req) {
        auto& tmp = val.AsMap();
        if (tmp.at("type").AsString() == "Stop") {
            res.push_back(RequestStop(tmp));
        }
        if (tmp.at("type").AsString() == "Bus") {
            res.push_back(RequestBus(tmp));
        }
        if (tmp.at("type").AsString() == "Map") {
            res.push_back(RequestMap(tmp));
        }
    }
    json::Print(json::Document{res}, std::cout);
}

json::Node RequestHandler::RequestStop(const json::Dict& req) {
    json::Dict res;
    auto name_stop = req.at("name").AsString();
    auto stop_info = catalogue.SearchStop(name_stop);
    if (stop_info == nullptr) {
        res["error_message"] = json::Node{static_cast<std::string>("not found")};
    }
    else {
        json::Array buses;
        for (auto& bus : catalogue.GetStopInfo(stop_info)) {
            buses.push_back(bus->name);
        }
        res["buses"] = buses;
    }
    res["request_id"] = req.at("id").AsInt();

    return res;
}

json::Node RequestHandler::RequestBus(const json::Dict& req) {
    json::Dict res;
    auto name_bus = req.at("name").AsString();
    if (catalogue.SearchBus(name_bus) == nullptr) {
        res["error_message"] = json::Node{static_cast<std::string>("not found")};
        res["request_id"] = req.at("id").AsInt();
        return res;
    }
    auto bus_info = catalogue.GetBusInfo(name_bus);
    res["curvature"] = bus_info.curvature;
    res["request_id"] = req.at("id").AsInt();
    res["route_length"] = bus_info.distance;
    res["stop_count"] = bus_info.stops_number;
    res["unique_stop_count"] = bus_info.unique_stops_number;
    return res;
}

json::Node RequestHandler::RequestMap(const json::Dict& req) {
    json::Dict res;
    svg::Document doc = RenderMap();
    std::ostringstream ost;
    doc.Render(ost);
    res["map"] = ost.str();
    res["request_id"] = req.at("id").AsInt();
    return res;
}

svg::Document RequestHandler::RenderMap() const {
    return render.GetSvg(catalogue.GetAllBusesInfo(), catalogue.GetAllStopsInfo());
}

