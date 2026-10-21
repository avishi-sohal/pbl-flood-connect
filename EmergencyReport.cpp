#include "EmergencyReport.h"
#include "PriorityQueue.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

// =================================================================
//                      class EmergencyReport
// =================================================================

EmergencyReport::EmergencyReport() : reportId(0), status("Pending") {}

EmergencyReport::EmergencyReport(int id, const string& name, const string& loc,
                                 const string& type, const string& sev,
                                 const string& desc, const string& contact,
                                 const string& stat)
    : reportId(id), victimName(name), location(loc), emergencyType(type),
      severity(sev), description(desc), contactNumber(contact), status(stat) {}

int EmergencyReport::getId() const { return reportId; }
string EmergencyReport::getVictimName() const { return victimName; }
string EmergencyReport::getLocation() const { return location; }
string EmergencyReport::getEmergencyType() const { return emergencyType; }
string EmergencyReport::getSeverity() const { return severity; }
string EmergencyReport::getDescription() const { return description; }
string EmergencyReport::getContact() const { return contactNumber; }
string EmergencyReport::getStatus() const { return status; }

void EmergencyReport::setStatus(const string& newStatus) { status = newStatus; }

// HIGH = 1, MEDIUM = 2, LOW = 3  (smaller number = more urgent)
int EmergencyReport::getPriority() const {
    if (severity == "HIGH") return 1;
    if (severity == "MEDIUM") return 2;
    return 3;
}

bool EmergencyReport::isActive() const {
    return status == "Pending" || status == "In Progress";
}

bool EmergencyReport::isValidStatus(const string& s) {
    return s == "Pending" || s == "In Progress" || s == "Rescued" || s == "Closed";
}

// Module 1: read all details from the user. A new report always starts as "Pending".
void EmergencyReport::inputDetails(int id) {
    reportId = id;
    victimName    = readNonEmpty("Enter Victim Name: ");
    location      = readNonEmpty("Enter Location: ");
    emergencyType = readNonEmpty("Enter Emergency Type: ");
    severity      = readSeverity("Enter Severity (HIGH/MEDIUM/LOW): ");
    description   = readNonEmpty("Enter Description: ");
    contactNumber = readContact("Enter Contact Number (10 digits): ");
    status        = "Pending";
}

void EmergencyReport::display() const {
    cout << "----------------------------------------\n";
    cout << "Report ID      : " << reportId << "\n";
    cout << "Victim Name    : " << victimName << "\n";
    cout << "Location       : " << location << "\n";
    cout << "Emergency Type : " << emergencyType << "\n";
    cout << "Severity       : " << severity << " (Priority " << getPriority() << ")\n";
    cout << "Description    : " << description << "\n";
    cout << "Contact Number : " << contactNumber << "\n";
    cout << "Status         : " << status << "\n";
    cout << "----------------------------------------\n";
}

// File format (one report per line, fields separated by '|'):
// ID|Name|Location|Type|Severity|Description|Contact|Status
string EmergencyReport::toFileLine() const {
    ostringstream out;
    out << reportId << "|" << victimName << "|" << location << "|" << emergencyType << "|"
        << severity << "|" << description << "|" << contactNumber << "|" << status;
    return out.str();
}

bool EmergencyReport::fromFileLine(const string& line, EmergencyReport& out) {
    vector<string> parts;
    stringstream ss(line);
    string field;
    while (getline(ss, field, '|')) parts.push_back(trim(field));

    if (parts.size() != 8) return false;

    int id;
    if (!parseInt(parts[0], id) || id < 1) return false;

    string sev = toUpper(parts[4]);
    if (sev != "HIGH" && sev != "MEDIUM" && sev != "LOW") return false;
    if (!isValidStatus(parts[7])) return false;

    out = EmergencyReport(id, parts[1], parts[2], parts[3], sev, parts[5], parts[6], parts[7]);
    return true;
}

// =================================================================
//          class ReportManager  (singly linked list of reports)
// =================================================================

ReportManager::ReportManager(const string& file)
    : head(NULL), tail(NULL), count(0), fileName(file) {}

ReportManager::~ReportManager() {
    while (head != NULL) {
        ReportNode* temp = head;
        head = head->next;
        delete temp;
    }
}

