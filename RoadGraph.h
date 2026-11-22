#ifndef ROADGRAPH_H
#define ROADGRAPH_H

#include <string>
#include <vector>
#include "PriorityQueue.h"
#include "ReliefCamp.h"
using namespace std;

// MODULE 7: graph of flood-affected locations (nodes) and roads (weighted edges, km).
// Dijkstra's algorithm finds the shortest route and skips roads marked as flooded.
class RoadGraph {
private:
    struct Road { int a; int b; int km; bool blocked; };
    vector<string> names;
    vector<Road> roads;
    int base;                                  // rescue base (start of every route)
    string fileName;                           // roads.txt: remembers which roads are flooded

    void loadBlocked();
    void saveBlocked() const;

    void addRoad(int a, int b, int km);
    void dijkstra(int src, vector<int>& dist, vector<int>& prev) const;
    string pathText(const vector<int>& prev, int dst) const;

public:
    RoadGraph(const string& file = "roads.txt");
    int indexOf(const string& location) const;                 // -1 if not on the map
    int distanceBetween(const string& from, const string& to) const;   // km, -1 if no route
    bool setRoadBlocked(const string& a, const string& b, bool blocked);   // false if no such road

    void showRoads() const;
    void toggleRoad();
    void routeToNext(const PriorityQueue& pq, const CampManager& camps) const;
    void changeBase();
    void menu(const PriorityQueue& pq, const CampManager& camps);
};

#endif
