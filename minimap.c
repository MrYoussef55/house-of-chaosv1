#include "minimap.h"
#include "common.h"
#include <stdio.h>

void InitMinimap(Minimap* m, SDL_Renderer* renderer,
                 const char* mapPaths[], SDL_Texture* playerSprite,
                 SDL_Rect minimapPos)
{
    for (int i = 0; i < MAX_MINIMAP_ROOMS; i++) {
        SDL_Surface* s = IMG_Load(mapPaths[i]);
        if (s) {
            m->backgroundTextures[i] = SDL_CreateTextureFromSurface(renderer, s);
            SDL_FreeSurface(s);
        } else {
            printf("Minimap room %d failed: %s\n", i, IMG_GetError());
            m->backgroundTextures[i] = NULL;
        }
    }

    m->minimapPosition   = minimapPos;
    m->playerDot         = playerSprite;
    m->playerDotPosition = (SDL_Rect){minimapPos.x, minimapPos.y, 16, 16};
}

void MAJMinimap(Minimap* m, SDL_Rect playerPos, int currentRoom)
{
    (void)currentRoom;

    m->playerDotPosition.x = m->minimapPosition.x
        + (playerPos.x * m->minimapPosition.w) / SCREEN_WIDTH;
    m->playerDotPosition.y = m->minimapPosition.y
        + (playerPos.y * m->minimapPosition.h) / SCREEN_HEIGHT;

    if (m->playerDotPosition.x < m->minimapPosition.x)
        m->playerDotPosition.x = m->minimapPosition.x;
    if (m->playerDotPosition.x > m->minimapPosition.x + m->minimapPosition.w - m->playerDotPosition.w)
        m->playerDotPosition.x = m->minimapPosition.x + m->minimapPosition.w - m->playerDotPosition.w;
    if (m->playerDotPosition.y < m->minimapPosition.y)
        m->playerDotPosition.y = m->minimapPosition.y;
    if (m->playerDotPosition.y > m->minimapPosition.y + m->minimapPosition.h - m->playerDotPosition.h)
        m->playerDotPosition.y = m->minimapPosition.y + m->minimapPosition.h - m->playerDotPosition.h;
}

void AfficherMinimap(SDL_Renderer* renderer, Minimap m, int currentRoom)
{
    if (m.backgroundTextures[currentRoom])
        SDL_RenderCopy(renderer, m.backgroundTextures[currentRoom], NULL, &m.minimapPosition);

    if (m.playerDot)
        SDL_RenderCopy(renderer, m.playerDot, NULL, &m.playerDotPosition);
}

void LibererMinimap(Minimap* m)
{
    for (int i = 0; i < MAX_MINIMAP_ROOMS; i++) {
        if (m->backgroundTextures[i]) {
            SDL_DestroyTexture(m->backgroundTextures[i]);
            m->backgroundTextures[i] = NULL;
        }
    }
    m->playerDot = NULL;
}
