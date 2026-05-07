#ifndef MINIMAP_H
#define MINIMAP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct {
    SDL_Texture* backgroundTexture;
    SDL_Rect     minimapPosition;
    SDL_Texture* playerDot;      /* points to player's walk texture, NOT owned here */
    SDL_Rect     playerDotPosition;
} Minimap;

/* Pass the player's walk texture directly as playerSprite — minimap does NOT free it */
void InitMinimap    (Minimap* m, SDL_Renderer* renderer,
                     const char* mapPath, SDL_Texture* playerSprite,
                     SDL_Rect minimapPos);
void MAJMinimap     (Minimap* m, SDL_Rect playerPos);
void AfficherMinimap(SDL_Renderer* renderer, Minimap m);
void LibererMinimap (Minimap* m);

#endif
