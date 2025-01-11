#include <map>

#include "constants.hpp"
#include "sdl_helpers.hpp"

using namespace std;

SDL_Renderer *renderer = nullptr;
SDL_Window *window = nullptr;
SDL_Texture *texture = nullptr;

uint16_t keyboard = 0;

bool running = true;

void refresh_screen() {
    static const map<SDL_Keycode, uint8_t> KEY_MAP = {
        {SDLK_1, 0x1},
        {SDLK_2, 0x2},
        {SDLK_3, 0x3},
        {SDLK_4, 0xC},
        {SDLK_q, 0x4},
        {SDLK_w, 0x5},
        {SDLK_e, 0x6},
        {SDLK_r, 0xD},
        {SDLK_a, 0x7},
        {SDLK_s, 0x8},
        {SDLK_d, 0x9},
        {SDLK_f, 0xE},
        {SDLK_z, 0xA},
        {SDLK_x, 0x0},
        {SDLK_c, 0xB},
        {SDLK_v, 0xF},
    };

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            continue;
        }

        bool pressed_down;
        if (event.type == SDL_KEYDOWN) {
            pressed_down = true;
        } else if (event.type == SDL_KEYUP) {
            pressed_down = false;
        } else {
            continue;
        }

        auto key_value_it = KEY_MAP.find(event.key.keysym.sym);
        if (key_value_it == KEY_MAP.end()) {
            continue;
        }
        uint16_t bit_mask = 0b1 << key_value_it->second;

        if (pressed_down) {
            keyboard = keyboard | bit_mask;
        } else {
            keyboard = keyboard & ~bit_mask;
        }
    }

    SDL_RenderPresent(renderer);
}

void init_window() {
    SDL_Init(SDL_INIT_EVERYTHING);

    window =
        SDL_CreateWindow("chip8",
                         SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED,
                         constants::DISPLAY_SCALE * constants::SCREEN_WIDTH,
                         constants::DISPLAY_SCALE * constants::SCREEN_HEIGHT,
                         SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
}

void destroy_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void draw_pixel(uint8_t screen_y, uint8_t screen_x, bool on) {
    if (on) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
    SDL_Rect pixel = {screen_x * constants::DISPLAY_SCALE,
                      screen_y * constants::DISPLAY_SCALE,
                      constants::DISPLAY_SCALE,
                      constants::DISPLAY_SCALE};
    SDL_RenderFillRect(renderer, &pixel);
}
