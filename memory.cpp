#include "memory.hpp"

using namespace std;

memory_t::memory_t(const vector<char> &rom_bytes, bus_t &bus) : m_bus(bus) {
    // load fonts
    std::copy(constants::FONTSET.begin(),
              constants::FONTSET.end(),
              m_data.begin() + constants::FONTSET_OFFSET);

    // load rom
    std::copy(rom_bytes.begin(),
              rom_bytes.end(),
              m_data.begin() + constants::ROM_OFFSET);
}

void memory_t::write(uint16_t addr, uint8_t val) { m_data[addr] = val; }

uint8_t memory_t::read(uint16_t addr) { return m_data[addr]; }

void memory_t::service_request() {
    if (!(m_bus.m_chip_select & (1 << constants::MEMORY_CHIP_SELECT))) {
        return;
    }

    if (m_bus.m_read_enabled) {
        m_bus.m_data_line = read(m_bus.m_addr_line);
    }

    if (m_bus.m_write_enabled) {
        write(m_bus.m_addr_line, m_bus.m_data_line);
    }
}
