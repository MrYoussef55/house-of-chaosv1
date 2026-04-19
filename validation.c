#include "validation.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

/* Notice: No #include "button.c" here anymore! */

Button btnYes, btnNo, btn1, btn2;
TTF_Font* font = NULL;

SDL_Texture* background = NULL;
SDL_Rect bgRect;

/* Tracks which validation screen we are on: 1 for the save prompt, 2 for the second screen */
int current_window = 1;

void RenderBackground(App* app)
{
    if (!background) return;

    int texW, texH;
    SDL_QueryTexture(background, NULL, NULL, &texW, &texH);

    int winW, winH;
    SDL_GetWindowSize(app->window, &winW, &winH);

    float winRatio = (float)winW / winH;
    float texRatio = (float)texW / texH;

    if (winRatio > texRatio)
    {
        bgRect.h = winH;
        bgRect.w = (int)(winH * texRatio);
        bgRect.x = (winW - bgRect.w) / 2;
        bgRect.y = 0;
    }
    else
    {
        bgRect.w = winW;
        bgRect.h = (int)(winW / texRatio);
        bgRect.x = 0;
        bgRect.y = (winH - bgRect.h) / 2;
    }

    SDL_RenderCopy(app->renderer, background, NULL, &bgRect);
}

void Validation_Init(App* app)
{
    TTF_Init();
    font = TTF_OpenFont("/home/hadir/validation hadir/SouthernAire_Personal_Use_Only.ttf", 70);

    background = IMG_LoadTexture(app->renderer, "background.png");

    /* Initialize buttons for Window 1 (fenetre1) */
    btnYes.rect = (SDL_Rect){220, 300, 150, 80};
    btnNo.rect  = (SDL_Rect){430, 300, 150, 80};
    btnYes.normal = IMG_LoadTexture(app->renderer, "yes.png");
    btnYes.hover  = IMG_LoadTexture(app->renderer, "yes1.png");
    btnNo.normal  = IMG_LoadTexture(app->renderer, "no.png");
    btnNo.hover   = IMG_LoadTexture(app->renderer, "no1.png");

    /* Initialize buttons for Window 2 (fenetre2) */
    btn1.rect = (SDL_Rect){300, 200, 200, 80};
    btn2.rect = (SDL_Rect){300, 350, 200, 80};
    btn1.normal = IMG_LoadTexture(app->renderer, "bt1.png");
    btn1.hover  = IMG_LoadTexture(app->renderer, "bt10.png");
    btn2.normal = IMG_LoadTexture(app->renderer, "bt2.png");
    btn2.hover  = IMG_LoadTexture(app->renderer, "bt20.png");

    /* Reset to the first window every time the state is initialized */
    current_window = 1; 
}

void Validation_HandleEvent(App* app, SDL_Event* e)
{
    if (current_window == 1)
    {
        if (Button_Clicked(&btnYes, e))
        {
            Mix_PlayChannel(-1, app->clickSound, 0);
            current_window = 2; /* Move to fenetre2 */
        }
        if (Button_Clicked(&btnNo, e))
        {
            Mix_PlayChannel(-1, app->clickSound, 0);
            app->state = STATE_MAIN_MENU; /* Go back or exit validation state */
        }
    }
       else if (current_window == 2)
    {
        if (Button_Clicked(&btn1, e))
        {
            Mix_PlayChannel(-1, app->clickSound, 0);
            /* Button 1 = Continue Game */
            app->state = STATE_GAME; 
        }
        if (Button_Clicked(&btn2, e))
        {
            Mix_PlayChannel(-1, app->clickSound, 0);
            /* Button 2 = New Game -> Go to Avatar Menu! */
            app->state = STATE_AVATAR; 
        }
    }
}

void Validation_Update(App* app)
{
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    if (current_window == 1)
    {
        Button_HandleMouse(&btnYes, mx, my);
        Button_HandleMouse(&btnNo, mx, my);
    }
    else if (current_window == 2)
    {
        Button_HandleMouse(&btn1, mx, my);
        Button_HandleMouse(&btn2, mx, my);
    }
}

void Validation_Render(App* app)
{
    RenderBackground(app);

    if (current_window == 1)
    {
        /* Render Text */
        if (font)
        {
            SDL_Color color = {255, 255, 255, 255};
            SDL_Surface* textSurface = TTF_RenderText_Blended(font, "Would you like to save your game?", color);
            
            if (textSurface)
            {
                SDL_Texture* text = SDL_CreateTextureFromSurface(app->renderer, textSurface);
                SDL_Rect textPos = {
                    (800 - textSurface->w) / 2, /* Centered horizontally assuming 800px width */
                    200,
                    textSurface->w,
                    textSurface->h
                };
                SDL_RenderCopy(app->renderer, text, NULL, &textPos);
                SDL_DestroyTexture(text);
                SDL_FreeSurface(textSurface);
            }
        }

        /* Render Buttons */
        Button_Render(app->renderer, &btnYes);
        Button_Render(app->renderer, &btnNo);
    }
    else if (current_window == 2)
    {
        Button_Render(app->renderer, &btn1);
        Button_Render(app->renderer, &btn2);
    }
}

void Validation_Destroy(void)
{
    Button_Destroy(&btnYes);
    Button_Destroy(&btnNo);
    Button_Destroy(&btn1);
    Button_Destroy(&btn2);

    if (background)
        SDL_DestroyTexture(background);

    if (font)
        TTF_CloseFont(font);
}
