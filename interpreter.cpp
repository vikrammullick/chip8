#include "interpreter.hpp"

using namespace std;

interpreter_t::interpreter_t(const vector<char> &rom_bytes)
    : m_memory(rom_bytes) {}

void interpreter_t::run() {}
