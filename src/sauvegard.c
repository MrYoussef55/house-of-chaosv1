#include "validation.h"
#include <stdio.h>

// Charger une texture
SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *path) {
    SDL_Surface *surf = IMG_Load(path);
    if(!surf) {
        printf("Impossible de charger %s : %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return tex;
}

// Afficher un texte
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y) {
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, color);
    if(!surf) return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect dst = {x, y, surf->w, surf->h};
    SDL_RenderCopy(renderer, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}

// Fenêtre 2 (boutons verticaux avec hover)
int fenetre2() {
    Fenetre f;
    f.window = SDL_CreateWindow("Fenetre 2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    f.renderer = SDL_CreateRenderer(f.window, -1, SDL_RENDERER_ACCELERATED);

    f.background = loadTexture(f.renderer, "validation.png");
    f.btn1.normal = loadTexture(f.renderer, "bt10.png");
    f.btn1.hover = loadTexture(f.renderer, "bt1.png");
    f.btn2.normal = loadTexture(f.renderer, "bt20.png");
    f.btn2.hover = loadTexture(f.renderer, "bt2.png");

    f.btn1.rect = (SDL_Rect){275, 200, 250, 100};
    f.btn2.rect = (SDL_Rect){275, 350, 250, 100};

    f.bgMusic = Mix_LoadMUS("validation.mp3");
    f.clickSound = Mix_LoadWAV("click.wav");
    if(f.bgMusic) Mix_PlayMusic(f.bgMusic, -1);

    f.font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 28);

    SDL_Event e;
    int quit = 0;
    while(!quit) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) quit = 1;
            if(e.type == SDL_MOUSEBUTTONDOWN) Mix_PlayChannel(-1, f.clickSound, 0);
        }

        int mx, my;
        SDL_GetMouseState(&mx, &my);

        SDL_RenderClear(f.renderer);
        SDL_RenderCopy(f.renderer, f.background, NULL, NULL);

        SDL_Texture *btn1Tex = (mx >= f.btn1.rect.x && mx <= f.btn1.rect.x + f.btn1.rect.w &&
                                my >= f.btn1.rect.y && my <= f.btn1.rect.y + f.btn1.rect.h) ? f.btn1.hover : f.btn1.normal;

        SDL_Texture *btn2Tex = (mx >= f.btn2.rect.x && mx <= f.btn2.rect.x + f.btn2.rect.w &&
                                my >= f.btn2.rect.y && my <= f.btn2.rect.y + f.btn2.rect.h) ? f.btn2.hover : f.btn2.normal;

        SDL_RenderCopy(f.renderer, btn1Tex, NULL, &f.btn1.rect);
        SDL_RenderCopy(f.renderer, btn2Tex, NULL, &f.btn2.rect);

        SDL_RenderPresent(f.renderer);
    }

    SDL_DestroyTexture(f.background);
    SDL_DestroyTexture(f.btn1.normal);
    SDL_DestroyTexture(f.btn1.hover);
    SDL_DestroyTexture(f.btn2.normal);
    SDL_DestroyTexture(f.btn2.hover);
    if(f.clickSound) Mix_FreeChunk(f.clickSound);
    if(f.bgMusic) Mix_FreeMusic(f.bgMusic);
    if(f.font) TTF_CloseFont(f.font);
    SDL_DestroyRenderer(f.renderer);
    SDL_DestroyWindow(f.window);
    return 0;
}

