#ifndef RNG_H
#define RNG_H

#include "bus.hpp"

class rng_t {
    bus_t &m_bus;

  public:
    rng_t(bus_t &bus);
    void service_request();
};

#endif // RNG_H
