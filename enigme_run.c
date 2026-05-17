#include "enigme_run.h"
#include "enigme1.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

/*
 * Runs the quiz/puzzle loop using the existing App window & renderer.
 * Returns 1 if the player answered ONE question correctly (passed),
 *         0 if they ran out of lives (failed -> restart room).
 */
int Enigme_Run(App* app, int roomIndex)
{
    SDL_Renderer* ren = app->renderer;

    /* actual window size — scales everything from design res 1920x1080 */
    int winW = 0, winH = 0;
    SDL_GetRendererOutputSize(ren, &winW, &winH);
    float sx = (float)winW / 1920.0f;
    float sy = (float)winH / 1080.0f;
#define SX(v) ((int)((v) * sx))
#define SY(v) ((int)((v) * sy))

    /* textures */
    SDL_Texture* texMenu      = IMG_LoadTexture(ren, "choice_menu.png");
    SDL_Texture* texMenuHover = IMG_LoadTexture(ren, "image_effet.png");
    SDL_Texture* texQuiz      = IMG_LoadTexture(ren, "quizz_reponse.png");
    SDL_Texture* texCorrect   = IMG_LoadTexture(ren, "correct.png");
    SDL_Texture* texIncorrect = IMG_LoadTexture(ren, "incorrect.png");

    SDL_Texture* pTextures[4];
    pTextures[0] = IMG_LoadTexture(ren, "puzzle1.png");
    pTextures[1] = IMG_LoadTexture(ren, "puzzle2.png");
    pTextures[2] = IMG_LoadTexture(ren, "puzzle3.png");
    pTextures[3] = IMG_LoadTexture(ren, "puzzle4.png");
    SDL_Texture* pBackground = IMG_LoadTexture(ren, "backgroundsound.png");

    /* fonts scaled to window height */
    int fsXL = (int)(80 * sy); if (fsXL < 12) fsXL = 12;
    int fsG  = (int)(50 * sy); if (fsG  < 10) fsG  = 10;
    int fsQ  = (int)(32 * sy); if (fsQ  <  8) fsQ  = 8;
    int fsM  = (int)(26 * sy); if (fsM  <  8) fsM  = 8;
    TTF_Font* fontXL = TTF_OpenFont("arial.ttf", fsXL);
    TTF_Font* fontG  = TTF_OpenFont("arial.ttf", fsG);
    TTF_Font* fontQ  = TTF_OpenFont("arial.ttf", fsQ);
    TTF_Font* fontM  = TTF_OpenFont("arial.ttf", fsM);

    /* audio */
    Mix_Chunk* sonSurvol   = Mix_LoadWAV("son_bref.wav");
    Mix_Chunk* sonVictoire = Mix_LoadWAV("victoire.wav");
    Mix_Chunk* sonDefaite  = Mix_LoadWAV("defaite.wav");
    Mix_Chunk* zikMenu     = Mix_LoadWAV("theme.mp3");
    Mix_Chunk* zikQuiz     = Mix_LoadWAV("tambour.mp3");
    if (zikMenu) Mix_PlayChannel(0, zikMenu, -1);

    /* game data */
    Joueur joueur = {0, 3, 1, 0};

    Enigme enigmes[MAX_ENIGMES];
    SDL_Color coulQ = {35, 22, 8, 255};
    int nb  = charger_enigmes("questions.txt", enigmes, ren, fontQ, coulQ);
    int idx = -1;
    int i;

    /* fix texture positions to scaled coords */
    for (i = 0; i < nb; i++) {
        if (enigmes[i].texQuestion) {
            int qw, qh;
            SDL_QueryTexture(enigmes[i].texQuestion, NULL, NULL, &qw, &qh);
            enigmes[i].posQuestion.x = winW/2 - qw/2;
            enigmes[i].posQuestion.y = SY(380);
            enigmes[i].posQuestion.w = qw;
            enigmes[i].posQuestion.h = qh;
        }
        if (enigmes[i].texRep1) {
            int rw, rh;
            SDL_QueryTexture(enigmes[i].texRep1, NULL, NULL, &rw, &rh);
            enigmes[i].posRep1 = (SDL_Rect){SX(565)-rw/2, SY(682), rw, rh};
        }
        if (enigmes[i].texRep2) {
            int rw, rh;
            SDL_QueryTexture(enigmes[i].texRep2, NULL, NULL, &rw, &rh);
            enigmes[i].posRep2 = (SDL_Rect){SX(935)-rw/2, SY(682), rw, rh};
        }
        if (enigmes[i].texRep3) {
            int rw, rh;
            SDL_QueryTexture(enigmes[i].texRep3, NULL, NULL, &rw, &rh);
            enigmes[i].posRep3 = (SDL_Rect){SX(1305)-rw/2, SY(682), rw, rh};
        }
    }

    Timer timer = {0, 1875};

    /* buttons scaled to actual window */
    Bouton btnQuiz   = {{SX(710), SY(410), SX(500), SY( 90)}, 0};
    Bouton btnPuzzle = {{SX(710), SY(540), SX(500), SY( 90)}, 0};
    Bouton btnA      = {{SX(390), SY(620), SX(350), SY(160)}, 0};
    Bouton btnB      = {{SX(760), SY(620), SX(350), SY(160)}, 0};
    Bouton btnC      = {{SX(1130),SY(620), SX(350), SY(160)}, 0};

    Card pDeck[TOTAL_CARDS];
    int pFlipped      = 0;
    int pFirst        = -1;
    int pSecond       = -1;
    int pWaitStart    = -1;
    int pPreviewStart = -1;
    int pReady        = 0;

    int etat   = ETAT_SOUS_MENU;
    int frame  = 0;
    int mouseX = 0, mouseY = 0;
    int result = 0;
    int done   = 0;
    char buf[128];
    SDL_Event ev;

    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Color jaune = {255, 215,  60, 255};
    SDL_Color rouge = {255,  50,  50, 255};
    SDL_Color gris  = {180, 180, 180, 255};
    SDL_Color vert  = { 50, 255,  80, 255};

    while (!done && app->running)
    {
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_QUIT) { app->running = false; done = 1; break; }
            if (ev.type == SDL_MOUSEMOTION) {
                mouseX = ev.motion.x;
                mouseY = ev.motion.y;
            }
            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {
                if (etat == ETAT_PUZZLE) {
                    etat = ETAT_SOUS_MENU;
                    if (zikMenu) Mix_PlayChannel(0, zikMenu, -1);
                }
            }

            if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT) {
                if (etat == ETAT_SOUS_MENU && btnQuiz.is_hovered) {
                    idx = choisir_enigme(enigmes, nb);
                    timer.debut = frame;
                    timer.duree = (30-(joueur.level-1)*5)*1000/16;
                    if (timer.duree < 625) timer.duree = 625;
                    etat = ETAT_QUIZ;
                    if (zikQuiz) Mix_PlayChannel(0, zikQuiz, -1);
                }
                else if (etat == ETAT_SOUS_MENU && btnPuzzle.is_hovered) {
                    initCards(pDeck, winW, winH);
                    shuffleDeck(pDeck);
                    pFlipped = 0; pFirst = -1; pSecond = -1;
                    pWaitStart = -1; pReady = 0;
                    for (i = 0; i < TOTAL_CARDS; i++) pDeck[i].isFlipped = 1;
                    pPreviewStart = frame;
                    etat = ETAT_PUZZLE;
                    if (zikQuiz) Mix_PlayChannel(0, zikQuiz, -1);
                }
                else if (etat == ETAT_QUIZ && idx >= 0) {
                    int rep = 0;
                    if      (btnA.is_hovered) rep = 1;
                    else if (btnB.is_hovered) rep = 2;
                    else if (btnC.is_hovered) rep = 3;
                    if (rep != 0) {
                        if (rep == enigmes[idx].numbr) {
                            enigmes[idx].etat = 1;
                            if (sonVictoire) Mix_PlayChannel(0, sonVictoire, 0);
                            etat = ETAT_FIN;
                        } else {
                            enigmes[idx].etat = -1;
                            joueur.vies--;
                            if (sonDefaite) Mix_PlayChannel(0, sonDefaite, 0);
                            if (joueur.vies <= 0)
                                etat = ETAT_GAME_OVER;
                            else
                                etat = ETAT_RESULTAT;
                        }
                    }
                }
                else if (etat == ETAT_RESULTAT) {
                    idx = choisir_enigme(enigmes, nb);
                    timer.debut = frame;
                    timer.duree = (30-(joueur.level-1)*5)*1000/16;
                    if (timer.duree < 625) timer.duree = 625;
                    etat = ETAT_QUIZ;
                    if (zikQuiz) Mix_PlayChannel(0, zikQuiz, -1);
                }
                else if (etat == ETAT_FIN)       { result = 1; done = 1; }
                else if (etat == ETAT_GAME_OVER) { result = 0; done = 1; }
                else if (etat == ETAT_PUZZLE && pReady) {
                    int oldFlipped = pFlipped;
                    handleEventsPuzzle(&ev, pDeck, &pFlipped, &pFirst, &pSecond);
                    if (pFlipped == 2 && oldFlipped < 2)
                        pWaitStart = frame;
                }
            }

            if (ev.type == SDL_KEYDOWN) {
                if ((etat == ETAT_FIN || etat == ETAT_GAME_OVER) &&
                    (ev.key.keysym.sym == SDLK_SPACE || ev.key.keysym.sym == SDLK_RETURN)) {
                    if (etat == ETAT_FIN) { result = 1; done = 1; }
                    else                  { result = 0; done = 1; }
                }
                if (etat == ETAT_RESULTAT &&
                    (ev.key.keysym.sym == SDLK_SPACE || ev.key.keysym.sym == SDLK_RETURN)) {
                    idx = choisir_enigme(enigmes, nb);
                    timer.debut = frame;
                    timer.duree = (30-(joueur.level-1)*5)*1000/16;
                    if (timer.duree < 625) timer.duree = 625;
                    etat = ETAT_QUIZ;
                    if (zikQuiz) Mix_PlayChannel(0, zikQuiz, -1);
                }
                if (etat == ETAT_QUIZ && idx >= 0) {
                    int rep = 0;
                    if      (ev.key.keysym.sym == SDLK_a) rep = 1;
                    else if (ev.key.keysym.sym == SDLK_b) rep = 2;
                    else if (ev.key.keysym.sym == SDLK_c) rep = 3;
                    if (rep != 0) {
                        if (rep == enigmes[idx].numbr) {
                            enigmes[idx].etat = 1;
                            if (sonVictoire) Mix_PlayChannel(0, sonVictoire, 0);
                            etat = ETAT_FIN;
                        } else {
                            enigmes[idx].etat = -1;
                            joueur.vies--;
                            if (sonDefaite) Mix_PlayChannel(0, sonDefaite, 0);
                            if (joueur.vies <= 0)
                                etat = ETAT_GAME_OVER;
                            else
                                etat = ETAT_RESULTAT;
                        }
                    }
                }
            }
        }

        /* timer expiry */
        if (etat == ETAT_QUIZ && idx >= 0 && (frame - timer.debut) >= timer.duree) {
            enigmes[idx].etat = -1;
            joueur.vies--;
            if (sonDefaite) Mix_PlayChannel(0, sonDefaite, 0);
            if (joueur.vies <= 0)
                etat = ETAT_GAME_OVER;
            else
                etat = ETAT_RESULTAT;
        }

        /* puzzle match logic */
        if (etat == ETAT_PUZZLE) {
            if (pPreviewStart >= 0 && frame - pPreviewStart >= PREVIEW_FRAMES) {
                for (i = 0; i < TOTAL_CARDS; i++) pDeck[i].isFlipped = 0;
                pPreviewStart = -1;
                pReady = 1;
            }
            if (pReady && pWaitStart >= 0 && frame - pWaitStart >= 56) {
                if (pDeck[pFirst].textureIndex != pDeck[pSecond].textureIndex) {
                    pDeck[pFirst].isFlipped  = 0;
                    pDeck[pSecond].isFlipped = 0;
                } else {
                    pDeck[pFirst].isMatched  = 1;
                    pDeck[pSecond].isMatched = 1;
                }
                pFlipped = 0; pFirst = -1; pSecond = -1;
                pWaitStart = -1;
            }
            if (pReady) {
                int matched = 0;
                for (i = 0; i < TOTAL_CARDS; i++) if (pDeck[i].isMatched) matched++;
                if (matched == TOTAL_CARDS) { result = 1; done = 1; }
            }
        }

        /* button hover */
        if (etat == ETAT_SOUS_MENU) {
            maj_bouton(&btnQuiz,   mouseX, mouseY, sonSurvol);
            maj_bouton(&btnPuzzle, mouseX, mouseY, sonSurvol);
        } else if (etat == ETAT_QUIZ) {
            maj_bouton(&btnA, mouseX, mouseY, sonSurvol);
            maj_bouton(&btnB, mouseX, mouseY, sonSurvol);
            maj_bouton(&btnC, mouseX, mouseY, sonSurvol);
        }

        /* render */
        SDL_RenderClear(ren);

        if (etat == ETAT_SOUS_MENU) {
            SDL_RenderCopy(ren,
                (btnQuiz.is_hovered || btnPuzzle.is_hovered) ? texMenuHover : texMenu,
                NULL, NULL);
            sprintf(buf, "Score: %d", joueur.score);
            afficher_texte(ren, fontM, buf, winW/4,   SY(1020), blanc);
            sprintf(buf, "Lives: %d", joueur.vies);
            afficher_texte(ren, fontM, buf, winW/2,   SY(1020), blanc);
            sprintf(buf, "Level: %d", joueur.level);
            afficher_texte(ren, fontM, buf, winW*3/4, SY(1020), blanc);
        }
        else if (etat == ETAT_QUIZ && idx >= 0) {
            SDL_RenderCopy(ren, texQuiz, NULL, NULL);
            afficher_barre_timer(ren, &timer, frame);
            SDL_RenderCopy(ren, enigmes[idx].texQuestion, NULL, &enigmes[idx].posQuestion);
            SDL_RenderCopy(ren, enigmes[idx].texRep1,     NULL, &enigmes[idx].posRep1);
            SDL_RenderCopy(ren, enigmes[idx].texRep2,     NULL, &enigmes[idx].posRep2);
            SDL_RenderCopy(ren, enigmes[idx].texRep3,     NULL, &enigmes[idx].posRep3);
            sprintf(buf, "Score: %d   Lives: %d   Level: %d",
                joueur.score, joueur.vies, joueur.level);
            afficher_texte(ren, fontM, buf, winW/2, SY(158), jaune);
        }
        else if (etat == ETAT_RESULTAT && idx >= 0) {
            SDL_RenderCopy(ren, texIncorrect, NULL, NULL);
            afficher_texte(ren, fontXL, "INCORRECT !", winW/2, SY(50),  rouge);
            afficher_texte(ren, fontG,  "Correct answer:", winW/2, SY(170), blanc);
            {
                char* bonneRep = (enigmes[idx].numbr==1) ? enigmes[idx].reponse1
                               : (enigmes[idx].numbr==2) ? enigmes[idx].reponse2
                                                         : enigmes[idx].reponse3;
                afficher_texte(ren, fontG, bonneRep, winW/2, SY(240), jaune);
            }
            sprintf(buf, "Lives: %d", joueur.vies);
            afficher_texte(ren, fontG, buf, winW/2, SY(320), blanc);
            afficher_texte(ren, fontM, "Press SPACE or click to try again",
                winW/2, SY(1040), gris);
        }
        else if (etat == ETAT_FIN) {
            SDL_RenderCopy(ren, texCorrect, NULL, NULL);
            afficher_texte(ren, fontXL, "CORRECT!",
                winW/2, SY(40), vert);
            afficher_texte(ren, fontG,  "You passed! Moving to next room.",
                winW/2, SY(155), blanc);
            afficher_texte(ren, fontM,  "Press SPACE or click to continue",
                winW/2, SY(1040), gris);
        }
        else if (etat == ETAT_GAME_OVER) {
            SDL_RenderCopy(ren, texIncorrect, NULL, NULL);
            afficher_texte(ren, fontXL, "GAME  OVER",
                winW/2, SY(60), rouge);
            afficher_texte(ren, fontG,  "No lives left. Restarting room...",
                winW/2, SY(180), blanc);
            afficher_texte(ren, fontM,  "Press SPACE or click to restart",
                winW/2, SY(1040), gris);
        }
        else if (etat == ETAT_PUZZLE) {
            renderPuzzle(ren, pDeck, pTextures, pBackground);
            afficher_texte(ren, fontM, "Memory Game - Find all pairs!",
                winW/2, SY(30), jaune);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16);
        frame++;
    }

#undef SX
#undef SY

    /* cleanup */
    liberer_enigmes(enigmes, nb);
    if (texMenu)      SDL_DestroyTexture(texMenu);
    if (texMenuHover) SDL_DestroyTexture(texMenuHover);
    if (texQuiz)      SDL_DestroyTexture(texQuiz);
    if (texCorrect)   SDL_DestroyTexture(texCorrect);
    if (texIncorrect) SDL_DestroyTexture(texIncorrect);
    for (i = 0; i < 4; i++) if (pTextures[i]) SDL_DestroyTexture(pTextures[i]);
    if (pBackground)  SDL_DestroyTexture(pBackground);
    if (fontXL) TTF_CloseFont(fontXL);
    if (fontG)  TTF_CloseFont(fontG);
    if (fontQ)  TTF_CloseFont(fontQ);
    if (fontM)  TTF_CloseFont(fontM);
    if (sonSurvol)   Mix_FreeChunk(sonSurvol);
    if (sonVictoire) Mix_FreeChunk(sonVictoire);
    if (sonDefaite)  Mix_FreeChunk(sonDefaite);
    if (zikMenu)     Mix_FreeChunk(zikMenu);
    if (zikQuiz)     Mix_FreeChunk(zikQuiz);

    return result;
}
