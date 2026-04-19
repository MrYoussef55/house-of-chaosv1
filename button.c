#include "button.h"

void Button_HandleMouse(Button* btn, int mx, int my)
{
    btn->isHovered =
        mx >= btn->rect.x &&
        mx <= btn->rect.x + btn->rect.w &&
        my >= btn->rect.y &&
        my <= btn->rect.y + btn->rect.h;
}

void Button_Render(SDL_Renderer* renderer, Button* btn)
{
    SDL_Texture* tex = btn->isHovered ? btn->hover : btn->normal;
    SDL_RenderCopy(renderer, tex, NULL, &btn->rect);
}

bool Button_Clicked(Button* btn, SDL_Event* e)
{
    return (e->type == SDL_MOUSEBUTTONDOWN && btn->isHovered);
}

void Button_Destroy(Button* btn)
{
    if(btn->normal) SDL_DestroyTexture(btn->normal);
    if(btn->hover) SDL_DestroyTexture(btn->hover);
}
