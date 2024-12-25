#include "cpu.hpp"

#include <sstream>

using namespace std;

cpu_t::cpu_t(memory_t &memory) : m_memory(memory) {}

void cpu_t::run() {
    while (1) {
        tick();
    }
}

void cpu_t::tick() {
    uint16_t opcode = m_memory.read(m_pc);

    if (opcode == 0x00E0) {
    }

    if (opcode >> 12 == 0x000F) {
    }

    stringstream oss;
    oss << "unknown opcode: 0x" << std::hex << opcode;
    throw std::runtime_error(oss.str());
}
