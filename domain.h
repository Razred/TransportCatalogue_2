#pragma once

#include <vector>
#include <string_view>

#include "geo.h"

namespace domain {
    struct Bus {
        std::string name;
        std::vector<std::string> stops;
        bool is_Circle;

    };

    struct Stop {
        std::string name;
        geo::Coordinates coordinates;
    };

}