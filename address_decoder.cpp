#include "address_decoder.hpp"
#include "constants.hpp"

address_decoder_t::address_decoder_t(bus_t &bus) : m_bus(bus) {}

void address_decoder_t::select_chip() {
    // TODO: move into compile flag
    bool use_fpga = true;
    if (!use_fpga) {
        uint8_t chip_select_shift;

        switch (m_bus.m_addr_line) {
        case constants::KEYBOARD_ADDR_LO:
        case constants::KEYBOARD_ADDR_HI:
        case constants::KEYBOARD_WAIT_REL_ADDR:
            chip_select_shift = constants::KEYBOARD_CHIP_SELECT;
            break;
        case constants::DELAY_TIMER_ADDR:
            chip_select_shift = constants::DELAY_TIMER_CHIP_SELECT;
            break;
        case constants::SOUND_TIMER_ADDR:
            chip_select_shift = constants::SOUND_TIMER_CHIP_SELECT;
            break;
        case constants::PPU_CLEAR_OR_READ_VBLANK_ADDR:
        case constants::PPU_SPRITE_X_ADDR:
        case constants::PPU_SPRITE_Y_ADDR:
        case constants::PPU_SPRITE_ADDR_LO:
        case constants::PPU_SPRITE_ADDR_HI:
        case constants::PPU_DRAW_SPRITE_OR_READ_TOGGLED_OFF_ADDR:
            chip_select_shift = constants::PPU_CHIP_SELECT;
            break;
        case constants::RNG_ADDR:
            chip_select_shift = constants::RNG_CHIP_SELECT;
            break;
        default:
            chip_select_shift = constants::MEMORY_CHIP_SELECT;
        }

        m_bus.m_chip_select = 1 << chip_select_shift;
    } else {
        m_fpga_address_decoder.addr = m_bus.m_addr_line;
        m_fpga_address_decoder.eval();
        m_bus.m_chip_select = m_fpga_address_decoder.chip_select;
    }
}
