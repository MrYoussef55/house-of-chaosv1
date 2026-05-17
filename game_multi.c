#include "game_multi.h"
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

/* --- helpers (same as singleplayer) --- */
static int enemies_alive_multi(void)
{
    Room* r = &rooms[currentRoom];
    int alive = 0;
    for (int i = 0; i < r->enemyCount; i++)
        if (!r->enemies[i].dead) alive++;
    return alive;
}

static void reset_room_multi(SDL_Renderer* renderer)
{
    Room* r = &rooms[currentRoom];
    for (int i = 0; i < r->enemyCount; i++) {
        initEnnemi(&r->enemies[i], renderer);
        r->enemies[i].destRect.x = r->enemySpawnPos[i].x;
        r->enemies[i].destRect.y = r->enemySpawnPos[i].y;
    }
}

void Game_Run_Multi(App* app)
{
    srand((unsigned)time(NULL));

    /* two players */
    Player p1, p2;
    initPlayer(&p1, app->renderer);
    initPlayer(&p2, app->renderer);

    /* different start positions so they don't overlap */
    p1.destRect.x = 400; p1.destRect.y = 400;
    p2.destRect.x = 700; p2.destRect.y = 400;

    /* track whose turn it is for the quiz (0 = P1, 1 = P2) */
    int quizTurn = 0;

    /* rooms */
    initRooms(app->renderer);

    /* minimap */
    Minimap minimap;
    SDL_Rect minimapPos = {SCREEN_WIDTH - 210, 10, 200, 150};
    const char* mapPaths[] = {
        "background.png", "background2.png",
        "background3.png", "background4.png"
    };
    InitMinimap(&minimap, app->renderer, mapPaths,
                p1.walk[0][0], minimapPos);

    SDL_Rect camera    = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    int tick           = 0;

    /* non-blocking message overlay */
    TTF_Font* msgFont      = TTF_OpenFont("arial.ttf", 36);
    char      msgText[128] = "";
    int       msgTimer     = 0;
    int       doorCooldown = 0;

    while (app->running && app->state == STATE_GAME_MULTI)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                app->running = false;
            if (event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == SDLK_ESCAPE)
                app->state = STATE_MAIN_MENU;
        }

        if (!app->running || app->state != STATE_GAME_MULTI) break;

        Room* room = &rooms[currentRoom];
        tick++;

        if (doorCooldown > 0) doorCooldown--;
        if (msgTimer    > 0) msgTimer--;

        /* update both players */
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        handleInput (&p1, keys, room->mask);
        handleInput2(&p2, keys, room->mask);
        updatePlayer(&p1);
        updatePlayer(&p2);

        /* door check — either player touching door triggers it */
        if (doorCooldown == 0) {
            int p1_door = 0, p2_door = 0;
            for (int d = 0; d < room->doorCount; d++) {
                SDL_Rect door = room->doors[d];
                if (p1.destRect.x < door.x + door.w &&
                    p1.destRect.x + p1.destRect.w > door.x &&
                    p1.destRect.y < door.y + door.h &&
                    p1.destRect.y + p1.destRect.h > door.y)
                    p1_door = 1;
                if (p2.destRect.x < door.x + door.w &&
                    p2.destRect.x + p2.destRect.w > door.x &&
                    p2.destRect.y < door.y + door.h &&
                    p2.destRect.y + p2.destRect.h > door.y)
                    p2_door = 1;
            }

            if (p1_door || p2_door) {
                if (enemies_alive_multi() > 0) {
                    strncpy(msgText, "Defeat all enemies first!",
                            sizeof(msgText)-1);
                    msgTimer    = 180;
                    doorCooldown = 180;
                } else {
                    doorCooldown = 60;

                    /* alternate quiz turn between players */
                    int passed = Enigme_Run(app, currentRoom);
                    quizTurn = 1 - quizTurn; /* flip for next time */

                    if (!app->running) break;

                    if (passed) {
                        if (currentRoom == MAX_ROOMS - 1) {
                            strncpy(msgText,
                                "You completed the game! Well done!",
                                sizeof(msgText)-1);
                            msgTimer   = 180;
                            app->state = STATE_MAIN_MENU;
                        } else {
                            currentRoom++;
                            p1.destRect.x = 400; p1.destRect.y = 400;
                            p2.destRect.x = 700; p2.destRect.y = 400;
                        }
                    } else {
                        strncpy(msgText, "Quiz failed! Restarting room...",
                                sizeof(msgText)-1);
                        msgTimer = 120;
                        reset_room_multi(app->renderer);
                        p1.destRect.x = 400; p1.destRect.y = 400;
                        p2.destRect.x = 700; p2.destRect.y = 400;
                    }
                }
            }
        }

        /* update enemies — chase whoever is closer */
        for (int i = 0; i < room->enemyCount; i++) {
            if (room->enemies[i].dead) continue;

            /* pick closer player as target */
            int ex = room->enemies[i].destRect.x;
            int ey = room->enemies[i].destRect.y;
            int d1 = abs(ex - p1.destRect.x) + abs(ey - p1.destRect.y);
            int d2 = abs(ex - p2.destRect.x) + abs(ey - p2.destRect.y);
            Player* target = (d1 <= d2) ? &p1 : &p2;

            poursuivreJoueur(&room->enemies[i], target, room->mask);
            animerEnnemi    (&room->enemies[i]);
        }

        playerAttackEnemies(&p1, room->enemies, room->enemyCount, tick);
        playerAttackEnemies(&p2, room->enemies, room->enemyCount, tick);
        MAJMinimap(&minimap, p1.destRect, currentRoom);

        /* render */
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
        SDL_RenderClear(app->renderer);

        SDL_RenderCopy(app->renderer, room->background, NULL, NULL);

        for (int i = 0; i < room->enemyCount; i++) {
            if (room->enemies[i].dead) continue;
            afficherEnnemi(room->enemies[i], app->renderer, camera);
        }

        renderPlayer(&p1, app->renderer);
        renderPlayer(&p2, app->renderer);

        /* HUD: P1 bottom-left (white), P2 bottom-right (cyan) */
        renderUI (&p1, app->renderer);
        renderUI2(&p2, app->renderer);

        /* minimap */
        SDL_Rect border = {minimapPos.x - 4, minimapPos.y - 4,
                           minimapPos.w + 8, minimapPos.h + 8};
        SDL_SetRenderDrawColor(app->renderer, 80, 80, 80, 255);
        SDL_RenderFillRect(app->renderer, &border);
        AfficherMinimap(app->renderer, minimap, currentRoom);

        /* message overlay */
        if (msgTimer > 0 && msgFont && msgText[0]) {
            SDL_Color red  = {255, 60, 60, 255};
            SDL_Surface* surf = TTF_RenderText_Blended(msgFont, msgText, red);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(app->renderer, surf);
                int w = surf->w, h = surf->h;
                SDL_FreeSurface(surf);
                SDL_Rect bg  = {SCREEN_WIDTH/2 - w/2 - 20,
                                SCREEN_HEIGHT/2 - h/2 - 15, w+40, h+30};
                SDL_Rect pos = {SCREEN_WIDTH/2 - w/2,
                                SCREEN_HEIGHT/2 - h/2, w, h};
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

    /* cleanup */
    if (msgFont) TTF_CloseFont(msgFont);
    destroyPlayer(&p1);
    destroyPlayer(&p2);
    LibererMinimap(&minimap);
    freeRooms();

    if (app->running && app->state == STATE_GAME_MULTI)
        app->state = STATE_MAIN_MENU;
}
