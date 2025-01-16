#include "address_decoder.hpp"
#include "constants.hpp"

address_decoder_t::address_decoder_t(bus_t &bus) : m_bus(bus) {}

void address_decoder_t::select_chip() {
    if (m_bus.m_addr_line == constants::DELAY_TIMER_ADDR) {
        m_bus.m_chip_select = 1 << constants::DELAY_TIMER_CHIP_SELECT;
    } else if (m_bus.m_addr_line == constants::SOUND_TIMER_ADDR) {
        m_bus.m_chip_select = 1 << constants::SOUND_TIMER_CHIP_SELECT;
    } else {
        m_bus.m_chip_select = 1 << constants::MEMORY_CHIP_SELECT;
    }
}
