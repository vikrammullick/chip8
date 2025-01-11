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

    bool toggle_pixel(uint8_t screen_y, uint8_t screen_x);

    void clear();

    void refresh_screen();

    void unset_vblank() { m_vblank = false; }

    bool vblank() const { return m_vblank; }
};

#endif // PPU_H
