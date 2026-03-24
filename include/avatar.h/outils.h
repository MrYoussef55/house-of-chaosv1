#ifndef OUTILS_H
#define OUTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

typedef struct
{
    SDL_Rect rect;
    SDL_Texture *textureOff;
    SDL_Texture *textureOn;
    int isHovered;
} Button;


SDL_Texture* loadTexture(char* path, SDL_Renderer* renderer);

int init(SDL_Window **window, SDL_Renderer **renderer);
void cleanup(SDL_Window *window, SDL_Renderer *renderer);


void initButton(Button *b, char* normalPath, char* hoverPath,
                int x, int y, SDL_Renderer *renderer);

void renderButton(Button *b, SDL_Renderer *renderer);
void handleHover(Button *b, int mouseX, int mouseY);

int runSubMenu(SDL_Window *window, SDL_Renderer *renderer);

#endif
