#include "Allocation.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

static bool contains(const string& text, const string& word) {
    return toUpper(text).find(word) != string::npos;
}

void buildPlan(const PriorityQueue& pq, const CampManager& camps, const RoadGraph& graph,
               vector<Assignment>& plan) {
    int n = camps.getCampCount();
    vector<int> beds(n), food(n), kits(n);
    for (int i = 0; i < n; i++) {
        beds[i] = camps.campAt(i).getAvailableBeds();
        food[i] = camps.campAt(i).getFoodPackets();
        kits[i] = camps.campAt(i).getMedicalKits();
    }

    vector<EmergencyReport> waiting;
    pq.toVector(waiting);                         // already sorted: most urgent first

    for (size_t k = 0; k < waiting.size(); k++) {
        const EmergencyReport& r = waiting[k];
        bool needsFood = contains(r.getEmergencyType(), "FOOD") || contains(r.getEmergencyType(), "WATER");
        bool needsMed  = contains(r.getEmergencyType(), "MEDIC");

        int best = -1, bestKm = 0;
        for (int i = 0; i < n; i++) {
            bool stockOk = needsFood ? food[i] > 0 : (beds[i] > 0 && (!needsMed || kits[i] > 0));
            if (!stockOk) continue;
            int km = graph.distanceBetween(r.getLocation(), camps.campAt(i).getLocation());
            if (km < 0) continue;                 // unknown place or all roads flooded
            if (best == -1 || km < bestKm) { best = i; bestKm = km; }
        }
        if (best != -1) {
            if (needsFood) food[best]--;
            else { beds[best]--; if (needsMed) kits[best]--; }
        }

        Assignment a;
        a.report = r;
        a.need = needsFood ? "Food packet" : (needsMed ? "Bed + medical kit" : "Bed");
        a.campIndex = best;
        a.km = bestKm;
        plan.push_back(a);
    }
}

static void showPlan(const vector<Assignment>& plan, const CampManager& camps) {
    printHeader("PROPOSED ALLOCATION");
    if (plan.empty()) { cout << "No pending emergencies to allocate.\n"; return; }
    cout << left << setw(6) << "ID" << setw(12) << "Victim" << setw(15) << "Location"
         << setw(10) << "Severity" << setw(19) << "Needs" << setw(16) << "Assigned Camp" << "Distance\n";
    cout << string(86, '-') << "\n";
    for (size_t i = 0; i < plan.size(); i++) {
        const Assignment& a = plan[i];
        cout << left << setw(6) << a.report.getId()
             << setw(12) << fit(a.report.getVictimName(), 11)
             << setw(15) << fit(a.report.getLocation(), 14)
             << setw(10) << a.report.getSeverity()
             << setw(19) << a.need;
        if (a.campIndex >= 0)
            cout << setw(16) << fit(camps.campAt(a.campIndex).getName(), 15) << a.km << " km\n";
        else
            cout << "None available\n";
    }
}

int applyPlan(const vector<Assignment>& plan, ReportManager& reports, PriorityQueue& pq, CampManager& camps) {
    int done = 0;
    for (size_t i = 0; i < plan.size(); i++) {
        const Assignment& a = plan[i];
        if (a.campIndex < 0) continue;
        ReliefCamp& c = camps.campAt(a.campIndex);
        if (a.need == "Food packet") c.setFoodPackets(c.getFoodPackets() - 1);
        else {
            c.setAvailableBeds(c.getAvailableBeds() - 1);
            if (a.need != "Bed") c.setMedicalKits(c.getMedicalKits() - 1);
        }
        reports.setStatusById(a.report.getId(), "In Progress");   // also saves reports.txt
        pq.removeById(a.report.getId());
        done++;
    }
    camps.saveToFile();
    return done;
}

void allocationMenu(ReportManager& reports, PriorityQueue& pq, CampManager& camps, const RoadGraph& graph) {
    int choice;
    do {
        cout << "\n====================================\n";
        cout << "        RESOURCE ALLOCATION\n";
        cout << "====================================\n\n";
        cout << "1. Show Proposed Allocation\n";
        cout << "2. Apply Allocation\n";
        cout << "3. Return to Main Menu\n\n";
        choice = readInt("Enter your choice: ", 1, 3);

        if (choice == 1 || choice == 2) {
            vector<Assignment> plan;
            buildPlan(pq, camps, graph, plan);
            showPlan(plan, camps);
            if (choice == 2 && !plan.empty()) {
                if (confirm("\nApply this allocation? (Y/N): ")) {
                    int n = applyPlan(plan, reports, pq, camps);
                    cout << "Allocated " << n << " emergency(ies). Camp stock updated, reports set to In Progress.\n";
                } else cout << "Allocation cancelled.\n";
            }
        }
    } while (choice != 3);
}
