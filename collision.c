#include "collision.h"
#include "common.h"
#include <string.h>

int CollisionAABB(SDL_Rect a, SDL_Rect b)
{
    return !(a.x + a.w <= b.x || a.x >= b.x + b.w ||
             a.y + a.h <= b.y || a.y >= b.y + b.h);
}

SDL_Color GetPixel(SDL_Surface* surface, int x, int y)
{
    SDL_Color color = {255, 255, 255, 255};

    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h)
        return color;

    Uint8* p = (Uint8*)surface->pixels
             + (y * surface->pitch)
             + (x * surface->format->BytesPerPixel);

    Uint32 pixel = 0;
    memcpy(&pixel, p, surface->format->BytesPerPixel);
    SDL_GetRGB(pixel, surface->format, &color.r, &color.g, &color.b);
    return color;
}

/*
 * Set DEBUG_COLLISION to 1 to disable wall collision entirely.
 * Use this to confirm movement works, then set back to 0.
 */
#define DEBUG_COLLISION 0

/*
 * Scale screen coordinates to mask image coordinates.
 * Handles masks at any resolution (e.g. 1024x1024 mask on a 1280x720 screen).
 */
static int toMaskX(SDL_Surface* mask, int screenX)
{
    return (screenX * mask->w) / SCREEN_WIDTH;
}
static int toMaskY(SDL_Surface* mask, int screenY)
{
    return (screenY * mask->h) / SCREEN_HEIGHT;
}

/*
 * Returns a smaller collision rect centered at the bottom half of the sprite.
 * This lets the player squeeze through tight gaps and feels natural
 * for top-down RPGs where only the feet matter for collision.
 * Shrink by HBOX_SHRINK pixels on each side.
 */
#define HBOX_SHRINK 14

static SDL_Rect hitbox(SDL_Rect sprite)
{
    return (SDL_Rect){
        sprite.x + HBOX_SHRINK,
        sprite.y + sprite.h / 2,          /* start from vertical midpoint */
        sprite.w - HBOX_SHRINK * 2,
        sprite.h / 2 - 4                  /* only bottom half of sprite */
    };
}

/*
 * Tests 8 border points of a SHRUNKEN rect against the mask.
 * Returns 1 if any point hits a black pixel (= wall).
 */
int CollisionParfaite(SDL_Surface* mask, SDL_Rect r)
{
    if (!mask || DEBUG_COLLISION) return 0;

    SDL_Rect hb = hitbox(r);   /* use small hitbox, not full sprite */

    SDL_Point pts[8] = {
        {hb.x,            hb.y            },
        {hb.x + hb.w / 2, hb.y            },
        {hb.x + hb.w - 1, hb.y            },
        {hb.x,            hb.y + hb.h / 2 },
        {hb.x + hb.w - 1, hb.y + hb.h / 2 },
        {hb.x,            hb.y + hb.h - 1 },
        {hb.x + hb.w / 2, hb.y + hb.h - 1 },
        {hb.x + hb.w - 1, hb.y + hb.h - 1 },
    };

    for (int i = 0; i < 8; i++) {
        int mx = toMaskX(mask, pts[i].x);
        int my = toMaskY(mask, pts[i].y);
        SDL_Color c = GetPixel(mask, mx, my);
        if (c.r == 0 && c.g == 0 && c.b == 0)
            return 1;
    }
    return 0;
}
