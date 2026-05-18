#include "minimap.h"
#include "common.h"
#include <stdio.h>

void InitMinimap(Minimap* m, SDL_Renderer* renderer,
                 const char* mapPaths[], SDL_Texture* playerSprite,
                 SDL_Texture* playerSprite2, SDL_Rect minimapPos)
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
    m->minimapPosition    = minimapPos;
    m->playerDot          = playerSprite;
    m->playerDot2         = playerSprite2;
    m->playerDotPosition  = (SDL_Rect){minimapPos.x,      minimapPos.y, 16, 16};
    m->playerDot2Position = (SDL_Rect){minimapPos.x + 16, minimapPos.y, 16, 16};
}

void MAJMinimap(Minimap* m, SDL_Rect playerPos, SDL_Rect player2Pos, int currentRoom)
{
    (void)currentRoom;

    /* player 1 dot */
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

    /* player 2 dot */
    m->playerDot2Position.x = m->minimapPosition.x
        + (player2Pos.x * m->minimapPosition.w) / SCREEN_WIDTH;
    m->playerDot2Position.y = m->minimapPosition.y
        + (player2Pos.y * m->minimapPosition.h) / SCREEN_HEIGHT;
    if (m->playerDot2Position.x < m->minimapPosition.x)
        m->playerDot2Position.x = m->minimapPosition.x;
    if (m->playerDot2Position.x > m->minimapPosition.x + m->minimapPosition.w - m->playerDot2Position.w)
        m->playerDot2Position.x = m->minimapPosition.x + m->minimapPosition.w - m->playerDot2Position.w;
    if (m->playerDot2Position.y < m->minimapPosition.y)
        m->playerDot2Position.y = m->minimapPosition.y;
    if (m->playerDot2Position.y > m->minimapPosition.y + m->minimapPosition.h - m->playerDot2Position.h)
        m->playerDot2Position.y = m->minimapPosition.y + m->minimapPosition.h - m->playerDot2Position.h;
}

/*
 * showPlayer2 = 1 in multiplayer, 0 in solo.
 * The second dot is simply not rendered when showPlayer2 == 0.
 */
void AfficherMinimap(SDL_Renderer* renderer, Minimap m, int currentRoom, int showPlayer2)
{
    if (currentRoom >= 0 && currentRoom < MAX_MINIMAP_ROOMS &&
        m.backgroundTextures[currentRoom])
        SDL_RenderCopy(renderer, m.backgroundTextures[currentRoom], NULL, &m.minimapPosition);

    if (m.playerDot)
        SDL_RenderCopy(renderer, m.playerDot, NULL, &m.playerDotPosition);

    if (showPlayer2 && m.playerDot2)
        SDL_RenderCopy(renderer, m.playerDot2, NULL, &m.playerDot2Position);
}

void LibererMinimap(Minimap* m)
{
    for (int i = 0; i < MAX_MINIMAP_ROOMS; i++) {
        if (m->backgroundTextures[i]) {
            SDL_DestroyTexture(m->backgroundTextures[i]);
            m->backgroundTextures[i] = NULL;
        }
    }
    m->playerDot  = NULL;
    m->playerDot2 = NULL;
}