// Add a new node at the END of the list (keeps reports in the order they arrived)
void ReportManager::insertAtEnd(const EmergencyReport& r) {
    ReportNode* node = new ReportNode(r);
    if (head == NULL) {
        head = tail = node;
    } else {
        tail->next = node;
        tail = node;
    }
    count++;
}

// Walk the list from the head until the ID matches
ReportNode* ReportManager::findNode(int id) const {
    ReportNode* cur = head;
    while (cur != NULL) {
        if (cur->data.getId() == id) return cur;
        cur = cur->next;
    }
    return NULL;
}

bool ReportManager::exists(int id) const { return findNode(id) != NULL; }

bool ReportManager::getReport(int id, EmergencyReport& out) const {
    ReportNode* node = findNode(id);
    if (node == NULL) return false;
    out = node->data;
    return true;
}

bool ReportManager::setStatusById(int id, const string& newStatus) {
    ReportNode* node = findNode(id);
    if (node == NULL) return false;
    node->data.setStatus(newStatus);
    saveToFile();
    return true;
}

// ------------------------- file handling -------------------------

void ReportManager::loadFromFile() {
    ifstream in(fileName.c_str());
    if (!in) return;                       // no file yet -> start empty

    string line;
    int skipped = 0;
    while (getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        EmergencyReport r;
        if (EmergencyReport::fromFileLine(line, r) && !exists(r.getId()))
            insertAtEnd(r);
        else
            skipped++;
    }
    in.close();
    if (skipped > 0)
        cout << "Warning: " << skipped << " invalid/duplicate line(s) in "
             << fileName << " were skipped.\n";
}

// Rewrites the whole file from the linked list (simple and safe for a small project)
void ReportManager::saveToFile() const {
    ofstream out(fileName.c_str());
    if (!out) {
        cout << "Error: could not write to " << fileName << "\n";
        return;
    }
    ReportNode* cur = head;
    while (cur != NULL) {
        out << cur->data.toFileLine() << "\n";
        cur = cur->next;
    }
    out.close();
}

// On start-up every Pending report is placed in the priority queue
void ReportManager::loadPendingIntoQueue(PriorityQueue& pq) const {
    ReportNode* cur = head;
    while (cur != NULL) {
        if (cur->data.getStatus() == "Pending")
            pq.enqueue(cur->data);
        cur = cur->next;
    }
}

// ------------------------- menu operations -------------------------

void ReportManager::addReport(PriorityQueue& pq) {
    printHeader("ADD EMERGENCY REPORT");

    int id;
    while (true) {
        id = readInt("Enter Report ID: ", 1, 999999);
        if (!exists(id)) break;
        cout << "  Error: Report ID " << id << " already exists. Use a different ID.\n";
    }

    EmergencyReport r;
    r.inputDetails(id);

    insertAtEnd(r);
    saveToFile();
    pq.enqueue(r);                         // severity decides its place in the queue

    cout << "\nReport added successfully!\n";
    cout << "Severity " << r.getSeverity() << " = Priority " << r.getPriority()
         << ". Report was added to the priority queue.\n";
}

void ReportManager::viewAll() const {
    printHeader("ALL EMERGENCY REPORTS");
    if (head == NULL) {
        cout << "No reports found.\n";
        return;
    }

    cout << left << setw(8) << "ID" << setw(14) << "Victim" << setw(18) << "Location"
         << setw(15) << "Type" << setw(10) << "Severity" << "Status\n";
    cout << string(75, '-') << "\n";

    ReportNode* cur = head;
    while (cur != NULL) {
        const EmergencyReport& r = cur->data;
        cout << left << setw(8) << r.getId()
             << setw(14) << fit(r.getVictimName(), 13)
             << setw(18) << fit(r.getLocation(), 17)
             << setw(15) << fit(r.getEmergencyType(), 14)
             << setw(10) << r.getSeverity()
             << r.getStatus() << "\n";
        cur = cur->next;
    }
    cout << string(75, '-') << "\n";
    cout << "Total reports: " << count << "\n";
}

