#include "avatar.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

typedef struct {
    SDL_Rect rect;
    SDL_Texture* normal;
    SDL_Texture* hover;
    int hovered;
} Button;

int avatar_run(SDL_Renderer* renderer)
{
    Button confirm, back;

    confirm.normal = IMG_LoadTexture(renderer, "assets/images/confirm.png");
    confirm.hover  = IMG_LoadTexture(renderer, "assets/images/confirm_hover.png");
    confirm.rect = (SDL_Rect){800, 400, 320, 100};

    back.normal = IMG_LoadTexture(renderer, "assets/images/back.png");
    back.hover  = IMG_LoadTexture(renderer, "assets/images/back_hover.png");
    back.rect = (SDL_Rect){50, 50, 200, 80};

    SDL_Event event;
    int running = 1;
    int state = 3;

    while (running)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        confirm.hovered = (mouseX > confirm.rect.x && mouseX < confirm.rect.x + confirm.rect.w &&
                           mouseY > confirm.rect.y && mouseY < confirm.rect.y + confirm.rect.h);
        back.hovered = (mouseX > back.rect.x && mouseX < back.rect.x + back.rect.w &&
                        mouseY > back.rect.y && mouseY < back.rect.y + back.rect.h);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                return -1;

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (confirm.hovered) { state = 4; running = 0; } // Game later
                if (back.hovered)    { state = 2; running = 0; } // Back to SAUVEGARDE
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, confirm.hovered ? confirm.hover : confirm.normal, NULL, &confirm.rect);
        SDL_RenderCopy(renderer, back.hovered ? back.hover : back.normal, NULL, &back.rect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(confirm.normal);
    SDL_DestroyTexture(confirm.hover);
    SDL_DestroyTexture(back.normal);
    SDL_DestroyTexture(back.hover);

    return state;
}
}
