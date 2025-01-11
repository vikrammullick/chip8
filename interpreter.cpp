#include "interpreter.hpp"
#include "sdl_helpers.hpp"

using namespace std;

interpreter_t::interpreter_t(const vector<char> &rom_bytes)
    : m_memory(rom_bytes, m_bus), m_cpu(m_memory, m_ppu, m_bus) {
    sdl_init_window();
}

interpreter_t::~interpreter_t() { sdl_destroy_window(); }

void interpreter_t::run() { m_cpu.run(); }
