#ifndef CPU_H
#define CPU_H

#include "constants.hpp"
#include "memory.hpp"
#include "ppu.hpp"

class cpu_t {
    memory_t &m_memory;
    ppu_t &m_ppu;

    std::array<uint8_t, constants::NUM_REGS> m_Vx;
    uint16_t m_I;
    uint16_t m_PC = constants::ROM_OFFSET;
    uint16_t m_SP =
        constants::MEM_SIZE - sizeof(uint16_t) * constants::STACK_SIZE;

    void tick();

  public:
    cpu_t(memory_t &memory, ppu_t &ppu);

    void run();
};

#endif // CPU_H
