#ifndef TIMER_H
#define TIMER_H

#include "bus.hpp"

class timer_t {
    bus_t &m_bus;
    uint8_t m_chip_select;

    uint8_t m_counter = 0;
    uint64_t m_ticks = 0;

  public:
    timer_t(bus_t &bus, uint8_t chip_select);
    void tick();
    void service_request();
};

#endif // TIMER_H
