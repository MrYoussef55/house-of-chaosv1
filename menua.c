#include "menua.h"
#include "button.h"
#include "video.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

static Button btnMono, btnMulti, btnIn1, btnAv1, btnIn2, btnAv2, btnValid, btnRet;
static SDL_Texture* bg = NULL;
static SDL_Rect bgRect;

static int state = 0;
static int selectedMode = 0;
static int selectedIndex = 0;

static void RenderBackground(App* app)
{
    if (!bg) return;
    int texW, texH;
    SDL_QueryTexture(bg, NULL, NULL, &texW, &texH);
    int winW, winH;
    SDL_GetWindowSize(app->window, &winW, &winH);
    float winRatio = (float)winW / winH;
    float texRatio = (float)texW / texH;

    if (winRatio > texRatio) {
        bgRect.h = winH;
        bgRect.w = (int)(winH * texRatio);
        bgRect.x = (winW - bgRect.w) / 2;
        bgRect.y = 0;
    } else {
        bgRect.w = winW;
        bgRect.h = (int)(winW / texRatio);
        bgRect.x = 0;
        bgRect.y = (winH - bgRect.h) / 2;
    }
    SDL_RenderCopy(app->renderer, bg, NULL, &bgRect);
}

void MenuA_Init(App* app)
{
    bg = IMG_LoadTexture(app->renderer, "backgrounddd.png");

    btnMono.rect   = (SDL_Rect){300, 150, 200, 80};
    btnMono.normal = IMG_LoadTexture(app->renderer, "monooff.png");
    btnMono.hover  = IMG_LoadTexture(app->renderer, "monoon.png");

    btnMulti.rect   = (SDL_Rect){300, 250, 200, 80};
    btnMulti.normal = IMG_LoadTexture(app->renderer, "multioff.png");
    btnMulti.hover  = IMG_LoadTexture(app->renderer, "multion.png");
    
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

    btnRet.rect   = (SDL_Rect){20, 520, 120, 50};
    btnRet.normal = IMG_LoadTexture(app->renderer, "retouroff.png");
    btnRet.hover  = IMG_LoadTexture(app->renderer, "retouron.png");

    state = 0;
    selectedIndex = 0;
}

void MenuA_HandleEvent(App* app, SDL_Event* e)
{
    if (e->type == SDL_KEYDOWN) 
    {
        int maxIndex = (state == 0) ? 2 : 5; // State 0: 3 buttons. State 1: 6 buttons.

        if (e->key.keysym.sym == SDLK_UP || e->key.keysym.sym == SDLK_LEFT) {
            selectedIndex--;
            if (selectedIndex < 0) selectedIndex = maxIndex;
            Mix_PlayChannel(-1, app->clickSound, 0);
        }
        else if (e->key.keysym.sym == SDLK_DOWN || e->key.keysym.sym == SDLK_RIGHT) {
            selectedIndex++;
            if (selectedIndex > maxIndex) selectedIndex = 0;
            Mix_PlayChannel(-1, app->clickSound, 0);
        }
        else if (e->key.keysym.sym == SDLK_RETURN) {
            Mix_PlayChannel(-1, app->clickSound, 0);

            if (state == 0) {
                if (selectedIndex == 0) { selectedMode = 0; state = 1; selectedIndex = 0; }
                else if (selectedIndex == 1) { selectedMode = 1; state = 1; selectedIndex = 0; }
                else if (selectedIndex == 2) { app->state = STATE_MAIN_MENU; }
            } 
            else if (state == 1) {
                if (selectedIndex == 4) { // Valid
                    play_video(app->window, INTRO_VIDEO);
                    app->state = (selectedMode == 1) ? STATE_GAME_MULTI : STATE_GAME;
                }
                else if (selectedIndex == 5) { // Return
                    state = 0;
                    selectedIndex = 0;
                }
            }
        }
    }
}

void MenuA_Update(App* app)
{
    if (state == 0) {
        Button_SetHovered(&btnMono, (selectedIndex == 0));
        Button_SetHovered(&btnMulti, (selectedIndex == 1));
        Button_SetHovered(&btnRet, (selectedIndex == 2));
    } else if (state == 1) {
        Button_SetHovered(&btnIn1, (selectedIndex == 0));
        Button_SetHovered(&btnAv1, (selectedIndex == 1));
        Button_SetHovered(&btnIn2, (selectedIndex == 2));
        Button_SetHovered(&btnAv2, (selectedIndex == 3));
        Button_SetHovered(&btnValid, (selectedIndex == 4));
        Button_SetHovered(&btnRet, (selectedIndex == 5));
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
    if (bg) SDL_DestroyTexture(bg);
}
