#ifndef RELIEFCAMP_H
#define RELIEFCAMP_H

#include <string>
#include <vector>
using namespace std;

// ---------------------------------------------------------------
// MODULE 4: one relief camp (one object = one camp)
// ---------------------------------------------------------------
class ReliefCamp {
private:
    int campId;
    string campName;
    string location;
    int capacity;
    int availableBeds;
    int foodPackets;
    int medicalKits;

public:
    ReliefCamp();
    ReliefCamp(int id, const string& name, const string& loc, int cap,
               int beds, int food, int kits);

    // getters
    int getId() const;
    string getName() const;
    string getLocation() const;
    int getCapacity() const;
    int getAvailableBeds() const;
    int getFoodPackets() const;
    int getMedicalKits() const;

    // setters (used by Update Camp Resources)
    void setAvailableBeds(int beds);
    void setFoodPackets(int food);
    void setMedicalKits(int kits);

    void display() const;
    string toFileLine() const;
    static bool fromFileLine(const string& line, ReliefCamp& out);
};

// Manages all camps. Camps are kept in a vector (a dynamic array).
class CampManager {
private:
    vector<ReliefCamp> camps;
    string fileName;

    int findIndex(int id) const;   // returns -1 if not found

public:
    CampManager(const string& file);

    void loadFromFile();
    void saveToFile() const;

    void menu();
    void addCamp();
    void viewCamps() const;
    void updateResources();

    // for the dashboard
    int getCampCount() const;
    int getTotalAvailableBeds() const;
};

#endif
