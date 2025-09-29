
CXX ?= g++
# Enable common warnings and pthread (i may make converter use threads later)
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pthread

SOURCES = ascii_art.cpp converter.cpp
TARGET = Converter

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o
