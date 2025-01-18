#ifndef PPU_H
#define PPU_H

#include "bus.hpp"
#include "constants.hpp"
#include "memory.hpp"
#include "sdl_helpers.hpp"

#include <iostream>
#include <vector>

class ppu_t {
    bus_t &m_bus;
    memory_t &m_memory;

    uint8_t *m_data;

    uint64_t m_ticks = 0;

    bool m_vblank;
    bool m_any_pixel_toggled_off;
    uint8_t m_sprite_x;
    uint8_t m_sprite_y;
    uint16_t m_sprite_addr;

    static constexpr size_t NUM_PIXELS =
        constants::SCREEN_WIDTH * constants::SCREEN_HEIGHT;

    void refresh_screen();
    void clear();

    void toggle_pixel(uint8_t screen_y, uint8_t screen_x);
    void draw_sprite(uint8_t num_rows);

  public:
    ppu_t(bus_t &bus, memory_t &memory);
    ~ppu_t();

    void tick();
    void service_request();

    // TODO: move out of cpu
    void unset_vblank() { m_vblank = false; }
};

#endif // PPU_H