// Fenêtre 1 (texte + boutons horizontaux avec hover)
int fenetre1() {
    Fenetre f;
    f.window = SDL_CreateWindow("Fenetre 1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    f.renderer = SDL_CreateRenderer(f.window, -1, SDL_RENDERER_ACCELERATED);

    f.background = loadTexture(f.renderer, "validation.png");
    f.btn1.normal = loadTexture(f.renderer, "oui.png");
    f.btn1.hover  = loadTexture(f.renderer, "yes.png");
    f.btn2.normal = loadTexture(f.renderer, "non.png");
    f.btn2.hover  = loadTexture(f.renderer, "no.png");

    f.btn1.rect = (SDL_Rect){175, 320, 200, 90};
    f.btn2.rect = (SDL_Rect){425, 320, 200, 90};

    f.bgMusic = Mix_LoadMUS("validation.mp3");
    f.clickSound = Mix_LoadWAV("click.wav");
    if(f.bgMusic) Mix_PlayMusic(f.bgMusic, -1);

    f.font = TTF_OpenFont("/home/hadir/validation hadir/SouthernAire_Personal_Use_Only.ttf", 65);

    SDL_Event e;
    int quit = 0;
    while(!quit) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) quit = 1;
            if(e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x, my = e.button.y;
                if(mx >= f.btn1.rect.x && mx <= f.btn1.rect.x + f.btn1.rect.w &&
                   my >= f.btn1.rect.y && my <= f.btn1.rect.y + f.btn1.rect.h) {
                    Mix_PlayChannel(-1, f.clickSound, 0);
                    SDL_DestroyTexture(f.background);
                    SDL_DestroyTexture(f.btn1.normal);
                    SDL_DestroyTexture(f.btn1.hover);
                    SDL_DestroyTexture(f.btn2.normal);
                    SDL_DestroyTexture(f.btn2.hover);
                    if(f.font) TTF_CloseFont(f.font);
                    SDL_DestroyRenderer(f.renderer);
                    SDL_DestroyWindow(f.window);
                    return fenetre2();
                }
                if(mx >= f.btn2.rect.x && mx <= f.btn2.rect.x + f.btn2.rect.w &&
                   my >= f.btn2.rect.y && my <= f.btn2.rect.y + f.btn2.rect.h) {
                    Mix_PlayChannel(-1, f.clickSound, 0);
                    quit = 1;
                }
            }
        }

        int mx, my;
        SDL_GetMouseState(&mx, &my);

        SDL_RenderClear(f.renderer);
        SDL_RenderCopy(f.renderer, f.background, NULL, NULL);
        renderText(f.renderer, f.font, "Would you like to save your game?", 120, 230);

        SDL_Texture *btn1Tex = (mx >= f.btn1.rect.x && mx <= f.btn1.rect.x + f.btn1.rect.w &&
                                my >= f.btn1.rect.y && my <= f.btn1.rect.y + f.btn1.rect.h) ? f.btn1.hover : f.btn1.normal;

        SDL_Texture *btn2Tex = (mx >= f.btn2.rect.x && mx <= f.btn2.rect.x + f.btn2.rect.w &&
                                my >= f.btn2.rect.y && my <= f.btn2.rect.y + f.btn2.rect.h) ? f.btn2.hover : f.btn2.normal;

        SDL_RenderCopy(f.renderer, btn1Tex, NULL, &f.btn1.rect);
        SDL_RenderCopy(f.renderer, btn2Tex, NULL, &f.btn2.rect);

        SDL_RenderPresent(f.renderer);
    }

    SDL_DestroyTexture(f.background);
    SDL_DestroyTexture(f.btn1.normal);
    SDL_DestroyTexture(f.btn1.hover);
    SDL_DestroyTexture(f.btn2.normal);
    SDL_DestroyTexture(f.btn2.hover);
    if(f.clickSound) Mix_FreeChunk(f.clickSound);
    if(f.bgMusic) Mix_FreeMusic(f.bgMusic);
    if(f.font) TTF_CloseFont(f.font);
    SDL_DestroyRenderer(f.renderer);
    SDL_DestroyWindow(f.window);
    return 0;
}

// Init SDL
int initSDL() {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) { printf("SDL error: %s\n", SDL_GetError()); return 1; }
    if(IMG_Init(IMG_INIT_PNG) == 0) { printf("IMG error: %s\n", IMG_GetError()); return 1; }
    if(TTF_Init() != 0) { printf("TTF error: %s\n", TTF_GetError()); return 1; }
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) { printf("Mix error: %s\n", Mix_GetError()); return 1; }
    return 0;
}

// Clean SDL
void cleanSDL() {
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
