#ifndef PPU_H
#define PPU_H

#include "constants.hpp"
#include "sdl_helpers.hpp"

#include <iostream>
#include <vector>

class ppu_t {
    std::vector<uint8_t> m_data;
    bool m_vblank;

  public:
    // TODO: clean
    ppu_t() {
        for (auto i = 0; i < 2048; ++i) {
            m_data.push_back(0);
        }
    }

    bool toggle(uint8_t screen_y, uint8_t screen_x);
    void clear();

    // TODO: migrate to pixel buffer
    void refresh_screen() {
        for (uint8_t i = 0; i < constants::SCREEN_HEIGHT; ++i)
            for (uint8_t j = 0; j < constants::SCREEN_WIDTH; ++j)
                sdl_draw_pixel(i, j, m_data[i * constants::SCREEN_WIDTH + j]);
        m_vblank = true;
        sdl_refresh_screen();
    }

    void unset_vblank() { m_vblank = false; }

    bool vblank() const { return m_vblank; }
};

#endif // PPU_H
