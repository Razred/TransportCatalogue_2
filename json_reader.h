#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_render.h"

class JsonReader {
public:
    JsonReader(std::istream& input) : input_(json::Load(input)) {
    }

    const json::Node& GetBaseRequest() const;
    const json::Node& GetStatRequest() const;
    const json::Node& GetRenderRequest() const;

    void FillingCatalogue(transport_catalogue::TransportCatalogue& catalogue);
    const render::MapRender FillingRenderSettings();


private:
    json::Document input_;
    json::Node nullptr_return = nullptr;
    std::tuple<std::string, geo::Coordinates, std::map<std::string, int>> FillingStop(const json::Dict&) const;
    std::tuple<std::string, std::vector<std::string>, bool> FillingBus(const json::Dict&) const;
};