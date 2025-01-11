#ifndef SDL_HELPERS_HPP
#define SDL_HELPERS_HPP

#include <SDL2/SDL.h>
extern SDL_Renderer *renderer;
extern SDL_Window *window;

extern uint16_t g_keyboard;

extern bool g_running;

extern void sdl_refresh_screen();
extern void sdl_poll_keyboard();

extern void sdl_init_window();

extern void sdl_destroy_window();

extern void sdl_draw_pixel(uint8_t screen_y, uint8_t screen_x, bool on);

#endif
