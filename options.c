#include "options.h"
#include "button.h"
#include "button.c"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

static Button volMinus, volPlus, musOn, musOff, normalBtn, fullBtn, backBtn;
static TTF_Font* font = NULL;

/* ===== ADDED BACKGROUND ===== */
static SDL_Texture* background = NULL;
static SDL_Rect bgRect;
/* ============================= */

static void ApplyVolume(App* app)
{
    int sdlVolume = app->volume * MIX_MAX_VOLUME / 100;
    Mix_VolumeMusic(sdlVolume);
    Mix_Volume(-1, sdlVolume);
}

/* ===== ADDED BACKGROUND RENDER FUNCTION ===== */
static void RenderBackground(App* app)
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
/* ============================================ */

void Options_Init(App* app)
{
    TTF_Init();
    font = TTF_OpenFont("font.ttf", 28);

    /* ===== LOAD BACKGROUND ===== */
    background = IMG_LoadTexture(app->renderer, "backgroundddd.png");

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
    if(Button_Clicked(&volMinus,e)){ app->volume-=5; if(app->volume<0)app->volume=0; ApplyVolume(app); Mix_PlayChannel(-1,app->clickSound,0);}
    if(Button_Clicked(&volPlus,e)){ app->volume+=5; if(app->volume>100)app->volume=100; ApplyVolume(app); Mix_PlayChannel(-1,app->clickSound,0);}
    if(Button_Clicked(&musOn,e)){ Mix_ResumeMusic(); Mix_PlayChannel(-1,app->clickSound,0);}
    if(Button_Clicked(&musOff,e)){ Mix_PauseMusic(); Mix_PlayChannel(-1,app->clickSound,0);}
    if(Button_Clicked(&normalBtn,e)){ SDL_SetWindowFullscreen(app->window,0); Mix_PlayChannel(-1,app->clickSound,0);}
    if(Button_Clicked(&fullBtn,e)){ SDL_SetWindowFullscreen(app->window,SDL_WINDOW_FULLSCREEN_DESKTOP); Mix_PlayChannel(-1,app->clickSound,0);}
    if(Button_Clicked(&backBtn,e)){ app->state=STATE_MAIN_MENU; Mix_PlayChannel(-1,app->clickSound,0);}
}

void Options_Update(App* app)
{
    int mx,my;
    SDL_GetMouseState(&mx,&my);

    Button_HandleMouse(&volMinus,mx,my);
    Button_HandleMouse(&volPlus,mx,my);
    Button_HandleMouse(&musOn,mx,my);
    Button_HandleMouse(&musOff,mx,my);
    Button_HandleMouse(&normalBtn,mx,my);
    Button_HandleMouse(&fullBtn,mx,my);
    Button_HandleMouse(&backBtn,mx,my);
}

void Options_Render(App* app)
{
    /* ===== DRAW BACKGROUND FIRST ===== */
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

    if(background)
        SDL_DestroyTexture(background);

    TTF_CloseFont(font);
}	
