#ifndef HEADER_H
#define HEADER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

typedef struct {
    SDL_Rect rect;
    SDL_Texture* textureOff;
    SDL_Texture* textureOn;
    int isHovered;
} Button;

// Core functions
int initSDL(SDL_Window** window, SDL_Renderer** renderer);
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer);
void handleEvents(int* running, Button* buttons, int numButtons, Mix_Chunk* clickSound);
void updateButtonHover(Button* b, int mouseX, int mouseY);
void renderMenu(SDL_Renderer* renderer, SDL_Texture* background, Button* buttons, int numButtons);
void cleanup(SDL_Window* window, SDL_Renderer* renderer, Button* buttons, int numButtons, Mix_Music* music, Mix_Chunk* click);

#endif
