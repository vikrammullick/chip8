#include "cpu.hpp"
#include "sdl_helpers.hpp"

#include <iostream>
#include <optional>
#include <sstream>

using namespace std;

cpu_t::cpu_t(bus_t &bus) : m_bus(bus) {
    m_fpga_cpu.clk = 0;
    m_fpga_cpu.eval();

    m_fpga_cpu.reset = 1;
    m_fpga_cpu.clk = 1;
    m_fpga_cpu.eval();

    m_fpga_cpu.reset = 0;
}

void cpu_t::add_control_unit_write(const uint16_t &addr, const uint8_t &val) {
    add_handler([this, &addr, &val]() {
        m_bus.m_data_line_out = val;
        m_bus.m_addr_line = addr;
        m_bus.m_rw_select = 1 << constants::WRITE_SELECT;
    });
    constexpr bool NEXT_TICK = true;
    add_handler([this]() { m_bus.m_rw_select = 0; }, NEXT_TICK);
}

void cpu_t::add_control_unit_read(const uint16_t &addr, uint8_t &ret) {
    add_handler([this, &addr]() {
        m_bus.m_addr_line = addr;
        m_bus.m_rw_select = 1 << constants::READ_SELECT;
    });
    constexpr bool NEXT_TICK = true;
    add_handler(
        [this, &ret]() {
            ret = m_bus.m_data_line_in;
            m_bus.m_rw_select = 0;
        },
        NEXT_TICK);
}

void cpu_t::add_handler(std::function<void()> &&handler, bool next_tick) {
    if (m_handlers.empty() || next_tick) {
        m_handlers.push({});
    }
    m_handlers.back().push(std::move(handler));
}

void cpu_t::tick() {

    // TODO: move into compile flag
    bool use_fpga = true;
    if (!use_fpga) {
        if (m_ticks == 0) {
            // verify instruction has fully executed
            assert(m_handlers.empty());

            add_read_opcode();
        }

        if (!m_handlers.empty()) {
            auto &current_handlers = m_handlers.front();
            while (!current_handlers.empty()) {
                current_handlers.front()();
                current_handlers.pop();
            }
            m_handlers.pop();
        }

        if (++m_ticks ==
            (constants::INTERPRETER_CLOCK_RATE / constants::CPU_CLOCK_RATE)) {
            m_ticks = 0;
        }
    } else {
        m_fpga_cpu.clk = 0;
        m_fpga_cpu.eval();

        m_fpga_cpu.clk = 1;
        m_fpga_cpu.data_in = m_bus.m_data_line_in;
        m_fpga_cpu.eval();

        m_bus.m_addr_line = m_fpga_cpu.addr;
        m_bus.m_data_line_out = m_fpga_cpu.data_out;
        m_bus.m_rw_select = m_fpga_cpu.cu_state;

        uint16_t err_opcode = m_fpga_cpu.error;
        if (err_opcode) {
            stringstream oss;
            oss << "unknown opcode: 0x" << std::hex << err_opcode;
            throw std::runtime_error(oss.str());
        }
    }
}

void cpu_t::add_read_opcode() {
    uint8_t &msb = m_bytes[0];
    uint8_t &lsb = m_bytes[1];
    add_control_unit_read(m_PC, msb);
    add_handler([this]() { m_PC++; });
    add_control_unit_read(m_PC, lsb);
    add_handler([this, &msb, &lsb]() {
        m_PC++;
        uint16_t opcode = (msb << 8) | lsb;
        add_opcode(opcode);
    });
}

