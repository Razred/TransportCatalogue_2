#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <set>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include "domain.h"

namespace transport_catalogue {

    struct BusInfo {
        int stops_number;
        int unique_stops_number;
        int distance;
        double curvature;
    };

    struct PairHash{
        size_t operator()(const std::pair<const domain::Stop*, const domain::Stop*>& p) const;
    };

    class TransportCatalogue {
    public:
        void addBus(std::string, std::vector<std::string>, bool );
        void addStop(std::string, geo::Coordinates);

        const domain::Bus* SearchBus(const std::string&);
        const domain::Stop* SearchStop(const std::string&);

        const BusInfo GetBusInfo(std::string);
        std::vector<const domain::Bus*> GetStopInfo(const domain::Stop*);

        void SetDistance(const std::string&, const std::string&, int distance);
        int GetDistance(const domain::Stop*, const domain::Stop*);

        std::map<std::string_view, const domain::Bus*> GetAllBusesInfo();

        std::map<std::string_view, const domain::Stop*> GetAllStopsInfo();

    private:
        // Хранение Автобусов и остановок
        std::deque<domain::Bus> Buses;
        std::deque<domain::Stop> Stops;

        std::unordered_map<std::string_view, domain::Bus*> finderBuses;
        std::unordered_map<std::string_view, domain::Stop*> finderStop;

        std::unordered_map<const domain::Stop*, std::unordered_set<domain::Bus*>> stop_to_bus;

        std::unordered_map<std::pair<const domain::Stop*, const domain::Stop*>, int, PairHash> m_distance;
    };
}