#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "common.h"

typedef struct {
    SDL_Texture* walk[DIRECTIONS][MAX_FRAMES];
    SDL_Texture* attack[DIRECTIONS][MAX_FRAMES];
    int row;
    int col;
    int frameCounter;
    int frameDelay;
    int moving;
    int attacking;
    int speed;
    int score;
    int lives;
    SDL_Rect destRect;
} Player;

int  initPlayer   (Player* player, SDL_Renderer* renderer);
void handleInput  (Player* player, const Uint8* keystate);
void updatePlayer (Player* player);
void renderPlayer (Player* player, SDL_Renderer* renderer);
void renderUI     (Player* player, SDL_Renderer* renderer);
void destroyPlayer(Player* player);

#endif
