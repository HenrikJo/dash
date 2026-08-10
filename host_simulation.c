#include "model.h"
#include "view.h"
#include "control.h"

int main(void)
{
    /* Create model */
    struct vehicle_info *yamaha_fjr_1300 = create_vehicle();

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
