#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include "EmergencyReport.h"

class ReportManager;   // forward declaration

// MODULE 3: Priority queue built on a linked list that is always kept sorted.
// Front of the list = most urgent emergency.
// Same priority -> first come, first served.
struct QueueNode {
    EmergencyReport data;
    QueueNode* next;
    QueueNode(const EmergencyReport& r) : data(r), next(NULL) {}
};

class PriorityQueue {
private:
    QueueNode* front;
    int size;

    PriorityQueue(const PriorityQueue&);              // copying blocked (raw pointers)
    PriorityQueue& operator=(const PriorityQueue&);

public:
    PriorityQueue();
    ~PriorityQueue();

    void enqueue(const EmergencyReport& r);   // insert at the correct position
    bool dequeue(EmergencyReport& out);       // remove the highest-priority report
    bool peek(EmergencyReport& out) const;    // look at the front WITHOUT removing it
    bool removeById(int id);                  // remove a report from anywhere in the queue
    bool contains(int id) const;
    bool isEmpty() const;
    int getSize() const;
    void display() const;

    void menu(ReportManager& reports);        // Priority Management menu
};

#endif
