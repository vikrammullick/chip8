#include "address_decoder.hpp"
#include "constants.hpp"

address_decoder_t::address_decoder_t(bus_t &bus) : m_bus(bus) {}

void address_decoder_t::select_chip() {
    // TODO: switch to switch case
    if (m_bus.m_addr_line == constants::KEYBOARD_ADDR_LO ||
        m_bus.m_addr_line == constants::KEYBOARD_ADDR_HI ||
        m_bus.m_addr_line == constants::KEYBOARD_WAIT_REL_ADDR) {
        m_bus.m_chip_select = 1 << constants::KEYBOARD_CHIP_SELECT;
    } else if (m_bus.m_addr_line == constants::DELAY_TIMER_ADDR) {
        m_bus.m_chip_select = 1 << constants::DELAY_TIMER_CHIP_SELECT;
    } else if (m_bus.m_addr_line == constants::SOUND_TIMER_ADDR) {
        m_bus.m_chip_select = 1 << constants::SOUND_TIMER_CHIP_SELECT;
    } else if (m_bus.m_addr_line == constants::PPU_CLEAR_OR_READ_VBLANK_ADDR ||
               m_bus.m_addr_line == constants::PPU_SPRITE_X_ADDR ||
               m_bus.m_addr_line == constants::PPU_SPRITE_Y_ADDR ||
               m_bus.m_addr_line == constants::PPU_SPRITE_ADDR_LO ||
               m_bus.m_addr_line == constants::PPU_SPRITE_ADDR_HI ||
               m_bus.m_addr_line ==
                   constants::PPU_DRAW_SPRITE_OR_READ_TOGGLED_OFF_ADDR) {
        m_bus.m_chip_select = 1 << constants::PPU_CHIP_SELECT;
    } else if (m_bus.m_addr_line == constants::RNG_ADDR) {
        m_bus.m_chip_select = 1 << constants::RNG_CHIP_SELECT;
    } else {
        m_bus.m_chip_select = 1 << constants::MEMORY_CHIP_SELECT;
    }
}
