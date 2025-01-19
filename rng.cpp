#include "rng.hpp"
#include "constants.hpp"

#include <random>

rng_t::rng_t(bus_t &bus) : m_bus(bus) {}

void rng_t::service_request() {
    if (!(m_bus.m_chip_select & (1 << constants::RNG_CHIP_SELECT))) {
        return;
    }

    if (m_bus.m_rw_select & (1 << constants::READ_SELECT)) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<uint16_t> dis(0, 255);
        uint8_t rand_byte = static_cast<uint8_t>(dis(gen));
        m_bus.m_data_line = rand_byte;
    }

    if (m_bus.m_rw_select & (1 << constants::WRITE_SELECT)) {
        // should not write to rng
        assert(false);
    }
}
