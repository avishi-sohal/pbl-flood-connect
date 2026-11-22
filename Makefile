CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -pedantic
SOURCES = EmergencyReport.cpp PriorityQueue.cpp ReliefCamp.cpp Dashboard.cpp ChatBot.cpp \
	FloodGuide.cpp RoadGraph.cpp Allocation.cpp ImageAnalysis.cpp UndoManager.cpp
HEADERS = $(wildcard *.h)
APP = FloodConnect.exe
TEST = run_tests.exe

.PHONY: all test clean
all: $(APP)

$(APP): main.cpp $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) main.cpp $(SOURCES) -o $@

$(TEST): tests.cpp $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) tests.cpp $(SOURCES) -o $@

test: $(TEST)
	./$(TEST)

clean:
	$(RM) $(APP) $(TEST) *.o
