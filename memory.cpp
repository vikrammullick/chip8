#include "memory.hpp"

using namespace std;

memory_t::memory_t(const vector<char> &rom_bytes) {
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

uint16_t memory_t::read_opcode(uint16_t &pc) {
    uint8_t msb = m_data[pc++];
    uint8_t lsb = m_data[pc++];

    return (msb << 8) | lsb;
}

void memory_t::push_stack(uint16_t &sp, uint16_t pc) {
    m_data[sp++] = pc & 0xFF;
    m_data[sp++] = pc >> 8;
}

uint16_t memory_t::pop_stack(uint16_t &sp) {
    uint16_t ret = m_data[--sp];
    ret = ret << 8;
    ret |= m_data[--sp];
    return ret;
}
