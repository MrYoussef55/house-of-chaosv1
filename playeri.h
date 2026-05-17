#ifndef PLAYER_H
#define PLAYER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "common.h"

#define DEATH_FRAMES 4

typedef struct {
    SDL_Texture* walk  [DIRECTIONS][MAX_FRAMES];
    SDL_Texture* attack[DIRECTIONS][MAX_FRAMES];
    SDL_Texture* deathAnim[DEATH_FRAMES];
    int row;
    int col;
    int frameCounter;
    int frameDelay;
    int moving;
    int attacking;
    int dying;
    int deathFrame;
    int deathCounter;
    int deathDelay;
    int dead;
    int respawnTimer;
    int speed;
    int score;
    int lives;
    int health;
    SDL_Rect destRect;
} Player;

int  initPlayer          (Player* player, SDL_Renderer* renderer);
void handleInput         (Player* player, const Uint8* keystate, SDL_Surface* mask);
void handleInput2        (Player* player, const Uint8* keystate, SDL_Surface* mask);
void updatePlayer        (Player* player);
void renderPlayer        (Player* player, SDL_Renderer* renderer);
void renderUI            (Player* player, SDL_Renderer* renderer);
void renderUI2           (Player* player, SDL_Renderer* renderer);
void destroyPlayer       (Player* player);
void playerAttackEnemies (Player* player, void* enemies, int count, int tick);

#endif
