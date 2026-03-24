#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "menu.h"
#include "options.h"
#include "save.h"
#include "avatar.h"

#define WIDTH 1920
#define HEIGHT 1080

int main()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_Window *window = SDL_CreateWindow("House of Chaos",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int running = 1;
    int state = 0;

    while (running)
    {
        switch (state)
        {
            case 0: // MAIN MENU
                state = menu_run(renderer);
                break;

            case 1: // OPTIONS
                state = options_run(renderer);
                break;

            case 2: // SAUVEGARDE
                state = save_run(renderer);
                break;

            case 3: // AVATAR
                state = avatar_run(renderer);
                break;

            case 4: // GAME (later)
                // game_run(renderer);
                break;

            case -1: // EXIT
                running = 0;
                break;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
