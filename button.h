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

void Button_HandleMouse(Button* btn, int mx, int my);
void Button_Render(SDL_Renderer* renderer, Button* btn);
bool Button_Clicked(Button* btn, SDL_Event* e);
void Button_Destroy(Button* btn);

#endif
