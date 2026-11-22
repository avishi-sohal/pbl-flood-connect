# make        builds the program (run ./FloodConnect from inside this folder)
# make test   builds and runs the automated tests
LIB = EmergencyReport.cpp PriorityQueue.cpp ReliefCamp.cpp Dashboard.cpp ChatBot.cpp FloodGuide.cpp \
      RoadGraph.cpp Allocation.cpp ImageAnalysis.cpp UndoManager.cpp
FLAGS = -Wall -Wextra -pedantic

FloodConnect: main.cpp $(LIB) $(wildcard *.h)
	g++ $(FLAGS) main.cpp $(LIB) -o FloodConnect

test: tests.cpp $(LIB) $(wildcard *.h)
	g++ $(FLAGS) tests.cpp $(LIB) -o run_tests && ./run_tests
