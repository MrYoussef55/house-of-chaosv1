#ifndef SUBMENU_H
#define SUBMENU_H

#include "outils.h"

typedef struct
{
    SDL_Rect rect;
    SDL_Texture *textureOff;
    SDL_Texture *textureOn;
    int isHovered;
} Button;

void initButton(Button *b, char* normalPath, char* hoverPath,
                int x, int y, SDL_Renderer *renderer);

void renderButton(Button *b, SDL_Renderer *renderer);
void handleHover(Button *b, int mouseX, int mouseY);

int runSubMenu(SDL_Window *window, SDL_Renderer *renderer);

#endif
