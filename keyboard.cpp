#include "keyboard.hpp"
#include "constants.hpp"
#include "sdl_helpers.hpp"

keyboard_t::keyboard_t(bus_t &bus) : m_bus(bus) {}

void keyboard_t::service_request() {
    if (!(m_bus.m_chip_select & (1 << constants::KEYBOARD_CHIP_SELECT))) {
        return;
    }

    if (m_bus.m_rw_select & (1 << constants::READ_SELECT)) {
        if (m_bus.m_addr_line == constants::KEYBOARD_ADDR_LO) {
            m_bus.m_data_line = g_keyboard & 0xFF;
        } else {
            assert(m_bus.m_addr_line == constants::KEYBOARD_ADDR_HI);
            m_bus.m_data_line = g_keyboard >> 8;
        }
    }

    if (m_bus.m_rw_select & (1 << constants::WRITE_SELECT)) {
        // should not write to keyboard
        assert(false);
    }
}