void cpu_t::add_opcode(uint16_t opcode) {
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
        add_control_unit_write(constants::PPU_CLEAR_OR_READ_VBLANK_ADDR, 0);
        return;
    }

    if (match(0x0, 0x0, 0xE, 0xE)) {
        uint8_t &msb = m_bytes[0];
        uint8_t &lsb = m_bytes[1];
        add_handler([this]() { m_SP--; });
        add_control_unit_read(m_SP, msb);
        add_handler([this]() { m_SP--; });
        add_control_unit_read(m_SP, lsb);
        add_handler([this, &msb, &lsb]() { m_PC = (msb << 8) | lsb; });
        return;
    }

    if (match(0x0, nullopt, nullopt, nullopt)) {
        // This instruction is only used on the old computers on which Chip-8
        // was originally implemented. It is ignored by modern interpreters.
        return;
    }

    if (match(0x1, nullopt, nullopt, nullopt)) {
        add_handler([this, nnn]() { m_PC = nnn; });
        return;
    }

    if (match(0x2, nullopt, nullopt, nullopt)) {
        uint8_t *pc_bytes = reinterpret_cast<uint8_t *>(&m_PC);

        add_control_unit_write(m_SP, pc_bytes[0]);
        add_handler([this]() { m_SP++; });
        add_control_unit_write(m_SP, pc_bytes[1]);
        add_handler([this, nnn]() {
            m_SP++;
            m_PC = nnn;
        });
        return;
    }

    if (match(0x3, nullopt, nullopt, nullopt)) {
        add_handler([this, x, kk]() {
            if (m_Vx[x] == kk) {
                m_PC += 2;
            }
        });
        return;
    }

    if (match(0x4, nullopt, nullopt, nullopt)) {
        add_handler([this, x, kk]() {
            if (m_Vx[x] != kk) {
                m_PC += 2;
            }
        });
        return;
    }

    if (match(0x5, nullopt, nullopt, 0x0)) {
        add_handler([this, x, y]() {
            if (m_Vx[x] == m_Vx[y]) {
                m_PC += 2;
            }
        });
        return;
    }

    if (match(0x6, nullopt, nullopt, nullopt)) {
        add_handler([this, x, kk]() { m_Vx[x] = kk; });
        return;
    }

    if (match(0x7, nullopt, nullopt, nullopt)) {
        add_handler([this, x, kk]() { m_Vx[x] += kk; });
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x0)) {
        add_handler([this, x, y]() {
            m_Vx[x] = m_Vx[y];
            m_Vx[0xF] = 0;
        });
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x1)) {
        add_handler([this, x, y]() {
            m_Vx[x] |= m_Vx[y];
            m_Vx[0xF] = 0;
        });
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x2)) {
        add_handler([this, x, y]() {
            m_Vx[x] &= m_Vx[y];
            m_Vx[0xF] = 0;
        });
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x3)) {
        add_handler([this, x, y]() {
            m_Vx[x] ^= m_Vx[y];
            m_Vx[0xF] = 0;
        });
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x4)) {
        add_handler([this, x, y]() {
            uint16_t sum = m_Vx[x] + m_Vx[y];
            m_Vx[x] = sum & 0xFF;
            m_Vx[0xF] = (sum > 0xFF) ? 1 : 0;
        });
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x5)) {
        add_handler([this, x, y]() {
            uint8_t vf = (m_Vx[x] >= m_Vx[y]) ? 1 : 0;
            m_Vx[x] -= m_Vx[y];
            m_Vx[0xF] = vf;
        });
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x6)) {
        add_handler([this, x, y]() {
            m_Vx[x] = m_Vx[y];
            uint8_t vf = (m_Vx[x] & 0b00000001) ? 1 : 0;
            m_Vx[x] >>= 1;
            m_Vx[0xF] = vf;
        });
        return;
    }

    if (match(0x8, nullopt, nullopt, 0x7)) {
        add_handler([this, x, y]() {
            uint8_t vf = (m_Vx[y] >= m_Vx[x]) ? 1 : 0;
            m_Vx[x] = m_Vx[y] - m_Vx[x];
            m_Vx[0xF] = vf;
        });
        return;
    }

    if (match(0x8, nullopt, nullopt, 0xE)) {
        add_handler([this, x, y]() {
            m_Vx[x] = m_Vx[y];
            uint8_t vf = (m_Vx[x] & 0b10000000) ? 1 : 0;
            m_Vx[x] <<= 1;
            m_Vx[0xF] = vf;
        });
        return;
    }

    if (match(0x9, nullopt, nullopt, 0x0)) {
        add_handler([this, x, y]() {
            if (m_Vx[x] != m_Vx[y]) {
                m_PC += 2;
            }
        });
        return;
    }

    if (match(0xA, nullopt, nullopt, nullopt)) {
        add_handler([this, nnn]() { m_I = nnn; });
        return;
    }

    if (match(0xB, nullopt, nullopt, nullopt)) {
        add_handler([this, nnn]() { m_PC = nnn + m_Vx[0x0]; });
        return;
    }

    if (match(0xC, nullopt, nullopt, nullopt)) {
        uint8_t &rand_byte = m_bytes[0];
        add_control_unit_read(constants::RNG_ADDR, rand_byte);
        add_handler([this, x, kk, &rand_byte]() { m_Vx[x] = kk & rand_byte; });
        return;
    }

    if (match(0xD, nullopt, nullopt, nullopt)) {
        uint8_t &vblank = m_bytes[0];
        uint8_t &n_byte = m_bytes[1];
        uint8_t *i_bytes = reinterpret_cast<uint8_t *>(&m_I);

        // stall while vblank
        add_control_unit_read(constants::PPU_CLEAR_OR_READ_VBLANK_ADDR, vblank);

        add_handler([this, x, y, n, &vblank, &n_byte, i_bytes]() {
            n_byte = n;
            if (vblank) {
                m_PC -= 2;
            } else {
                add_control_unit_write(constants::PPU_SPRITE_X_ADDR, m_Vx[x]);
                add_control_unit_write(constants::PPU_SPRITE_Y_ADDR, m_Vx[y]);
                add_control_unit_write(constants::PPU_SPRITE_ADDR_LO,
                                       i_bytes[0]);
                add_control_unit_write(constants::PPU_SPRITE_ADDR_HI,
                                       i_bytes[1]);
                add_control_unit_write(
                    constants::PPU_DRAW_SPRITE_OR_READ_TOGGLED_OFF_ADDR,
                    n_byte);
                add_control_unit_read(
                    constants::PPU_DRAW_SPRITE_OR_READ_TOGGLED_OFF_ADDR,
                    m_Vx[0xF]);
            }
        });
        return;
    }

    if (match(0xE, nullopt, 0x9, 0xE)) {
        uint8_t &keyboard_lo = m_bytes[0];
        uint8_t &keyboard_hi = m_bytes[1];
        add_control_unit_read(constants::KEYBOARD_ADDR_LO, keyboard_lo);
        add_control_unit_read(constants::KEYBOARD_ADDR_HI, keyboard_hi);
        add_handler([this, x, &keyboard_lo, &keyboard_hi]() {
            uint16_t keyboard_state = (keyboard_hi << 8) | keyboard_lo;
            if (keyboard_state & (0b00000001 << m_Vx[x])) {
                m_PC += 2;
            }
        });
        return;
    }

    if (match(0xE, nullopt, 0xA, 0x1)) {
        uint8_t &keyboard_lo = m_bytes[0];
        uint8_t &keyboard_hi = m_bytes[1];
        add_control_unit_read(constants::KEYBOARD_ADDR_LO, keyboard_lo);
        add_control_unit_read(constants::KEYBOARD_ADDR_HI, keyboard_hi);
        add_handler([this, x, &keyboard_lo, &keyboard_hi]() {
            uint16_t keyboard_state = (keyboard_hi << 8) | keyboard_lo;
            if (!(keyboard_state & (0b00000001 << m_Vx[x]))) {
                m_PC += 2;
            }
        });
        return;
    }

    if (match(0xF, nullopt, 0x0, 0x7)) {
        add_control_unit_read(constants::DELAY_TIMER_ADDR, m_Vx[x]);
        return;
    }

    if (match(0xF, nullopt, 0x0, 0xA)) {
        uint8_t &key_released = m_bytes[0];
        add_control_unit_write(constants::KEYBOARD_WAIT_REL_ADDR, 0);
        add_control_unit_read(constants::KEYBOARD_WAIT_REL_ADDR, key_released);

        add_handler([this, x, &key_released]() {
            if (key_released == constants::NULL_KEY) {
                m_PC -= 2;
            } else {
                m_Vx[x] = key_released;
            }
        });
        return;
    }

    if (match(0xF, nullopt, 0x1, 0x5)) {
        add_control_unit_write(constants::DELAY_TIMER_ADDR, m_Vx[x]);
        return;
    }

    if (match(0xF, nullopt, 0x1, 0x8)) {
        add_control_unit_write(constants::SOUND_TIMER_ADDR, m_Vx[x]);
        return;
    }

    if (match(0xF, nullopt, 0x1, 0xE)) {
        add_handler([this, x]() { m_I += m_Vx[x]; });
        return;
    }

    if (match(0xF, nullopt, 0x2, 0x9)) {
        add_handler([this, x]() {
            m_I = constants::FONTSET_OFFSET + constants::FONTSET_SIZE * m_Vx[x];
        });
        return;
    }

    if (match(0xF, nullopt, 0x3, 0x3)) {
        uint8_t &val = m_bytes[0];
        uint8_t &val_mod_10 = m_bytes[1];

        add_handler([this, x, &val, &val_mod_10]() {
            val = m_Vx[x];
            val_mod_10 = val % 10;
            m_I += 2;
        });
        add_control_unit_write(m_I, val_mod_10);
        add_handler([this, &val, &val_mod_10]() {
            val /= 10;
            val_mod_10 = val % 10;
            m_I--;
        });
        add_control_unit_write(m_I, val_mod_10);
        add_handler([this, &val, &val_mod_10]() {
            val /= 10;
            val_mod_10 = val % 10;
            m_I--;
        });
        add_control_unit_write(m_I, val_mod_10);
        return;
    }

    if (match(0xF, nullopt, 0x5, 0x5)) {
        for (uint8_t offset = 0; offset <= x; ++offset) {
            add_control_unit_write(m_I, m_Vx[offset]);
            add_handler([this]() { m_I++; });
        }
        return;
    }

    if (match(0xF, nullopt, 0x6, 0x5)) {
        for (uint8_t offset = 0; offset <= x; ++offset) {
            add_control_unit_read(m_I, m_Vx[offset]);
            add_handler([this]() { m_I++; });
        }
        return;
    }

    stringstream oss;
    oss << "unknown opcode: 0x" << std::hex << opcode;
    throw std::runtime_error(oss.str());
}
