#include "enemiei.h"
#include <stdio.h>
#include <stdlib.h>

int checkCollision(SDL_Rect a, SDL_Rect b)
{
    return !(a.x + a.w <= b.x || a.x >= b.x + b.w ||
             a.y + a.h <= b.y || a.y >= b.y + b.h);
}

void initEnnemi(Ennemi* e, SDL_Renderer* renderer)
{
    /* stand */
    SDL_Surface* tmp = IMG_Load("assets/ir1.png");
    if (tmp) { e->stand = SDL_CreateTextureFromSurface(renderer, tmp); SDL_FreeSurface(tmp); }
    else      { printf("stand load error: %s\n", IMG_GetError()); e->stand = NULL; }

    /* walk */
    const char* walkFiles[DIRECTIONS][MAX_FRAMES] = {
        {"assets/wd1.png","assets/wd2.png","assets/wd3.png","assets/wd4.png"},
        {"assets/wl1.png","assets/wl2.png","assets/wl3.png","assets/wl4.png"},
        {"assets/wr1.png","assets/wr2.png","assets/wr3.png","assets/wr4.png"},
        {"assets/wu1.png","assets/wu2.png","assets/wu3.png","assets/wu4.png"},
    };
    /* attack */
    const char* attackFiles[DIRECTIONS][MAX_FRAMES] = {
        {"assets/fd1.png","assets/fd2.png","assets/fd3.png","assets/fd4.png"},
        {"assets/fl1.png","assets/fl2.png","assets/fl3.png","assets/fl4.png"},
        {"assets/fr1.png","assets/fr2.png","assets/fr3.png","assets/fr4.png"},
        {"attack_up1.png","attack_up2.png","attack_up3.png","attack_up4.png"},
    };

    for (int i = 0; i < DIRECTIONS; i++)
        for (int j = 0; j < MAX_FRAMES; j++) {
            e->walk  [i][j] = IMG_LoadTexture(renderer, walkFiles  [i][j]);
            e->attack[i][j] = IMG_LoadTexture(renderer, attackFiles[i][j]);
        }

    /* state */
    e->row          = 0;  e->col          = 0;
    e->frameCounter = 0;  e->frameDelay   = 2;
    e->frameTimer   = 0;
    e->moving       = 0;  e->attacking    = 0;
    e->speed        = 3;

    e->destRect = (SDL_Rect){200, 100, 64, 64};

    e->currentAction  = -1;
    e->frame          = 0;
    e->dx = 0;  e->dy = 0;
    e->moveTimer      = 0;

    e->health         = 100;
    e->lastDamageTime = 0;
}

void deplacerAleatoire(Ennemi* e, SDL_Rect hazard)
{
    if (e->currentAction == 4) return;

    e->moving = 1;
    e->speed  = 3;
    e->moveTimer--;

    if (e->moveTimer <= 0) {
        e->dx = (rand() % 3) - 1;
        e->dy = (rand() % 3) - 1;
        if (e->dx == 0 && e->dy == 0) e->dx = 1;
        e->moveTimer = 30 + (rand() % 60);

        if      (e->dy > 0) e->row = 0;
        else if (e->dx < 0) e->row = 1;
        else if (e->dx > 0) e->row = 2;
        else                e->row = 3;
    }

    e->destRect.x += e->dx * e->speed;
    e->destRect.y += e->dy * e->speed;

    /* boundary clamp */
    if (e->destRect.x < 0)               { e->destRect.x = 0;                       e->dx =  1; e->row = 2; }
    if (e->destRect.x > 1280-e->destRect.w){ e->destRect.x = 1280-e->destRect.w;    e->dx = -1; e->row = 1; }
    if (e->destRect.y < 0)               { e->destRect.y = 0;                       e->dy =  1; e->row = 0; }
    if (e->destRect.y > 720-e->destRect.h) { e->destRect.y = 720-e->destRect.h;     e->dy = -1; e->row = 3; }

    /* hazard damage */
    if (checkCollision(e->destRect, hazard)) {
        Uint32 now = SDL_GetTicks();
        if (now - (Uint32)e->lastDamageTime > 1000) {
            e->health -= 15;
            e->lastDamageTime = (int)now;
            if (e->health <= 0) mourir(e);
        }
    }
}

void afficherEnnemi(Ennemi e, SDL_Renderer* renderer, SDL_Rect camera)
{
    SDL_Texture* current = e.attacking ? e.attack[e.row][e.col]
                                       : e.walk  [e.row][e.col];

    if (current && e.currentAction != 4)
        SDL_RenderCopy(renderer, current, NULL, &e.destRect);

    /* health bar */
    if (e.health > 0) {
        int maxW = 50, barH = 6;
        int curW = (e.health * maxW) / 100;

        SDL_Rect bar = {
            e.destRect.x + e.destRect.w/2 - maxW/2 - camera.x,
            e.destRect.y - 15 - camera.y,
            curW, barH
        };

        if      (e.health > 66) SDL_SetRenderDrawColor(renderer,   0, 255,   0, 255);
        else if (e.health > 33) SDL_SetRenderDrawColor(renderer, 255, 255,   0, 255);
        else                    SDL_SetRenderDrawColor(renderer, 255,   0,   0, 255);

        SDL_RenderFillRect(renderer, &bar);
    }
}

void animerEnnemi(Ennemi* e)
{
    if (e->currentAction == 4) return;

    e->frameCounter++;
    if (e->frameCounter >= e->frameDelay) {
        e->frameCounter = 0;
        e->col++;
        if (e->col >= MAX_FRAMES) {
            e->col = 0;
            if (e->attacking) e->attacking = 0;
        }
    }
    if (!e->moving && !e->attacking) e->col = 0;
}

void mourir(Ennemi* e)
{
    if (e->currentAction != 4) {
        e->currentAction = 4;
        e->frame         = 0;
        e->moving        = 0;
    }
}
