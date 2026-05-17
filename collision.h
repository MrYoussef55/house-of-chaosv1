#ifndef COLLISION_H
#define COLLISION_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Color GetPixel        (SDL_Surface* surface, int x, int y);
int       CollisionParfaite(SDL_Surface* mask, SDL_Rect rect);
int       CollisionAABB   (SDL_Rect a, SDL_Rect b);

#endif
