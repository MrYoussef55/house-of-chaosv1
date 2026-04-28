#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>

#include "playeri.h"
#include "enemiei.h"

int main(int argc, char* argv[])
{
    srand((unsigned)time(NULL));

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    SDL_Window*   window   = SDL_CreateWindow("House Of Chaos",
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 1280, 720, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    /* background */
    SDL_Surface* bgSurf = IMG_Load("assets/back.png");
    SDL_Texture* bg     = SDL_CreateTextureFromSurface(renderer, bgSurf);
    SDL_FreeSurface(bgSurf);

    /* player */
    Player player;
    initPlayer(&player, renderer);

    /* enemies */
    Ennemi e1, e2;
    initEnnemi(&e1, renderer);
    initEnnemi(&e2, renderer);
    e2.destRect.x = 800;
    e2.destRect.y = 400;

    /* hazard zone */
    SDL_Rect hazard = {540, 260, 200, 200};
    SDL_Rect camera = {0, 0, 1280, 720};

    SDL_Event event;
    int running = 1;

    while (running)
    {
        /* ---- events ---- */
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT) running = 0;

        /* ---- update ---- */
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        handleInput(&player, keys);
        updatePlayer(&player);

        deplacerAleatoire(&e1, hazard);
        deplacerAleatoire(&e2, hazard);
        animerEnnemi(&e1);
        animerEnnemi(&e2);

        /* ---- render ---- */
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, bg, NULL, NULL);           

        SDL_SetRenderDrawColor(renderer, 150, 0, 0, 150);
        SDL_RenderFillRect(renderer, &hazard);              

        renderPlayer(&player, renderer);                    
        afficherEnnemi(e1, renderer, camera);               
        afficherEnnemi(e2, renderer, camera);

        renderUI(&player, renderer);                        

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    /* ---- cleanup ---- */
    destroyPlayer(&player);
    SDL_DestroyTexture(bg);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
