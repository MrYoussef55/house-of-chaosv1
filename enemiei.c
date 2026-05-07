#include "enemiei.h"
#include "collision.h"
#include <stdio.h>
#include <stdlib.h>

void initEnnemi(Ennemi* e, SDL_Renderer* renderer)
{
    /* stand */
    SDL_Surface* tmp = IMG_Load("assets/ir1.png");
    if (tmp) { e->stand = SDL_CreateTextureFromSurface(renderer, tmp); SDL_FreeSurface(tmp); }
    else      { printf("stand load error: %s\n", IMG_GetError()); e->stand = NULL; }

    /* walk - 4 frames */
    const char* walkFiles[DIRECTIONS][MAX_FRAMES] = {
        {"assets/wd1.png","assets/wd2.png","assets/wd3.png","assets/wd4.png","assets/wd4.png","assets/wd4.png"},
        {"assets/wl1.png","assets/wl2.png","assets/wl3.png","assets/wl4.png","assets/wl4.png","assets/wl4.png"},
        {"assets/wr1.png","assets/wr2.png","assets/wr3.png","assets/wr4.png","assets/wr4.png","assets/wr4.png"},
        {"assets/wu1.png","assets/wu2.png","assets/wu3.png","assets/wu4.png","assets/wu4.png","assets/wu4.png"},
    };
    /* attack - 6 frames */
    const char* attackFiles[DIRECTIONS][MAX_FRAMES] = {
        {"assets/fd1.png","assets/fd2.png","assets/fd3.png","assets/fd4.png","assets/fd5.png","assets/fd6.png"},
        {"assets/fl1.png","assets/fl2.png","assets/fl3.png","assets/fl4.png","assets/fl5.png","assets/fl6.png"},
        {"assets/fr1.png","assets/fr2.png","assets/fr3.png","assets/fr4.png","assets/fr5.png","assets/fr6.png"},
        {"assets/fu1.png","assets/fu2.png","assets/fu3.png","assets/fu4.png","assets/fu5.png","assets/fu6.png"},
    };

    for (int i = 0; i < DIRECTIONS; i++)
        for (int j = 0; j < MAX_FRAMES; j++) {
            e->walk  [i][j] = IMG_LoadTexture(renderer, walkFiles  [i][j]);
            e->attack[i][j] = IMG_LoadTexture(renderer, attackFiles[i][j]);
        }

    /* state */
    e->row          = 0;  e->col          = 0;
    e->frameCounter = 0;  e->frameDelay   = 3;
    e->frameTimer   = 0;
    e->moving       = 0;  e->attacking    = 0;
    e->attackCooldown = 60;   /* 1 second at 60fps between attacks */
    e->attackTimer    = 0;
    e->speed        = 2;

    e->destRect = (SDL_Rect){200, 100, 64, 64};

    e->currentAction = -1;
    e->frame         = 0;
    e->dx = 0; e->dy = 0;
    e->moveTimer     = 0;

    e->health         = 100;
    e->lastDamageTime = 0;
}

/*
 * Chase and attack the player.
 * - Within ATTACK_RANGE pixels: stop moving, play attack animation, deal damage.
 * - Outside range: move toward player with wall collision.
 */
#define ATTACK_RANGE 60
#define ATTACK_DAMAGE 10

void poursuivreJoueur(Ennemi* e, SDL_Rect playerRect, SDL_Surface* mask, int* playerHealth)
{
    if (e->currentAction == 4) return;

    int ex = e->destRect.x + e->destRect.w / 2;
    int ey = e->destRect.y + e->destRect.h / 2;
    int px = playerRect.x  + playerRect.w  / 2;
    int py = playerRect.y  + playerRect.h  / 2;

    int diffX = px - ex;
    int diffY = py - ey;

    /* Euclidean distance to player */
    float dist = SDL_sqrtf((float)(diffX * diffX + diffY * diffY));

    /* always face the player */
    int absDx = diffX < 0 ? -diffX : diffX;
    int absDy = diffY < 0 ? -diffY : diffY;
    if (absDy >= absDx) {
        e->row = (diffY > 0) ? 0 : 3;   /* down / up   */
    } else {
        e->row = (diffX < 0) ? 1 : 2;   /* left / right */
    }

    /* countdown attack cooldown every frame */
    if (e->attackTimer > 0) e->attackTimer--;

    if (dist <= ATTACK_RANGE) {
        /* --- in attack range --- */
        e->moving = 0;

        /* start a new attack when cooldown is ready and not already attacking */
        if (!e->attacking && e->attackTimer <= 0) {
            e->attacking    = 1;
            e->col          = 0;
            e->frameCounter = 0;
            e->attackTimer  = e->attackCooldown;

            /* deal damage on attack start */
            if (playerHealth) {
                *playerHealth -= ATTACK_DAMAGE;
                if (*playerHealth < 0) *playerHealth = 0;
            }
        }

    } else {
        /* --- chase player --- */
        if (!e->attacking) {   /* don't move mid-attack */
            int dx = (diffX > 0) - (diffX < 0);
            int dy = (diffY > 0) - (diffY < 0);

            e->moving = 1;

            /* horizontal */
            e->destRect.x += dx * e->speed;
            if (e->destRect.x < 0)                            e->destRect.x = 0;
            if (e->destRect.x > SCREEN_WIDTH - e->destRect.w) e->destRect.x = SCREEN_WIDTH - e->destRect.w;
            if (CollisionParfaite(mask, e->destRect))          e->destRect.x -= dx * e->speed;

            /* vertical */
            e->destRect.y += dy * e->speed;
            if (e->destRect.y < 0)                             e->destRect.y = 0;
            if (e->destRect.y > SCREEN_HEIGHT - e->destRect.h) e->destRect.y = SCREEN_HEIGHT - e->destRect.h;
            if (CollisionParfaite(mask, e->destRect))           e->destRect.y -= dy * e->speed;
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
            e.destRect.x + e.destRect.w / 2 - maxW / 2 - camera.x,
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

        int maxFrames = e->attacking ? MAX_FRAMES : 4;  /* walk uses 4, attack uses 6 */
        if (e->col >= maxFrames) {
            e->col = 0;
            if (e->attacking) e->attacking = 0;   /* attack animation finished */
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
