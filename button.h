#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    SDL_Rect rect;
    SDL_Texture* normal;
    SDL_Texture* hover;
    bool isHovered;
} Button;

// NEW: Directly set whether the button is hovered by the controller
void Button_SetHovered(Button* btn, bool isHovered);

void Button_Render(SDL_Renderer* renderer, Button* btn);
void Button_Destroy(Button* btn);

#endif
