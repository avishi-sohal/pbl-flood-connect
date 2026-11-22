#include "RoadGraph.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

static const int INF = 1000000000;

RoadGraph::RoadGraph(const string& file) : base(0), fileName(file) {
    // Sample map of Dehradun. Edit the names and distances to match your own map.
    const char* n[] = {"Dehradun", "Rajpur Road", "Sahastradhara", "Prem Nagar", "Clement Town", "Rispana"};
    for (int i = 0; i < 6; i++) names.push_back(n[i]);
    addRoad(0, 1, 5);  addRoad(0, 3, 8);  addRoad(0, 4, 9);  addRoad(0, 5, 4);
    addRoad(1, 2, 6);  addRoad(3, 4, 7);  addRoad(5, 4, 5);  addRoad(1, 3, 10);
    loadBlocked();                             // flooded roads survive a restart
}

// roads.txt: one flooded road per line, "PlaceA|PlaceB"
void RoadGraph::loadBlocked() {
    ifstream in(fileName.c_str());
    string line;
    while (in && getline(in, line)) {
        size_t bar = line.find('|');
        if (bar == string::npos) continue;
        int a = indexOf(line.substr(0, bar)), b = indexOf(line.substr(bar + 1));
        for (size_t i = 0; i < roads.size(); i++)
            if ((roads[i].a == a && roads[i].b == b) || (roads[i].a == b && roads[i].b == a))
                roads[i].blocked = true;
    }
}

void RoadGraph::saveBlocked() const {
    ofstream out(fileName.c_str());
    if (!out) { cout << "Warning: could not write " << fileName << "\n"; return; }
    for (size_t i = 0; i < roads.size(); i++)
        if (roads[i].blocked) out << names[roads[i].a] << "|" << names[roads[i].b] << "\n";
}

bool RoadGraph::setRoadBlocked(const string& a, const string& b, bool blocked) {
    int x = indexOf(a), y = indexOf(b);
    for (size_t i = 0; i < roads.size(); i++)
        if ((roads[i].a == x && roads[i].b == y) || (roads[i].a == y && roads[i].b == x)) {
            roads[i].blocked = blocked;
            saveBlocked();
            return true;
        }
    return false;
}

void RoadGraph::addRoad(int a, int b, int km) {
    Road r; r.a = a; r.b = b; r.km = km; r.blocked = false;
    roads.push_back(r);
}

int RoadGraph::indexOf(const string& location) const {
    string key = toUpper(trim(location));
    for (size_t i = 0; i < names.size(); i++)
        if (toUpper(names[i]) == key) return (int)i;
    return -1;
}

// Dijkstra with a simple array scan (fine for a small map). Blocked roads are ignored.
void RoadGraph::dijkstra(int src, vector<int>& dist, vector<int>& prev) const {
    int n = (int)names.size();
    dist.assign(n, INF);
    prev.assign(n, -1);
    vector<bool> done(n, false);
    dist[src] = 0;
    for (;;) {
        int u = -1;
        for (int i = 0; i < n; i++)
            if (!done[i] && dist[i] < INF && (u == -1 || dist[i] < dist[u])) u = i;
        if (u == -1) break;
        done[u] = true;
        for (size_t k = 0; k < roads.size(); k++) {
            const Road& r = roads[k];
            if (r.blocked) continue;
            int v = -1;
            if (r.a == u) v = r.b;
            else if (r.b == u) v = r.a;
            if (v != -1 && dist[u] + r.km < dist[v]) {
                dist[v] = dist[u] + r.km;
                prev[v] = u;
            }
        }
    }
}

int RoadGraph::distanceBetween(const string& from, const string& to) const {
    int a = indexOf(from), b = indexOf(to);
    if (a < 0 || b < 0) return -1;
    vector<int> dist, prev;
    dijkstra(a, dist, prev);
    return dist[b] >= INF ? -1 : dist[b];
}

string RoadGraph::pathText(const vector<int>& prev, int dst) const {
    vector<int> p;
    for (int v = dst; v != -1; v = prev[v]) p.insert(p.begin(), v);
    string s;
    for (size_t i = 0; i < p.size(); i++) {
        if (i > 0) s += " -> ";
        s += names[p[i]];
    }
    return s;
}

