#ifndef ADDRESS_DECODER_H
#define ADDRESS_DECODER_H

#include "bus.hpp"

#include "Vaddress_decoder.h"
#include <verilated.h>

class address_decoder_t {
    bus_t &m_bus;

    Vaddress_decoder m_fpga_address_decoder;

  public:
    address_decoder_t(bus_t &bus);
    void select_chip();
};

#endif // ADDRESS_DECODER_H
