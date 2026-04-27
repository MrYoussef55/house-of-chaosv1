#ifndef ENEMIE_H_INCLUDED
#define ENEMIE_H_INCLUDED
#define MAX_FRAMES 4
#define DIRECTIONS 4

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h> 
#include <time.h>   

typedef struct Ennemi {
    SDL_Texture* walk[DIRECTIONS][MAX_FRAMES];
    SDL_Texture* attack[DIRECTIONS][MAX_FRAMES];
    SDL_Texture *stand;
    SDL_Rect destRect;   
    
    int frameCounter;
    int frameDelay;
    int row;
    int col;
    int frameTimer;      
    
    // Core Stats
    int health;
    int lastDamageTime; // The cooldown timer
    
    int moving;
    int attacking;
    int speed;
    int currentAction;
    int frame;
    
    // Random AI Variables
    int dx;
    int dy;
    int moveTimer;
} Ennemi;

void initEnnemi(Ennemi *e, SDL_Renderer *renderer);
void afficherEnnemi(Ennemi e, SDL_Renderer *renderer, SDL_Rect camera);
void animerEnnemi(Ennemi *e);
void mourir(Ennemi *E);
void deplacerAleatoire(Ennemi *player, SDL_Rect hazard);
int checkCollision(SDL_Rect a, SDL_Rect b);

#endif
