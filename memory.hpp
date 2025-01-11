#ifndef MEMORY_H
#define MEMORY_H

#include "bus.hpp"
#include "constants.hpp"

#include <array>
#include <iostream>
#include <vector>

class memory_t {
    std::array<uint8_t, constants::MEM_SIZE> m_data{};
    bus_t &m_bus;

    void write(uint16_t addr, uint8_t val);
    uint8_t read(uint16_t addr);

  public:
    memory_t(const std::vector<char> &rom_bytes, bus_t &bus);
    void service_request();
};

#endif // MEMORY_H
