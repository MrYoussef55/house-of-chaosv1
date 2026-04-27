#include "enemie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int checkCollision(SDL_Rect a, SDL_Rect b) {
    if (a.x + a.w <= b.x) return 0;
    if (a.x >= b.x + b.w) return 0;
    if (a.y + a.h <= b.y) return 0;
    if (a.y >= b.y + b.h) return 0;
    return 1;
}

void initEnnemi(Ennemi *e, SDL_Renderer *renderer) {
    SDL_Surface *tmp = NULL;

    tmp = IMG_Load("assets/ir1.png");
    if (tmp) {
        e->stand = SDL_CreateTextureFromSurface(renderer, tmp);
        SDL_FreeSurface(tmp);
    } else {
        printf("Erreur chargement stand.png : %s\n", IMG_GetError());
    }
    
    e->walk[0][0] = IMG_LoadTexture(renderer, "assets/wd1.png");
    e->walk[0][1] = IMG_LoadTexture(renderer, "assets/wd2.png");
    e->walk[0][2] = IMG_LoadTexture(renderer, "assets/wd3.png");
    e->walk[0][3] = IMG_LoadTexture(renderer, "assets/wd4.png");

    e->walk[1][0] = IMG_LoadTexture(renderer, "assets/wl1.png");
    e->walk[1][1] = IMG_LoadTexture(renderer, "assets/wl2.png");
    e->walk[1][2] = IMG_LoadTexture(renderer, "assets/wl3.png");
    e->walk[1][3] = IMG_LoadTexture(renderer, "assets/wl4.png");

    e->walk[2][0] = IMG_LoadTexture(renderer, "assets/wr1.png");
    e->walk[2][1] = IMG_LoadTexture(renderer, "assets/wr2.png");
    e->walk[2][2] = IMG_LoadTexture(renderer, "assets/wr3.png");
    e->walk[2][3] = IMG_LoadTexture(renderer, "assets/wr4.png");

    e->walk[3][0] = IMG_LoadTexture(renderer, "assets/wu1.png");
    e->walk[3][1] = IMG_LoadTexture(renderer, "assets/wu2.png");
    e->walk[3][2] = IMG_LoadTexture(renderer, "assets/wu3.png");
    e->walk[3][3] = IMG_LoadTexture(renderer, "assets/wu4.png");

    e->attack[0][0] = IMG_LoadTexture(renderer, "assets/fd1.png");
    e->attack[0][1] = IMG_LoadTexture(renderer, "assets/fd2.png");
    e->attack[0][2] = IMG_LoadTexture(renderer, "assets/fd3.png");
    e->attack[0][3] = IMG_LoadTexture(renderer, "assets/fd4.png");

    e->attack[1][0] = IMG_LoadTexture(renderer, "assets/fl1.png");
    e->attack[1][1] = IMG_LoadTexture(renderer, "assets/fl2.png");
    e->attack[1][2] = IMG_LoadTexture(renderer, "assets/fl3.png");
    e->attack[1][3] = IMG_LoadTexture(renderer, "assets/fl4.png");

    e->attack[2][0] = IMG_LoadTexture(renderer, "assets/fr1.png");
    e->attack[2][1] = IMG_LoadTexture(renderer, "assets/fr2.png");
    e->attack[2][2] = IMG_LoadTexture(renderer, "assets/fr3.png");
    e->attack[2][3] = IMG_LoadTexture(renderer, "assets/fr4.png");

    e->attack[3][0] = IMG_LoadTexture(renderer, "attack_up1.png");
    e->attack[3][1] = IMG_LoadTexture(renderer, "attack_up2.png");
    e->attack[3][2] = IMG_LoadTexture(renderer, "attack_up3.png");
    e->attack[3][3] = IMG_LoadTexture(renderer, "attack_up4.png");
    
    e->row = 0;
    e->col = 0;
    e->frameCounter = 0;
    e->frameDelay = 2;
    e->moving = 0;
    e->attacking = 0;
    e->speed = 3; 
    e->destRect.x = 200;
    e->destRect.y = 100;
    e->destRect.w = 64;
    e->destRect.h = 64;
    
    e->currentAction = -1;
    e->frame = 0;
    e->dx = 0;
    e->dy = 0;
    e->moveTimer = 0;
    
    e->health = 100;
    e->lastDamageTime = 0;
}

void deplacerAleatoire(Ennemi *player, SDL_Rect hazard) {
    if (player->currentAction == 4) return; // Don't move if dead

    player->moving = 1;
    player->speed = 3;

    player->moveTimer--;

    if (player->moveTimer <= 0) {
        player->dx = (rand() % 3) - 1; 
        player->dy = (rand() % 3) - 1;

        if (player->dx == 0 && player->dy == 0) {
            player->dx = 1; 
        }

        player->moveTimer = 30 + (rand() % 60);

        if (player->dy > 0) player->row = 0;      
        else if (player->dx < 0) player->row = 1; 
        else if (player->dx > 0) player->row = 2; 
        else if (player->dy < 0) player->row = 3; 
    }

    player->destRect.x += player->dx * player->speed;
    player->destRect.y += player->dy * player->speed;

    if (player->destRect.x < 0) { 
        player->destRect.x = 0; 
        player->dx = 1; 
        player->row = 2; 
    }
    if (player->destRect.x > 1280 - player->destRect.w) { 
        player->destRect.x = 1280 - player->destRect.w; 
        player->dx = -1; 
        player->row = 1; 
    }
    if (player->destRect.y < 0) { 
        player->destRect.y = 0; 
        player->dy = 1; 
        player->row = 0; 
    }
    if (player->destRect.y > 720 - player->destRect.h) { 
        player->destRect.y = 720 - player->destRect.h; 
        player->dy = -1; 
        player->row = 3; 
    }

    // Hazard Collision Check
    if (checkCollision(player->destRect, hazard)) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - player->lastDamageTime > 1000) { 
            player->health -= 15;
            player->lastDamageTime = currentTime;
            
            if (player->health <= 0) {
                mourir(player);
            }
        }
    }
}

void afficherEnnemi(Ennemi e, SDL_Renderer *renderer, SDL_Rect camera) {
    SDL_Texture* current = NULL;

    // Draw NPC
    if (e.attacking)
        current = e.attack[e.row][e.col];
    else
        current = e.walk[e.row][e.col];

    if (current && e.currentAction != 4) { // Only draw if not dead
        SDL_RenderCopy(renderer, current, NULL, &e.destRect);
    }

    // Draw Health Bar
    if (e.health > 0) {
        int maxBarWidth = 50; 
        int barHeight = 6;
        int currentBarWidth = (e.health * maxBarWidth) / 100; 

        SDL_Rect healthBar = {
            e.destRect.x + (e.destRect.w / 2) - (maxBarWidth / 2) - camera.x,
            e.destRect.y - 15 - camera.y,
            currentBarWidth, 
            barHeight
        };

        if (e.health > 66) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        } else if (e.health > 33) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        }
        
        SDL_RenderFillRect(renderer, &healthBar);
    }
}

void animerEnnemi(Ennemi *e) {
    if (e->currentAction == 4) return; // Dead don't animate

    e->frameCounter++;
    if (e->frameCounter >= e->frameDelay) {
        e->frameCounter = 0;
        e->col++;
        if (e->col >= MAX_FRAMES) {
            e->col = 0;
            if (e->attacking)
                e->attacking = 0;
        }
    }
    if (!e->moving && !e->attacking) {
        e->col = 0;
    }
}

void mourir(Ennemi *E) {
    if (E->currentAction != 4) {
        E->currentAction = 4;
        E->frame = 0;
        E->moving = 0;
    }
}
