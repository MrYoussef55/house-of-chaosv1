#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "app.h"
#include "menu.h"
#include "validation.h"
#include "options.h"
#include "menua.h"
#include "menus.h"
int main()
{
    App app;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    app.window = SDL_CreateWindow("House of Chaos",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1024,576,0);

    app.renderer = SDL_CreateRenderer(app.window,-1,SDL_RENDERER_ACCELERATED);

    app.clickSound = Mix_LoadWAV("click.wav");
    app.mainMenuMusic = Mix_LoadMUS("backsong.mp3");
    app.optionsMusic  = Mix_LoadMUS("backsong.mp3");
    app.sauvgardemusic  = Mix_LoadMUS("backgroundsong.mp3");

    app.volume = 50;
    app.state = STATE_MAIN_MENU;
    app.previousState = -1;
    app.running = true;

    MainMenu_Init(&app);
    Options_Init(&app);
    Validation_Init(&app);
    MenuA_Init(&app);
    Menus_Init(&app);
    SDL_Event e;

    while(app.running)
    {
        if(app.state != app.previousState)
        {
            Mix_HaltMusic();
            if(app.state == STATE_MAIN_MENU)
                Mix_PlayMusic(app.mainMenuMusic,-1);
            else if(app.state == STATE_OPTIONS)
                Mix_PlayMusic(app.optionsMusic,-1);
            else if (app.state ==STATE_SAUVGARD)
            	Mix_PlayMusic(app.sauvgardemusic,-1);

            app.previousState = app.state;
        }

        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT)
                app.running = false;

            if(app.state == STATE_MAIN_MENU)
                MainMenu_HandleEvent(&app,&e,app.renderer);
            else if(app.state == STATE_OPTIONS)
                Options_HandleEvent(&app,&e);
            else if(app.state == STATE_SAUVGARD)
                Validation_HandleEvent(&app,&e);
            else if(app.state == STATE_AVATAR)    
                MenuA_HandleEvent(&app,&e);
            else if(app.state == STATE_SCORE)     
                Menus_HandleEvent(&app,&e);
        }

        if(app.state == STATE_MAIN_MENU)
            MainMenu_Update(&app);
        else if(app.state == STATE_OPTIONS)
            Options_Update(&app);
        else if(app.state == STATE_SAUVGARD)
            Validation_Update(&app);
        else if(app.state == STATE_AVATAR)
            MenuA_Update(&app);
        else if(app.state == STATE_SCORE)        
            Menus_Update(&app);

        SDL_RenderClear(app.renderer);

        if(app.state == STATE_MAIN_MENU)
            MainMenu_Render(&app);
        else if(app.state == STATE_OPTIONS)
            Options_Render(&app);
        else if(app.state == STATE_SAUVGARD)
            Validation_Render(&app);
        else if(app.state == STATE_AVATAR)         
            MenuA_Render(&app);
        else if(app.state == STATE_SCORE)          
            Menus_Render(&app);
        SDL_RenderPresent(app.renderer);
    }

    MainMenu_Destroy();
    Options_Destroy();
    Validation_Destroy();
    MenuA_Destroy();
    Menus_Destroy();

    Mix_FreeChunk(app.clickSound);
    Mix_FreeMusic(app.mainMenuMusic);
    Mix_FreeMusic(app.optionsMusic);
    Mix_FreeMusic(app.sauvgardemusic);
    Mix_CloseAudio();

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}



