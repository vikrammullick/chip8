#include "cpu.hpp"
#include "sdl_helpers.hpp"

#include <iostream>
#include <optional>
#include <random>
#include <sstream>

using namespace std;

cpu_t::cpu_t(memory_t &memory, ppu_t &ppu) : m_memory(memory), m_ppu(ppu) {}

void cpu_t::run() {
    while (g_running) {
        if (m_DT) {
            --m_DT;
        }

        if (m_ST) {
            // TODO: play sound
            --m_ST;
        }

        // was told to run this at 660 hz
        for (size_t i = 0; i < 11; ++i) {
            tick();
            m_ppu.unset_vblank();
        }

        m_ppu.refresh_screen();
        sdl_poll_keyboard();

        // TODO: do something cleaner than this
        SDL_Delay(16);
    }
}

void cpu_t::tick() {
    uint16_t opcode = m_memory.read_opcode(m_PC);

    uint8_t n = opcode & 0x000F;
    uint8_t x = (opcode >> 8) & 0x000F;
    uint8_t y = (opcode >> 4) & 0x000F;
    uint8_t kk = opcode & 0x00FF;
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
        m_ppu.clear();
        return;
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

    if (match(0x4, nullopt, nullopt, nullopt)) {
        if (m_Vx[x] != kk) {
            m_PC += 2;
        }
        return;
    }

    if (match(0x5, nullopt, nullopt, 0x0)) {
        if (m_Vx[x] == m_Vx[y]) {
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

    if (match(0x8, nullopt, nullopt, 0x0)) {
        m_Vx[x] = m_Vx[y];
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x1)) {
        m_Vx[x] |= m_Vx[y];
        m_Vx[0xF] = 0;
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x2)) {
        m_Vx[x] &= m_Vx[y];
        m_Vx[0xF] = 0;
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x3)) {
        m_Vx[x] ^= m_Vx[y];
        m_Vx[0xF] = 0;
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x4)) {
        uint16_t sum = m_Vx[x] + m_Vx[y];
        m_Vx[x] = sum & 0xFF;
        m_Vx[0xF] = (sum > 0xFF) ? 1 : 0;
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x5)) {
        uint8_t vf = (m_Vx[x] >= m_Vx[y]) ? 1 : 0;
        m_Vx[x] -= m_Vx[y];
        m_Vx[0xF] = vf;
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x6)) {
        m_Vx[x] = m_Vx[y];
        uint8_t vf = (m_Vx[x] & 0b00000001) ? 1 : 0;
        m_Vx[x] >>= 1;
        m_Vx[0xF] = vf;
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x7)) {
        uint8_t vf = (m_Vx[y] >= m_Vx[x]) ? 1 : 0;
        m_Vx[x] = m_Vx[y] - m_Vx[x];
        m_Vx[0xF] = vf;
        return;
    }

    if (match(0x8, nullopt, nullopt, 0xE)) {
        m_Vx[x] = m_Vx[y];
        uint8_t vf = (m_Vx[x] & 0b10000000) ? 1 : 0;
        m_Vx[x] <<= 1;
        m_Vx[0xF] = vf;
        return;
    }

    if (match(0x9, nullopt, nullopt, 0x0)) {
        if (m_Vx[x] != m_Vx[y]) {
            m_PC += 2;
        }
        return;
    }

    if (match(0xA, nullopt, nullopt, nullopt)) {
        m_I = nnn;
        return;
    }

    if (match(0xB, nullopt, nullopt, nullopt)) {
        m_PC = nnn + m_Vx[0x0];
        return;
    }

    if (match(0xC, nullopt, nullopt, nullopt)) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<uint16_t> dis(0, 255);
        uint8_t rand_byte = static_cast<uint8_t>(dis(gen));

        m_Vx[x] = kk & rand_byte;
        return;
    }

    if (match(0xD, nullopt, nullopt, nullopt)) {
        // stall until vblank
        if (!m_ppu.vblank()) {
            m_PC -= 2;
            return;
        }

        m_Vx[0xF] = 0;

        bool should_wrap_vertical = m_Vx[y] >= constants::SCREEN_HEIGHT;
        bool should_wrap_horizontal = m_Vx[x] >= constants::SCREEN_WIDTH;
        for (uint8_t row = 0; row < n; ++row) {
            uint8_t screen_y = m_Vx[y] + row;
            if (screen_y >= constants::SCREEN_HEIGHT) {
                if (should_wrap_vertical) {
                    screen_y %= constants::SCREEN_HEIGHT;
                } else {
                    continue;
                }
            }

            uint8_t row_sprite = m_memory.read(m_I + row);
            for (uint8_t col = 0; col < 8; ++col) {
                uint8_t screen_x = m_Vx[x] + col;
                if (screen_x >= constants::SCREEN_WIDTH) {
                    if (should_wrap_horizontal) {
                        screen_x %= constants::SCREEN_WIDTH;
                    } else {
                        continue;
                    }
                }

                bool toggle_pixel = row_sprite & (0b10000000 >> col);
                if (toggle_pixel) {
                    if (m_ppu.toggle_pixel(screen_y, screen_x)) {
                        m_Vx[0xF] = 1;
                    }
                }
            }
        }
        return;
    }

    if (match(0xE, nullopt, 0x9, 0xE)) {
        if (g_keyboard & (0b00000001 << m_Vx[x])) {
            m_PC += 2;
        }
        return;
    }

    if (match(0xE, nullopt, 0xA, 0x1)) {
        if (!(g_keyboard & (0b00000001 << m_Vx[x]))) {
            m_PC += 2;
        }
        return;
    }

    if (match(0xF, nullopt, 0x0, 0x7)) {
        m_Vx[x] = m_DT;
        return;
    }

    if (match(0xF, nullopt, 0x0, 0xA)) {
        static bool waiting_key_release = false;

        if (waiting_key_release) {
            if (g_keyboard & (0b1 << m_Vx[x])) {
                m_PC -= 2;
            } else {
                waiting_key_release = false;
            }
            return;
        }

        uint16_t keyboard_state = g_keyboard;
        uint8_t key_pressed = 0;
        while (keyboard_state) {
            if (keyboard_state & 0b1) {
                m_Vx[x] = key_pressed;
                m_PC -= 2;
                waiting_key_release = true;
                return;
            }

            keyboard_state = keyboard_state >> 1;
            key_pressed++;
        }

        m_PC -= 2;
        return;
    }

    if (match(0xF, nullopt, 0x1, 0x5)) {
        m_DT = m_Vx[x];
        return;
    }

    if (match(0xF, nullopt, 0x1, 0x8)) {
        m_ST = m_Vx[x];
        return;
    }

    if (match(0xF, nullopt, 0x1, 0xE)) {
        m_I += m_Vx[x];
        return;
    }

    if (match(0xF, nullopt, 0x2, 0x9)) {
        m_I = constants::FONTSET_OFFSET + constants::FONTSET_SIZE * m_Vx[x];
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

    if (match(0xF, nullopt, 0x5, 0x5)) {
        for (uint8_t offset = 0; offset <= x; ++offset) {
            m_memory.write(m_I++, m_Vx[offset]);
        }
        return;
    }

    if (match(0xF, nullopt, 0x6, 0x5)) {
        for (uint8_t offset = 0; offset <= x; ++offset) {
            m_Vx[offset] = m_memory.read(m_I++);
        }
        return;
    }

    stringstream oss;
    oss << "unknown opcode: 0x" << std::hex << opcode;
    throw std::runtime_error(oss.str());
}
