#include <iostream>
#include "Utils.h"
#include "EmergencyReport.h"
#include "PriorityQueue.h"
#include "ReliefCamp.h"
#include "Dashboard.h"
#include "ChatBot.h"
using namespace std;

void showMainMenu() {
    cout << "\n========================================\n";
    cout << "       FLOODCONNECT\n";
    cout << " SMART EMERGENCY MANAGEMENT SYSTEM\n";
    cout << "========================================\n\n";
    cout << "1. Emergency Report Management\n";
    cout << "2. Priority Management\n";
    cout << "3. Relief Camp Management\n";
    cout << "4. Rescue Dashboard\n";
    cout << "5. FloodBot Assistant (Chatbot)\n";
    cout << "6. Exit\n\n";
}

int main() {
    // Objects of our classes
    ReportManager reports("reports.txt");
    CampManager camps("camps.txt");
    PriorityQueue priorityQueue;

    reports.loadFromFile();
    camps.loadFromFile();
    reports.loadPendingIntoQueue(priorityQueue);   

    Dashboard dashboard(reports, camps, priorityQueue);
    ChatBot bot(reports, camps, priorityQueue);

    cout << "Loaded " << reports.getTotal() << " report(s) and "
         << camps.getCampCount() << " camp(s) from file.\n";

    int choice;
    do {
        showMainMenu();
        choice = readInt("Enter your choice: ", 1, 6);

        switch (choice) {
            case 1: reports.menu(priorityQueue); break;
            case 2: priorityQueue.menu(reports); break;
            case 3: camps.menu(); break;
            case 4: dashboard.show(); break;
            case 5: bot.run(); break;
            default: break;
        }
    } while (choice != 6);

    cout << "\nAll data is saved. Stay safe. Goodbye!\n";
    return 0;
}
