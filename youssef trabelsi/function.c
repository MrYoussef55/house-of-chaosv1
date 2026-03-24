#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>
#include "header.h"


/* =================== FUNCTIONS =============== */

/* check if (x,y) inside a rectangle */
bool isInside(SDL_Rect rect, int x, int y)
{
    return (
        x >= rect.x &&
        x <= rect.x + rect.w &&
        y >= rect.y &&
        y <= rect.y + rect.h
    );
}


/* render button (normal or hover texture) */
void renderButton(SDL_Renderer* renderer, button* btn)
{
    SDL_Texture* texture =
        btn->hover ? btn->hoverTex : btn->normalTex;

    SDL_RenderCopy(renderer, texture, NULL, &btn->rect);
}


void applyVolume(int volume)
{
    int sdlVolume = volume * MIX_MAX_VOLUME / 100;

    Mix_VolumeMusic(sdlVolume);
    Mix_Volume(-1, sdlVolume);
}


/* Render text to texture */
SDL_Texture* renderText(SDL_Renderer* renderer,
                        TTF_Font* font,
                        const char* text)
{
    SDL_Color white = {255, 255, 255, 255};

    SDL_Surface* surface =
        TTF_RenderText_Blended(font, text, white);

    if (!surface)
        return NULL;

    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);
    return texture;
}


/* =========== BACKGROUND FUNCTIONS======= */

Background initBackground(SDL_Renderer* renderer, const char* path)
{
    Background bg;

    bg.texture = IMG_LoadTexture(renderer, path);

    if (!bg.texture)
        printf("Background load error: %s\n", IMG_GetError());

    bg.destRect.x = 0;
    bg.destRect.y = 0;
    bg.destRect.w = 0;
    bg.destRect.h = 0;

    return bg;
}


void renderBackground(SDL_Renderer* renderer,
                      Background* bg,
                      SDL_Window* window)
{
    int texW, texH;
    SDL_QueryTexture(bg->texture, NULL, NULL, &texW, &texH);

    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);

    float winRatio = (float)winW / winH;
    float texRatio = (float)texW / texH;

    if (winRatio > texRatio)
    {
        bg->destRect.h = winH;
        bg->destRect.w = (int)(winH * texRatio);
        bg->destRect.x = (winW - bg->destRect.w) / 2;
        bg->destRect.y = 0;
    }
    else
    {
        bg->destRect.w = winW;
        bg->destRect.h = (int)(winW / texRatio);
        bg->destRect.x = 0;
        bg->destRect.y = (winH - bg->destRect.h) / 2;
    }

    SDL_RenderCopy(renderer, bg->texture, NULL, &bg->destRect);
}


void destroyBackground(Background* bg)
{
    if (bg->texture)
        SDL_DestroyTexture(bg->texture);
}
