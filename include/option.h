#ifndef HEADER_H
#define HEADER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 600


/* ===================== STRUCT ===================== */


/ ===================== BACKGROUND STRUCT ===================== /

typedef struct
{
    SDL_Texture texture;
    SDL_Rect destRect;
} Background;

typedef struct
{
    SDL_Rect rect;

    SDL_Texture* normalTex;
    SDL_Texture* hoverTex;

    bool hover;

    /* Navigation indices */
    int up;
    int down;
    int left;
    int right;

} button;


#endif
