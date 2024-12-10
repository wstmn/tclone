# Compiler and flags
CXX = g++
CXXFLAGS = -Isrc/include -Lsrc/lib

# Output and source files
TARGET = build/main
SRC = src/main.cpp src/lTexture.cpp src/player.cpp
OBJ = build/main.o build/lTexture.o build/player.o

# Libraries
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

# Rules
all: build $(TARGET)

build:
	mkdir -p build

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS) -Wl,-rpath,./build

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	rm -f $(OBJ) $(TARGET)

