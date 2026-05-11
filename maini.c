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
#include "room.h"

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

    /* ---------- player ---------- */
    Player player;
    initPlayer(&player, renderer);

    /* ---------- rooms ---------- */
    initRooms(renderer);

    /* ---------- minimap ---------- */
    Minimap minimap;
    SDL_Rect minimapPos = {SCREEN_WIDTH - 210, 10, 200, 150};
    const char* mapPaths[] = { "background.png", "background2.png", "background3.png", "background4.png" };
    InitMinimap(&minimap, renderer, mapPaths, player.walk[0][0], minimapPos);

    SDL_Rect camera  = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Event event;
    int       running = 1;
    int       tick    = 0;

    while (running)
    {
        /* ---- events ---- */
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT) running = 0;

        /* ---- current room shorthand ---- */
        Room* room = &rooms[currentRoom];

        /* ---- update ---- */
        tick++;

        const Uint8* keys = SDL_GetKeyboardState(NULL);
        handleInput(&player, keys, room->mask);
        updatePlayer(&player);

        /* door transition */
        checkDoorEntry(player.destRect, &player);

        /* update enemies */
        for (int i = 0; i < room->enemyCount; i++) {
            if (room->enemies[i].dead) continue;
            poursuivreJoueur(&room->enemies[i], &player, room->mask);
            animerEnnemi    (&room->enemies[i]);
        }

        /* player attacks enemies */
        playerAttackEnemies(&player, room->enemies, room->enemyCount, tick);

        MAJMinimap(&minimap, player.destRect, currentRoom);

        /* ---- render ---- */
        SDL_RenderClear(renderer);

        /* background */
        SDL_RenderCopy(renderer, room->background, NULL, NULL);

        /* enemies */
        for (int i = 0; i < room->enemyCount; i++) {
            if (room->enemies[i].dead) continue;
            afficherEnnemi(room->enemies[i], renderer, camera);
        }

        /* player */
        renderPlayer(&player, renderer);

        /* minimap border */
        SDL_Rect border = {minimapPos.x - 4, minimapPos.y - 4,
                           minimapPos.w + 8, minimapPos.h + 8};
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_RenderFillRect(renderer, &border);
        AfficherMinimap(renderer, minimap, currentRoom);

        /* HUD */
        renderUI(&player, renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    /* ---- cleanup ---- */
    destroyPlayer(&player);
    LibererMinimap(&minimap);
    freeRooms();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
