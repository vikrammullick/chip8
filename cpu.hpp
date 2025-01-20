#ifndef CPU_H
#define CPU_H

#include "address_decoder.hpp"
#include "constants.hpp"
#include "keyboard.hpp"
#include "memory.hpp"
#include "ppu.hpp"
#include "rng.hpp"
#include "timer.hpp"

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

    void control_unit_write(uint16_t addr, uint8_t val);
    uint8_t control_unit_read(uint16_t addr);

    uint16_t read_opcode();

    void process_next_opcode();

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
