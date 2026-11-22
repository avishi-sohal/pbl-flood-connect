// Automated tests for FloodConnect. Build and run with:  make test
// Uses temporary files (t_*.txt) which are deleted at the end; your real data is never touched.
#include <iostream>
#include <fstream>
#include <cstdio>
#include "EmergencyReport.h"
#include "PriorityQueue.h"
#include "ReliefCamp.h"
#include "RoadGraph.h"
#include "Allocation.h"
#include "UndoManager.h"
#include "Auth.h"
#include "ChatBot.h"
using namespace std;

static int passed = 0, failed = 0;
#define CHECK(name, cond) do { if (cond) { passed++; cout << "  PASS  " << name << "\n"; } \
                               else { failed++; cout << "  FAIL  " << name << "\n"; } } while (0)

static void writeFile(const char* name, const char* text) {
    ofstream out(name);
    out << text;
}

static EmergencyReport rep(int id, const char* loc, const char* type, const char* sev) {
    return EmergencyReport(id, "Test", loc, type, sev, "desc", "9876543210", "Pending");
}

int main() {
    cout << "FloodConnect automated tests\n\n";

    cout << "Priority queue\n";
    {
        PriorityQueue pq;
        pq.enqueue(rep(1, "A", "x", "LOW"));
        pq.enqueue(rep(2, "A", "x", "HIGH"));
        pq.enqueue(rep(3, "A", "x", "MEDIUM"));
        pq.enqueue(rep(4, "A", "x", "HIGH"));
        EmergencyReport r;
        pq.peek(r);
        CHECK("peek shows highest priority without removing", r.getId() == 2 && pq.getSize() == 4);
        int order[4]; for (int i = 0; i < 4; i++) { pq.dequeue(r); order[i] = r.getId(); }
        CHECK("order is HIGH, HIGH (first come first), MEDIUM, LOW", order[0] == 2 && order[1] == 4 && order[2] == 3 && order[3] == 1);
        CHECK("empty after processing all", pq.isEmpty() && !pq.dequeue(r));
        pq.enqueue(rep(5, "A", "x", "LOW")); pq.enqueue(rep(6, "A", "x", "HIGH"));
        CHECK("removeById removes from the middle/front", pq.removeById(6) && !pq.contains(6) && pq.contains(5) && !pq.removeById(99));
    }

    cout << "\nReport list, file loading and hash index\n";
    writeFile("t_reports.txt",
        "101|Rahul|Rajpur Road|Trapped|HIGH|d|9876543210|Pending\n"
        "102|Priya|Prem Nagar|Medical|MEDIUM|d|9876501234|Pending\n"
        "102|Dup|Prem Nagar|Medical|MEDIUM|d|9876501234|Pending\n"
        "103|Bad|Rispana|Trapped|URGENT|d|9876501234|Pending\n"
        "104|Aman|Sahastradhara|Trapped|HIGH|d|9899001122|Pending\n");
    writeFile("t_camps.txt", "201|Safe Haven|Dehradun|100|1|200|30\n202|Hope Camp|Clement Town|80|5|5|5\n");
    ReportManager reports("t_reports.txt");
    CampManager camps("t_camps.txt");
    PriorityQueue pq;
    reports.loadFromFile();
    camps.loadFromFile();
    reports.loadPendingIntoQueue(pq);
    CHECK("duplicate and invalid lines are skipped (3 valid reports)", reports.getTotal() == 3);
    CHECK("hash lookup finds existing IDs", reports.exists(101) && reports.exists(104));
    CHECK("hash lookup rejects missing IDs", !reports.exists(102 + 100) && !reports.exists(103));
    CHECK("queue holds the 3 pending reports", pq.getSize() == 3);
    CHECK("counts by severity", reports.countBySeverity("HIGH") == 2 && reports.countBySeverity("MEDIUM") == 1);
    CHECK("status update works", reports.setStatusById(101, "Rescued") && reports.countByStatus("Rescued") == 1 && reports.countActive() == 2);
    reports.setStatusById(101, "Pending");

    cout << "\nUndo stack\n";
    {
        UndoManager undo;
        undo.push(reports, camps);
        reports.setStatusById(101, "Rescued");
        pq.removeById(101);
        camps.campAt(0).setAvailableBeds(0);
        CHECK("change is visible before undo", reports.countByStatus("Rescued") == 1 && camps.campAt(0).getAvailableBeds() == 0);
        CHECK("undo succeeds", undo.undo(reports, camps, pq));
        EmergencyReport r;
        CHECK("report status restored", reports.getReport(101, r) && r.getStatus() == "Pending");
        CHECK("camp stock restored", camps.campAt(0).getAvailableBeds() == 1);
        CHECK("queue rebuilt from pending reports", pq.getSize() == 3 && pq.contains(101));
        CHECK("hash index works after restore", reports.exists(104) && !reports.exists(999));
        CHECK("nothing left to undo", !undo.undo(reports, camps, pq));
    }

    cout << "\nGraph and Dijkstra\n";
    {
        remove("t_roads.txt");
        RoadGraph g("t_roads.txt");
        CHECK("shortest distance Dehradun -> Rajpur Road = 5 km", g.distanceBetween("Dehradun", "Rajpur Road") == 5);
        CHECK("multi-hop distance Dehradun -> Sahastradhara = 11 km", g.distanceBetween("Dehradun", "Sahastradhara") == 11);
        CHECK("place names are not case sensitive", g.distanceBetween("dehradun", "RISPANA") == 4);
        CHECK("unknown place gives no route", g.distanceBetween("Dehradun", "Mars") == -1);
        g.setRoadBlocked("Dehradun", "Rajpur Road", true);
        CHECK("flooded road: route goes around (Dehradun-Prem Nagar-Rajpur Road = 18 km)", g.distanceBetween("Dehradun", "Rajpur Road") == 18);
        g.setRoadBlocked("Rajpur Road", "Sahastradhara", true);
        CHECK("all roads to a place flooded: no route", g.distanceBetween("Dehradun", "Sahastradhara") == -1);
        RoadGraph again("t_roads.txt");
        CHECK("flooded roads are remembered after restart", again.distanceBetween("Dehradun", "Rajpur Road") == 18);
        CHECK("setRoadBlocked rejects a road that does not exist", !g.setRoadBlocked("Dehradun", "Sahastradhara", true));
    }

    cout << "\nResource allocation\n";
    {
        remove("t_roads.txt");
        RoadGraph g("t_roads.txt");
        PriorityQueue q;
        ReportManager rm("t_reports2.txt");
        CampManager cm("t_camps2.txt");
        writeFile("t_camps2.txt", "201|A|Dehradun|10|1|0|0\n202|B|Clement Town|10|5|5|5\n");
        cm.loadFromFile();
        q.enqueue(rep(1, "Rajpur Road", "Trapped", "MEDIUM"));
        q.enqueue(rep(2, "Rajpur Road", "Trapped", "HIGH"));
        q.enqueue(rep(3, "Clement Town", "Food and Water", "LOW"));
        q.enqueue(rep(4, "Prem Nagar", "Medical", "LOW"));
        vector<Assignment> plan;
        buildPlan(q, cm, g, plan);
        CHECK("HIGH case is planned first", plan.size() == 4 && plan[0].report.getId() == 2);
        CHECK("HIGH gets the nearest camp's last bed", plan[0].campIndex == 0 && plan[0].km == 5);
        CHECK("next case cannot reuse that bed, goes to next camp", plan[1].campIndex == 1);
        CHECK("food request needs a food packet (camp B)", plan[2].need == "Food packet" && plan[2].campIndex == 1);
        CHECK("medical request needs a bed and a kit", plan[3].need == "Bed + medical kit" && plan[3].campIndex == 1);
        g.setRoadBlocked("Dehradun", "Clement Town", true);
        g.setRoadBlocked("Rispana", "Clement Town", true);
        g.setRoadBlocked("Prem Nagar", "Clement Town", true);
        vector<Assignment> blocked;
        buildPlan(q, cm, g, blocked);
        CHECK("unreachable camp is not assigned", blocked[1].campIndex == -1);
        remove("t_roads.txt"); remove("t_reports2.txt"); remove("t_camps2.txt");
    }

    cout << "\nFloodBot\n";
    {
        ChatBot bot(reports, camps, pq);
        bot.setCitizenMode(true);
        string a;
        CHECK("danger message gives 112 first", bot.reply("I am trapped on my roof, water is rising").find("112") != string::npos);
        a = bot.reply("what is the status of report 101");
        CHECK("citizen can see own report status", a.find("Pending") != string::npos && a.find("number 1 of 3") != string::npos);
        CHECK("citizen never sees victim names or contacts", a.find("Rahul") == string::npos && bot.reply("who is next?").find("Rahul") == string::npos);
        CHECK("unknown report ID is handled", bot.reply("status of report 555").find("could not find") != string::npos);
        CHECK("helpline number question is not mistaken for a report ID", bot.reply("what is 112").find("112") != string::npos);
        CHECK("relief camps answer uses live data", bot.reply("where are the relief camps?").find("Safe Haven") != string::npos);
        CHECK("safe drinking water advice", bot.reply("how can I make water safe to drink").find("oil") != string::npos);
        CHECK("snake bite advice mentions 108", bot.reply("my child got a snake bite").find("108") != string::npos);
        CHECK("whole-word match: 'high' is not a greeting", bot.reply("which high places are safe").find("Hello") == string::npos);
        CHECK("greeting works", bot.reply("hi").find("FloodBot") != string::npos);
        CHECK("unknown question gives topics and 112", bot.reply("xyzzy plugh").find("112") != string::npos);
        bot.setCitizenMode(false);
        CHECK("rescue staff still get queue details", bot.reply("who is next?").find("Rahul") != string::npos);
        CHECK("rescue staff also get safety knowledge", bot.reply("how do I treat a snake bite").find("108") != string::npos);
    }

    cout << "\nLogin hashing\n";
    CHECK("same password gives same hash", hashPassword("admin123") == hashPassword("admin123"));
    CHECK("different password gives different hash", hashPassword("admin123") != hashPassword("admin124"));

    remove("t_reports.txt"); remove("t_camps.txt");
    cout << "\nResult: " << passed << " passed, " << failed << " failed.\n";
    return failed == 0 ? 0 : 1;
}
