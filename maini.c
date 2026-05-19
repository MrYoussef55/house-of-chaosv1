#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"
#include "collision.h"
#include "minimap.h"
#include "playeri.h"
#include "enemiei.h"
#include "room.h"
#include "menu.h"

int main(int argc, char* argv[])
{
    srand((unsigned)time(NULL));
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_Window*   window   = SDL_CreateWindow("House Of Chaos",
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    /* ---- menu ---- */
    MenuResult cfg = runMenu(renderer);
    if (cfg.mode == MODE_NONE) goto cleanup;

    /* ---- players ---- */
    Player player1, player2;
    initPlayer(&player1, renderer, cfg.avatar1);
    initPlayer(&player2, renderer, cfg.avatar2);
    player1.inputScheme = cfg.input1;
    player2.inputScheme = cfg.input2;
    player2.destRect.x  = 500;
    player2.destRect.y  = 600;

    /* In solo mode, park player2 far off-screen and mark inactive
     * so enemies never target the invisible second player. */
    if (cfg.mode == MODE_SOLO) {
        player2.active       = 0;
        player2.dead         = 1;
        player2.lives        = 0;
        player2.health       = 0;
        player2.destRect.x   = -9999;
        player2.destRect.y   = -9999;
    }

    /* ---- rooms ---- */
    initRooms(renderer);

    /* ---- minimap ---- */
    Minimap  minimap;
    SDL_Rect minimapPos = {SCREEN_WIDTH - 210, 10, 200, 150};
    const char* mapPaths[] = {
        "background.png", "background2.png",
        "background3.png", "background4.png"
    };
    InitMinimap(&minimap, renderer, mapPaths,
                player1.walk[0][0], player2.walk[0][0], minimapPos);

    /* showPlayer2 dot on minimap only in multiplayer */
    int showP2 = (cfg.mode == MODE_MULTI) ? 1 : 0;

    SDL_Rect  camera  = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Event event;
    int       running = 1;
    int       tick    = 0;

    while (running)
    {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == SDLK_p) {
                MenuResult newcfg = runMenu(renderer);
                if (newcfg.mode == MODE_NONE) { running = 0; break; }
                cfg   = newcfg;
                showP2 = (cfg.mode == MODE_MULTI) ? 1 : 0;
                player1.inputScheme = cfg.input1;
                player2.inputScheme = cfg.input2;
            }
        }
        if (!running) break;

        Room* room = &rooms[currentRoom];

        /* ---- update ---- */
        tick++;
        const Uint8* keys = SDL_GetKeyboardState(NULL);

        handleInput(&player1, keys, room->mask);
        updatePlayer(&player1);
        playerAttackEnemies(&player1, room->enemies, room->enemyCount, tick);

        if (cfg.mode == MODE_MULTI) {
            handleInput(&player2, keys, room->mask);
            updatePlayer(&player2);
            playerAttackEnemies(&player2, room->enemies, room->enemyCount, tick);
        }

        checkDoorEntry(player1.destRect, &player1, &player2);

        for (int i = 0; i < room->enemyCount; i++) {
            if (room->enemies[i].dead) continue;
            poursuivreJoueur(&room->enemies[i], &player1, &player2, room->mask);
            animerEnnemi    (&room->enemies[i]);
        }

        MAJMinimap(&minimap, player1.destRect, player2.destRect, currentRoom);

        /* ---- render ---- */
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, room->background, NULL, NULL);

        for (int i = 0; i < room->enemyCount; i++) {
            if (room->enemies[i].dead) continue;
            afficherEnnemi(room->enemies[i], renderer, camera);
        }

        renderPlayer(&player1, renderer);
        if (cfg.mode == MODE_MULTI)
            renderPlayer(&player2, renderer);

        SDL_Rect border = {minimapPos.x-4, minimapPos.y-4,
                           minimapPos.w+8, minimapPos.h+8};
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_RenderFillRect(renderer, &border);
        AfficherMinimap(renderer, minimap, currentRoom, showP2);

        renderUI(&player1, renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    destroyPlayer(&player1);
    destroyPlayer(&player2);
    LibererMinimap(&minimap);
    freeRooms();

cleanup:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
