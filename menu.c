#include "menu.h"
#include "button.h"
#include <SDL2/SDL_image.h>

#define NUM_BUTTONS 5

static Button buttons[NUM_BUTTONS];
SDL_Texture* bg = NULL;
char* offFiles[NUM_BUTTONS] = {
    "startoff.png",
    "scoreoff.png",
    "historyoff.png",
    "optionoff.png",
    "quitoff.png"
};

char* onFiles[NUM_BUTTONS] = {
    "starton.png",
    "scoreon.png",
    "historyon.png",
    "optionon.png",
    "quiton.png"
};

void MainMenu_Init(App* app)
{
    bg = IMG_LoadTexture(app->renderer, "back.png");

    for(int i = 0; i < NUM_BUTTONS; i++)
    {
        buttons[i].rect = (SDL_Rect){400, 150 + (i * 70), 220, 60};
        buttons[i].normal = IMG_LoadTexture(app->renderer, offFiles[i]);
        buttons[i].hover  = IMG_LoadTexture(app->renderer, onFiles[i]);
        buttons[i].isHovered = false;
    }
}
void fade(SDL_Renderer* renderer, int fadeIn)
{
    SDL_Rect rect = {0, 0, 1920, 1080};

    for (int i = 0; i <= 255; i += 5)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        if (fadeIn)
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 - i);
        else
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, i);

        SDL_RenderFillRect(renderer, &rect);
        SDL_RenderPresent(renderer);
        SDL_Delay(5);
    }
}
void MainMenu_HandleEvent(App* app, SDL_Event* e, SDL_Renderer* renderer)
{
    for(int i = 0; i < NUM_BUTTONS; i++)
    {
        if(Button_Clicked(&buttons[i], e))
        {
            Mix_PlayChannel(-1, app->clickSound, 0);

            switch(i)
            {
                case 0: fade(renderer,0); app->state = STATE_SAUVGARD; break;
                case 1: fade(renderer,0);app->state = STATE_SCORE; break;
                case 2: fade(renderer,0);app->state = STATE_HISTORY; break;
                case 3: fade(renderer,0);app->state = STATE_OPTIONS; break;
                case 4: app->running = false; break;
            }
        }
    }
}

void MainMenu_Update(App* app)
{
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    for(int i = 0; i < NUM_BUTTONS; i++)
        Button_HandleMouse(&buttons[i], mx, my);
}
void MainMenu_Render(App* app)
{
    if(bg)
        SDL_RenderCopy(app->renderer, bg, NULL, NULL);
    for(int i = 0; i < NUM_BUTTONS; i++)
        Button_Render(app->renderer, &buttons[i]);
}
void MainMenu_Destroy()
{
    for(int i = 0; i < NUM_BUTTONS; i++)
        Button_Destroy(&buttons[i]);
    if(bg)
        SDL_DestroyTexture(bg);
}

