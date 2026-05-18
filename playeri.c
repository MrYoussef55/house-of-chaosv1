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

int initPlayer(Player* player, SDL_Renderer* renderer, int avatar)
{
    player->avatar = avatar;

    /* ── Walk frames ──────────────────────────────────────────────────── */
    const char* walkFiles[DIRECTIONS][4];
    if (avatar == 0) {
        const char* t[DIRECTIONS][4] = {
            {"down1.png",  "down2.png",  "down3.png",  "down4.png" },
            {"left1.png",  "left2.png",  "left3.png",  "left4.png" },
            {"right1.png", "right2.png", "right3.png", "right4.png"},
            {"up1.png",    "up2.png",    "up3.png",    "up4.png"   },
        };
        for (int i=0;i<DIRECTIONS;i++) for (int j=0;j<4;j++) walkFiles[i][j]=t[i][j];
    } else {
        const char* t[DIRECTIONS][4] = {
            {"yd1.png", "yd2.png", "yd3.png", "yd4.png"},
            {"yl1.png", "yl2.png", "yl3.png", "yl4.png"},
            {"yr1.png", "yr2.png", "yr3.png", "yr4.png"},
            {"yu1.png", "yu2.png", "yu3.png", "yu4.png"},
        };
        for (int i=0;i<DIRECTIONS;i++) for (int j=0;j<4;j++) walkFiles[i][j]=t[i][j];
    }
    for (int i=0;i<DIRECTIONS;i++) {
        for (int j=0;j<4;j++)
            player->walk[i][j] = loadTexture(renderer, walkFiles[i][j]);
        for (int j=4;j<MAX_FRAMES;j++)
            player->walk[i][j] = NULL;
    }

    /* ── Attack frames ────────────────────────────────────────────────── */
    const char* attackFiles[DIRECTIONS][4];
    if (avatar == 0) {
        const char* t[DIRECTIONS][4] = {
            {"ad1.png","ad2.png","ad3.png","ad4.png"},
            {"al1.png","al2.png","al3.png","al4.png"},
            {"ar1.png","ar2.png","ar3.png","ar4.png"},
            {"au1.png","au2.png","au3.png","au4.png"},
        };
        for (int i=0;i<DIRECTIONS;i++) for (int j=0;j<4;j++) attackFiles[i][j]=t[i][j];
    } else {
        const char* t[DIRECTIONS][4] = {
            {"yad1.png","yad2.png","yad3.png","yad4.png"},
            {"yal1.png","yal2.png","yal3.png","yal4.png"},
            {"yar1.png","yar2.png","yar3.png","yar4.png"},
            {"yau1.png","yau2.png","yau3.png","yau4.png"},
        };
        for (int i=0;i<DIRECTIONS;i++) for (int j=0;j<4;j++) attackFiles[i][j]=t[i][j];
    }
    for (int i=0;i<DIRECTIONS;i++) {
        for (int j=0;j<4;j++)
            player->attack[i][j] = loadTexture(renderer, attackFiles[i][j]);
        for (int j=4;j<MAX_FRAMES;j++)
            player->attack[i][j] = NULL;
    }

    /* ── Death frames ─────────────────────────────────────────────────── */
    const char* dn[DEATH_FRAMES];
    if (avatar == 0) {
        dn[0]="death1.png"; dn[1]="death2.png";
        dn[2]="death3.png"; dn[3]="death4.png";
    } else {
        dn[0]="ydeath1.png"; dn[1]="ydeath2.png";
        dn[2]="ydeath3.png"; dn[3]="ydeath4.png";
    }
    for (int i=0;i<DEATH_FRAMES;i++)
        player->deathAnim[i] = loadTexture(renderer, dn[i]);

    /* ── State ────────────────────────────────────────────────────────── */
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
    player->inputScheme  = INPUT_WASD;

    player->destRect.x = 600;
    player->destRect.y = 600;
    /* Both avatars rendered at the same 64×64 size regardless of
     * the source image dimensions — SDL_RenderCopy stretches/shrinks
     * to fill destRect, so this is all that matters for on-screen size. */
    player->destRect.w = 64;
    player->destRect.h = 64;

    TTF_Init();
    font = TTF_OpenFont("arial.ttf", 24);
    if (!font) printf("Failed to load font: %s\n", TTF_GetError());

    return 1;
}

