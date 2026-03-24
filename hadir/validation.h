#ifndef VALIDATION_H
#define VALIDATION_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    SDL_Texture *normal;
    SDL_Texture *hover;
    SDL_Rect rect;
    int isHovered;
} Button;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *background;
    Button btn1;
    Button btn2;
    Mix_Chunk *clickSound;
    Mix_Music *bgMusic;
    TTF_Font *font;
} Fenetre;

// Prototypes
SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *path);
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y);
int fenetre1();
int fenetre2();
int initSDL();
void cleanSDL();

#endif
