#ifndef MEMORY_H
#define MEMORY_H

#include <array>
#include <iostream>
#include <vector>

class memory_t {
    static constexpr size_t MEM_SIZE = 0x1000;
    std::array<uint8_t, MEM_SIZE> m_data{};

  public:
    memory_t(const std::vector<char> &rom_bytes);
};

#endif // MEMORY_H