void ReportManager::viewById() const {
    printHeader("SEARCH REPORT BY ID");
    int id = readInt("Enter Report ID: ", 1, 999999);
    ReportNode* node = findNode(id);
    if (node == NULL) {
        cout << "Report not found.\n";
        return;
    }
    node->data.display();
}

void ReportManager::updateStatus(PriorityQueue& pq) {
    printHeader("UPDATE REPORT STATUS");
    int id = readInt("Enter Report ID: ", 1, 999999);
    ReportNode* node = findNode(id);
    if (node == NULL) {
        cout << "Report not found.\n";
        return;
    }

    cout << "Current status: " << node->data.getStatus() << "\n\n";
    cout << "1. Pending\n2. In Progress\n3. Rescued\n4. Closed\n";
    int choice = readInt("Choose new status: ", 1, 4);

    string newStatus;
    if (choice == 1) newStatus = "Pending";
    else if (choice == 2) newStatus = "In Progress";
    else if (choice == 3) newStatus = "Rescued";
    else newStatus = "Closed";

    node->data.setStatus(newStatus);
    saveToFile();

    // Keep the queue in step: only Pending reports wait in the priority queue
    if (newStatus == "Pending") {
        if (!pq.contains(id)) pq.enqueue(node->data);
    } else {
        pq.removeById(id);
    }
    cout << "Status updated to " << newStatus << ".\n";
}

void ReportManager::deleteReport(PriorityQueue& pq) {
    printHeader("DELETE REPORT");
    int id = readInt("Enter Report ID: ", 1, 999999);
    ReportNode* node = findNode(id);
    if (node == NULL) {
        cout << "Report not found.\n";
        return;
    }

    node->data.display();
    if (!confirm("Delete this report? (Y/N): ")) {
        cout << "Delete cancelled.\n";
        return;
    }

    // Unlink the node: remember the previous node while searching
    ReportNode* prev = NULL;
    ReportNode* cur = head;
    while (cur != NULL && cur->data.getId() != id) {
        prev = cur;
        cur = cur->next;
    }
    if (prev == NULL) head = cur->next;    // deleting the first node
    else prev->next = cur->next;
    if (cur == tail) tail = prev;          // deleting the last node
    delete cur;
    count--;

    pq.removeById(id);
    saveToFile();
    cout << "Report deleted.\n";
}

void ReportManager::menu(PriorityQueue& pq) {
    int choice;
    do {
        cout << "\n====================================\n";
        cout << "   EMERGENCY REPORT MANAGEMENT\n";
        cout << "====================================\n\n";
        cout << "1. Add Report\n";
        cout << "2. View All Reports\n";
        cout << "3. Search/View a Report by ID\n";
        cout << "4. Update Report Status\n";
        cout << "5. Delete Report\n";
        cout << "6. Return to Main Menu\n\n";
        choice = readInt("Enter your choice: ", 1, 6);

        switch (choice) {
            case 1: addReport(pq); break;
            case 2: viewAll(); break;
            case 3: viewById(); break;
            case 4: updateStatus(pq); break;
            case 5: deleteReport(pq); break;
            default: break;
        }
    } while (choice != 6);
}

// ------------------------- dashboard numbers -------------------------

int ReportManager::getTotal() const { return count; }

int ReportManager::countBySeverity(const string& sev) const {
    int n = 0;
    for (ReportNode* cur = head; cur != NULL; cur = cur->next)
        if (cur->data.getSeverity() == sev) n++;
    return n;
}

int ReportManager::countByStatus(const string& stat) const {
    int n = 0;
    for (ReportNode* cur = head; cur != NULL; cur = cur->next)
        if (cur->data.getStatus() == stat) n++;
    return n;
}

int ReportManager::countActive() const {
    int n = 0;
    for (ReportNode* cur = head; cur != NULL; cur = cur->next)
        if (cur->data.isActive()) n++;
    return n;
}

// The list is in arrival order, so the LAST matching node is the latest case
bool ReportManager::getLatestActiveHigh(EmergencyReport& out) const {
    bool found = false;
    for (ReportNode* cur = head; cur != NULL; cur = cur->next) {
        if (cur->data.getSeverity() == "HIGH" && cur->data.isActive()) {
            out = cur->data;
            found = true;
        }
    }
    return found;
}
