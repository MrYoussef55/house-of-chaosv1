/**
 * @file main.c
 * @brief Application entry point. Handles state machine integration.
 */

#include "header.h"
#include <stdio.h>

/**
 * @brief High-level states for the master application loop.
 */
typedef enum {
    MAIN_MENU,
    SPLIT_SCREEN_GAME,
    SAVE_PROMPT,
    SCORE_BOARD
} AppState;

AppState currentAppState = MAIN_MENU;

/**
 * @brief The core execution thread.
 */
int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    TTF_Init();
    SDL_StartTextInput();

    win = SDL_CreateWindow("Home Alone - Integrated Edition",
                           SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           W, H, SDL_WINDOW_SHOWN);
    
    if (!win) {
        printf("Error creating window: %s\n", SDL_GetError());
        return 1;
    }
    
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    Fonds fonds; Sons sons; Texte texte;
    Bouton btn_verify  = {NULL, {W/2 - 82, H/2 + 70, 165, 70}, 0};
    Bouton btn_retour  = {NULL, {W/2 + 50, H/2 + 190, 160, 60}, 0};

    if (!charger_fonds(ren, &fonds)) { printf("Error: Missing Menu Backgrounds!\n"); return 1; }
    if (!charger_boutons(ren, &btn_verify, &btn_retour, NULL)) { printf("Error: Missing Button Images!\n"); return 1; }
    if (!charger_sons(&sons)) { printf("Error: Missing Sound Files (.mp3)!\n"); return 1; }
    if (!charger_texte(&texte)) { printf("Error: Missing Font File (AUGUSTUS.ttf)!\n"); return 1; }
    
    load(); 
    
    SDL_Surface* maskSurf = IMG_Load("backgroundmask2.png");
    if (!maskSurf) {
        printf("Warning: backgroundmask2.png is missing! Perfect collisions disabled.\n");
    }
    
    Barrier b; 
    InitBarrier(&b, (SDL_Rect){600, 400, 80, 80}, NULL, 4, 1);
    
    Minimap m; 
    InitMinimap(&m, ren, "map.png", "dot.png", (SDL_Rect){20, 20, 200, 200});

    char nom[64] = {0};
    int len_nom = 0;
    bool running = true;
    SDL_Event e;

    while (running) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (currentAppState == MAIN_MENU) {
                if (e.type == SDL_TEXTINPUT && len_nom < 62) {
                    strcat(nom, e.text.text);
                    len_nom = strlen(nom);
                }
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_BACKSPACE && len_nom > 0) nom[--len_nom] = '\0';
                    if (e.key.keysym.sym == SDLK_RETURN && len_nom > 0) currentAppState = SPLIT_SCREEN_GAME;
                }
                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    if (souris_sur_bouton(mx, my, &btn_verify) && len_nom > 0) currentAppState = SPLIT_SCREEN_GAME;
                }
            } 
            else if (currentAppState == SPLIT_SCREEN_GAME) {
                handleInput(&e, &running);
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_s) {
                    fenetre1(ren); 
                }
            }
        }

        if (currentAppState == SPLIT_SCREEN_GAME) {
            MoveBarrier(&b);

            updateGame(maskSurf, &b); 

            SDL_Rect p1Rect = {p1X, p1Y, (int)(FRAME_W * SCALE), (int)(FRAME_H * SCALE)};
            MAJMinimap(&m, p1Rect, (SDL_Rect){0,0,W,H});
        }

        SDL_RenderClear(ren);

        if (currentAppState == MAIN_MENU) {
            SDL_RenderCopy(ren, fonds.bg_nom, NULL, NULL);
            if (len_nom > 0) {
                afficher_texte(ren, texte.police, nom, (SDL_Color){255, 255, 255, 255}, W/2 - 100, H/2 - 50);
            }
            afficher_bouton(ren, &btn_verify);
        } 
        else if (currentAppState == SPLIT_SCREEN_GAME) {
            render(); 
            
            SDL_RenderSetViewport(ren, &(SDL_Rect){0, 0, W/2, H});
            AfficherBarrier(ren, b);
            
            SDL_RenderSetViewport(ren, NULL);
            AfficherMinimap(ren, m);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16); 
    }

    if (maskSurf) SDL_FreeSurface(maskSurf);
    LibererMinimap(&m);
    clean();
    return 0;
}

