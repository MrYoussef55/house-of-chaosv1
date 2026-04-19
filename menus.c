#include "menus.h"
#include "button.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

/* ===== BUTTONS AND TEXTURES ===== */
static Button btnVerify, btnRetour, btnQuitter;
static SDL_Texture *bgNom = NULL, *bgScores = NULL, *bgQuitterBG = NULL;
static TTF_Font *font = NULL;

static SDL_Rect bgRect;

/* Internal State: 0 = Nom (Player Name), 1 = Scores */
static int menuState = 0; 

/* ===== AUTO-SCALING BACKGROUND RENDER ===== */
static void RenderBackground(App* app, SDL_Texture* currentBg)
{
    if (!currentBg) return;

    int texW, texH;
    SDL_QueryTexture(currentBg, NULL, NULL, &texW, &texH);

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

    SDL_RenderCopy(app->renderer, currentBg, NULL, &bgRect);
}

/* ===== TEXT RENDERING HELPER ===== */
static void AfficherTexte(SDL_Renderer *renderer, const char *texte, SDL_Color couleur, int x, int y)
{
    if (!font || !texte || strlen(texte) == 0) return;

    SDL_Surface *surface = TTF_RenderText_Blended(font, texte, couleur);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return;

    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect pos = {x, y, w, h};
    SDL_RenderCopy(renderer, texture, NULL, &pos);
    SDL_DestroyTexture(texture);
}

/* ===== SCORE RENDERING HELPER ===== */
static void AfficherMeilleursScores(SDL_Renderer *renderer)
{
    FILE *f = fopen("players.txt", "r");
    if (!f) return; /* No scores saved yet */

    SDL_Color blanc = {255, 255, 255, 255};
    char ligne[64];
    int i = 0;
    int y = 220;

    /* Read up to 3 scores */
    while (i < 3 && fgets(ligne, sizeof(ligne), f)) {
        ligne[strcspn(ligne, "\n")] = '\0'; /* Remove newline character */
        AfficherTexte(renderer, ligne, blanc, 350, y);
        y += 70;
        i++;
    }

    fclose(f);
}

/* ===== APP STATE FUNCTIONS ===== */

void Menus_Init(App* app)
{
    TTF_Init();
    font = TTF_OpenFont("AUGUSTUS.ttf", 28);

    /* Load Backgrounds */
    bgNom       = IMG_LoadTexture(app->renderer, "playername.png");
    bgScores    = IMG_LoadTexture(app->renderer, "bestrunmen.png");
    bgQuitterBG = IMG_LoadTexture(app->renderer, "menuplaceholder.png");

    /* Initialize Buttons */
    /* Note: Since your original code only had one image per button, we apply it to both normal and hover for now */
    btnVerify.rect   = (SDL_Rect){350, 450, 150, 60};
    btnVerify.normal = IMG_LoadTexture(app->renderer, "varifysmal.png");
    btnVerify.hover  = IMG_LoadTexture(app->renderer, "varifysmal.png");

    btnRetour.rect   = (SDL_Rect){50, 500, 150, 60};
    btnRetour.normal = IMG_LoadTexture(app->renderer, "returnsmal.png");
    btnRetour.hover  = IMG_LoadTexture(app->renderer, "returnsmal.png");

    btnQuitter.rect   = (SDL_Rect){600, 500, 150, 60};
    btnQuitter.normal = IMG_LoadTexture(app->renderer, "quitsmal.png");
    btnQuitter.hover  = IMG_LoadTexture(app->renderer, "quitsmal.png");

    menuState = 0; /* Default to Player Name entry screen */
}

void Menus_HandleEvent(App* app, SDL_Event* e)
{
    /* --- RETURN BUTTON LOGIC --- */
    if (Button_Clicked(&btnRetour, e)) {
        Mix_PlayChannel(-1, app->clickSound, 0);
        
        if (menuState == 1) {
            /* If we are on the Scores screen, go back to the Name screen */
            menuState = 0; 
        } else {
            /* If we are on the Name screen, go back to the Main Menu */
             
        }
    }

    /* --- QUIT BUTTON LOGIC --- */
    if (Button_Clicked(&btnQuitter, e)) {
        Mix_PlayChannel(-1, app->clickSound, 0);
        app->state = STATE_MAIN_MENU;
    }

    /* --- VERIFY BUTTON LOGIC (Only works on Name screen) --- */
    if (menuState == 0) {
        if (Button_Clicked(&btnVerify, e)) {
            Mix_PlayChannel(-1, app->clickSound, 0);
            /* Add actual name saving logic here if you have text input */
            
            menuState = 1; /* Move to Scores screen */
        }
    }
}

void Menus_Update(App* app)
{
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    Button_HandleMouse(&btnRetour, mx, my);
    Button_HandleMouse(&btnQuitter, mx, my);

    if (menuState == 0) {
        Button_HandleMouse(&btnVerify, mx, my);
    }
}

void Menus_Render(App* app)
{
    if (menuState == 0) {
        RenderBackground(app, bgNom);
        Button_Render(app->renderer, &btnVerify);
    } else if (menuState == 1) {
        RenderBackground(app, bgScores);
        AfficherMeilleursScores(app->renderer);
    }

    /* Render shared buttons on all screens */
    Button_Render(app->renderer, &btnRetour);
    Button_Render(app->renderer, &btnQuitter);
}

void Menus_Destroy(void)
{
    Button_Destroy(&btnVerify);
    Button_Destroy(&btnRetour);
    Button_Destroy(&btnQuitter);

    if (bgNom) SDL_DestroyTexture(bgNom);
    if (bgScores) SDL_DestroyTexture(bgScores);
    if (bgQuitterBG) SDL_DestroyTexture(bgQuitterBG);

    if (font) TTF_CloseFont(font);
}
