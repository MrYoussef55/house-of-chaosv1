#ifndef ROOM_H
#define ROOM_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "common.h"
#include "enemiei.h"
#include "playeri.h"
#define ENEMIES_PER_ROOM 5
typedef struct {
    SDL_Texture*  background;
    SDL_Surface*  mask;
    Ennemi        enemies[ENEMIES_PER_ROOM];
    int           enemyCount;
    SDL_Rect      doors[4];
    int           doorCount;
    int           doorTargetRoom[4];
    SDL_Point     doorSpawnPoint[4];
} Room;
extern Room rooms[MAX_ROOMS];
extern int  currentRoom;
void initRooms     (SDL_Renderer* renderer);
void freeRooms     (void);
void checkDoorEntry(SDL_Rect playerRect, Player* player, Player* player2);
#endif
