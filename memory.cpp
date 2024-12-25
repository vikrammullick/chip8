#include "memory.hpp"
#include "constants.hpp"

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

uint16_t memory_t::read(uint16_t &pc) {
    uint8_t msb = m_data[pc++];
    uint8_t lsb = m_data[pc++];

    return (msb << 8) | lsb;
}
