#ifndef CPU_H
#define CPU_H

#include "address_decoder.hpp"
#include "constants.hpp"
#include "keyboard.hpp"
#include "memory.hpp"
#include "ppu.hpp"
#include "rng.hpp"
#include "timer.hpp"

#include <queue>

class cpu_t {
    bus_t &m_bus;

    std::array<uint8_t, constants::NUM_REGS> m_Vx;
    uint16_t m_I;
    uint16_t m_PC = constants::ROM_OFFSET;
    uint16_t m_SP =
        constants::MEM_SIZE - sizeof(uint16_t) * constants::STACK_SIZE;

    uint64_t m_ticks = 0;

    std::queue<std::queue<std::function<void()>>> m_handlers;
    void add_handler(std::function<void()> &&handler, bool next_tick = false);

    // inter-cycle history
    uint8_t m_bytes[2];

    void add_control_unit_write(const uint16_t &addr, const uint8_t &val);
    void add_control_unit_read(const uint16_t &addr, uint8_t &ret);

    void add_read_opcode();
    void add_opcode(uint16_t opcode);

  public:
    cpu_t(bus_t &bus);

    void tick();
};

#endif // CPU_H
