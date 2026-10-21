#ifndef EMERGENCYREPORT_H
#define EMERGENCYREPORT_H

#include <string>
using namespace std;

class PriorityQueue;   

// ---------------------------------------------------------------
// MODULE 1: one flood emergency report (a single object = one report)
// ---------------------------------------------------------------
class EmergencyReport {
private:                       
    int reportId;
    string victimName;
    string location;
    string emergencyType;
    string severity;           
    string description;
    string contactNumber;
    string status;             

public:                        
    EmergencyReport();
    EmergencyReport(int id, const string& name, const string& loc, const string& type,
                    const string& sev, const string& desc, const string& contact,
                    const string& stat);

    // getters
    int getId() const;
    string getVictimName() const;
    string getLocation() const;
    string getEmergencyType() const;
    string getSeverity() const;
    string getDescription() const;
    string getContact() const;
    string getStatus() const;

    
    void setStatus(const string& newStatus);

    int getPriority() const;           // HIGH=1, MEDIUM=2, LOW=3
    bool isActive() const;             // Pending or In Progress

    void inputDetails(int id);         
    void display() const;             
    string toFileLine() const;         
    static bool fromFileLine(const string& line, EmergencyReport& out);
    static bool isValidStatus(const string& s);
};

// ---------------------------------------------------------------
// MODULE 2: manages all reports using a SINGLY LINKED LIST
// ---------------------------------------------------------------
struct ReportNode {
    EmergencyReport data;
    ReportNode* next;
    ReportNode(const EmergencyReport& r) : data(r), next(NULL) {}
};

class ReportManager {
private:
    ReportNode* head;
    ReportNode* tail;
    int count;
    string fileName;

    ReportNode* findNode(int id) const;
    void insertAtEnd(const EmergencyReport& r);

    // copying a list with raw pointers is dangerous, so copying is blocked
    ReportManager(const ReportManager&);
    ReportManager& operator=(const ReportManager&);

public:
    ReportManager(const string& file);
    ~ReportManager();

    // file handling
    void loadFromFile();
    void saveToFile() const;

    // menu operations
    void menu(PriorityQueue& pq);
    void addReport(PriorityQueue& pq);
    void viewAll() const;
    void viewById() const;
    void updateStatus(PriorityQueue& pq);
    void deleteReport(PriorityQueue& pq);

    // helpers used by other modules
    bool exists(int id) const;
    bool getReport(int id, EmergencyReport& out) const;
    bool setStatusById(int id, const string& newStatus);
    void loadPendingIntoQueue(PriorityQueue& pq) const;

    // numbers for the dashboard (calculated from the real list)
    int getTotal() const;
    int countBySeverity(const string& sev) const;
    int countByStatus(const string& stat) const;
    int countActive() const;
    bool getLatestActiveHigh(EmergencyReport& out) const;
};

#endif
