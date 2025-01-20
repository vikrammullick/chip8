#include "cpu.hpp"
#include "sdl_helpers.hpp"

#include <iostream>
#include <optional>
#include <sstream>

using namespace std;

cpu_t::cpu_t(memory_t &memory,
             ppu_t &ppu,
             keyboard_t &keyboard,
             timer_t &delay_timer,
             timer_t &sound_timer,
             rng_t &rng,
             bus_t &bus,
             address_decoder_t &address_decoder)
    : m_control_unit(memory,
                     ppu,
                     keyboard,
                     delay_timer,
                     sound_timer,
                     rng,
                     bus,
                     address_decoder) {}

cpu_t::control_unit_t::control_unit_t(memory_t &memory,
                                      ppu_t &ppu,
                                      keyboard_t &keyboard,
                                      timer_t &delay_timer,
                                      timer_t &sound_timer,
                                      rng_t &rng,
                                      bus_t &bus,
                                      address_decoder_t &address_decoder)
    : m_memory(memory),
      m_ppu(ppu),
      m_keyboard(keyboard),
      m_delay_timer(delay_timer),
      m_sound_timer(sound_timer),
      m_rng(rng),
      m_bus(bus),
      m_address_decoder(address_decoder) {}

void cpu_t::control_unit_t::write(uint16_t addr, uint8_t val) {
    m_bus.m_data_line = val;
    m_bus.m_addr_line = addr;
    m_bus.m_rw_select = 1 << constants::WRITE_SELECT;
    // TODO: move to combinational logic once cpu implemented at cycle level
    m_address_decoder.select_chip();
    m_memory.service_request();
    m_keyboard.service_request();
    m_delay_timer.service_request();
    m_sound_timer.service_request();
    m_rng.service_request();
    m_ppu.service_request();
    m_bus.m_rw_select = 0;
}

uint8_t cpu_t::control_unit_t::read(uint16_t addr) {
    m_bus.m_addr_line = addr;
    m_bus.m_rw_select = 1 << constants::READ_SELECT;
    // TODO: move to combinational logic once cpu implemented at cycle level
    m_address_decoder.select_chip();
    m_memory.service_request();
    m_keyboard.service_request();
    m_delay_timer.service_request();
    m_sound_timer.service_request();
    m_rng.service_request();
    m_ppu.service_request();
    uint8_t data = m_bus.m_data_line;
    m_bus.m_rw_select = 0;
    return data;
}

void cpu_t::tick() {
    if (m_ticks == 0) {
        process_next_opcode();
    }

    if (++m_ticks ==
        (constants::INTERPRETER_CLOCK_RATE / constants::CPU_CLOCK_RATE)) {
        m_ticks = 0;
    }
}

uint16_t cpu_t::read_opcode() {
    uint8_t msb = m_control_unit.read(m_PC++);
    uint8_t lsb = m_control_unit.read(m_PC++);

    return (msb << 8) | lsb;
}

void cpu_t::process_next_opcode() {
    uint16_t opcode = read_opcode();

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
        m_control_unit.write(constants::PPU_CLEAR_OR_READ_VBLANK_ADDR, 0);
        return;
    }

    if (match(0x0, 0x0, 0xE, 0xE)) {
        m_PC = m_control_unit.read(--m_SP);
        m_PC = m_PC << 8;
        m_PC |= m_control_unit.read(--m_SP);
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
        m_control_unit.write(m_SP++, m_PC & 0xFF);
        m_control_unit.write(m_SP++, m_PC >> 8);
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
        uint8_t rand_byte = m_control_unit.read(constants::RNG_ADDR);
        m_Vx[x] = kk & rand_byte;
        return;
    }

    if (match(0xD, nullopt, nullopt, nullopt)) {
        // stall while vblank
        if (m_control_unit.read(constants::PPU_CLEAR_OR_READ_VBLANK_ADDR)) {
            m_PC -= 2;
            return;
        }

        m_control_unit.write(constants::PPU_SPRITE_X_ADDR, m_Vx[x]);
        m_control_unit.write(constants::PPU_SPRITE_Y_ADDR, m_Vx[y]);
        m_control_unit.write(constants::PPU_SPRITE_ADDR_LO, m_I & 0xFF);
        m_control_unit.write(constants::PPU_SPRITE_ADDR_HI, m_I >> 8);
        m_control_unit.write(
            constants::PPU_DRAW_SPRITE_OR_READ_TOGGLED_OFF_ADDR, n);
        m_Vx[0xF] = m_control_unit.read(
            constants::PPU_DRAW_SPRITE_OR_READ_TOGGLED_OFF_ADDR);
        return;
    }

    if (match(0xE, nullopt, 0x9, 0xE)) {
        uint8_t keyboard_lo = m_control_unit.read(constants::KEYBOARD_ADDR_LO);
        uint8_t keyboard_hi = m_control_unit.read(constants::KEYBOARD_ADDR_HI);
        uint16_t keyboard_state = (keyboard_hi << 8) | keyboard_lo;
        if (keyboard_state & (0b00000001 << m_Vx[x])) {
            m_PC += 2;
        }
        return;
    }

    if (match(0xE, nullopt, 0xA, 0x1)) {
        uint8_t keyboard_lo = m_control_unit.read(constants::KEYBOARD_ADDR_LO);
        uint8_t keyboard_hi = m_control_unit.read(constants::KEYBOARD_ADDR_HI);
        uint16_t keyboard_state = (keyboard_hi << 8) | keyboard_lo;
        if (!(keyboard_state & (0b00000001 << m_Vx[x]))) {
            m_PC += 2;
        }
        return;
    }

    if (match(0xF, nullopt, 0x0, 0x7)) {
        m_Vx[x] = m_control_unit.read(constants::DELAY_TIMER_ADDR);
        return;
    }

    if (match(0xF, nullopt, 0x0, 0xA)) {
        m_control_unit.write(constants::KEYBOARD_WAIT_REL_ADDR, 0);
        uint8_t key_released =
            m_control_unit.read(constants::KEYBOARD_WAIT_REL_ADDR);

        if (key_released == constants::NULL_KEY) {
            m_PC -= 2;
        } else {
            m_Vx[x] = key_released;
        }
        return;
    }

    if (match(0xF, nullopt, 0x1, 0x5)) {
        m_control_unit.write(constants::DELAY_TIMER_ADDR, m_Vx[x]);
        return;
    }

    if (match(0xF, nullopt, 0x1, 0x8)) {
        m_control_unit.write(constants::SOUND_TIMER_ADDR, m_Vx[x]);
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
        m_control_unit.write(m_I + 2, val % 10);
        val /= 10;
        m_control_unit.write(m_I + 1, val % 10);
        val /= 10;
        m_control_unit.write(m_I, val % 10);
        return;
    }

    if (match(0xF, nullopt, 0x5, 0x5)) {
        for (uint8_t offset = 0; offset <= x; ++offset) {
            m_control_unit.write(m_I++, m_Vx[offset]);
        }
        return;
    }

    if (match(0xF, nullopt, 0x6, 0x5)) {
        for (uint8_t offset = 0; offset <= x; ++offset) {
            m_Vx[offset] = m_control_unit.read(m_I++);
        }
        return;
    }

    stringstream oss;
    oss << "unknown opcode: 0x" << std::hex << opcode;
    throw std::runtime_error(oss.str());
}
