#ifndef CPU_H
#define CPU_H

#include "constants.hpp"
#include "memory.hpp"

class cpu_t {
    memory_t &m_memory;
    std::array<uint8_t, constants::NUM_REGS> m_Vx;
    uint16_t m_I;
    uint16_t m_PC = constants::ROM_OFFSET;

    void tick();

  public:
    cpu_t(memory_t &memory);

    void run();
};

#endif // CPU_H
