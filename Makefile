CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
TARGET = digital_clock
SOURCE = digital_clock.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE) $(SFML_FLAGS)

clean:
	rm -f $(TARGET) alarms.txt

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run