void RoadGraph::showRoads() const {
    printHeader("ROAD NETWORK");
    cout << "Rescue base: " << names[base] << "\n\n";
    cout << left << setw(5) << "No." << setw(36) << "Road" << setw(10) << "Distance" << "Status\n";
    cout << string(62, '-') << "\n";
    for (size_t i = 0; i < roads.size(); i++) {
        string road = names[roads[i].a] + " - " + names[roads[i].b];
        cout << left << setw(5) << (i + 1) << setw(36) << road
             << setw(10) << (toString(roads[i].km) + " km")
             << (roads[i].blocked ? "FLOODED (blocked)" : "Open") << "\n";
    }
}

void RoadGraph::toggleRoad() {
    showRoads();
    int no = readInt("\nEnter road number to flood / reopen: ", 1, (int)roads.size());
    Road& r = roads[no - 1];
    r.blocked = !r.blocked;
    saveBlocked();
    cout << "Road " << names[r.a] << " - " << names[r.b]
         << (r.blocked ? " is now marked FLOODED.\n" : " is open again.\n");
}

void RoadGraph::changeBase() {
    cout << "\n";
    for (size_t i = 0; i < names.size(); i++) cout << (i + 1) << ". " << names[i] << "\n";
    base = readInt("Choose rescue base: ", 1, (int)names.size()) - 1;
    cout << "Rescue base is now " << names[base] << ".\n";
}

void RoadGraph::routeToNext(const PriorityQueue& pq, const CampManager& camps) const {
    printHeader("ROUTE TO NEXT EMERGENCY");
    EmergencyReport r;
    if (!pq.peek(r)) { cout << "The priority queue is empty.\n"; return; }

    cout << "Next emergency: #" << r.getId() << " " << r.getVictimName()
         << " at " << r.getLocation() << " (" << r.getSeverity() << ")\n";
    int target = indexOf(r.getLocation());
    if (target < 0) { cout << "Error: " << r.getLocation() << " is not on the road graph.\n"; return; }

    vector<int> dist, prev;
    dijkstra(base, dist, prev);
    if (dist[target] >= INF) { cout << "No safe route: every road to this location is flooded.\n"; return; }
    cout << "Route: " << pathText(prev, target) << " (" << dist[target] << " km)\n";

    // nearest camp with free beds, measured from the emergency location
    vector<int> d2, p2;
    dijkstra(target, d2, p2);
    int best = -1;
    for (int i = 0; i < camps.getCampCount(); i++) {
        const ReliefCamp& c = camps.campAt(i);
        int ci = indexOf(c.getLocation());
        if (c.getAvailableBeds() > 0 && ci >= 0 && d2[ci] < INF && (best == -1 || d2[ci] < d2[indexOf(camps.campAt(best).getLocation())]))
            best = i;
    }
    if (best == -1) cout << "No reachable camp with free beds.\n";
    else {
        const ReliefCamp& c = camps.campAt(best);
        cout << "Nearest camp with free beds: " << c.getName() << " (" << c.getLocation() << "), "
             << d2[indexOf(c.getLocation())] << " km, " << c.getAvailableBeds() << " bed(s) free.\n";
    }
}

void RoadGraph::menu(const PriorityQueue& pq, const CampManager& camps) {
    int choice;
    do {
        cout << "\n====================================\n";
        cout << "          RESCUE ROUTES\n";
        cout << "====================================\n\n";
        cout << "1. Show Road Network\n";
        cout << "2. Mark Road Flooded / Reopen\n";
        cout << "3. Route to Next Emergency\n";
        cout << "4. Change Rescue Base\n";
        cout << "5. Return to Main Menu\n\n";
        choice = readInt("Enter your choice: ", 1, 5);
        if (choice == 1) showRoads();
        else if (choice == 2) toggleRoad();
        else if (choice == 3) routeToNext(pq, camps);
        else if (choice == 4) changeBase();
    } while (choice != 5);
}
