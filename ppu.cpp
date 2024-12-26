#include "ppu.hpp"

using namespace std;

bool ppu_t::toggle(uint8_t screen_y, uint8_t screen_x) {
    uint16_t idx = screen_y * constants::SCREEN_WIDTH + screen_x;
    uint8_t &pixel = m_data[idx];
    pixel = 1 - pixel;
    return !pixel;
}

void ppu_t::clear() {
    for (auto &pixel : m_data) {
        pixel = 0;
    }
    print();
}
