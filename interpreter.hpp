#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "constants.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "ppu.hpp"

class interpreter_t {
    memory_t m_memory;
    ppu_t m_ppu;
    cpu_t m_cpu;

  public:
    interpreter_t(const std::vector<char> &rom_bytes);
    ~interpreter_t();

    void run();
};

#endif // INTERPRETER_H
