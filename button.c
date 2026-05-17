#include "button.h"

// Replaces the old Mouse logic. We just tell it if it's highlighted!
void Button_SetHovered(Button* btn, bool isHovered)
{
    btn->isHovered = isHovered;
}

void Button_Render(SDL_Renderer* renderer, Button* btn)
{
    SDL_Texture* tex = btn->isHovered ? btn->hover : btn->normal;
    SDL_RenderCopy(renderer, tex, NULL, &btn->rect);
}

void Button_Destroy(Button* btn)
{
    if(btn->normal) SDL_DestroyTexture(btn->normal);
    if(btn->hover) SDL_DestroyTexture(btn->hover);
}
