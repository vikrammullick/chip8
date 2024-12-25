#ifndef CPU_H
#define CPU_H

#include "constants.hpp"
#include "memory.hpp"

class cpu_t {
    memory_t &m_memory;
    uint16_t m_pc = constants::ROM_OFFSET;

    void tick();

  public:
    cpu_t(memory_t &memory);

    void run();
};

#endif // CPU_H
