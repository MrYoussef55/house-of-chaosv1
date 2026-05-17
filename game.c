#include "game.h"
#include "common.h"
#include "collision.h"
#include "minimap.h"
#include "playeri.h"
#include "enemiei.h"
#include "room.h"
#include "enigme_run.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* ---- helper: count living enemies in current room ---- */
static int enemies_alive(void)
{
    Room* r = &rooms[currentRoom];
    int alive = 0;
    for (int i = 0; i < r->enemyCount; i++)
        if (!r->enemies[i].dead) alive++;
    return alive;
}

/* ---- helper: reset a room's enemies for restart ---- */
static void reset_room(SDL_Renderer* renderer)
{
    Room* r = &rooms[currentRoom];
    for (int i = 0; i < r->enemyCount; i++) {
        initEnnemi(&r->enemies[i], renderer);
        r->enemies[i].destRect.x = r->enemySpawnPos[i].x;
        r->enemies[i].destRect.y = r->enemySpawnPos[i].y;
    }
}

void Game_Run(App* app)
{
    srand((unsigned)time(NULL));

    /* ---------- player ---------- */
    Player player;
    initPlayer(&player, app->renderer);

    /* ---------- rooms ---------- */
    initRooms(app->renderer);

    /* ---------- minimap ---------- */
    Minimap minimap;
    SDL_Rect minimapPos = {SCREEN_WIDTH - 210, 10, 200, 150};
    const char* mapPaths[] = {
        "background.png",
        "background2.png",
        "background3.png",
        "background4.png"
    };
    InitMinimap(&minimap, app->renderer, mapPaths, player.walk[0][0], minimapPos);

    SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    int tick = 0;

    /* --- message overlay state (non-blocking) --- */
    TTF_Font* msgFont       = TTF_OpenFont("arial.ttf", 36);
    char      msgText[128]  = "";
    int       msgTimer      = 0;   /* frames remaining to show message */
    int       doorCooldown  = 0;   /* prevent re-triggering door immediately */

    /* ---- game loop ---- */
    while (app->running && app->state == STATE_GAME)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                app->running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                app->state = STATE_MAIN_MENU;
        }

        if (!app->running || app->state != STATE_GAME) break;

        Room* room = &rooms[currentRoom];
        tick++;

        /* tick down cooldowns */
        if (doorCooldown > 0) doorCooldown--;
        if (msgTimer    > 0) msgTimer--;

        /* ---- update ---- */
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        handleInput(&player, keys, room->mask);
        updatePlayer(&player);

        /* ---- door / room transition (only when no cooldown) ---- */
        if (doorCooldown == 0) {
            for (int d = 0; d < room->doorCount; d++) {
                SDL_Rect door = room->doors[d];
                if (player.destRect.x < door.x + door.w &&
                    player.destRect.x + player.destRect.w > door.x &&
                    player.destRect.y < door.y + door.h &&
                    player.destRect.y + player.destRect.h > door.y)
                {
                    if (enemies_alive() > 0) {
                        /* Show error message for 3 seconds (180 frames), then free */
                        strncpy(msgText, "Defeat all enemies first!", sizeof(msgText)-1);
                        msgTimer = 180;
                        doorCooldown = 180; /* don't re-trigger while message shows */
                    } else {
                        /* All enemies dead -> launch quiz */
                        doorCooldown = 60;
                        int passed = Enigme_Run(app, currentRoom);

                        if (!app->running) break;

                        if (passed) {
                            if (currentRoom == MAX_ROOMS - 1) {
                                strncpy(msgText, "You completed the game! Well done!", sizeof(msgText)-1);
                                msgTimer = 180;
                                /* let message render one frame then go to menu */
                                app->state = STATE_MAIN_MENU;
                            } else {
                                currentRoom++;
                                player.destRect.x = 600;
                                player.destRect.y = 600;
                            }
                        } else {
                            /* Failed quiz -> restart current room */
                            strncpy(msgText, "Quiz failed! Restarting room...", sizeof(msgText)-1);
                            msgTimer = 120;
                            reset_room(app->renderer);
                            player.destRect.x = 600;
                            player.destRect.y = 600;
                        }
                    }
                    break;
                }
            }
        }

        /* ---- update enemies ---- */
        for (int i = 0; i < room->enemyCount; i++) {
            if (room->enemies[i].dead) continue;
            poursuivreJoueur(&room->enemies[i], &player, room->mask);
            animerEnnemi    (&room->enemies[i]);
        }

        playerAttackEnemies(&player, room->enemies, room->enemyCount, tick);
        MAJMinimap(&minimap, player.destRect, currentRoom);

        /* ---- render ---- */
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
        SDL_RenderClear(app->renderer);

        SDL_RenderCopy(app->renderer, room->background, NULL, NULL);

        for (int i = 0; i < room->enemyCount; i++) {
            if (room->enemies[i].dead) continue;
            afficherEnnemi(room->enemies[i], app->renderer, camera);
        }

        renderPlayer(&player, app->renderer);

        SDL_Rect border = {minimapPos.x - 4, minimapPos.y - 4,
                           minimapPos.w + 8, minimapPos.h + 8};
        SDL_SetRenderDrawColor(app->renderer, 80, 80, 80, 255);
        SDL_RenderFillRect(app->renderer, &border);
        AfficherMinimap(app->renderer, minimap, currentRoom);

        renderUI(&player, app->renderer);

        /* --- draw message overlay if active --- */
        if (msgTimer > 0 && msgFont && msgText[0]) {
            SDL_Color red  = {255, 60, 60, 255};
            SDL_Surface* surf = TTF_RenderText_Blended(msgFont, msgText, red);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(app->renderer, surf);
                int w = surf->w, h = surf->h;
                SDL_FreeSurface(surf);
                SDL_Rect bg  = {SCREEN_WIDTH/2 - w/2 - 20, SCREEN_HEIGHT/2 - h/2 - 15, w+40, h+30};
                SDL_Rect pos = {SCREEN_WIDTH/2 - w/2,       SCREEN_HEIGHT/2 - h/2,       w,    h};
                SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 180);
                SDL_RenderFillRect(app->renderer, &bg);
                SDL_RenderCopy(app->renderer, tex, NULL, &pos);
                SDL_DestroyTexture(tex);
            }
        }

        SDL_RenderPresent(app->renderer);
        SDL_Delay(16);
    }

cleanup:
    if (msgFont) TTF_CloseFont(msgFont);
    destroyPlayer(&player);
    LibererMinimap(&minimap);
    freeRooms();

    if (app->running && app->state == STATE_GAME)
        app->state = STATE_MAIN_MENU;
}
