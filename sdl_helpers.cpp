#include "sdl_helpers.hpp"
#include "constants.hpp"

SDL_Renderer *renderer = nullptr;
SDL_Window *window = nullptr;
SDL_Texture *texture = nullptr;

bool running = true;

void refresh_screen() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            default:
                break;
            }
            break;
        default:
            break;
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
