#include "menu.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

int menu_run(SDL_Renderer* renderer)
{
    // Example buttons
    Button start, options, quit;

    // Load textures (replace with your actual images)
    start.normal = IMG_LoadTexture(renderer, "assets/images/start.png");
    start.hover  = IMG_LoadTexture(renderer, "assets/images/start_hover.png");
    start.rect = (SDL_Rect){800, 300, 320, 100};

    options.normal = IMG_LoadTexture(renderer, "assets/images/options.png");
    options.hover  = IMG_LoadTexture(renderer, "assets/images/options_hover.png");
    options.rect = (SDL_Rect){800, 450, 320, 100};

    quit.normal = IMG_LoadTexture(renderer, "assets/images/quit.png");
    quit.hover  = IMG_LoadTexture(renderer, "assets/images/quit_hover.png");
    quit.rect = (SDL_Rect){800, 600, 320, 100};

    SDL_Event event;
    int running = 1;
    int state = 0;

    while (running)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Hover logic
        start.hovered = (mouseX > start.rect.x && mouseX < start.rect.x + start.rect.w &&
                         mouseY > start.rect.y && mouseY < start.rect.y + start.rect.h);
        options.hovered = (mouseX > options.rect.x && mouseX < options.rect.x + options.rect.w &&
                            mouseY > options.rect.y && mouseY < options.rect.y + options.rect.h);
        quit.hovered = (mouseX > quit.rect.x && mouseX < quit.rect.x + quit.rect.w &&
                        mouseY > quit.rect.y && mouseY < quit.rect.y + quit.rect.h);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                return -1;

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (start.hovered) state = 2;   // SAUVEGARDE
                if (options.hovered) state = 1; // OPTIONS
                if (quit.hovered) state = -1;   // EXIT

                if (state != 0) running = 0;
            }
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, start.hovered ? start.hover : start.normal, NULL, &start.rect);
        SDL_RenderCopy(renderer, options.hovered ? options.hover : options.normal, NULL, &options.rect);
        SDL_RenderCopy(renderer, quit.hovered ? quit.hover : quit.normal, NULL, &quit.rect);

        SDL_RenderPresent(renderer);
    }

    // Cleanup textures
    SDL_DestroyTexture(start.normal);
    SDL_DestroyTexture(start.hover);
    SDL_DestroyTexture(options.normal);
    SDL_DestroyTexture(options.hover);
    SDL_DestroyTexture(quit.normal);
    SDL_DestroyTexture(quit.hover);

    return state;
}
