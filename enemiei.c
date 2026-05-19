#include "enemiei.h"
#include "collision.h"
#include "playeri.h"
#include <stdio.h>
#include <stdlib.h>

#define WALK_FRAMES_D  8
#define WALK_FRAMES_L  9
#define WALK_FRAMES_R  9
#define WALK_FRAMES_U  8
#define ATTACK_FRAMES  6

void initEnnemi(Ennemi* e, SDL_Renderer* renderer)
{
    SDL_Surface* tmp = IMG_Load("assets/ir1.png");
    if (tmp) { e->stand = SDL_CreateTextureFromSurface(renderer, tmp); SDL_FreeSurface(tmp); }
    else      { printf("stand load error: %s\n", IMG_GetError()); e->stand = NULL; }

    const char* walkFiles[DIRECTIONS][MAX_FRAMES] = {
        {"assets/wd1.png","assets/wd2.png","assets/wd3.png","assets/wd4.png","assets/wd5.png","assets/wd6.png","assets/wd7.png","assets/wd8.png",NULL},
        {"assets/wl1.png","assets/wl2.png","assets/wl3.png","assets/wl4.png","assets/wl5.png","assets/wl6.png","assets/wl7.png","assets/wl8.png","assets/wl9.png"},
        {"assets/wr1.png","assets/wr2.png","assets/wr3.png","assets/wr4.png","assets/wr5.png","assets/wr6.png","assets/wr7.png","assets/wr8.png","assets/wr9.png"},
        {"assets/wu1.png","assets/wu2.png","assets/wu3.png","assets/wu4.png","assets/wu5.png","assets/wu6.png","assets/wu7.png","assets/wu8.png",NULL},
    };
    const char* attackFiles[DIRECTIONS][MAX_FRAMES] = {
        {"assets/fd1.png","assets/fd2.png","assets/fd3.png","assets/fd4.png","assets/fd5.png","assets/fd6.png",NULL,NULL,NULL},
        {"assets/fl1.png","assets/fl2.png","assets/fl3.png","assets/fl4.png","assets/fl5.png","assets/fl6.png",NULL,NULL,NULL},
        {"assets/fr1.png","assets/fr2.png","assets/fr3.png","assets/fr4.png","assets/fr5.png","assets/fr6.png",NULL,NULL,NULL},
        {"assets/fu1.png","assets/fu2.png","assets/fu3.png","assets/fu4.png","assets/fu5.png","assets/fu6.png",NULL,NULL,NULL},
    };

    for (int i = 0; i < DIRECTIONS; i++)
        for (int j = 0; j < MAX_FRAMES; j++) {
            e->walk  [i][j] = walkFiles  [i][j] ? IMG_LoadTexture(renderer, walkFiles  [i][j]) : NULL;
            e->attack[i][j] = attackFiles[i][j] ? IMG_LoadTexture(renderer, attackFiles[i][j]) : NULL;
        }

    e->row            = 0;  e->col           = 0;
    e->frameCounter   = 0;  e->frameDelay    = 3;
    e->frameTimer     = 0;
    e->moving         = 0;  e->attacking     = 0;
    e->attackCooldown = 60;
    e->attackTimer    = 0;
    e->speed          = 2;
    e->destRect       = (SDL_Rect){200, 100, 64, 64};
    e->currentAction  = -1;
    e->frame          = 0;
    e->dx = 0; e->dy  = 0;
    e->moveTimer      = 0;
    e->health         = 100;
    e->lastDamageTime = 0;
    e->dead           = 0;
}

#define ATTACK_RANGE  60
#define ATTACK_DAMAGE 10

/*
 * A player is a valid target only if:
 *   - active == 1   (not a disabled solo P2)
 *   - not dying
 *   - not dead
 */
static int isValidTarget(Player* p)
{
    return p->active && !p->dying && !p->dead;
}

