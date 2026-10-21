#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "EmergencyReport.h"
#include "PriorityQueue.h"
#include "ReliefCamp.h"

// MODULE 5: read-only summary screen.
// It does not store any numbers itself; it asks the other objects every time it is shown.
class Dashboard {
private:
    const ReportManager& reports;
    const CampManager& camps;
    const PriorityQueue& queue;

public:
    Dashboard(const ReportManager& r, const CampManager& c, const PriorityQueue& q);
    void show() const;
};

#endif
