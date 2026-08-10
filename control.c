#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define WIN_W 800
#define WIN_H 600

typedef struct {
    SDL_Rect bar;
    int min, max;
    int value;
    bool dragging;
} Slider;

void draw_slider(SDL_Renderer *r, Slider *s)
{
    /* Bar */
    SDL_SetRenderDrawColor(r, 80, 80, 80, 255);
    SDL_RenderFillRect(r, &s->bar);

    /* Knob */
    float t = (float)(s->value - s->min) / (s->max - s->min);
    int knob_x = s->bar.x + (int)(t * s->bar.w);

    SDL_Rect knob = { knob_x - 5, s->bar.y - 5, 10, s->bar.h + 10 };
    SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
    SDL_RenderFillRect(r, &knob);
}

void handle_slider_event(Slider *s, SDL_Event *e)
{
    if (e->type == SDL_MOUSEBUTTONDOWN)
    {
        int mx = e->button.x;
        int my = e->button.y;

        if (SDL_PointInRect(&(SDL_Point){mx, my}, &s->bar))
            s->dragging = true;
    }
    else if (e->type == SDL_MOUSEBUTTONUP)
    {
        s->dragging = false;
    }
    else if (e->type == SDL_MOUSEMOTION && s->dragging)
    {
        int mx = e->motion.x;
        if (mx < s->bar.x) mx = s->bar.x;
        if (mx > s->bar.x + s->bar.w) mx = s->bar.x + s->bar.w;

        float t = (float)(mx - s->bar.x) / s->bar.w;
        s->value = s->min + (int)(t * (s->max - s->min));
    }
}

int main(void)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win = SDL_CreateWindow(
        "SDL Dashboard",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H,
        0
    );

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    /* Sliders */
    Slider speed = { {100, 100, 600, 20}, 0, 300, 0, false };
    Slider rpm   = { {100, 180, 600, 20}, 0, 9000, 0, false };

    int gear = 0; // 0=N, 1..6

    bool running = true;

    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                running = false;

            /* Gear flanks */
            if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            {
                if (e.key.keysym.sym == SDLK_UP && gear < 6)
                    gear++;
                if (e.key.keysym.sym == SDLK_DOWN && gear > 0)
                    gear--;
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
            }

            handle_slider_event(&speed, &e);
            handle_slider_event(&rpm, &e);
        }

        /* HELD keys */
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_W])
            rpm.value += 50;
        if (keys[SDL_SCANCODE_SPACE])
            speed.value += 1;

        if (rpm.value > rpm.max) rpm.value = rpm.max;
        if (speed.value > speed.max) speed.value = speed.max;

        /* Render */
        SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
        SDL_RenderClear(ren);

        draw_slider(ren, &speed);
        draw_slider(ren, &rpm);

        /* Gear indicator */
        SDL_Rect gear_box = {350, 260, 100, 60};
        SDL_SetRenderDrawColor(ren, 40, 40, 40, 255);
        SDL_RenderFillRect(ren, &gear_box);

        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderDrawRect(ren, &gear_box);

        /* Simple gear visualization (bars) */
        for (int i = 0; i < 7; i++)
        {
            SDL_Rect g = { 260 + i * 40, 350, 30, (i == gear) ? 40 : 20 };
            SDL_SetRenderDrawColor(ren,
                i == gear ? 200 : 80,
                i == gear ? 200 : 80,
                i == gear ? 50  : 80,
                255
            );
            SDL_RenderFillRect(ren, &g);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
