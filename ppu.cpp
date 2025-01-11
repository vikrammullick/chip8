#include "ppu.hpp"

using namespace std;

bool ppu_t::toggle_pixel(uint8_t screen_y, uint8_t screen_x) {
    uint16_t idx = screen_y * constants::SCREEN_WIDTH + screen_x;
    uint8_t &pixel = m_data[idx];
    pixel = 1 - pixel;
    return !pixel;
}

void ppu_t::clear() {
    for (auto &pixel : m_data) {
        pixel = 0;
    }
}

// TODO: migrate to pixel buffer
void ppu_t::refresh_screen() {
    for (uint8_t i = 0; i < constants::SCREEN_HEIGHT; ++i)
        for (uint8_t j = 0; j < constants::SCREEN_WIDTH; ++j)
            sdl_draw_pixel(i, j, m_data[i * constants::SCREEN_WIDTH + j]);
    m_vblank = true;
    sdl_refresh_screen();
}
