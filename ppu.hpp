#ifndef PPU_H
#define PPU_H

#include "constants.hpp"
#include "sdl_helpers.hpp"

#include <iostream>
#include <vector>

class ppu_t {
    uint8_t *m_data;
    bool m_vblank;

    uint64_t m_ticks = 0;

    static constexpr size_t NUM_PIXELS =
        constants::SCREEN_WIDTH * constants::SCREEN_HEIGHT;

    void refresh_screen();

  public:
    ppu_t();
    ~ppu_t();

    bool toggle_pixel(uint8_t screen_y, uint8_t screen_x);

    void clear();

    void tick();

    void unset_vblank() { m_vblank = false; }

    bool vblank() const { return m_vblank; }
};

#endif // PPU_H
