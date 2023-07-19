#include "transport_catalogue.h"

namespace transport_catalogue {

    size_t PairHash::operator()(const std::pair<const domain::Stop *, const domain::Stop *> &p) const {
        return std::hash<const void*>{}(p.first) + std::hash<const void*>{}(p.second);
    }

    void TransportCatalogue::addBus(std::string busname, std::vector<std::string> stops, bool circle) {
        Buses.push_back({std::move(busname), std::move(stops), circle});
        finderBuses[Buses.back().name] = &Buses.back();

        for (auto &stop : Buses.back().stops) {
            if (SearchStop(stop) == nullptr) {
                Stops.push_back({stop, {0, 0}});
                finderStop[Stops.back().name] = &Stops.back();
            }
            stop_to_bus[SearchStop(stop)].emplace(&Buses.back());
        }
    }

    void TransportCatalogue::addStop(std::string stopname, geo::Coordinates coord) {
        auto tmp = finderStop[stopname];
        if (tmp != nullptr) {
            tmp->coordinates = std::move(coord);
            return;
        }
        Stops.push_back({std::move(stopname), std::move(coord)});
        finderStop[Stops.back().name] = &Stops.back();
        stop_to_bus[&Stops.back()];
    }

    const domain::Bus* TransportCatalogue::SearchBus(const std::string &busname) {
        if (finderBuses.find(busname) != finderBuses.end()){
            return finderBuses.find(busname)->second;
        }
        return nullptr;
    }

    const domain::Stop* TransportCatalogue::SearchStop(const std::string &stopname) {
       if (finderStop.find(stopname) != finderStop.end()) {
           return finderStop.at(stopname);
       }
       return nullptr;
    }

    const BusInfo TransportCatalogue::GetBusInfo(std::string busname){
        BusInfo info({-1, -1, 0});
        auto tmp = finderBuses.find(busname);
        if (tmp == finderBuses.end()) {
            return info;
        }
        auto stop = SearchStop(tmp->second->stops[0]);
        double straight_dist;
        double total_straight_dist;
        int real_dist;
       if (SearchBus(busname)->is_Circle) {
           info.stops_number = tmp->second->stops.size();
           for (int i = 1; i < tmp->second->stops.size(); i++) {
               straight_dist = ComputeDistance(stop->coordinates, SearchStop(tmp->second->stops[i])->coordinates);
               real_dist = GetDistance(stop, SearchStop(tmp->second->stops[i]));
               stop = SearchStop(tmp->second->stops[i]);
               total_straight_dist += straight_dist;
               info.distance += real_dist;
           }
       }
       else {
           info.stops_number = (tmp->second->stops.size() * 2) - 1 ;
           for (int i = 1; i < tmp->second->stops.size(); i++) {
               straight_dist = ComputeDistance(stop->coordinates, SearchStop(tmp->second->stops[i])->coordinates);
               real_dist = GetDistance(stop, SearchStop(tmp->second->stops[i]));
               real_dist += GetDistance(SearchStop(tmp->second->stops[i]), stop);
               stop = SearchStop(tmp->second->stops[i]);
               total_straight_dist += straight_dist * 2;
               info.distance += real_dist;
           }
       }

       info.curvature = info.distance / total_straight_dist;

       std::unordered_set<std::string_view> unique_stops;
       for (auto &val : finderBuses.at(busname)->stops) {
          unique_stops.insert(val);
       }
       info.unique_stops_number = unique_stops.size();
       return info;
    }

    std::vector<const domain::Bus*> TransportCatalogue::GetStopInfo(const domain::Stop* stop) {
        return {stop_to_bus.at(stop).begin(), stop_to_bus.at(stop).end()};
    }

    void TransportCatalogue::SetDistance(const std::string &stopname1, const std::string &stopname2, int distance) {
        auto tmp1 = SearchStop(stopname1);
        auto tmp2 = SearchStop(stopname2);
        if (tmp2 == nullptr) {
            Stops.push_back({stopname2, {0, 0}});
            finderStop[Stops.back().name] = &Stops.back();
            stop_to_bus[&Stops.back()];
            tmp2 = &Stops.back();
        }
       m_distance[{tmp1, tmp2}] = distance;
        if (!m_distance.contains({tmp2, tmp1})) {
            m_distance[{tmp2, tmp1}] = distance;
        }
    }

    int TransportCatalogue::GetDistance(const domain::Stop *stopname1, const domain::Stop *stopname2) {
        return m_distance[{stopname1, stopname2}];
    }

    std::map<std::string_view, const domain::Bus*> TransportCatalogue::GetAllBusesInfo() {
        std::map<std::string_view, const domain::Bus*> buses(finderBuses.begin(), finderBuses.end());
        return buses;
    }

    std::map<std::string_view, const domain::Stop*> TransportCatalogue::GetAllStopsInfo() {
        std::map<std::string_view, const domain::Stop*> stops(finderStop.begin(), finderStop.end());
        return stops;
    }

}