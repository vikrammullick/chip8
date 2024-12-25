CXX := g++

CXXFLAGS := -std=c++17 -Wall -O3

SRCS := run.cpp interpreter.cpp cpu.cpp ppu.cpp memory.cpp 

OBJS := $(SRCS:.cpp=.o)

HEADERS := interpreter.hpp cpu.hpp ppu.h memory.h constants.hpp

TARGET := chip8

all: $(TARGET)

run: all
	./chip8 breakout.rom

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)


