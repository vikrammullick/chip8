#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "bus.hpp"

#include <optional>

class keyboard_t {
    bus_t &m_bus;

    bool m_waiting_key_release = false;
    std::optional<uint8_t> m_active_key_opt;

  public:
    keyboard_t(bus_t &bus);
    void service_request();
};

#endif // KEYBOARD_H
