
CXX ?= g++
# Enable common warnings and pthread (i may make converter use threads later)
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pthread

SOURCES = ascii_art.cpp converter.cpp

# On Windows (when using GNU make from MSYS/MinGW) the OS variable is set to Windows_NT
ifeq ($(OS),Windows_NT)
	EXE_EXT := .exe
	RM := del /Q
else
	EXE_EXT :=
	RM := rm -f
endif

TARGET = Converter$(EXE_EXT)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	$(RM) $(TARGET) *.o
