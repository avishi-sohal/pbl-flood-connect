#include "ReliefCamp.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

// =================================================================
//                         class ReliefCamp
// =================================================================

ReliefCamp::ReliefCamp()
    : campId(0), capacity(0), availableBeds(0), foodPackets(0), medicalKits(0) {}

ReliefCamp::ReliefCamp(int id, const string& name, const string& loc, int cap,
                       int beds, int food, int kits)
    : campId(id), campName(name), location(loc), capacity(cap),
      availableBeds(beds), foodPackets(food), medicalKits(kits) {}

int ReliefCamp::getId() const { return campId; }
string ReliefCamp::getName() const { return campName; }
string ReliefCamp::getLocation() const { return location; }
int ReliefCamp::getCapacity() const { return capacity; }
int ReliefCamp::getAvailableBeds() const { return availableBeds; }
int ReliefCamp::getFoodPackets() const { return foodPackets; }
int ReliefCamp::getMedicalKits() const { return medicalKits; }

void ReliefCamp::setAvailableBeds(int beds) { availableBeds = beds; }
void ReliefCamp::setFoodPackets(int food) { foodPackets = food; }
void ReliefCamp::setMedicalKits(int kits) { medicalKits = kits; }

void ReliefCamp::display() const {
    cout << "===== RELIEF CAMP =====\n\n";
    cout << "Camp ID: " << campId << "\n";
    cout << "Camp Name: " << campName << "\n";
    cout << "Location: " << location << "\n";
    cout << "Capacity: " << capacity << "\n";
    cout << "Available Beds: " << availableBeds << "\n";
    cout << "Food Packets: " << foodPackets << "\n";
    cout << "Medical Kits: " << medicalKits << "\n\n";
}

// File format: ID|Name|Location|Capacity|Beds|Food|Kits
string ReliefCamp::toFileLine() const {
    ostringstream out;
    out << campId << "|" << campName << "|" << location << "|" << capacity << "|"
        << availableBeds << "|" << foodPackets << "|" << medicalKits;
    return out.str();
}

bool ReliefCamp::fromFileLine(const string& line, ReliefCamp& out) {
    vector<string> parts;
    stringstream ss(line);
    string field;
    while (getline(ss, field, '|')) parts.push_back(trim(field));

    if (parts.size() != 7) return false;

    int id, cap, beds, food, kits;
    if (!parseInt(parts[0], id) || id < 1) return false;
    if (!parseInt(parts[3], cap) || !parseInt(parts[4], beds) ||
        !parseInt(parts[5], food) || !parseInt(parts[6], kits)) return false;
    if (beds > cap) return false;

    out = ReliefCamp(id, parts[1], parts[2], cap, beds, food, kits);
    return true;
}

// =================================================================
//                        class CampManager
// =================================================================

CampManager::CampManager(const string& file) : fileName(file) {}

int CampManager::findIndex(int id) const {
    for (size_t i = 0; i < camps.size(); i++)
        if (camps[i].getId() == id) return (int)i;
    return -1;
}

void CampManager::loadFromFile() {
    ifstream in(fileName.c_str());
    if (!in) return;                       // no file yet -> start empty

    string line;
    int skipped = 0;
    while (getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        ReliefCamp c;
        if (ReliefCamp::fromFileLine(line, c) && findIndex(c.getId()) == -1)
            camps.push_back(c);
        else
            skipped++;
    }
    in.close();
    if (skipped > 0)
        cout << "Warning: " << skipped << " invalid/duplicate line(s) in "
             << fileName << " were skipped.\n";
}

void CampManager::saveToFile() const {
    ofstream out(fileName.c_str());
    if (!out) {
        cout << "Error: could not write to " << fileName << "\n";
        return;
    }
    for (size_t i = 0; i < camps.size(); i++)
        out << camps[i].toFileLine() << "\n";
    out.close();
}

void CampManager::addCamp() {
    printHeader("ADD RELIEF CAMP");

    int id;
    while (true) {
        id = readInt("Enter Camp ID: ", 1, 999999);
        if (findIndex(id) == -1) break;
        cout << "  Error: Camp ID " << id << " already exists. Use a different ID.\n";
    }

    string name = readNonEmpty("Enter Camp Name: ");
    string loc  = readNonEmpty("Enter Location: ");
    int capacity = readInt("Enter Capacity: ", 1, 100000);
    int beds     = readInt("Enter Available Beds: ", 0, capacity);   // cannot exceed capacity
    int food     = readInt("Enter Food Packets: ", 0, 1000000);
    int kits     = readInt("Enter Medical Kits: ", 0, 1000000);

    camps.push_back(ReliefCamp(id, name, loc, capacity, beds, food, kits));
    saveToFile();
    cout << "\nRelief camp added successfully!\n";
}

void CampManager::viewCamps() const {
    printHeader("ALL RELIEF CAMPS");
    if (camps.empty()) {
        cout << "No relief camps found.\n";
        return;
    }
    for (size_t i = 0; i < camps.size(); i++)
        camps[i].display();
    cout << "Total camps: " << camps.size() << "\n";
}

void CampManager::updateResources() {
    printHeader("UPDATE CAMP RESOURCES");
    int id = readInt("Enter Camp ID: ", 1, 999999);
    int idx = findIndex(id);
    if (idx == -1) {
        cout << "Camp not found.\n";
        return;
    }

    ReliefCamp& camp = camps[idx];     // reference: changes go straight into the vector
    camp.display();

    cout << "1. Update Available Beds\n";
    cout << "2. Update Food Packets\n";
    cout << "3. Update Medical Kits\n";
    int choice = readInt("What do you want to update? ", 1, 3);

    if (choice == 1)
        camp.setAvailableBeds(readInt("Enter new Available Beds: ", 0, camp.getCapacity()));
    else if (choice == 2)
        camp.setFoodPackets(readInt("Enter new Food Packets: ", 0, 1000000));
    else
        camp.setMedicalKits(readInt("Enter new Medical Kits: ", 0, 1000000));

    saveToFile();
    cout << "\nCamp resources updated.\n";
}

void CampManager::menu() {
    int choice;
    do {
        cout << "\n====================================\n";
        cout << "     RELIEF CAMP MANAGEMENT\n";
        cout << "====================================\n\n";
        cout << "1. Add Camp\n";
        cout << "2. View Camps\n";
        cout << "3. Update Camp Resources\n";
        cout << "4. Return to Main Menu\n\n";
        choice = readInt("Enter your choice: ", 1, 4);

        switch (choice) {
            case 1: addCamp(); break;
            case 2: viewCamps(); break;
            case 3: updateResources(); break;
            default: break;
        }
    } while (choice != 4);
}

int CampManager::getCampCount() const { return (int)camps.size(); }

int CampManager::getTotalAvailableBeds() const {
    int total = 0;
    for (size_t i = 0; i < camps.size(); i++)
        total += camps[i].getAvailableBeds();
    return total;
}
