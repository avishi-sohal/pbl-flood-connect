#include "Dashboard.h"
#include <iostream>
using namespace std;

// Draws one text bar, e.g.  HIGH    | ##########  2
// Each '#' is one block; the bar is scaled so the biggest value never exceeds 30 blocks.
static void drawBar(const string& label, int value, int maxValue) {
    const int MAX_BLOCKS = 30;
    int blocks = (maxValue > 0) ? (value * MAX_BLOCKS) / maxValue : 0;
    if (value > 0 && blocks == 0) blocks = 1;          // tiny values still show one block
    cout << label;
    for (size_t i = label.size(); i < 12; i++) cout << ' ';
    cout << "| ";
    for (int i = 0; i < blocks; i++) cout << '#';
    cout << "  " << value << "\n";
}

static int biggest(const int a[], int n) {
    int m = 0;
    for (int i = 0; i < n; i++) if (a[i] > m) m = a[i];
    return m;
}

Dashboard::Dashboard(const ReportManager& r, const CampManager& c, const PriorityQueue& q)
    : reports(r), camps(c), queue(q) {}

void Dashboard::show() const {
    cout << "\n========================================\n";
    cout << "       FLOODCONNECT DASHBOARD\n";
    cout << "========================================\n\n";

    // Every number below is calculated from the real stored data
    cout << "Total Emergency Reports : " << reports.getTotal() << "\n";
    cout << "High Priority Cases     : " << reports.countBySeverity("HIGH") << "\n";
    cout << "Medium Priority Cases   : " << reports.countBySeverity("MEDIUM") << "\n";
    cout << "Low Priority Cases      : " << reports.countBySeverity("LOW") << "\n\n";

    cout << "Active Reports          : " << reports.countActive()
         << "  (Pending + In Progress)\n";
    cout << "Rescued Cases           : " << reports.countByStatus("Rescued") << "\n";
    cout << "Closed Cases            : " << reports.countByStatus("Closed") << "\n";
    cout << "Waiting in Queue        : " << queue.getSize() << "\n\n";

    cout << "Relief Camps            : " << camps.getCampCount() << "\n";
    cout << "Beds Available (total)  : " << camps.getTotalAvailableBeds() << "\n\n";

    cout << "----------------------------------------\n";
    cout << "LATEST ACTIVE HIGH PRIORITY CASE\n";
    cout << "----------------------------------------\n";

    EmergencyReport latest;
    if (reports.getLatestActiveHigh(latest)) {
        cout << "Report ID   : " << latest.getId() << "\n";
        cout << "Victim      : " << latest.getVictimName() << "\n";
        cout << "Location    : " << latest.getLocation() << "\n";
        cout << "Severity    : " << latest.getSeverity() << "\n";
        cout << "Status      : " << latest.getStatus() << "\n";
    } else {
        cout << "No active HIGH priority case right now.\n";
    }
    // ---------- GRAPHS (text bar charts) ----------
    cout << "\n----------------------------------------\n";
    cout << "GRAPH 1: REPORTS BY SEVERITY\n";
    cout << "----------------------------------------\n";
    int sev[3] = { reports.countBySeverity("HIGH"),
                   reports.countBySeverity("MEDIUM"),
                   reports.countBySeverity("LOW") };
    int sevMax = biggest(sev, 3);
    drawBar("HIGH", sev[0], sevMax);
    drawBar("MEDIUM", sev[1], sevMax);
    drawBar("LOW", sev[2], sevMax);

    cout << "\n----------------------------------------\n";
    cout << "GRAPH 2: REPORTS BY STATUS\n";
    cout << "----------------------------------------\n";
    int stat[4] = { reports.countByStatus("Pending"),
                    reports.countByStatus("In Progress"),
                    reports.countByStatus("Rescued"),
                    reports.countByStatus("Closed") };
    int statMax = biggest(stat, 4);
    drawBar("Pending", stat[0], statMax);
    drawBar("In Progress", stat[1], statMax);
    drawBar("Rescued", stat[2], statMax);
    drawBar("Closed", stat[3], statMax);
    cout << "\n========================================\n";
}
