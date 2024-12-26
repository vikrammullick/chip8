#ifndef PPU_H
#define PPU_H

#include "constants.hpp"

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

    void print() {
        std::cout << "\33[2J\33[H" << std::endl;
        for (uint8_t row = 0; row < constants::SCREEN_HEIGHT; row++) {
            std::cout << '[';
            for (uint8_t col = 0; col < constants::SCREEN_WIDTH; col++) {
                std::cout << (m_data[row * constants::SCREEN_WIDTH + col]
                                  ? '*'
                                  : ' ');
            }
            std::cout << ']';
            std::cout << std::endl;
        }
    }
};

#endif // PPU_H
