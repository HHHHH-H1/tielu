CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = railway_system
SOURCES = Station.cpp Route.cpp Train.cpp PassengerFlow.cpp DataAnalyzer.cpp FileManager.cpp main.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run 