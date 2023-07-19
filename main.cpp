#include <sstream>
#include <fstream>
#include "json_reader.h"
#include "request_handler.h"
#include "map_render.h"

using namespace std;
using namespace transport_catalogue;
using namespace json;

int main() {
    TransportCatalogue tc;
    JsonReader jr(std::cin);
    jr.FillingCatalogue(tc);
    render::MapRender map_render(jr.FillingRenderSettings());
    RequestHandler rh(jr, tc, map_render);
    rh.RequestProccesing();
    return 0;
}