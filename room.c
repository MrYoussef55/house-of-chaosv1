#include "room.h"
#include "playeri.h"
#include "collision.h"   // <-- add this
#include <stdio.h>

Room rooms[MAX_ROOMS];
int  currentRoom = 0;
static int doorCooldown = 0;

static void setupDoors(void)
{
    /* Room 0 → Room 1 */
    rooms[0].doorCount         = 1;
    rooms[0].doors[0]          = (SDL_Rect){870, 640, 110, 80};
    rooms[0].doorTargetRoom[0] = 1;
    rooms[0].doorSpawnPoint[0] = (SDL_Point){870, 600};

    /* Room 1 → Room 2 */
    rooms[1].doorCount         = 1;
    rooms[1].doors[0]          = (SDL_Rect){870, 640, 110, 80};
    rooms[1].doorTargetRoom[0] = 2;
    rooms[1].doorSpawnPoint[0] = (SDL_Point){870, 600};

    /* Room 2 → Room 3 */
    rooms[2].doorCount         = 1;
    rooms[2].doors[0]          = (SDL_Rect){870, 640, 110, 80};
    rooms[2].doorTargetRoom[0] = 3;
    rooms[2].doorSpawnPoint[0] = (SDL_Point){870, 600};

    /* Room 3 → Room 0 */
    rooms[3].doorCount         = 1;
    rooms[3].doors[0]          = (SDL_Rect){870, 640, 110, 80};
    rooms[3].doorTargetRoom[0] = 0;
    rooms[3].doorSpawnPoint[0] = (SDL_Point){870, 600};
}

static void spawnEnemySafe(Ennemi* e, SDL_Renderer* renderer,
                            SDL_Surface* mask, int x, int y)
{
    initEnnemi(e, renderer);
    e->destRect.x = x;
    e->destRect.y = y;

    /* if spawn is in collision, nudge right until free */
    int tries = 0;
    while (mask && CollisionParfaite(mask, e->destRect) && tries < 200) {
        e->destRect.x += 4;
        if (e->destRect.x > SCREEN_WIDTH - e->destRect.w) {
            e->destRect.x = x;
            e->destRect.y += 4;
        }
        tries++;
    }
}

static void setupEnemies(SDL_Renderer* renderer)
{
    /* --- Room 0: open floor areas visible in screenshot --- */
    rooms[0].enemyCount = 3;
    spawnEnemySafe(&rooms[0].enemies[0], renderer, rooms[0].mask, 500, 400);
    spawnEnemySafe(&rooms[0].enemies[1], renderer, rooms[0].mask, 650, 450);
    spawnEnemySafe(&rooms[0].enemies[2], renderer, rooms[0].mask, 750, 400);

    /* --- Room 1: center open floor --- */
    rooms[1].enemyCount = 3;
    spawnEnemySafe(&rooms[1].enemies[0], renderer, rooms[1].mask, 350, 350);
    spawnEnemySafe(&rooms[1].enemies[1], renderer, rooms[1].mask, 550, 400);
    spawnEnemySafe(&rooms[1].enemies[2], renderer, rooms[1].mask, 700, 350);

    /* --- Room 2: center open floor --- */
    rooms[2].enemyCount = 3;
    spawnEnemySafe(&rooms[2].enemies[0], renderer, rooms[2].mask, 400, 400);
    spawnEnemySafe(&rooms[2].enemies[1], renderer, rooms[2].mask, 600, 450);
    spawnEnemySafe(&rooms[2].enemies[2], renderer, rooms[2].mask, 750, 400);

    /* --- Room 3: center open floor --- */
    rooms[3].enemyCount = 3;
    spawnEnemySafe(&rooms[3].enemies[0], renderer, rooms[3].mask, 400, 300);
    spawnEnemySafe(&rooms[3].enemies[1], renderer, rooms[3].mask, 600, 350);
    spawnEnemySafe(&rooms[3].enemies[2], renderer, rooms[3].mask, 750, 300);
}

void initRooms(SDL_Renderer* renderer)
{
    const char* bgFiles[MAX_ROOMS] = {
        "background.png",
        "background2.png",
        "background3.png",
        "background4.png"
    };
    const char* maskFiles[MAX_ROOMS] = {
        "backgroundmask2.png",
        "backgroundmask_room1.png",
        "backgroundmask_room2.png",
        "backgroundmask_room3.png"
    };

    for (int i = 0; i < MAX_ROOMS; i++) {
        rooms[i].background = IMG_LoadTexture(renderer, bgFiles[i]);
        if (!rooms[i].background)
            printf("Room %d background failed: %s\n", i, IMG_GetError());

        rooms[i].mask = IMG_Load(maskFiles[i]);
        if (!rooms[i].mask)
            printf("Room %d mask failed: %s\n", i, IMG_GetError());
    }

    setupDoors();
    setupEnemies(renderer);
}

void freeRooms(void)
{
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (rooms[i].background) SDL_DestroyTexture(rooms[i].background);
        if (rooms[i].mask)       SDL_FreeSurface(rooms[i].mask);
    }
}

void checkDoorEntry(SDL_Rect playerRect, Player* player)
{
    if (player->dying || player->dead) return;

    if (doorCooldown > 0) {
        doorCooldown--;
        return;
    }

    Room* r = &rooms[currentRoom];
    for (int d = 0; d < r->doorCount; d++) {
        SDL_Rect door = r->doors[d];
        if (playerRect.x < door.x + door.w &&
            playerRect.x + playerRect.w > door.x &&
            playerRect.y < door.y + door.h &&
            playerRect.y + playerRect.h > door.y)
        {
            currentRoom        = r->doorTargetRoom[d];
            player->destRect.x = r->doorSpawnPoint[d].x;
            player->destRect.y = r->doorSpawnPoint[d].y;
            doorCooldown       = 60;
            return;
        }
    }
}
