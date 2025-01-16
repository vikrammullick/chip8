CXX := g++

CXXFLAGS := -std=c++17 -Wall -O3

SRCS := run.cpp interpreter.cpp cpu.cpp ppu.cpp memory.cpp address_decoder.cpp timer.cpp sdl_helpers.cpp 

OBJS := $(SRCS:.cpp=.o)

$HEADERS := interpreter.hpp cpu.hpp ppu.hpp memory.hpp address_decoder.hpp timer.hpp bus.hpp constants.hpp sdl_helpers.hpp

TARGET := chip8

all: $(TARGET)

run: all
	./chip8 roms/breakout.rom

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lSDL2

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)


