#include "cpu.hpp"

#include <iostream>
#include <optional>
#include <sstream>

using namespace std;

cpu_t::cpu_t(memory_t &memory) : m_memory(memory) {}

void cpu_t::run() {
    while (1) {
        tick();
    }
}

void cpu_t::tick() {
    uint16_t opcode = m_memory.read_opcode(m_PC);

    uint8_t n = opcode & 0xF;
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t y = (opcode >> 4) & 0xF;
    uint8_t kk = opcode & 0xFF;
    uint16_t nnn = opcode & 0x0FFF;

    auto match = [opcode](uint8_t a,
                          optional<uint8_t> b,
                          optional<uint8_t> c,
                          optional<uint8_t> d) {
        if (a != (opcode >> 12)) {
            return false;
        }

        if (b && *b != ((opcode >> 8) & 0xF)) {
            return false;
        }

        if (c && *c != ((opcode >> 4) & 0xF)) {
            return false;
        }

        if (d && *d != (opcode & 0xF)) {
            return false;
        }

        return true;
    };

    if (match(0x0, 0x0, 0xE, 0x0)) {
        throw std::runtime_error("00E0");
    }

    if (match(0x0, 0x0, 0xE, 0xE)) {
        m_PC = m_memory.pop_stack(m_SP);
        return;
    }

    if (match(0x0, nullopt, nullopt, nullopt)) {
        // This instruction is only used on the old computers on which Chip-8
        // was originally implemented. It is ignored by modern interpreters.
        return;
    }

    if (match(0x1, nullopt, nullopt, nullopt)) {
        m_PC = nnn;
        return;
    }

    if (match(0x2, nullopt, nullopt, nullopt)) {
        m_memory.push_stack(m_SP, m_PC);
        m_PC = nnn;
        return;
    }

    if (match(0x3, nullopt, nullopt, nullopt)) {
        if (m_Vx[x] == kk) {
            m_PC += 2;
        }
        return;
    }

    if (match(0x6, nullopt, nullopt, nullopt)) {
        m_Vx[x] = kk;
        return;
    }

    if (match(0x7, nullopt, nullopt, nullopt)) {
        m_Vx[x] += kk;
        return;
    }

    if (match(0xA, nullopt, nullopt, nullopt)) {
        m_I = nnn;
        return;
    }

    if (match(0xD, nullopt, nullopt, nullopt)) {
        // TODO: actual drawing
        return;
    }

    if (match(0xF, nullopt, 0x2, 0x9)) {
        m_I = constants::FONTSET_OFFSET + constants::FONTSET_SIZE * x;
        return;
    }

    if (match(0xF, nullopt, 0x3, 0x3)) {
        uint16_t val = m_Vx[x];
        m_memory.write(m_I + 2, val % 10);
        val /= 10;
        m_memory.write(m_I + 1, val % 10);
        val /= 10;
        m_memory.write(m_I, val % 10);
        return;
    }

    if (match(0xF, nullopt, 0x6, 0x5)) {
        for (uint8_t offset = 0; offset <= x; ++offset) {
            m_Vx[x] = m_memory.read(m_I + offset);
        }
        return;
    }

    stringstream oss;
    oss << "unknown opcode: 0x" << std::hex << opcode;
    throw std::runtime_error(oss.str());
}
