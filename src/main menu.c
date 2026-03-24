#include "header.h"

int initSDL(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) return 0;
    *window = SDL_CreateWindow("House of Chaos", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024,576, 0);
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    return 1;
}

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* temp = IMG_Load(path);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    return tex;
}

void updateButtonHover(Button* b, int mouseX, int mouseY) {
    if (mouseX >= b->rect.x && mouseX <= b->rect.x + b->rect.w &&
    
        mouseY >= b->rect.y && mouseY <= b->rect.y + b->rect.h) {
        b->isHovered = 1;
    } else {
        b->isHovered = 0;
    }
}

void renderMenu(SDL_Renderer* renderer, SDL_Texture* background, Button* buttons, int numButtons) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, NULL, NULL);
    for (int i = 0; i < numButtons; i++) {
        SDL_Texture* current = buttons[i].isHovered ? buttons[i].textureOn : buttons[i].textureOff;
        SDL_RenderCopy(renderer, current, NULL, &buttons[i].rect);
    }
    SDL_RenderPresent(renderer);
}
