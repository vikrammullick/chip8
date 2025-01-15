#ifndef BUS_H
#define BUS_H

#include <cstdint>

struct bus_t {
    uint16_t m_addr_line;

    uint8_t m_data_line;

    uint8_t m_rw_select = 0;

    uint8_t m_chip_select = 0;
};

#endif // BUS_H
