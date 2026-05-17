#ifndef MINIMAP_H
#define MINIMAP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define MAX_MINIMAP_ROOMS 4

typedef struct {
    SDL_Texture* backgroundTextures[MAX_MINIMAP_ROOMS];
    SDL_Rect     minimapPosition;
    SDL_Texture* playerDot;
    SDL_Rect     playerDotPosition;
} Minimap;

void InitMinimap    (Minimap* m, SDL_Renderer* renderer,
                     const char* mapPaths[], SDL_Texture* playerSprite,
                     SDL_Rect minimapPos);
void MAJMinimap     (Minimap* m, SDL_Rect playerPos, int currentRoom);
void AfficherMinimap(SDL_Renderer* renderer, Minimap m, int currentRoom);
void LibererMinimap (Minimap* m);

#endif
