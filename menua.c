#include "menua.h" /* Make sure you have a menua.h with these function prototypes! */
#include "button.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

/* Using the Button struct from button.h now, keeping your exact variable names */
static Button btnMono, btnMulti;
static Button btnIn1, btnAv1, btnIn2, btnAv2, btnValid;
static Button btnRet;

static SDL_Texture* bg = NULL;
static SDL_Rect bgRect;

/* Internal state: 0 = Selection, 1 = Config */
static int state = 0; 

/* ===== SCALABLE BACKGROUND RENDER (Matches options.c) ===== */
static void RenderBackground(App* app)
{
    if (!bg) return;

    int texW, texH;
    SDL_QueryTexture(bg, NULL, NULL, &texW, &texH);

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

    SDL_RenderCopy(app->renderer, bg, NULL, &bgRect);
}

void MenuA_Init(App* app)
{
    /* Load Background */
    bg = IMG_LoadTexture(app->renderer, "background.png");

    /* --- State 0 Buttons --- */
    btnMono.rect   = (SDL_Rect){300, 150, 200, 80};
    btnMono.normal = IMG_LoadTexture(app->renderer, "monooff.png");
    btnMono.hover  = IMG_LoadTexture(app->renderer, "monoon.png");

    btnMulti.rect   = (SDL_Rect){300, 250, 200, 80};
    btnMulti.normal = IMG_LoadTexture(app->renderer, "multioff.png");
    btnMulti.hover  = IMG_LoadTexture(app->renderer, "multion.png");
    
    /* --- State 1 Buttons --- */
    btnIn1.rect   = (SDL_Rect){150, 100, 200, 80};
    btnIn1.normal = IMG_LoadTexture(app->renderer, "input1off.png");
    btnIn1.hover  = IMG_LoadTexture(app->renderer, "input1on.png");

    btnAv1.rect   = (SDL_Rect){150, 200, 200, 80};
    btnAv1.normal = IMG_LoadTexture(app->renderer, "avatar1off.png");
    btnAv1.hover  = IMG_LoadTexture(app->renderer, "avatar1on.png");

    btnIn2.rect   = (SDL_Rect){450, 100, 200, 80};
    btnIn2.normal = IMG_LoadTexture(app->renderer, "input2off.png");
    btnIn2.hover  = IMG_LoadTexture(app->renderer, "input2on.png");

    btnAv2.rect   = (SDL_Rect){450, 200, 200, 80};
    btnAv2.normal = IMG_LoadTexture(app->renderer, "avatar2off.png");
    btnAv2.hover  = IMG_LoadTexture(app->renderer, "avatar2on.png");

    btnValid.rect   = (SDL_Rect){350, 450, 200, 80};
    btnValid.normal = IMG_LoadTexture(app->renderer, "valideroff.png");
    btnValid.hover  = IMG_LoadTexture(app->renderer, "valideron.png");

    /* Shared Button */
    btnRet.rect   = (SDL_Rect){20, 520, 120, 50};
    btnRet.normal = IMG_LoadTexture(app->renderer, "retouroff.png");
    btnRet.hover  = IMG_LoadTexture(app->renderer, "retouron.png");

    /* Reset internal state to Selection Menu every time we enter this screen */
    state = 0;
}

void MenuA_HandleEvent(App* app, SDL_Event* e)
{
    if (state == 0) 
    {
        if (Button_Clicked(&btnMono, e)) {
            Mix_PlayChannel(-1, app->clickSound, 0);
            state = 1; /* Move to config screen */
        }
        if (Button_Clicked(&btnMulti, e)) {
            Mix_PlayChannel(-1, app->clickSound, 0);
            state = 1; /* Move to config screen */
        }
        if (Button_Clicked(&btnRet, e)) {
            Mix_PlayChannel(-1, app->clickSound, 0);
            app->state = STATE_MAIN_MENU; /* Exit back to main menu */
        }
    } 
    else if (state == 1) 
    {
        /* Just playing sounds for the selection buttons for now */
        if (Button_Clicked(&btnIn1, e)) Mix_PlayChannel(-1, app->clickSound, 0);
        if (Button_Clicked(&btnAv1, e)) Mix_PlayChannel(-1, app->clickSound, 0);
        if (Button_Clicked(&btnIn2, e)) Mix_PlayChannel(-1, app->clickSound, 0);
        if (Button_Clicked(&btnAv2, e)) Mix_PlayChannel(-1, app->clickSound, 0);

        if (Button_Clicked(&btnRet, e)) {
            Mix_PlayChannel(-1, app->clickSound, 0);
            state = 0; /* Go back to the mono/multi selection */
        }
        if (Button_Clicked(&btnValid, e)) {
            Mix_PlayChannel(-1, app->clickSound, 0);
            app->state = STATE_GAME; /* Launch the game! */
        }
    }
}

void MenuA_Update(App* app)
{
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    if (state == 0) {
        Button_HandleMouse(&btnMono, mx, my);
        Button_HandleMouse(&btnMulti, mx, my);
        Button_HandleMouse(&btnRet, mx, my);
    } else if (state == 1) {
        Button_HandleMouse(&btnIn1, mx, my);
        Button_HandleMouse(&btnAv1, mx, my);
        Button_HandleMouse(&btnIn2, mx, my);
        Button_HandleMouse(&btnAv2, mx, my);
        Button_HandleMouse(&btnValid, mx, my);
        Button_HandleMouse(&btnRet, mx, my);
    }
}

void MenuA_Render(App* app)
{
    RenderBackground(app);

    if (state == 0) {
        Button_Render(app->renderer, &btnMono);
        Button_Render(app->renderer, &btnMulti);
        Button_Render(app->renderer, &btnRet);
    } else if (state == 1) {
        Button_Render(app->renderer, &btnIn1);
        Button_Render(app->renderer, &btnAv1);
        Button_Render(app->renderer, &btnIn2);
        Button_Render(app->renderer, &btnAv2);
        Button_Render(app->renderer, &btnValid);
        Button_Render(app->renderer, &btnRet);
    }
}

void MenuA_Destroy(void)
{
    Button_Destroy(&btnMono);
    Button_Destroy(&btnMulti);
    Button_Destroy(&btnIn1);
    Button_Destroy(&btnAv1);
    Button_Destroy(&btnIn2);
    Button_Destroy(&btnAv2);
    Button_Destroy(&btnValid);
    Button_Destroy(&btnRet);

    if (bg)
        SDL_DestroyTexture(bg);
}
