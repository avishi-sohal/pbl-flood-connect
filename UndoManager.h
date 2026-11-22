#ifndef UNDOMANAGER_H
#define UNDOMANAGER_H

#include <string>
#include <vector>
#include "EmergencyReport.h"
#include "PriorityQueue.h"
#include "ReliefCamp.h"
using namespace std;

// MODULE 8: undo using a STACK (last in, first out).
// Before a change is made, a snapshot of all reports and camps is pushed on the stack.
// Undo pops the newest snapshot and restores it.
struct Snapshot {
    vector<string> reports;
    vector<string> camps;
};

class UndoManager {
private:
    vector<Snapshot> stack;          // top of the stack = last element
    static const size_t MAX_SNAPSHOTS = 20;

public:
    void push(const ReportManager& reports, const CampManager& camps);
    bool undo(ReportManager& reports, CampManager& camps, PriorityQueue& pq);
    int size() const;
};

#endif
