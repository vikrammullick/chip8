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

    uint16_t read(uint16_t &pc);
};

#endif // MEMORY_H
