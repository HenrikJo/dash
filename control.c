#include "control.h"

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
