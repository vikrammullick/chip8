#include "ppu.hpp"

ppu_t::ppu_t() {
    m_data = new uint8_t[NUM_PIXELS];
    clear();
}

ppu_t::~ppu_t() { delete[] m_data; }

bool ppu_t::toggle_pixel(uint8_t screen_y, uint8_t screen_x) {
    uint16_t idx = screen_y * constants::SCREEN_WIDTH + screen_x;
    uint8_t &pixel = m_data[idx];
    bool pixel_was_on = pixel;
    pixel = 1 - pixel;
    return pixel_was_on;
}

void ppu_t::clear() {
    for (size_t i = 0; i < NUM_PIXELS; ++i) {
        m_data[i] = 0;
    }
}

void ppu_t::refresh_screen() {
    for (uint8_t i = 0; i < constants::SCREEN_HEIGHT; ++i) {
        for (uint8_t j = 0; j < constants::SCREEN_WIDTH; ++j) {
            sdl_draw_pixel(i, j, m_data[i * constants::SCREEN_WIDTH + j]);
        }
    }
    m_vblank = true;
    sdl_refresh_screen();
}