void handleInput(Player* player, const Uint8* keystate, SDL_Surface* mask)
{
    if (player->dying || player->dead) return;
    if (player->attacking) return;

    player->moving = 0;

    SDL_Scancode up, down, left, right, atk, sprint;
    if (player->inputScheme == INPUT_WASD) {
        up=SDL_SCANCODE_W; down=SDL_SCANCODE_S;
        left=SDL_SCANCODE_A; right=SDL_SCANCODE_D;
        atk=SDL_SCANCODE_SPACE; sprint=SDL_SCANCODE_LSHIFT;
    } else {
        up=SDL_SCANCODE_UP; down=SDL_SCANCODE_DOWN;
        left=SDL_SCANCODE_LEFT; right=SDL_SCANCODE_RIGHT;
        atk=SDL_SCANCODE_RETURN; sprint=SDL_SCANCODE_RSHIFT;
    }

    player->speed = keystate[sprint] ? 5 : 3;

    if (keystate[down]) {
        player->row = 0;
        player->destRect.y += player->speed;
        if (CollisionParfaite(mask, player->destRect))
            player->destRect.y -= player->speed;
        else player->moving = 1;
    }
    if (keystate[left]) {
        player->row = 1;
        player->destRect.x -= player->speed;
        if (CollisionParfaite(mask, player->destRect))
            player->destRect.x += player->speed;
        else player->moving = 1;
    }
    if (keystate[right]) {
        player->row = 2;
        player->destRect.x += player->speed;
        if (CollisionParfaite(mask, player->destRect))
            player->destRect.x -= player->speed;
        else player->moving = 1;
    }
    if (keystate[up]) {
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

    if (keystate[atk] && !player->attacking) {
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
        if (player->respawnTimer <= 0 && player->lives > 0) {
            player->dead       = 0;
            player->health     = 100;
            player->deathFrame = 0;
            player->destRect.x = 600;
            player->destRect.y = 600;
        }
        return;
    }

    player->frameCounter++;
    if (player->frameCounter >= player->frameDelay) {
        player->frameCounter = 0;
        player->col++;
        if (player->col >= 4) {
            player->col = 0;
            if (player->attacking) player->attacking = 0;
        }
    }
    if (!player->moving && !player->attacking) player->col = 0;
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
    if (current) SDL_RenderCopy(renderer, current, NULL, &player->destRect);

    if (player->health > 0) {
        int maxW = 50, barH = 6;
        int curW = (player->health * maxW) / 100;
        SDL_Rect bar = {
            player->destRect.x + player->destRect.w/2 - maxW/2,
            player->destRect.y - 15, curW, barH
        };
        if      (player->health > 66) SDL_SetRenderDrawColor(renderer,   0,255,  0,255);
        else if (player->health > 33) SDL_SetRenderDrawColor(renderer, 255,255,  0,255);
        else                          SDL_SetRenderDrawColor(renderer, 255,  0,  0,255);
        SDL_RenderFillRect(renderer, &bar);
    }
}

void renderUI(Player* player, SDL_Renderer* renderer)
{
    if (!font) return;
    char text[64];
    sprintf(text, "Score: %d  Lives: %d  HP: %d",
            player->score, player->lives, player->health);
    SDL_Color    white = {255,255,255,255};
    SDL_Surface* surf  = TTF_RenderText_Solid(font, text, white);
    SDL_Texture* tex   = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect     rect  = {10, SCREEN_HEIGHT - surf->h - 10, surf->w, surf->h};
    SDL_FreeSurface(surf);
    SDL_RenderCopy(renderer, tex, NULL, &rect);
    SDL_DestroyTexture(tex);
}

void destroyPlayer(Player* player)
{
    for (int i=0;i<DIRECTIONS;i++)
        for (int j=0;j<MAX_FRAMES;j++) {
            if (player->walk  [i][j]) SDL_DestroyTexture(player->walk  [i][j]);
            if (player->attack[i][j]) SDL_DestroyTexture(player->attack[i][j]);
        }
    for (int i=0;i<DEATH_FRAMES;i++)
        if (player->deathAnim[i]) SDL_DestroyTexture(player->deathAnim[i]);
    if (font) { TTF_CloseFont(font); font = NULL; TTF_Quit(); }
}

void playerAttackEnemies(Player* player, void* enemiesPtr, int count, int tick)
{
    if (!player->attacking) return;
    if (player->col != 1)   return;

    Ennemi* enemies = (Ennemi*)enemiesPtr;
    SDL_Rect hit    = player->destRect;
    int      reach  = 60;
    switch (player->row) {
        case 0: hit.y += player->destRect.h; hit.h = reach; break;
        case 1: hit.x -= reach;              hit.w = reach; break;
        case 2: hit.x += player->destRect.w; hit.w = reach; break;
        case 3: hit.y -= reach;              hit.h = reach; break;
    }

    for (int i=0;i<count;i++) {
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
