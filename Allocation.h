#ifndef ALLOCATION_H
#define ALLOCATION_H

#include "EmergencyReport.h"
#include "PriorityQueue.h"
#include "ReliefCamp.h"
#include "RoadGraph.h"

// MODULE 10: resource allocation (greedy).
// Pending emergencies are taken in priority order (HIGH first). Each one is matched to the
// NEAREST reachable camp (Dijkstra) that still has the stock it needs. Stock is reserved
// while the plan is built, so two emergencies never get the same last bed.
struct Assignment {
    EmergencyReport report;
    string need;        // "Food packet", "Bed + medical kit" or "Bed"
    int campIndex;      // -1 = nothing available
    int km;
};

void buildPlan(const PriorityQueue& pq, const CampManager& camps, const RoadGraph& graph,
               vector<Assignment>& plan);
int applyPlan(const vector<Assignment>& plan, ReportManager& reports, PriorityQueue& pq, CampManager& camps);

void allocationMenu(ReportManager& reports, PriorityQueue& pq, CampManager& camps, const RoadGraph& graph);

#endif
