#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"
#include "collision.h"
#include "minimap.h"
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
                                 SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    /* ---------- background & collision mask ---------- */
    SDL_Texture* bg   = IMG_LoadTexture(renderer, "background.png");
    SDL_Surface* mask = IMG_Load("backgroundmask2.png");

    if (!bg)   printf("background failed: %s\n", IMG_GetError());
    if (!mask) printf("mask failed: %s\n",       IMG_GetError());
    else       printf("mask loaded: %dx%d (screen: %dx%d)\n",
                      mask->w, mask->h, SCREEN_WIDTH, SCREEN_HEIGHT);

    /* ---------- player ---------- */
    Player player;
    initPlayer(&player, renderer);

    /* ---------- enemies ---------- */
    /* Spawn on open wooden floor, spread across the room */
    Ennemi e1, e2, e3, e4, e5;

    initEnnemi(&e1, renderer);
    e1.destRect.x = 400; e1.destRect.y = 580;

    initEnnemi(&e2, renderer);
    e2.destRect.x = 880; e2.destRect.y = 580;

    initEnnemi(&e3, renderer);
    e3.destRect.x = 300; e3.destRect.y = 600;

    initEnnemi(&e4, renderer);
    e4.destRect.x = 600; e4.destRect.y = 600;

    initEnnemi(&e5, renderer);
    e5.destRect.x = 950; e5.destRect.y = 600;

    /* ---------- minimap ---------- */
    Minimap minimap;
    SDL_Rect minimapPos = {SCREEN_WIDTH - 210, 10, 200, 150};
    /* Use the player's own walk-down frame 0 as the minimap icon */
    InitMinimap(&minimap, renderer, "map.png",
                player.walk[0][0], minimapPos);

    /* fixed camera (no scroll) */
    SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    SDL_Event event;
    int running = 1;

    while (running)
    {
        /* ---- events ---- */
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT) running = 0;

        /* ---- update ---- */
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        handleInput(&player, keys, mask);
        updatePlayer(&player);

        poursuivreJoueur(&e1, player.destRect, mask, &player.health);
        poursuivreJoueur(&e2, player.destRect, mask, &player.health);
        poursuivreJoueur(&e3, player.destRect, mask, &player.health);
        poursuivreJoueur(&e4, player.destRect, mask, &player.health);
        poursuivreJoueur(&e5, player.destRect, mask, &player.health);
        animerEnnemi(&e1);
        animerEnnemi(&e2);
        animerEnnemi(&e3);
        animerEnnemi(&e4);
        animerEnnemi(&e5);

        MAJMinimap(&minimap, player.destRect);

        /* ---- render ---- */
        SDL_RenderClear(renderer);

        /* background */
        SDL_RenderCopy(renderer, bg, NULL, NULL);

        /* entities */
        renderPlayer(&player, renderer);
        afficherEnnemi(e1, renderer, camera);
        afficherEnnemi(e2, renderer, camera);
        afficherEnnemi(e3, renderer, camera);
        afficherEnnemi(e4, renderer, camera);
        afficherEnnemi(e5, renderer, camera);

        /* minimap border */
        SDL_Rect border = {minimapPos.x - 4, minimapPos.y - 4,
                           minimapPos.w + 8, minimapPos.h + 8};
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_RenderFillRect(renderer, &border);
        AfficherMinimap(renderer, minimap);

        /* HUD */
        renderUI(&player, renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    /* ---- cleanup ---- */
    destroyPlayer(&player);
    LibererMinimap(&minimap);
    SDL_FreeSurface(mask);
    SDL_DestroyTexture(bg);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
