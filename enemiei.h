#ifndef ENEMIE_H
#define ENEMIE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "common.h"

typedef struct {
    SDL_Texture* walk  [DIRECTIONS][MAX_FRAMES];
    SDL_Texture* attack[DIRECTIONS][MAX_FRAMES];
    SDL_Texture* stand;
    SDL_Rect     destRect;

    int frameCounter;
    int frameDelay;
    int frameTimer;
    int row, col;

    int health;
    int lastDamageTime;

    int moving;
    int attacking;
    int speed;
    int currentAction;
    int frame;

    int dx, dy;
    int moveTimer;
} Ennemi;

void initEnnemi      (Ennemi* e, SDL_Renderer* renderer);
void afficherEnnemi  (Ennemi  e, SDL_Renderer* renderer, SDL_Rect camera);
void animerEnnemi    (Ennemi* e);
void mourir          (Ennemi* e);
void deplacerAleatoire(Ennemi* e, SDL_Rect hazard);
int  checkCollision  (SDL_Rect a, SDL_Rect b);

#endif
