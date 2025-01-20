#include "ppu.hpp"

ppu_t::ppu_t(bus_t &bus, memory_t &memory) : m_bus(bus), m_memory(memory) {
    m_data = new uint8_t[NUM_PIXELS];
    clear();
}

ppu_t::~ppu_t() { delete[] m_data; }

void ppu_t::toggle_pixel(uint8_t screen_y, uint8_t screen_x) {
    uint16_t idx = screen_y * constants::SCREEN_WIDTH + screen_x;
    uint8_t &pixel = m_data[idx];
    pixel = 1 - pixel;
    m_any_pixel_toggled_off |= !pixel;
}

void ppu_t::clear() {
    for (size_t i = 0; i < NUM_PIXELS; ++i) {
        m_data[i] = 0;
    }
}

void ppu_t::tick() {
    if (m_ticks == 0) {
        m_vblank = false;
        refresh_screen();
    }

    if (m_ticks ==
        constants::INTERPRETER_CLOCK_RATE / constants::CPU_CLOCK_RATE) {
        m_vblank = true;
    }

    if (++m_ticks ==
        (constants::INTERPRETER_CLOCK_RATE / constants::PPU_CLOCK_RATE)) {
        m_ticks = 0;
    }
}

void ppu_t::draw_sprite(uint8_t num_rows) {
    m_any_pixel_toggled_off = false;

    bool should_wrap_vertical = m_sprite_y >= constants::SCREEN_HEIGHT;
    bool should_wrap_horizontal = m_sprite_x >= constants::SCREEN_WIDTH;
    for (uint8_t row = 0; row < num_rows; ++row) {
        uint8_t screen_y = m_sprite_y + row;
        if (screen_y >= constants::SCREEN_HEIGHT) {
            if (should_wrap_vertical) {
                screen_y %= constants::SCREEN_HEIGHT;
            } else {
                continue;
            }
        }

        // NOTE: not exactly faithful, but for simplicity, don't do ppu dma read
        // over normal bus
        uint8_t row_sprite = m_memory.ppu_dma_read(m_sprite_addr + row);

        for (uint8_t col = 0; col < 8; ++col) {
            uint8_t screen_x = m_sprite_x + col;
            if (screen_x >= constants::SCREEN_WIDTH) {
                if (should_wrap_horizontal) {
                    screen_x %= constants::SCREEN_WIDTH;
                } else {
                    continue;
                }
            }

            bool should_toggle_pixel = row_sprite & (0b10000000 >> col);
            if (should_toggle_pixel) {
                toggle_pixel(screen_y, screen_x);
            }
        }
    }
}

void ppu_t::service_request() {
    if (!(m_bus.m_chip_select & (1 << constants::PPU_CHIP_SELECT))) {
        return;
    }

    if (m_bus.m_rw_select & (1 << constants::READ_SELECT)) {
        if (m_bus.m_addr_line == constants::PPU_CLEAR_OR_READ_VBLANK_ADDR) {
            m_bus.m_data_line = m_vblank ? 0xFF : 0x00;
        } else if (m_bus.m_addr_line ==
                   constants::PPU_DRAW_SPRITE_OR_READ_TOGGLED_OFF_ADDR) {
            m_bus.m_data_line = m_any_pixel_toggled_off ? 1 : 0;
        } else {
            assert(false);
        }
    }

    if (m_bus.m_rw_select & (1 << constants::WRITE_SELECT)) {
        if (m_bus.m_addr_line == constants::PPU_CLEAR_OR_READ_VBLANK_ADDR) {
            clear();
        } else if (m_bus.m_addr_line == constants::PPU_SPRITE_X_ADDR) {
            m_sprite_x = m_bus.m_data_line;
        } else if (m_bus.m_addr_line == constants::PPU_SPRITE_Y_ADDR) {
            m_sprite_y = m_bus.m_data_line;
        } else if (m_bus.m_addr_line == constants::PPU_SPRITE_ADDR_LO) {
            m_sprite_addr &= 0xFF00;
            m_sprite_addr |= m_bus.m_data_line;
        } else if (m_bus.m_addr_line == constants::PPU_SPRITE_ADDR_HI) {
            m_sprite_addr &= 0x00FF;
            m_sprite_addr |= m_bus.m_data_line << 8;
        } else if (m_bus.m_addr_line ==
                   constants::PPU_DRAW_SPRITE_OR_READ_TOGGLED_OFF_ADDR) {
            draw_sprite(m_bus.m_data_line);
        } else {
            assert(false);
        }
    }
}

void ppu_t::refresh_screen() {
    for (uint8_t i = 0; i < constants::SCREEN_HEIGHT; ++i) {
        for (uint8_t j = 0; j < constants::SCREEN_WIDTH; ++j) {
            sdl_draw_pixel(i, j, m_data[i * constants::SCREEN_WIDTH + j]);
        }
    }
    sdl_refresh_screen();
}
