#include "address_decoder.hpp"
#include "constants.hpp"

address_decoder_t::address_decoder_t(bus_t &bus) : m_bus(bus) {}

void address_decoder_t::select_chip() {
    m_bus.m_chip_select = 1 << constants::MEMORY_CHIP_SELECT;
}
