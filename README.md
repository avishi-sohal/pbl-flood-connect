# FloodConnect – Smart Emergency Management System

FloodConnect is a console-based C++ project designed to manage flood emergency reports, prioritize rescue requests, track victim status, and manage relief camps and resources.

**Current Status:** Phase 2 – Core functionality implemented.

## Features

- Register, view, search, update, and delete emergency reports.
- Prioritize emergencies by severity: HIGH, MEDIUM, and LOW.
- Track rescue status: Pending, In Progress, Rescued, and Closed.
- Manage relief camps, available beds, food packets, and medical kits.
- Display a dashboard with emergency and resource statistics.
- Save data in text files for persistence between runs.
- Validate user input and prevent duplicate IDs.

## Tech Stack

- **Language:** C++, HTML, CSS
- **Concepts:** Object-Oriented Programming (OOP), classes, objects, encapsulation, and file handling.
- **Data Structures:** Singly linked list, priority queue implemented using a linked list, and vector.
- **File Storage:** Plain-text files (`.txt`)
- **Compiler:** g++ 
- **Development Environment:** Visual Studio Code (VS Code)

No database or external libraries are required.

## Project Structure

```text
FloodConnect/
├── main.cpp                 # Main menu and program entry point
├── Utils.h                  # Input validation utilities
├── EmergencyReport.h/.cpp   # Emergency reports and linked-list management
├── PriorityQueue.h/.cpp     # Priority-based emergency management
├── ReliefCamp.h/.cpp        # Relief camps and resource management
├── Dashboard.h/.cpp         # Emergency and resource summary
├── reports.txt              # Saved emergency reports
├── camps.txt                # Saved relief camp data
└── README.md                # Project documentation
```

## How to Run

### Prerequisites

Install a C++ compiler such as `g++` and ensure it is available in your terminal.

### 1. Clone the repository

```bash
git clone https://github.com/avishi-sohal/pbl-flood-connect.git
```

### 2. Open the project folder

```bash
cd pbl-flood-connect
```

### 3. Compile the program

Run this command from inside the project folder:

```bash
g++ main.cpp EmergencyReport.cpp PriorityQueue.cpp ReliefCamp.cpp Dashboard.cpp ChatBot.cpp -o FloodConnect.exe
```

### 4. Run the program

**Windows (PowerShell):**

```powershell
.\FloodConnect.exe
```

**Linux/macOS:**

```bash
./FloodConnect.exe
```

Run the program from inside the project folder so it can access `reports.txt` and `camps.txt`. If the data files are missing, the program starts with empty data and creates the files when saving.


## Team Members

- **Avishi Sohal**
- **Abhrioop**
- **Simmi**
- **Mouli**

## Future Enhancements

The following features are planned for Phase 3 and are not implemented in the current version:

- AI-based classification of flood images as real, fake, or AI-generated.
- Graph-based road mapping and shortest-path rescue routing.
- User authentication and role-based access.
- Improved dashboard and resource allocation.

---

*FloodConnect aims to support more organized emergency response and resource management during floods.*