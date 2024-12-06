# Compiler and flags
CXX = g++
CXXFLAGS = -Isrc/include -Lsrc/lib

# Output and source files
TARGET = build/main
SRC = src/main.cpp

# Libraries
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

# Rules
all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	@echo "Cleaning up..."
	rm -f $(TARGET)
