#pragma once

#include "json_reader.h"
#include "transport_catalogue.h"
#include "map_render.h"

#include <sstream>



class RequestHandler {
public:
    RequestHandler(JsonReader& request_, transport_catalogue::TransportCatalogue& catalogue_, render::MapRender& render_) : request(request_), catalogue(catalogue_), render(render_) {
    }

    void RequestProccesing();

    svg::Document RenderMap() const;

private:
    JsonReader& request;
    transport_catalogue::TransportCatalogue& catalogue;
    render::MapRender& render;

    json::Node RequestBus(const json::Dict&);
    json::Node RequestStop(const json::Dict&);
    json::Node RequestMap(const json::Dict&);
};