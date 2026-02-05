#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define WIN_W 800
#define WIN_H 600

typedef struct {
    SDL_Rect bar;
    float min, max;
    float value;
    bool dragging;
} Slider;

void draw_slider(SDL_Renderer *r, Slider *s);
void handle_slider_event(Slider *s, SDL_Event *e);