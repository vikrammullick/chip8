#ifndef PPU_H
#define PPU_H

#include "constants.hpp"
#include "sdl_helpers.hpp"

#include <iostream>
#include <vector>

class ppu_t {
    std::vector<uint8_t> m_data;

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
    void print() {
        for (uint8_t i = 0; i < constants::SCREEN_HEIGHT; ++i)
            for (uint8_t j = 0; j < constants::SCREEN_WIDTH; ++j)
                draw_pixel(i, j, m_data[i * constants::SCREEN_WIDTH + j]);
    }
};

#endif // PPU_H
