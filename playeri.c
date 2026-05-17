#include "playeri.h"
#include "enemiei.h"
#include "collision.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

static TTF_Font* font = NULL;

static SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("Failed to load %s: %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

int initPlayer(Player* player, SDL_Renderer* renderer)
{
    player->walk[0][0] = loadTexture(renderer, "down1.png");
    player->walk[0][1] = loadTexture(renderer, "down2.png");
    player->walk[0][2] = loadTexture(renderer, "down3.png");
    player->walk[0][3] = loadTexture(renderer, "down4.png");

    player->walk[1][0] = loadTexture(renderer, "left1.png");
    player->walk[1][1] = loadTexture(renderer, "left2.png");
    player->walk[1][2] = loadTexture(renderer, "left3.png");
    player->walk[1][3] = loadTexture(renderer, "left4.png");

    player->walk[2][0] = loadTexture(renderer, "right1.png");
    player->walk[2][1] = loadTexture(renderer, "right2.png");
    player->walk[2][2] = loadTexture(renderer, "right3.png");
    player->walk[2][3] = loadTexture(renderer, "right4.png");

    player->walk[3][0] = loadTexture(renderer, "up1.png");
    player->walk[3][1] = loadTexture(renderer, "up2.png");
    player->walk[3][2] = loadTexture(renderer, "up3.png");
    player->walk[3][3] = loadTexture(renderer, "up4.png");

    player->attack[0][0] = loadTexture(renderer, "ad1.png");
    player->attack[0][1] = loadTexture(renderer, "ad2.png");
    player->attack[0][2] = loadTexture(renderer, "ad3.png");
    player->attack[0][3] = loadTexture(renderer, "ad4.png");

    player->attack[1][0] = loadTexture(renderer, "al1.png");
    player->attack[1][1] = loadTexture(renderer, "al2.png");
    player->attack[1][2] = loadTexture(renderer, "al3.png");
    player->attack[1][3] = loadTexture(renderer, "al4.png");

    player->attack[2][0] = loadTexture(renderer, "ar1.png");
    player->attack[2][1] = loadTexture(renderer, "ar2.png");
    player->attack[2][2] = loadTexture(renderer, "ar3.png");
    player->attack[2][3] = loadTexture(renderer, "ar4.png");

    player->attack[3][0] = loadTexture(renderer, "au1.png");
    player->attack[3][1] = loadTexture(renderer, "au2.png");
    player->attack[3][2] = loadTexture(renderer, "au3.png");
    player->attack[3][3] = loadTexture(renderer, "au4.png");

    player->deathAnim[0] = loadTexture(renderer, "death1.png");
    player->deathAnim[1] = loadTexture(renderer, "death2.png");
    player->deathAnim[2] = loadTexture(renderer, "death3.png");
    player->deathAnim[3] = loadTexture(renderer, "death4.png");

    player->row          = 0;
    player->col          = 0;
    player->frameCounter = 0;
    player->frameDelay   = 2;
    player->moving       = 0;
    player->attacking    = 0;
    player->dying        = 0;
    player->deathFrame   = 0;
    player->deathCounter = 0;
    player->deathDelay   = 18;
    player->dead         = 0;
    player->respawnTimer = 0;
    player->speed        = 3;
    player->score        = 0;
    player->lives        = 3;
    player->health       = 100;

    player->destRect.x = 500;
    player->destRect.y = 600;
    player->destRect.w = 64;
    player->destRect.h = 64;


    font = TTF_OpenFont("arial.ttf", 24);
    if (!font)
        printf("Failed to load font: %s\n", TTF_GetError());

    return 1;
}

void handleInput(Player* player, const Uint8* keystate, SDL_Surface* mask)
{
    if (player->dying || player->dead) return;
    if (player->attacking) return;

    player->moving = 0;
    player->speed  = keystate[SDL_SCANCODE_LSHIFT] ? 5 : 3;

    if (keystate[SDL_SCANCODE_S]) {
        player->row = 0;
        player->destRect.y += player->speed;
        if (CollisionParfaite(mask, player->destRect))
            player->destRect.y -= player->speed;
        else player->moving = 1;
    }
    if (keystate[SDL_SCANCODE_A]) {
        player->row = 1;
        player->destRect.x -= player->speed;
        if (CollisionParfaite(mask, player->destRect))
            player->destRect.x += player->speed;
        else player->moving = 1;
    }
    if (keystate[SDL_SCANCODE_D]) {
        player->row = 2;
        player->destRect.x += player->speed;
        if (CollisionParfaite(mask, player->destRect))
            player->destRect.x -= player->speed;
        else player->moving = 1;
    }
    if (keystate[SDL_SCANCODE_W]) {
        player->row = 3;
        player->destRect.y -= player->speed;
        if (CollisionParfaite(mask, player->destRect))
            player->destRect.y += player->speed;
        else player->moving = 1;
    }

    if (player->destRect.x < 0) player->destRect.x = 0;
    if (player->destRect.y < 0) player->destRect.y = 0;
    if (player->destRect.x > SCREEN_WIDTH  - player->destRect.w)
        player->destRect.x = SCREEN_WIDTH  - player->destRect.w;
    if (player->destRect.y > SCREEN_HEIGHT - player->destRect.h)
        player->destRect.y = SCREEN_HEIGHT - player->destRect.h;

    if (keystate[SDL_SCANCODE_SPACE] && !player->attacking) {
        player->attacking = 1;
        player->col       = 0;
    }
}

void updatePlayer(Player* player)
{
    if (player->health <= 0 && !player->dying && !player->dead) {
        player->dying        = 1;
        player->deathFrame   = 0;
        player->deathCounter = 0;
        player->moving       = 0;
        player->attacking    = 0;
        return;
    }

    if (player->dying) {
        player->deathCounter++;
        if (player->deathCounter >= player->deathDelay) {
            player->deathCounter = 0;
            player->deathFrame++;
            if (player->deathFrame >= DEATH_FRAMES) {
                player->deathFrame   = DEATH_FRAMES - 1;
                player->dying        = 0;
                player->dead         = 1;
                player->lives--;
                if (player->lives < 0) player->lives = 0;
                player->respawnTimer = 120;
            }
        }
        return;
    }

    if (player->dead) {
        player->respawnTimer--;
        if (player->respawnTimer <= 0) {
            if (player->lives > 0) {
                player->dead       = 0;
                player->health     = 100;
                player->deathFrame = 0;
                player->destRect.x = 600;
                player->destRect.y = 600;
            }
        }
        return;
    }

    player->frameCounter++;
    if (player->frameCounter >= player->frameDelay) {
        player->frameCounter = 0;
        player->col++;
        if (player->col >= 4) {
            player->col = 0;
            if (player->attacking)
                player->attacking = 0;
        }
    }
    if (!player->moving && !player->attacking)
        player->col = 0;
}

void renderPlayer(Player* player, SDL_Renderer* renderer)
{
    if (player->dying) {
        int f = player->deathFrame;
        if (f < 0) f = 0;
        if (f >= DEATH_FRAMES) f = DEATH_FRAMES - 1;
        SDL_Texture* df = player->deathAnim[f];
        if (df) SDL_RenderCopy(renderer, df, NULL, &player->destRect);
        return;
    }

    if (player->dead) {
        SDL_Texture* df = player->deathAnim[DEATH_FRAMES - 1];
        if (df) SDL_RenderCopy(renderer, df, NULL, &player->destRect);
        return;
    }

    SDL_Texture* current = NULL;
    if (player->attacking) {
        current = player->attack[player->row][player->col];
        if (!current) current = player->walk[player->row][player->col];
    } else {
        current = player->walk[player->row][player->col];
    }
    if (current)
        SDL_RenderCopy(renderer, current, NULL, &player->destRect);

    if (player->health > 0) {
        int maxW = 50, barH = 6;
        int curW = (player->health * maxW) / 100;
        SDL_Rect bar = {
            player->destRect.x + player->destRect.w / 2 - maxW / 2,
            player->destRect.y - 15,
            curW, barH
        };
        if      (player->health > 66) SDL_SetRenderDrawColor(renderer,   0, 255,   0, 255);
        else if (player->health > 33) SDL_SetRenderDrawColor(renderer, 255, 255,   0, 255);
        else                          SDL_SetRenderDrawColor(renderer, 255,   0,   0, 255);
        SDL_RenderFillRect(renderer, &bar);
    }
}

void renderUI(Player* player, SDL_Renderer* renderer)
{
    if (!font) return;
    char text[64];
    sprintf(text, "Score: %d  Lives: %d", player->score, player->lives);
    SDL_Color white   = {255, 255, 255, 255};
    SDL_Surface* surf = TTF_RenderText_Solid(font, text, white);
    SDL_Texture* tex  = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect rect     = {10, SCREEN_HEIGHT - surf->h - 10, surf->w, surf->h};
    SDL_FreeSurface(surf);
    SDL_RenderCopy(renderer, tex, NULL, &rect);
    SDL_DestroyTexture(tex);
}

void destroyPlayer(Player* player)
{
    for (int i = 0; i < DIRECTIONS; i++)
        for (int j = 0; j < MAX_FRAMES; j++) {
            SDL_DestroyTexture(player->walk  [i][j]);
            SDL_DestroyTexture(player->attack[i][j]);
        }
    for (int i = 0; i < DEATH_FRAMES; i++)
        SDL_DestroyTexture(player->deathAnim[i]);
    if (font) { TTF_CloseFont(font); font = NULL; }
}

void playerAttackEnemies(Player* player, void* enemiesPtr, int count, int tick)
{
    Ennemi* enemies = (Ennemi*)enemiesPtr;
    if (!player->attacking) return;
    if (player->col != 1) return;

    SDL_Rect hit = player->destRect;
    int reach = 60;
    switch (player->row) {
        case 0: hit.y += player->destRect.h; hit.h = reach; break;
        case 1: hit.x -= reach; hit.w = reach;              break;
        case 2: hit.x += player->destRect.w; hit.w = reach; break;
        case 3: hit.y -= reach; hit.h = reach;              break;
    }

    for (int i = 0; i < count; i++) {
        Ennemi* e = &enemies[i];
        if (e->dead) continue;

        int overlap = (hit.x < e->destRect.x + e->destRect.w &&
                       hit.x + hit.w > e->destRect.x &&
                       hit.y < e->destRect.y + e->destRect.h &&
                       hit.y + hit.h > e->destRect.y);

        if (overlap && (tick - e->lastDamageTime > 20)) {
            e->health -= 25;
            e->lastDamageTime = tick;
            if (e->health <= 0) {
                e->health = 0;
                e->dead   = 1;
                player->score += 10;
            }
        }
    }
}

/* --- Player 2: arrows to move, KP0 to attack --- */
void handleInput2(Player* player, const Uint8* keystate, SDL_Surface* mask)
{
    if (player->dying || player->dead) return;
    if (player->attacking) return;

    player->moving = 0;
    player->speed  = 3;

    if (keystate[SDL_SCANCODE_DOWN]) {
        player->row = 0;
        player->destRect.y += player->speed;
        if (CollisionParfaite(mask, player->destRect))
            player->destRect.y -= player->speed;
        else player->moving = 1;
    }
    if (keystate[SDL_SCANCODE_LEFT]) {
        player->row = 1;
        player->destRect.x -= player->speed;
        if (CollisionParfaite(mask, player->destRect))
            player->destRect.x += player->speed;
        else player->moving = 1;
    }
    if (keystate[SDL_SCANCODE_RIGHT]) {
        player->row = 2;
        player->destRect.x += player->speed;
        if (CollisionParfaite(mask, player->destRect))
            player->destRect.x -= player->speed;
        else player->moving = 1;
    }
    if (keystate[SDL_SCANCODE_UP]) {
        player->row = 3;
        player->destRect.y -= player->speed;
        if (CollisionParfaite(mask, player->destRect))
            player->destRect.y += player->speed;
        else player->moving = 1;
    }

    if (player->destRect.x < 0) player->destRect.x = 0;
    if (player->destRect.y < 0) player->destRect.y = 0;
    if (player->destRect.x > SCREEN_WIDTH  - player->destRect.w)
        player->destRect.x = SCREEN_WIDTH  - player->destRect.w;
    if (player->destRect.y > SCREEN_HEIGHT - player->destRect.h)
        player->destRect.y = SCREEN_HEIGHT - player->destRect.h;

    if (keystate[SDL_SCANCODE_KP_0] && !player->attacking) {
        player->attacking = 1;
        player->col       = 0;
    }
}

/* --- Player 2 UI: score/lives shown on the right side --- */
void renderUI2(Player* player, SDL_Renderer* renderer)
{
    if (!font) return;
    char text[64];
    sprintf(text, "P2  Score: %d  Lives: %d", player->score, player->lives);
    SDL_Color cyan    = {100, 220, 255, 255};
    SDL_Surface* surf = TTF_RenderText_Solid(font, text, cyan);
    if (!surf) return;
    SDL_Texture* tex  = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect rect     = {SCREEN_WIDTH - surf->w - 10,
                         SCREEN_HEIGHT - surf->h - 10,
                         surf->w, surf->h};
    SDL_FreeSurface(surf);
    SDL_RenderCopy(renderer, tex, NULL, &rect);
    SDL_DestroyTexture(tex);
}
