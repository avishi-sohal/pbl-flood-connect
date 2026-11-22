# FloodConnect

**Smart Flood Emergency & Rescue Management System**

FloodConnect is a C++-based prototype for managing flood emergencies. It records and prioritises reports, plans rescue routes, allocates relief-camp resources, and provides role-based access and citizen assistance.

## Features

- Emergency reporting and severity-based prioritisation
- Graph-based route planning using Dijkstra's algorithm
- Flooded-road avoidance
- Greedy relief-resource allocation
- Hash-based report lookup and stack-based undo
- Admin, rescue-team and citizen roles
- Rule-based FloodBot and image analysis
- Dashboard and browser-based interface

## Tech Stack

- **Language:** C++
- **Concepts:** OOP, linked lists, priority queue, vector, graph, hashing and stack
- **Interface:** HTML
- **Storage:** Local text files (no database)

## Folder Structure

```text
FloodConnect/
├── main.cpp
├── Utils.h
├── EmergencyReport.h
├── EmergencyReport.cpp
├── PriorityQueue.h
├── PriorityQueue.cpp
├── ReliefCamp.h
├── ReliefCamp.cpp
├── Dashboard.h
├── Dashboard.cpp
├── ChatBot.h
├── ChatBot.cpp
├── FloodGuide.h
├── FloodGuide.cpp
├── RoadGraph.h
├── RoadGraph.cpp
├── Allocation.h
├── Allocation.cpp
├── ImageAnalysis.h
├── ImageAnalysis.cpp
├── HashIndex.h
├── UndoManager.h
├── UndoManager.cpp
├── Auth.h
├── tests.cpp
├── Makefile
├── FloodConnect_phase3.html
├── reports.txt
├── camps.txt
├── roads.txt
├── PRESENTATION_NOTES.md
└── README.md
```

*`roads.txt` is created when flooded-road data is first saved. The text data files are used for local persistence.*

## Build and Run

Build using the Makefile:

```bash
make
```

Run on Windows:

```powershell
.\FloodConnect.exe
```

Or compile manually with g++:

```bash
g++ -std=c++11 main.cpp EmergencyReport.cpp PriorityQueue.cpp ReliefCamp.cpp Dashboard.cpp ChatBot.cpp RoadGraph.cpp Allocation.cpp ImageAnalysis.cpp UndoManager.cpp FloodGuide.cpp -o FloodConnect
```

## Testing

```bash
make test
```

The project test suite includes 46 automated checks.

## Demo Login

| Role | Username | Password |
|---|---|---|
| Admin | `admin` | `admin123` |
| Rescue | `rescue` | `rescue123` |
| Citizen | `citizen` | `citizen123` |

*Demo credentials only. Authentication is not production-grade.*

## Browser Version

Open `FloodConnect_phase3.html` in a modern web browser.

## Limitations

- Uses sample locations and local text-file storage.
- C++ image analysis accepts PPM files and uses colour-based estimation, not a trained AI model.
- Resource allocation is greedy, not globally optimal.
- Designed as a single-user prototype.

## Documentation

See [`PRESENTATION_NOTES.md`](PRESENTATION_NOTES.md) for the demonstration flow and evaluator Q&A.
