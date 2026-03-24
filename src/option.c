#include "options.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>


int options_run(SDL_Renderer* renderer)
{
    Button back;

    back.normal = IMG_LoadTexture(renderer, "assets/images/back.png");
    back.hover  = IMG_LoadTexture(renderer, "assets/images/back_hover.png");
    back.rect = (SDL_Rect){50, 50, 200, 80};

    SDL_Event event;
    int running = 1;
    int state = 1;

    while (running)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        back.hovered = (mouseX > back.rect.x && mouseX < back.rect.x + back.rect.w &&
                        mouseY > back.rect.y && mouseY < back.rect.y + back.rect.h);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                return -1;

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (back.hovered) { state = 0; running = 0; } // Back to MAIN MENU
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, back.hovered ? back.hover : back.normal, NULL, &back.rect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(back.normal);
    SDL_DestroyTexture(back.hover);

    return state;
}
