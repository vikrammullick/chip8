#ifndef SDL_HELPERS_HPP
#define SDL_HELPERS_HPP

#include <SDL2/SDL.h>
extern SDL_Renderer *renderer;
extern SDL_Window *window;

extern uint16_t keyboard;

extern bool running;

extern void refresh_screen();

extern void init_window();

extern void destroy_window();

extern void draw_pixel(uint8_t screen_y, uint8_t screen_x, bool on);

#endif
