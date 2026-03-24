#ifndef OPTIONS_H
#define OPTIONS_H

#include <SDL2/SDL.h>
typedef struct {
    SDL_Rect rect;
    SDL_Texture* normal;
    SDL_Texture* hover;
    int hovered;
} Button;
int options_run(SDL_Renderer* renderer);

#endif
