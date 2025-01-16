#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "bus.hpp"
#include "constants.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "ppu.hpp"
#include "timer.hpp"

class interpreter_t {
    bus_t m_bus;
    address_decoder_t m_address_decoder;
    memory_t m_memory;
    ppu_t m_ppu;
    timer_t m_delay_timer;
    timer_t m_sound_timer; // TODO actually support sound
    cpu_t m_cpu;

  public:
    interpreter_t(const std::vector<char> &rom_bytes);
    ~interpreter_t();

    void run();
};

#endif // INTERPRETER_H
