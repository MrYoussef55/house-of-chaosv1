#include "minimap.h"
#include "common.h"

/* playerSprite is the player's walk texture — minimap borrows it, does NOT own it */
void InitMinimap(Minimap* m, SDL_Renderer* renderer,
                 const char* mapPath, SDL_Texture* playerSprite,
                 SDL_Rect minimapPos)
{
    SDL_Surface* s = IMG_Load(mapPath);
    m->backgroundTexture = SDL_CreateTextureFromSurface(renderer, s);
    SDL_FreeSurface(s);

    m->minimapPosition   = minimapPos;
    m->playerDot         = playerSprite;   /* just a pointer, not loaded here */
    m->playerDotPosition = (SDL_Rect){minimapPos.x, minimapPos.y, 16, 16};
}

void MAJMinimap(Minimap* m, SDL_Rect playerPos)
{
    /* Map screen position proportionally onto the minimap rectangle */
    m->playerDotPosition.x = m->minimapPosition.x
        + (playerPos.x * m->minimapPosition.w) / SCREEN_WIDTH;
    m->playerDotPosition.y = m->minimapPosition.y
        + (playerPos.y * m->minimapPosition.h) / SCREEN_HEIGHT;

    /* clamp inside minimap */
    if (m->playerDotPosition.x < m->minimapPosition.x)
        m->playerDotPosition.x = m->minimapPosition.x;
    if (m->playerDotPosition.x > m->minimapPosition.x + m->minimapPosition.w - m->playerDotPosition.w)
        m->playerDotPosition.x = m->minimapPosition.x + m->minimapPosition.w - m->playerDotPosition.w;
    if (m->playerDotPosition.y < m->minimapPosition.y)
        m->playerDotPosition.y = m->minimapPosition.y;
    if (m->playerDotPosition.y > m->minimapPosition.y + m->minimapPosition.h - m->playerDotPosition.h)
        m->playerDotPosition.y = m->minimapPosition.y + m->minimapPosition.h - m->playerDotPosition.h;
}

void AfficherMinimap(SDL_Renderer* renderer, Minimap m)
{
    SDL_RenderCopy(renderer, m.backgroundTexture, NULL, &m.minimapPosition);
    if (m.playerDot)
        SDL_RenderCopy(renderer, m.playerDot, NULL, &m.playerDotPosition);
}

/* Only free the background — playerDot is owned by the Player, not us */
void LibererMinimap(Minimap* m)
{
    SDL_DestroyTexture(m->backgroundTexture);
    m->backgroundTexture = NULL;
    m->playerDot         = NULL;   /* do NOT destroy — Player owns this */
}
