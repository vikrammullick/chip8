#include "interpreter.hpp"

using namespace std;

interpreter_t::interpreter_t(const vector<char> &rom_bytes)
    : m_memory(rom_bytes), m_cpu(m_memory) {}

void interpreter_t::run() { m_cpu.run(); }
