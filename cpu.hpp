#ifndef CPU_H
#define CPU_H

#include "address_decoder.hpp"
#include "constants.hpp"
#include "keyboard.hpp"
#include "memory.hpp"
#include "ppu.hpp"
#include "timer.hpp"

class cpu_t {
    class control_unit_t {
        memory_t &m_memory;
        ppu_t &m_ppu;
        keyboard_t &m_keyboard;
        timer_t &m_delay_timer;
        timer_t &m_sound_timer;
        bus_t &m_bus;
        address_decoder_t &m_address_decoder;

      public:
        control_unit_t(memory_t &memory,
                       ppu_t &ppu,
                       keyboard_t &m_keyboard,
                       timer_t &delay_timer,
                       timer_t &sound_timer,
                       bus_t &bus,
                       address_decoder_t &address_decoder);
        void write(uint16_t addr, uint8_t val);
        uint8_t read(uint16_t addr);
    };

    control_unit_t m_control_unit;

    std::array<uint8_t, constants::NUM_REGS> m_Vx;
    uint16_t m_I;
    uint16_t m_PC = constants::ROM_OFFSET;
    uint16_t m_SP =
        constants::MEM_SIZE - sizeof(uint16_t) * constants::STACK_SIZE;

    uint64_t m_ticks = 0;

    uint16_t read_opcode();
    uint16_t read_keyboard();
    void push_pc_to_stack();
    void pop_pc_from_stack();

    void process_next_opcode();

  public:
    cpu_t(memory_t &memory,
          ppu_t &ppu,
          keyboard_t &keyboard,
          timer_t &delay_timer,
          timer_t &sound_timer,
          bus_t &bus,
          address_decoder_t &address_decoder);

    void tick();
};

#endif // CPU_H
