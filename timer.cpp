#include "timer.hpp"
#include "constants.hpp"

timer_t::timer_t(bus_t &bus, uint8_t chip_select)
    : m_bus(bus), m_chip_select(chip_select) {}

void timer_t::tick() {
    if (m_ticks == 0) {
        if (m_counter) {
            --m_counter;
        }
    }

    if (++m_ticks ==
        (constants::INTERPRETER_CLOCK_RATE / constants::DT_ST_CLOCK_RATE)) {
        m_ticks = 0;
    }
}

void timer_t::service_request() {
    if (!(m_bus.m_chip_select & (1 << m_chip_select))) {
        return;
    }

    if (m_bus.m_rw_select & (1 << constants::READ_SELECT)) {
        m_bus.m_data_line_in = m_counter;
    }

    if (m_bus.m_rw_select & (1 << constants::WRITE_SELECT)) {
        m_counter = m_bus.m_data_line_out;
    }
}
