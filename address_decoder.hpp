#ifndef ADDRESS_DECODER_H
#define ADDRESS_DECODER_H

#include "bus.hpp"

class address_decoder_t {
    bus_t &m_bus;

  public:
    address_decoder_t(bus_t &bus);
    void select_chip();
};

#endif // ADDRESS_DECODER_H
