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
    memory_t &m_memory;
    ppu_t &m_ppu;
    keyboard_t &m_keyboard;
    timer_t &m_delay_timer;
    timer_t &m_sound_timer;
    rng_t &m_rng;
    bus_t &m_bus;
    address_decoder_t &m_address_decoder;

    std::array<uint8_t, constants::NUM_REGS> m_Vx;
    uint16_t m_I;
    uint16_t m_PC = constants::ROM_OFFSET;
    uint16_t m_SP =
        constants::MEM_SIZE - sizeof(uint16_t) * constants::STACK_SIZE;

    uint64_t m_ticks = 0;

    std::queue<std::function<void()>> m_handlers;
    void add_handler(std::function<void()> &&handler);

    // inter-cycle history
    uint8_t m_bytes[2];

    void add_control_unit_write(const uint16_t &addr, const uint8_t &val);
    void add_control_unit_read(const uint16_t &addr, uint8_t &ret);

    void add_read_opcode();
    void add_opcode(uint16_t opcode);

  public:
    cpu_t(memory_t &memory,
          ppu_t &ppu,
          keyboard_t &keyboard,
          timer_t &delay_timer,
          timer_t &sound_timer,
          rng_t &rng,
          bus_t &bus,
          address_decoder_t &address_decoder);

    void tick();
};

#endif // CPU_H
