#ifndef BUS_H
#define BUS_H

#include <cstdint>

struct bus_t {
    uint16_t m_addr_line;

    uint8_t m_data_line;

    bool m_read_enabled = false;
    bool m_write_enabled = false;

    uint8_t m_chip_select = 0;
};

#endif // BUS_H
