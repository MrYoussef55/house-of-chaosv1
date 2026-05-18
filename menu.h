#ifndef MENU_H
#define MENU_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include "playeri.h"

typedef enum {
    MODE_NONE,
    MODE_SOLO,
    MODE_MULTI
} GameMode;

typedef struct {
    SDL_Rect      rect;
    SDL_Texture*  texOff;
    SDL_Texture*  texOn;
} Button;

typedef struct {
    GameMode    mode;
    InputScheme input1;
    InputScheme input2;
    int         avatar1;   /* 0 = orange, 1 = yellow */
    int         avatar2;   /* 0 = orange, 1 = yellow */
} MenuResult;

MenuResult runMenu(SDL_Renderer* renderer);

#endif
