#include <iostream>
#include "Utils.h"
#include "EmergencyReport.h"
#include "PriorityQueue.h"
#include "ReliefCamp.h"
#include "Dashboard.h"
#include "ChatBot.h"
#include "Auth.h"
#include "RoadGraph.h"
#include "Allocation.h"
#include "ImageAnalysis.h"
#include "UndoManager.h"
using namespace std;

static void showMainMenu(Role role) {
    cout << "\n========================================\n"
         << "       FLOODCONNECT - " << roleName(role) << "\n"
         << "========================================\n\n"
         << "1. Emergency Report Management\n"
         << "2. Priority Management\n"
         << "3. Relief Camp Management\n"
         << "4. Rescue Dashboard\n"
         << "5. FloodBot Assistant\n"
         << "6. Rescue Routes\n"
         << "7. Resource Allocation\n"
         << "8. Flood Image Analysis\n"
         << "9. Undo Last Menu Operation\n"
         << "10. Exit\n\n";
}

int main() {
    Role role = login();
    if (role == ROLE_NONE) {
        cout << "Login failed. Goodbye.\n";
        return 1;
    }

    ReportManager reports("reports.txt");
    CampManager camps("camps.txt");
    PriorityQueue priorityQueue;
    RoadGraph graph("roads.txt");
    UndoManager undo;

    reports.loadFromFile();
    camps.loadFromFile();
    reports.loadPendingIntoQueue(priorityQueue);

    Dashboard dashboard(reports, camps, priorityQueue);
    ChatBot bot(reports, camps, priorityQueue);
    bot.setCitizenMode(role == ROLE_CITIZEN);

    cout << "Loaded " << reports.getTotal() << " report(s) and "
         << camps.getCampCount() << " camp(s) from file.\n";

    int choice;
    do {
        showMainMenu(role);
        choice = readInt("Enter your choice: ", 1, 10);
        if (choice == 10) break;
        if (role == ROLE_CITIZEN && choice != 1 && choice != 5) {
            cout << "This feature is available to rescue staff.\n";
            continue;
        }
        if (choice == 9) {
            if (undo.undo(reports, camps, priorityQueue)) cout << "Last saved operation was undone.\n";
            else cout << "There is no operation to undo.\n";
            continue;
        }
        if (choice == 1 && role == ROLE_CITIZEN) {
            undo.push(reports, camps);
            reports.addReport(priorityQueue);
        } else if (choice == 1) { undo.push(reports, camps); reports.menu(priorityQueue); }
        else if (choice == 2) { undo.push(reports, camps); priorityQueue.menu(reports); }
        else if (choice == 3) { undo.push(reports, camps); camps.menu(); }
        else if (choice == 4) dashboard.show();
        else if (choice == 5) bot.run();
        else if (choice == 6) graph.menu(priorityQueue, camps);
        else if (choice == 7) allocationMenu(reports, priorityQueue, camps, graph);
        else if (choice == 8) imageAnalysisMenu();
    } while (choice != 10);

    cout << "\nAll data is saved. Stay safe. Goodbye.\n";
    return 0;
}
