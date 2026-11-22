#include "UndoManager.h"

void UndoManager::push(const ReportManager& reports, const CampManager& camps) {
    Snapshot s;
    reports.snapshot(s.reports);
    camps.snapshot(s.camps);
    if (!stack.empty() && stack.back().reports == s.reports && stack.back().camps == s.camps)
        return;                                   // nothing changed since the last snapshot
    stack.push_back(s);
    if (stack.size() > MAX_SNAPSHOTS) stack.erase(stack.begin());   // forget the oldest
}

bool UndoManager::undo(ReportManager& reports, CampManager& camps, PriorityQueue& pq) {
    Snapshot now;
    reports.snapshot(now.reports);
    camps.snapshot(now.camps);

    // skip snapshots that are identical to the current state (nothing to undo there)
    while (!stack.empty() && stack.back().reports == now.reports && stack.back().camps == now.camps)
        stack.pop_back();
    if (stack.empty()) return false;

    Snapshot s = stack.back();
    stack.pop_back();                             // POP
    reports.restore(s.reports);
    camps.restore(s.camps);
    pq.clear();
    reports.loadPendingIntoQueue(pq);             // queue = exactly the Pending reports
    return true;
}

int UndoManager::size() const { return (int)stack.size(); }
