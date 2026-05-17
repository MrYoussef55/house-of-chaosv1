#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

#include "app.h"
#include "menu.h"
#include "options.h"
#include "validation.h"
#include "menua.h"
#include "menus.h"
#include "game.h"       /* Game_Run()       */
#include "game_multi.h" /* Game_Run_Multi() */

int main(int argc, char* argv[])
{
    App app;
    app.running = true;
    app.state   = STATE_MAIN_MENU;
    app.volume  = 50;

    /* --- SDL init (done ONCE here; sub-modules must NOT call these again) --- */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL Init Error: %s\n", SDL_GetError());
        return -1;
    }
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    app.window = SDL_CreateWindow(
        "ESPRIT Game Project",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_SHOWN);
    app.renderer = SDL_CreateRenderer(
        app.window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    app.clickSound     = Mix_LoadWAV("click.wav");
    app.sauvgardemusic = NULL;
    app.mainMenuMusic  = NULL;
    app.optionsMusic   = NULL;

    /* --- init all menu screens --- */
    MainMenu_Init (&app);
    Options_Init  (&app);
    Validation_Init(&app);
    MenuA_Init    (&app);
    Menus_Init    (&app);

    SDL_Event e;

    while (app.running)
    {
        /* ---- STATE_GAME: hand off entirely to the game module ---- */
        if (app.state == STATE_GAME)
        {
            Game_Run(&app);

        /* ---- STATE_GAME_MULTI: two-player mode ---- */
        } else if (app.state == STATE_GAME_MULTI) {
            Game_Run_Multi(&app);
            if (app.running)
            {
                MainMenu_Init (&app);
                Options_Init  (&app);
                Validation_Init(&app);
                MenuA_Init    (&app);
                Menus_Init    (&app);
            }
            continue;
        }

        /* ---- menu event loop ---- */
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                app.running = false;

            // MENU TRANSLATOR: Only alters keys when working through user configuration screens
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_z:         e.key.keysym.sym = SDLK_UP;     break;
                    case SDLK_q:         e.key.keysym.sym = SDLK_LEFT;   break;
                    case SDLK_s:         e.key.keysym.sym = SDLK_DOWN;   break;
                    case SDLK_d:         e.key.keysym.sym = SDLK_RIGHT;  break;
                    case SDLK_KP_ENTER:  e.key.keysym.sym = SDLK_RETURN; break;
                    default: break;
                }
            }

            switch (app.state)
            {
                case STATE_MAIN_MENU: MainMenu_HandleEvent (&app, &e, app.renderer); break;
                case STATE_OPTIONS:   Options_HandleEvent  (&app, &e);               break;
                case STATE_SAUVGARD:  Validation_HandleEvent(&app, &e);              break;
                case STATE_AVATAR:    MenuA_HandleEvent    (&app, &e);               break;
                case STATE_SCORE:     Menus_HandleEvent    (&app, &e);               break;
                default: break;
            }
        }

        /* ---- menu update ---- */
        switch (app.state)
        {
            case STATE_MAIN_MENU: MainMenu_Update (&app); break;
            case STATE_OPTIONS:   Options_Update  (&app); break;
            case STATE_SAUVGARD:  Validation_Update(&app); break;
            case STATE_AVATAR:    MenuA_Update    (&app); break;
            case STATE_SCORE:     Menus_Update    (&app); break;
            default: break;
        }

        /* ---- menu render ---- */
        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
        SDL_RenderClear(app.renderer);

        switch (app.state)
        {
            case STATE_MAIN_MENU: MainMenu_Render (&app); break;
            case STATE_OPTIONS:   Options_Render  (&app); break;
            case STATE_SAUVGARD:  Validation_Render(&app); break;
            case STATE_AVATAR:    MenuA_Render    (&app); break;
            case STATE_SCORE:     Menus_Render    (&app); break;
            default: break;
        }

        SDL_RenderPresent(app.renderer);
    }

    /* --- cleanup --- */
    MainMenu_Destroy();
    Options_Destroy();
    Validation_Destroy();
    MenuA_Destroy();
    Menus_Destroy();

    if (app.clickSound)     Mix_FreeChunk(app.clickSound);
    if (app.sauvgardemusic) Mix_FreeMusic(app.sauvgardemusic);
    if (app.mainMenuMusic)  Mix_FreeMusic(app.mainMenuMusic);
    if (app.optionsMusic)   Mix_FreeMusic(app.optionsMusic);

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);

    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
