#include "model.h"
#include "view.h"
#include "control.h"

static void update_view(const char *outdir, struct vehicle_info *self)
{
    write_field(outdir, "throttle", &self->throttle, 1);
    float gear = (float)self->selected_gear;
    printf("Gear %f\n", gear);
    write_field(outdir, "gear", &gear, 1);
    float torque = (float)self->present_motor_torque;
    write_field(outdir, "torque", &torque, 1);
    write_field(outdir, "rpm", &self->engine_rpm, 1);
    write_field(outdir, "speed", &self->vehicle_speed, 1);
}

int main(void)
{
    /* Create output directory */
    const char *outdir = TRACE_PATH;
    if (mkdir(outdir, 0755) && errno != EEXIST) {
        perror("mkdir");
        return 1;
    }
    create_format_file();

    /* Create model */
    struct vehicle_info *yamaha_fjr_1300 = create_vehicle();
    engage_clutch(yamaha_fjr_1300);

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
    Slider throttle = { {100, 100, 600, 20}, 0, 1.0f, 0, false };

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

            handle_slider_event(&throttle, &e);
        }

        /* HELD keys */
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_W]) {
            throttle.value += 0.01f;
        } else {
            throttle.value -= 0.01f;
        }

        if (throttle.value > throttle.max) throttle.value = throttle.max;
        if (throttle.value < throttle.min) throttle.value = throttle.min;

        printf("throttle.value: %f\n", throttle.value);
        /* Render */
        SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
        SDL_RenderClear(ren);

        draw_slider(ren, &throttle);

        /* Gear indicator */
        SDL_Rect gear_box = {350, 260, 100, 60};
        SDL_SetRenderDrawColor(ren, 40, 40, 40, 255);
        SDL_RenderFillRect(ren, &gear_box);

        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderDrawRect(ren, &gear_box);

        /* Simple gear visualization (bars) */
        for (int i = 0; i < 7; i++) {
            SDL_Rect g = { 260 + i * 40, 350, 30, (i == gear) ? 40 : 20 };
            SDL_SetRenderDrawColor(ren,
                i == gear ? 200 : 80,
                i == gear ? 200 : 80,
                i == gear ? 50  : 80,
                255
            );
            SDL_RenderFillRect(ren, &g);
        }
        
        yamaha_fjr_1300->throttle = throttle.value;
        set_gear(yamaha_fjr_1300, gear);
        update_speed(yamaha_fjr_1300, 0.016f);
        update_view(outdir, yamaha_fjr_1300);

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
