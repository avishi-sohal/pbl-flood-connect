#include "PriorityQueue.h"
#include "EmergencyReport.h"
#include "Utils.h"
#include <iostream>
using namespace std;

PriorityQueue::PriorityQueue() : front(NULL), size(0) {}

PriorityQueue::~PriorityQueue() {
    while (front != NULL) {
        QueueNode* temp = front;
        front = front->next;
        delete temp;
    }
}

// Insert so the list stays sorted by priority number (1 first, then 2, then 3).
// A new report goes AFTER existing reports of the same priority (first come, first served).
void PriorityQueue::enqueue(const EmergencyReport& r) {
    QueueNode* node = new QueueNode(r);

    if (front == NULL || r.getPriority() < front->data.getPriority()) {
        node->next = front;                // more urgent than everything -> new front
        front = node;
    } else {
        QueueNode* cur = front;
        while (cur->next != NULL && cur->next->data.getPriority() <= r.getPriority())
            cur = cur->next;
        node->next = cur->next;
        cur->next = node;
    }
    size++;
}

// Remove and return the front node = the most urgent emergency
bool PriorityQueue::dequeue(EmergencyReport& out) {
    if (front == NULL) return false;       // empty queue
    QueueNode* temp = front;
    out = temp->data;
    front = front->next;
    delete temp;
    size--;
    return true;
}

bool PriorityQueue::peek(EmergencyReport& out) const {
    if (front == NULL) return false;
    out = front->data;                     // copy only, the node stays in the queue
    return true;
}

bool PriorityQueue::removeById(int id) {
    QueueNode* prev = NULL;
    QueueNode* cur = front;
    while (cur != NULL && cur->data.getId() != id) {
        prev = cur;
        cur = cur->next;
    }
    if (cur == NULL) return false;
    if (prev == NULL) front = cur->next;
    else prev->next = cur->next;
    delete cur;
    size--;
    return true;
}

bool PriorityQueue::contains(int id) const {
    for (QueueNode* cur = front; cur != NULL; cur = cur->next)
        if (cur->data.getId() == id) return true;
    return false;
}

bool PriorityQueue::isEmpty() const { return front == NULL; }
int PriorityQueue::getSize() const { return size; }

void PriorityQueue::display() const {
    printHeader("PRIORITIZED EMERGENCIES");
    if (front == NULL) {
        cout << "The priority queue is empty.\n";
        return;
    }
    for (QueueNode* cur = front; cur != NULL; cur = cur->next) {
        const EmergencyReport& r = cur->data;
        cout << "Priority " << r.getPriority() << ":\n";
        cout << "ID: " << r.getId() << "\n";
        cout << "Victim: " << r.getVictimName() << "\n";
        cout << "Location: " << r.getLocation() << "\n";
        cout << "Severity: " << r.getSeverity() << "\n\n";
    }
    cout << "Emergencies waiting: " << size << "\n";
}

void PriorityQueue::menu(ReportManager& reports) {
    int choice;
    do {
        cout << "\n====================================\n";
        cout << "        PRIORITY MANAGEMENT\n";
        cout << "====================================\n\n";
        cout << "1. Add Emergency to Priority Queue\n";
        cout << "2. Display Prioritized Emergencies\n";
        cout << "3. Process Highest-Priority Emergency\n";
        cout << "4. Return to Main Menu\n\n";
        choice = readInt("Enter your choice: ", 1, 4);

        if (choice == 1) {
            printHeader("ADD TO PRIORITY QUEUE");
            int id = readInt("Enter Report ID: ", 1, 999999);
            EmergencyReport r;
            if (!reports.getReport(id, r)) {
                cout << "Report not found.\n";
            } else if (r.getStatus() != "Pending") {
                cout << "Only Pending reports can be queued. This report is " << r.getStatus() << ".\n";
            } else if (contains(id)) {
                cout << "Report " << id << " is already in the priority queue.\n";
            } else {
                enqueue(r);
                cout << "Report " << id << " added with Priority " << r.getPriority() << ".\n";
            }
        } else if (choice == 2) {
            display();
        } else if (choice == 3) {
            printHeader("PROCESS HIGHEST-PRIORITY EMERGENCY");
            EmergencyReport r;
            if (!dequeue(r)) {
                cout << "The priority queue is empty. Nothing to process.\n";
            } else {
                reports.setStatusById(r.getId(), "In Progress");   // rescue team dispatched
                cout << "Rescue team assigned to:\n";
                cout << "Priority " << r.getPriority() << " | ID " << r.getId()
                     << " | " << r.getVictimName() << " | " << r.getLocation()
                     << " | " << r.getSeverity() << "\n";
                cout << "Report status changed to In Progress.\n";
            }
        }
    } while (choice != 4);
}
