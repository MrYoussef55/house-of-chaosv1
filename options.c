#include "options.h"
#include "button.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

static Button volMinus, volPlus, musOn, musOff, normalBtn, fullBtn, backBtn;
static TTF_Font* font = NULL;
static SDL_Texture* background = NULL;
static SDL_Rect bgRect;

static int selectedIndex = 0; // 0 to 6

static void ApplyVolume(App* app)
{
    int sdlVolume = app->volume * MIX_MAX_VOLUME / 100;
    Mix_VolumeMusic(sdlVolume);
    Mix_Volume(-1, sdlVolume);
}

static void RenderBackground(App* app)
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

void Options_Init(App* app)
{
    font = TTF_OpenFont("AUGUSTUS.ttf", 28);
    background = IMG_LoadTexture(app->renderer, "backgroundddd.png");
    selectedIndex = 0;

    volMinus.rect = (SDL_Rect){350,160,60,60};
    volPlus.rect  = (SDL_Rect){610,160,60,60};
    musOn.rect    = (SDL_Rect){540,250,60,60};
    musOff.rect   = (SDL_Rect){420,250,60,60};
    normalBtn.rect= (SDL_Rect){380,350,120,60};
    fullBtn.rect  = (SDL_Rect){520,350,120,60};
    backBtn.rect  = (SDL_Rect){450,430,120,60};

    volMinus.normal = IMG_LoadTexture(app->renderer,"-b ub.png"); 
    volMinus.hover  = IMG_LoadTexture(app->renderer,"-d ub.png");
    volPlus.normal  = IMG_LoadTexture(app->renderer,"+b ub.png");
    volPlus.hover   = IMG_LoadTexture(app->renderer,"+d ub.png");

    musOn.normal = IMG_LoadTexture(app->renderer,"+b ub.png");
    musOn.hover  = IMG_LoadTexture(app->renderer,"+d ub.png");
    musOff.normal= IMG_LoadTexture(app->renderer,"-b ub.png");
    musOff.hover = IMG_LoadTexture(app->renderer,"-d ub.png");

    normalBtn.normal = IMG_LoadTexture(app->renderer,"normal b ub.png");
    normalBtn.hover  = IMG_LoadTexture(app->renderer,"normal d ub.png");
    fullBtn.normal   = IMG_LoadTexture(app->renderer,"fsb ub.png");
    fullBtn.hover    = IMG_LoadTexture(app->renderer,"fsd ub.png");

    backBtn.normal = IMG_LoadTexture(app->renderer,"back b ub.png");
    backBtn.hover  = IMG_LoadTexture(app->renderer,"back d ub.png");

    ApplyVolume(app);
}

void Options_HandleEvent(App* app, SDL_Event* e)
{
    if (e->type == SDL_KEYDOWN) 
    {
        if (e->key.keysym.sym == SDLK_UP || e->key.keysym.sym == SDLK_LEFT) {
            selectedIndex--;
            if (selectedIndex < 0) selectedIndex = 6;
            Mix_PlayChannel(-1, app->clickSound, 0);
        }
        else if (e->key.keysym.sym == SDLK_DOWN || e->key.keysym.sym == SDLK_RIGHT) {
            selectedIndex++;
            if (selectedIndex > 6) selectedIndex = 0;
            Mix_PlayChannel(-1, app->clickSound, 0);
        }
        else if (e->key.keysym.sym == SDLK_RETURN) {
            Mix_PlayChannel(-1, app->clickSound, 0);
            switch(selectedIndex) {
                case 0: app->volume-=5; if(app->volume<0)app->volume=0; ApplyVolume(app); break; // volMinus
                case 1: app->volume+=5; if(app->volume>100)app->volume=100; ApplyVolume(app); break; // volPlus
                case 2: Mix_ResumeMusic(); break; // musOn
                case 3: Mix_PauseMusic(); break; // musOff
                case 4: SDL_SetWindowFullscreen(app->window,0); break; // normalBtn
                case 5: SDL_SetWindowFullscreen(app->window,SDL_WINDOW_FULLSCREEN_DESKTOP); break; // fullBtn
                case 6: app->state=STATE_MAIN_MENU; break; // backBtn
            }
        }
    }
}

void Options_Update(App* app)
{
    Button_SetHovered(&volMinus, (selectedIndex == 0));
    Button_SetHovered(&volPlus, (selectedIndex == 1));
    Button_SetHovered(&musOn, (selectedIndex == 2));
    Button_SetHovered(&musOff, (selectedIndex == 3));
    Button_SetHovered(&normalBtn, (selectedIndex == 4));
    Button_SetHovered(&fullBtn, (selectedIndex == 5));
    Button_SetHovered(&backBtn, (selectedIndex == 6));
}

void Options_Render(App* app)
{
    RenderBackground(app);

    Button_Render(app->renderer,&volMinus);
    Button_Render(app->renderer,&volPlus);
    Button_Render(app->renderer,&musOn);
    Button_Render(app->renderer,&musOff);
    Button_Render(app->renderer,&normalBtn);
    Button_Render(app->renderer,&fullBtn);
    Button_Render(app->renderer,&backBtn);

    char buffer[32];
    sprintf(buffer,"Volume: %d",app->volume);

    SDL_Color white={255,255,255,255};
    SDL_Surface* s=TTF_RenderText_Blended(font,buffer,white);
    SDL_Texture* t=SDL_CreateTextureFromSurface(app->renderer,s);
    SDL_FreeSurface(s);

    int w,h;
    SDL_QueryTexture(t,NULL,NULL,&w,&h);
    SDL_Rect r={350,140,w,h};
    SDL_RenderCopy(app->renderer,t,NULL,&r);
    SDL_DestroyTexture(t);
}

void Options_Destroy()
{
    Button_Destroy(&volMinus);
    Button_Destroy(&volPlus);
    Button_Destroy(&musOn);
    Button_Destroy(&musOff);
    Button_Destroy(&normalBtn);
    Button_Destroy(&fullBtn);
    Button_Destroy(&backBtn);
    if(background) SDL_DestroyTexture(background);
    if(font) TTF_CloseFont(font);
}
