CXX      := g++
CXXFLAGS := -std=c++17 -pedantic-errors -Wall -Wextra
LDFLAGS  := -L/usr/lib -L/home/asger/pngwriter-build -lstdc++ -lm -lPNGwriter -lfreetype -lz -lpng -lblitz
OBJ_DIR  := obj
APP_DIR  := bin
TARGET   := hydrocode
INCLUDE  := -Iboundary -Ieos -Iintegrators -Imisc -Ioutput -Islopelimiters -Isolvers -I/usr/include/freetype2
SRC      :=                      \
   $(wildcard boundary/*.cpp) \
   $(wildcard eos/*.cpp) \
   $(wildcard integrators/*.cpp) \
   $(wildcard misc/*.cpp) \
   $(wildcard output/*.cpp) \
   $(wildcard slopelimiters/*.cpp) \
   $(wildcard solvers/*.cpp) \
   $(wildcard *.cpp)         \

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ $(LDFLAGS)

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

.PHONY: all build clean debug release

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
