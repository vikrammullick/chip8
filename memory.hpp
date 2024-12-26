#ifndef MEMORY_H
#define MEMORY_H

#include "constants.hpp"

#include <array>
#include <iostream>
#include <vector>

class memory_t {
    std::array<uint8_t, constants::MEM_SIZE> m_data{};

  public:
    memory_t(const std::vector<char> &rom_bytes);

    void write(uint16_t addr, uint8_t val);
    uint8_t read(uint16_t addr);

    // TODO: these should really live in the CPU
    uint16_t read_opcode(uint16_t &pc);
    void push_stack(uint16_t &sp, uint16_t pc);
    uint16_t pop_stack(uint16_t &sp);
};

#endif // MEMORY_H