void poursuivreJoueur(Ennemi* e, Player* player, Player* player2, SDL_Surface* mask)
{
    if (e->dead) return;
    if (e->currentAction == 4) return;

    /* pick nearest valid (active + alive) player */
    Player* target = NULL;
    float dist1 = 999999.0f, dist2 = 999999.0f;

    if (isValidTarget(player)) {
        int dx = player->destRect.x - e->destRect.x;
        int dy = player->destRect.y - e->destRect.y;
        dist1 = SDL_sqrtf((float)(dx*dx + dy*dy));
    }
    if (isValidTarget(player2)) {
        int dx = player2->destRect.x - e->destRect.x;
        int dy = player2->destRect.y - e->destRect.y;
        dist2 = SDL_sqrtf((float)(dx*dx + dy*dy));
    }

    if (dist1 <= dist2 && dist1 < 999999.0f)
        target = player;
    else if (dist2 < 999999.0f)
        target = player2;

    if (!target) {
        e->moving    = 0;
        e->attacking = 0;
        return;
    }

    int ex = e->destRect.x + e->destRect.w / 2;
    int ey = e->destRect.y + e->destRect.h / 2;
    int px = target->destRect.x + target->destRect.w / 2;
    int py = target->destRect.y + target->destRect.h / 2;

    int diffX = px - ex;
    int diffY = py - ey;
    float dist = SDL_sqrtf((float)(diffX*diffX + diffY*diffY));

    int absDx = diffX < 0 ? -diffX : diffX;
    int absDy = diffY < 0 ? -diffY : diffY;
    if (absDy >= absDx)
        e->row = (diffY > 0) ? 0 : 3;
    else
        e->row = (diffX < 0) ? 1 : 2;

    if (e->attackTimer > 0) e->attackTimer--;

    if (dist <= ATTACK_RANGE) {
        e->moving = 0;
        if (!e->attacking && e->attackTimer <= 0) {
            e->attacking    = 1;
            e->col          = 0;
            e->frameCounter = 0;
            e->attackTimer  = e->attackCooldown;
            if (target->health > 0) {
                target->health -= ATTACK_DAMAGE;
                if (target->health < 0) target->health = 0;
            }
        }
    } else {
        if (!e->attacking) {
            int dx = (diffX > 0) - (diffX < 0);
            int dy = (diffY > 0) - (diffY < 0);
            e->moving = 1;

            e->destRect.x += dx * e->speed;
            if (e->destRect.x < 0)                             e->destRect.x = 0;
            if (e->destRect.x > SCREEN_WIDTH - e->destRect.w)  e->destRect.x = SCREEN_WIDTH - e->destRect.w;
            if (CollisionParfaite(mask, e->destRect))           e->destRect.x -= dx * e->speed;

            e->destRect.y += dy * e->speed;
            if (e->destRect.y < 0)                             e->destRect.y = 0;
            if (e->destRect.y > SCREEN_HEIGHT - e->destRect.h) e->destRect.y = SCREEN_HEIGHT - e->destRect.h;
            if (CollisionParfaite(mask, e->destRect))           e->destRect.y -= dy * e->speed;
        }
    }
}

void afficherEnnemi(Ennemi e, SDL_Renderer* renderer, SDL_Rect camera)
{
    if (e.dead) return;

    SDL_Texture* current = e.attacking ? e.attack[e.row][e.col]
                                       : e.walk  [e.row][e.col];

    if (current && e.currentAction != 4)
        SDL_RenderCopy(renderer, current, NULL, &e.destRect);

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

static const int walkFrameCount[DIRECTIONS] = {8, 9, 9, 8};

void animerEnnemi(Ennemi* e)
{
    if (e->dead) return;
    if (e->currentAction == 4) return;

    e->frameCounter++;
    if (e->frameCounter >= e->frameDelay) {
        e->frameCounter = 0;
        e->col++;
        int maxFrames = e->attacking ? ATTACK_FRAMES : walkFrameCount[e->row];
        if (e->col >= maxFrames) {
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
        e->dead          = 1;
    }
}
