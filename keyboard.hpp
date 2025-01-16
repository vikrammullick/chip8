#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "bus.hpp"

class keyboard_t {
    bus_t &m_bus;

  public:
    keyboard_t(bus_t &bus);
    void service_request();
};

#endif // KEYBOARD_H
