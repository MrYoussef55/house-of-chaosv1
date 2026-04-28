#include "playeri.h"
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
    /* ---- walk frames ---- */
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

    /* ---- attack frames ---- */
    player->attack[0][0] = loadTexture(renderer, "attack_down1.png");
    player->attack[0][1] = loadTexture(renderer, "attack_down2.png");
    player->attack[0][2] = loadTexture(renderer, "attack_down3.png");
    player->attack[0][3] = loadTexture(renderer, "attack_down4.png");

    player->attack[1][0] = loadTexture(renderer, "attack_left1.png");
    player->attack[1][1] = loadTexture(renderer, "attack_left2.png");
    player->attack[1][2] = loadTexture(renderer, "attack_left3.png");
    player->attack[1][3] = loadTexture(renderer, "attack_left4.png");

    player->attack[2][0] = loadTexture(renderer, "attack_right1.png");
    player->attack[2][1] = loadTexture(renderer, "attack_right2.png");
    player->attack[2][2] = loadTexture(renderer, "attack_right3.png");
    player->attack[2][3] = loadTexture(renderer, "attack_right4.png");

    player->attack[3][0] = loadTexture(renderer, "attack_up1.png");
    player->attack[3][1] = loadTexture(renderer, "attack_up2.png");
    player->attack[3][2] = loadTexture(renderer, "attack_up3.png");
    player->attack[3][3] = loadTexture(renderer, "attack_up4.png");

    /* ---- state ---- */
    player->row          = 0;
    player->col          = 0;
    player->frameCounter = 0;
    player->frameDelay   = 2;
    player->moving       = 0;
    player->attacking    = 0;
    player->speed        = 2;
    player->score        = 0;
    player->lives        = 3;

    player->destRect.x = 300;
    player->destRect.y = 200;
    player->destRect.w = 64;
    player->destRect.h = 64;

    /* ---- font ---- */
    TTF_Init();
    font = TTF_OpenFont("arial.ttf", 24);
    if (!font)
        printf("Failed to load font: %s\n", TTF_GetError());

    return 1;
}

void handleInput(Player* player, const Uint8* keystate)
{
    player->moving = 0;

    player->speed = keystate[SDL_SCANCODE_LSHIFT] ? 4 : 2;

    if (keystate[SDL_SCANCODE_S]) { player->row = 0; player->destRect.y += player->speed; player->moving = 1; }
    if (keystate[SDL_SCANCODE_A]) { player->row = 1; player->destRect.x -= player->speed; player->moving = 1; }
    if (keystate[SDL_SCANCODE_D]) { player->row = 2; player->destRect.x += player->speed; player->moving = 1; }
    if (keystate[SDL_SCANCODE_W]) { player->row = 3; player->destRect.y -= player->speed; player->moving = 1; }

    if ((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && !player->attacking) {
        player->attacking = 1;
        player->col       = 0;
    }
}

void updatePlayer(Player* player)
{
    player->frameCounter++;
    if (player->frameCounter >= player->frameDelay) {
        player->frameCounter = 0;
        player->col++;
        if (player->col >= MAX_FRAMES) {
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
    SDL_Texture* current = player->attacking
        ? player->attack[player->row][player->col]
        : player->walk [player->row][player->col];

    SDL_RenderCopy(renderer, current, NULL, &player->destRect);
}

void renderUI(Player* player, SDL_Renderer* renderer)
{
    if (!font) return;

    char text[64];
    sprintf(text, "Score: %d  Lives: %d", player->score, player->lives);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surf = TTF_RenderText_Solid(font, text, white);
    SDL_Texture* tex  = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect rect     = {10, 10, surf->w, surf->h};
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

    if (font) { TTF_CloseFont(font); TTF_Quit(); }
}
