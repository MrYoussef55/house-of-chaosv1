#ifndef MINIMAP_H
#define MINIMAP_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define MAX_MINIMAP_ROOMS 4

typedef struct {
    SDL_Texture* backgroundTextures[MAX_MINIMAP_ROOMS];
    SDL_Rect     minimapPosition;
    SDL_Texture* playerDot;
    SDL_Texture* playerDot2;
    SDL_Rect     playerDotPosition;
    SDL_Rect     playerDot2Position;
} Minimap;

void InitMinimap    (Minimap* m, SDL_Renderer* renderer,
                     const char* mapPaths[], SDL_Texture* playerSprite,
                     SDL_Texture* playerSprite2, SDL_Rect minimapPos);
void MAJMinimap     (Minimap* m, SDL_Rect playerPos, SDL_Rect player2Pos, int currentRoom);
void AfficherMinimap(SDL_Renderer* renderer, Minimap m, int currentRoom, int showPlayer2);
void LibererMinimap (Minimap* m);

#endif
