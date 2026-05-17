#include "validation.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

static Button btnYes, btnNo, btn1, btn2;
static TTF_Font* font = NULL;
static SDL_Texture* background = NULL;
static SDL_Rect bgRect;

int current_window = 1;
static int selectedIndex = 0; 

static void Validation_RenderBackground(App* app)
{
    if (!background) return;
    int texW, texH;
    SDL_QueryTexture(background, NULL, NULL, &texW, &texH);
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
    SDL_RenderCopy(app->renderer, background, NULL, &bgRect);
}

void Validation_Init(App* app)
{
    font = TTF_OpenFont("SouthernAire_Personal_Use_Only.ttf", 70);
    background = IMG_LoadTexture(app->renderer, "backgrounddd.png");

    btnYes.rect = (SDL_Rect){220, 300, 150, 80};
    btnNo.rect  = (SDL_Rect){430, 300, 150, 80};
    btnYes.normal = IMG_LoadTexture(app->renderer, "yes.png");
    btnYes.hover  = IMG_LoadTexture(app->renderer, "yes1.png");
    btnNo.normal  = IMG_LoadTexture(app->renderer, "no.png");
    btnNo.hover   = IMG_LoadTexture(app->renderer, "no1.png");

    btn1.rect = (SDL_Rect){300, 200, 200, 80};
    btn2.rect = (SDL_Rect){300, 350, 200, 80};
    btn1.normal = IMG_LoadTexture(app->renderer, "bt1.png");
    btn1.hover  = IMG_LoadTexture(app->renderer, "bt10.png");
    btn2.normal = IMG_LoadTexture(app->renderer, "bt2.png");
    btn2.hover  = IMG_LoadTexture(app->renderer, "bt20.png");

    current_window = 1; 
    selectedIndex = 0;
}

void Validation_HandleEvent(App* app, SDL_Event* e)
{
    if (e->type == SDL_KEYDOWN) 
    {
        if (e->key.keysym.sym == SDLK_UP || e->key.keysym.sym == SDLK_LEFT || e->key.keysym.sym == SDLK_DOWN || e->key.keysym.sym == SDLK_RIGHT) {
            selectedIndex = (selectedIndex == 0) ? 1 : 0;
            Mix_PlayChannel(-1, app->clickSound, 0);
        }
        // FIXED: Handle menu transitions directly via keyboard confirmation
        else if (e->key.keysym.sym == SDLK_RETURN) {
            Mix_PlayChannel(-1, app->clickSound, 0);
            
            if (current_window == 1) {
                if (selectedIndex == 0) { 
                    app->state = STATE_AVATAR;  // YES -> Character Mode Menu
                } else { 
                    app->state = STATE_MAIN_MENU; // NO -> Main Menu
                }
            }
        }
    }
}

void Validation_Update(App* app)
{
    if (current_window == 1) {
        Button_SetHovered(&btnYes, (selectedIndex == 0));
        Button_SetHovered(&btnNo, (selectedIndex == 1));
    } else if (current_window == 2) {
        Button_SetHovered(&btn1, (selectedIndex == 0));
        Button_SetHovered(&btn2, (selectedIndex == 1));
    }
}

void Validation_Render(App* app)
{
    Validation_RenderBackground(app);

    if (current_window == 1) {
        if (font) {
            SDL_Color color = {255, 255, 255, 255};
            SDL_Surface* textSurface = TTF_RenderText_Blended(font, "Would you like to save your game?", color);
            if (textSurface) {
                SDL_Texture* text = SDL_CreateTextureFromSurface(app->renderer, textSurface);
                SDL_Rect textPos = { (800 - textSurface->w) / 2, 200, textSurface->w, textSurface->h };
                SDL_RenderCopy(app->renderer, text, NULL, &textPos);
                SDL_DestroyTexture(text);
                SDL_FreeSurface(textSurface);
            }
        }
        Button_Render(app->renderer, &btnYes);
        Button_Render(app->renderer, &btnNo);
    } else if (current_window == 2) {
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
    if (background) SDL_DestroyTexture(background);
    if (font) TTF_CloseFont(font);
}
