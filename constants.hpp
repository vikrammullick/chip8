#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <array>

namespace constants {
constexpr uint16_t ROM_OFFSET = 0x0200;

constexpr uint16_t FONTSET_OFFSET = 0x0050;
constexpr size_t NUM_FONTS = 16;
constexpr size_t FONTSET_SIZE = 5;
constexpr std::array<uint8_t, NUM_FONTS * FONTSET_SIZE> FONTSET = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

constexpr uint16_t KEYBOARD_ADDR_LO = 0x0000;
constexpr uint16_t KEYBOARD_ADDR_HI = 0x0001;
constexpr uint16_t DELAY_TIMER_ADDR = 0x0002;
constexpr uint16_t SOUND_TIMER_ADDR = 0x0003;
constexpr uint16_t PPU_CLEAR_OR_READ_VBLANK_ADDR = 0x00004;
constexpr uint16_t PPU_SPRITE_X_ADDR = 0x0005;
constexpr uint16_t PPU_SPRITE_Y_ADDR = 0x0006;
constexpr uint16_t PPU_SPRITE_ADDR_LO = 0x0007;
constexpr uint16_t PPU_SPRITE_ADDR_HI = 0x0008;
constexpr uint16_t PPU_DRAW_SPRITE_OR_READ_TOGGLED_OFF_ADDR = 0x0009;

constexpr uint16_t MEM_SIZE = 4096;
constexpr uint8_t NUM_REGS = 16;
constexpr uint8_t STACK_SIZE = 8;

constexpr uint8_t SCREEN_HEIGHT = 32;
constexpr uint8_t SCREEN_WIDTH = 64;
constexpr uint8_t DISPLAY_SCALE = 10;
constexpr uint16_t SCREEN_PIXEL_COUNT = SCREEN_HEIGHT * SCREEN_WIDTH;

constexpr uint64_t MICROS_PER_SEC = 1'000'000;
constexpr uint64_t INTERPRETER_CLOCK_RATE = 60000;
constexpr uint64_t PPU_CLOCK_RATE = 60;
constexpr uint64_t CPU_CLOCK_RATE = 600;
constexpr uint64_t DT_ST_CLOCK_RATE = 60;

constexpr uint8_t READ_SELECT = 0;
constexpr uint8_t WRITE_SELECT = 1;

constexpr uint8_t MEMORY_CHIP_SELECT = 0;
constexpr uint8_t KEYBOARD_CHIP_SELECT = 1;
constexpr uint8_t DELAY_TIMER_CHIP_SELECT = 2;
constexpr uint8_t SOUND_TIMER_CHIP_SELECT = 3;
constexpr uint8_t PPU_CHIP_SELECT = 4;

} // namespace constants

#endif // CONSTANTS_H
