#include "sauvgard.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>



int save_run(SDL_Renderer* renderer)
{
    Button new_game, back;

    new_game.normal = IMG_LoadTexture(renderer, "assets/images/new_game.png");
    new_game.hover  = IMG_LoadTexture(renderer, "assets/images/new_game_hover.png");
    new_game.rect = (SDL_Rect){800, 400, 320, 100};

    back.normal = IMG_LoadTexture(renderer, "assets/images/back.png");
    back.hover  = IMG_LoadTexture(renderer, "assets/images/back_hover.png");
    back.rect = (SDL_Rect){50, 50, 200, 80};

    SDL_Event event;
    int running = 1;
    int state = 2;

    while (running)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        new_game.hovered = (mouseX > new_game.rect.x && mouseX < new_game.rect.x + new_game.rect.w &&
                            mouseY > new_game.rect.y && mouseY < new_game.rect.y + new_game.rect.h);
        back.hovered = (mouseX > back.rect.x && mouseX < back.rect.x + back.rect.w &&
                        mouseY > back.rect.y && mouseY < back.rect.y + back.rect.h);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                return -1;

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (new_game.hovered) { state = 3; running = 0; } // Go to AVATAR MENU
                if (back.hovered)     { state = 0; running = 0; } // Back to MAIN MENU
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, new_game.hovered ? new_game.hover : new_game.normal, NULL, &new_game.rect);
        SDL_RenderCopy(renderer, back.hovered ? back.hover : back.normal, NULL, &back.rect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(new_game.normal);
    SDL_DestroyTexture(new_game.hover);
    SDL_DestroyTexture(back.normal);
    SDL_DestroyTexture(back.hover);

    return state;
}
