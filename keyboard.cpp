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
        } else if (m_bus.m_addr_line == constants::KEYBOARD_ADDR_HI) {
            m_bus.m_data_line = g_keyboard >> 8;
        } else {
            assert(m_bus.m_addr_line == constants::KEYBOARD_WAIT_REL_ADDR);

            if (!m_waiting_key_release && m_active_key_opt) {
                m_bus.m_data_line = *m_active_key_opt;
                m_active_key_opt.reset();
            } else {
                m_bus.m_data_line = constants::NULL_KEY;
            }
        }
    }

    if (m_bus.m_rw_select & (1 << constants::WRITE_SELECT)) {
        assert(m_bus.m_addr_line == constants::KEYBOARD_WAIT_REL_ADDR);

        uint16_t keyboard_state = g_keyboard;
        if (m_waiting_key_release) {
            if (!(keyboard_state & (0b1 << *m_active_key_opt))) {
                m_waiting_key_release = false;
            }
        } else {
            uint8_t key_pressed = 0;
            while (keyboard_state) {
                if (keyboard_state & 0b1) {
                    m_active_key_opt = key_pressed;
                    m_waiting_key_release = true;
                    break;
                }

                keyboard_state = keyboard_state >> 1;
                key_pressed++;
            }
        }
    }
}
