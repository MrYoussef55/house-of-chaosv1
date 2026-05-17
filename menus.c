#include "menus.h"
#include "button.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

static Button btnVerify, btnRetour, btnQuitter;
static SDL_Texture *bgNom = NULL, *bgScores = NULL, *bgQuitterBG = NULL;
static TTF_Font *font = NULL;
static SDL_Rect bgRect;

static int menuState = 0; 
static int selectedIndex = 0; 

static void RenderBackground(App* app, SDL_Texture* currentBg)
{
    if (!currentBg) return;
    int texW, texH;
    SDL_QueryTexture(currentBg, NULL, NULL, &texW, &texH);
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
    SDL_RenderCopy(app->renderer, currentBg, NULL, &bgRect);
}

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

static void AfficherMeilleursScores(SDL_Renderer *renderer)
{
    FILE *f = fopen("players.txt", "r");
    if (!f) return;
    SDL_Color blanc = {255, 255, 255, 255};
    char ligne[64];
    int i = 0;
    int y = 220;
    while (i < 3 && fgets(ligne, sizeof(ligne), f)) {
        ligne[strcspn(ligne, "\n")] = '\0'; 
        AfficherTexte(renderer, ligne, blanc, 350, y);
        y += 70;
        i++;
    }
    fclose(f);
}

void Menus_Init(App* app)
{
    font = TTF_OpenFont("AUGUSTUS.ttf", 28);
    bgNom       = IMG_LoadTexture(app->renderer, "playername.png");
    bgScores    = IMG_LoadTexture(app->renderer, "bestrunmen.png");
    bgQuitterBG = IMG_LoadTexture(app->renderer, "menuplaceholder.png");

    btnVerify.rect   = (SDL_Rect){350, 450, 150, 60};
    btnVerify.normal = IMG_LoadTexture(app->renderer, "varifysmal.png");
    btnVerify.hover  = IMG_LoadTexture(app->renderer, "varifysmal.png"); // Swap with hovered asset if you have it

    btnRetour.rect   = (SDL_Rect){50, 500, 150, 60};
    btnRetour.normal = IMG_LoadTexture(app->renderer, "returnsmal.png");
    btnRetour.hover  = IMG_LoadTexture(app->renderer, "returnsmal.png");

    btnQuitter.rect   = (SDL_Rect){600, 500, 150, 60};
    btnQuitter.normal = IMG_LoadTexture(app->renderer, "quitsmal.png");
    btnQuitter.hover  = IMG_LoadTexture(app->renderer, "quitsmal.png");

    menuState = 0;
    selectedIndex = 0;
}

void Menus_HandleEvent(App* app, SDL_Event* e)
{
    if (e->type == SDL_KEYDOWN) 
    {
        int maxIndex = (menuState == 0) ? 2 : 1; // 0=Verify, 1=Retour, 2=Quitter. If state 1, only 0=Retour, 1=Quitter

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
            
            if (menuState == 0) {
                if (selectedIndex == 0) menuState = 1; // Verify -> Scores
                else if (selectedIndex == 1) app->state = STATE_MAIN_MENU; // Return
                else if (selectedIndex == 2) app->state = STATE_MAIN_MENU; // Quit
            } 
            else if (menuState == 1) {
                if (selectedIndex == 0) menuState = 0; // Return to Name
                else if (selectedIndex == 1) app->state = STATE_MAIN_MENU; // Quit
            }
        }
    }
}

void Menus_Update(App* app)
{
    if (menuState == 0) {
        Button_SetHovered(&btnVerify, (selectedIndex == 0));
        Button_SetHovered(&btnRetour, (selectedIndex == 1));
        Button_SetHovered(&btnQuitter, (selectedIndex == 2));
    } else {
        Button_SetHovered(&btnRetour, (selectedIndex == 0));
        Button_SetHovered(&btnQuitter, (selectedIndex == 1));
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
