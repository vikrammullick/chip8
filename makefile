CXX := g++
VERILATOR = verilator

CXXFLAGS := -std=c++17 -Wall -O3 -I/usr/local/Cellar/verilator/5.032/share/verilator/include -Iad_obj_dir -Icpu_obj_dir
VERILATOR_FLAGS := --cc

AD_SRC := address_decoder.v
AD_OBJ_DIR := ad_obj_dir

CPU_SRC := cpu.v time_keeper.v alu.v
CPU_OBJ_DIR := cpu_obj_dir

VERILATED_LIB := /usr/local/Cellar/verilator/5.032/share/verilator/include/verilated.cpp
VERILATED_THREADS := /usr/local/Cellar/verilator/5.032/share/verilator/include/verilated_threads.cpp

SRCS := run.cpp interpreter.cpp cpu.cpp ppu.cpp memory.cpp address_decoder.cpp keyboard.cpp timer.cpp rng.cpp sdl_helpers.cpp 

OBJS := $(SRCS:.cpp=.o)

$HEADERS := interpreter.hpp cpu.hpp ppu.hpp memory.hpp address_decoder.hpp keyboard.hpp timer.hpp rng.hpp bus.hpp constants.hpp sdl_helpers.hpp

TARGET := chip8

all: $(TARGET)

run: all
	./chip8 roms/breakout.rom

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -undefined dynamic_lookup -o $@ $^ -lSDL2 $(AD_OBJ_DIR)/Vaddress_decoder__ALL.a $(CPU_OBJ_DIR)/Vcpu__ALL.a $(VERILATED_LIB) $(VERILATED_THREADS) 

%.o: %.cpp $(HEADERS) $(AD_OBJ_DIR)/Vaddress_decoder__ALL.a $(CPU_OBJ_DIR)/Vcpu__ALL.a
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(AD_OBJ_DIR)/Vaddress_decoder__ALL.a: $(AD_SRC)
	$(VERILATOR) $(VERILATOR_FLAGS) $(AD_SRC) -Mdir $(AD_OBJ_DIR)
	make -C $(AD_OBJ_DIR) -f Vaddress_decoder.mk

$(CPU_OBJ_DIR)/Vcpu__ALL.a: $(CPU_SRC)
	$(VERILATOR) $(VERILATOR_FLAGS) $(CPU_SRC) -Mdir $(CPU_OBJ_DIR)
	make -C $(CPU_OBJ_DIR) -f Vcpu.mk

clean:
	rm -rf $(TARGET) $(OBJS) $(AD_OBJ_DIR) $(CPU_OBJ_DIR)


