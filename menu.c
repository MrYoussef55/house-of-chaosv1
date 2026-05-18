#include "menu.h"
#include <stdio.h>

static SDL_Texture* load_t(SDL_Renderer* r, const char* path) {
    SDL_Texture* t = IMG_LoadTexture(r, path);
    if (!t) printf("Error loading: %s\n", path);
    return t;
}

static void drawButtons(SDL_Renderer* r, SDL_Point mouse,
                        Button** btns, int count) {
    for (int i = 0; i < count; i++) {
        SDL_Texture* t = SDL_PointInRect(&mouse, &btns[i]->rect)
                         ? btns[i]->texOn : btns[i]->texOff;
        SDL_RenderCopy(r, t, NULL, &btns[i]->rect);
    }
}

MenuResult runMenu(SDL_Renderer* renderer) {
    MenuResult result = { MODE_NONE, INPUT_WASD, INPUT_ARROWS, 0, 1 };

    Mix_Music*  bgm   = Mix_LoadMUS("backgroundsound.mp3");
    Mix_Chunk*  click = Mix_LoadWAV("clicksound.mp3");
    if (bgm) Mix_PlayMusic(bgm, -1);

    SDL_Texture* bg = load_t(renderer, "background.png");

    /* Portrait previews */
    SDL_Texture* portraitOrange = load_t(renderer, "down1.png");
    SDL_Texture* portraitYellow = load_t(renderer, "yd1.png");

    /* state 0 — mode selection */
    Button btnMono  = {{540, 400, 200, 80},
                       load_t(renderer, "monooff.png"),
                       load_t(renderer, "monoon.png")};
    Button btnMulti = {{540, 500, 200, 80},
                       load_t(renderer, "multioff.png"),
                       load_t(renderer, "multion.png")};
    Button btnRet0  = {{20,  650, 120, 50},
                       load_t(renderer, "retouroff.png"),
                       load_t(renderer, "retouron.png")};

    /* state 1 — input + avatar config */
    /* Player 1 input */
    Button btnWASD   = {{300, 200, 200, 60},
                        load_t(renderer, "input1off.png"),
                        load_t(renderer, "input1on.png")};
    Button btnARROWS = {{700, 200, 200, 60},
                        load_t(renderer, "input2off.png"),
                        load_t(renderer, "input2on.png")};

    /* Player 1 avatar */
    Button btnAvatar1P1 = {{300, 340, 160, 60},
                           load_t(renderer, "avatar1off.png"),
                           load_t(renderer, "avatar1on.png")};
    Button btnAvatar2P1 = {{700, 340, 160, 60},
                           load_t(renderer, "avatar2off.png"),
                           load_t(renderer, "avatar2on.png")};

    /* Player 2 input (multi only) */
    Button btnWASD2   = {{300, 480, 200, 60},
                         load_t(renderer, "input1off.png"),
                         load_t(renderer, "input1on.png")};
    Button btnARROWS2 = {{700, 480, 200, 60},
                         load_t(renderer, "input2off.png"),
                         load_t(renderer, "input2on.png")};

    /* Player 2 avatar (multi only) */
    Button btnAvatar1P2 = {{300, 560, 160, 60},
                           load_t(renderer, "avatar1off.png"),
                           load_t(renderer, "avatar1on.png")};
    Button btnAvatar2P2 = {{700, 560, 160, 60},
                           load_t(renderer, "avatar2off.png"),
                           load_t(renderer, "avatar2on.png")};

    Button btnValid = {{540, 620, 200, 60},
                       load_t(renderer, "valideroff.png"),
                       load_t(renderer, "valideron.png")};
    Button btnRet1  = {{20,  650, 120, 50},
                       load_t(renderer, "retouroff.png"),
                       load_t(renderer, "retouron.png")};

    int      state   = 0;
    int      sel1    = 0;   /* player1 input: 0=WASD, 1=ARROWS */
    int      sel2    = 1;   /* player2 input: 0=WASD, 1=ARROWS */
    int      av1     = 0;   /* player1 avatar: 0=orange, 1=yellow */
    int      av2     = 1;   /* player2 avatar: 0=orange, 1=yellow */
    GameMode pending = MODE_NONE;
    int      running = 1;
    SDL_Event ev;
    SDL_Point mouse  = {0, 0};

    while (running == 1) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) { result.mode = MODE_NONE; running = 0; }
            if (ev.type == SDL_MOUSEBUTTONDOWN) {
                if (click) Mix_PlayChannel(-1, click, 0);
                SDL_Point p = {ev.button.x, ev.button.y};

                if (state == 0) {
                    if (SDL_PointInRect(&p, &btnMono.rect))  { pending = MODE_SOLO;  state = 1; }
                    if (SDL_PointInRect(&p, &btnMulti.rect)) { pending = MODE_MULTI; state = 1; }
                    if (SDL_PointInRect(&p, &btnRet0.rect))  { result.mode = MODE_NONE; running = 0; }

                } else {
                    /* player1 input */
                    if (SDL_PointInRect(&p, &btnWASD.rect))   sel1 = 0;
                    if (SDL_PointInRect(&p, &btnARROWS.rect)) sel1 = 1;

                    /* player1 avatar */
                    if (SDL_PointInRect(&p, &btnAvatar1P1.rect)) av1 = 0;
                    if (SDL_PointInRect(&p, &btnAvatar2P1.rect)) av1 = 1;

                    if (pending == MODE_MULTI) {
                        /* player2 input */
                        if (SDL_PointInRect(&p, &btnWASD2.rect))   sel2 = 0;
                        if (SDL_PointInRect(&p, &btnARROWS2.rect)) sel2 = 1;

                        /* player2 avatar */
                        if (SDL_PointInRect(&p, &btnAvatar1P2.rect)) av2 = 0;
                        if (SDL_PointInRect(&p, &btnAvatar2P2.rect)) av2 = 1;
                    }

                    if (SDL_PointInRect(&p, &btnValid.rect)) {
                        result.mode   = pending;
                        result.input1 = (sel1 == 0) ? INPUT_WASD : INPUT_ARROWS;
                        result.input2 = (sel2 == 0) ? INPUT_WASD : INPUT_ARROWS;
                        result.avatar1 = av1;
                        result.avatar2 = av2;
                        running = 0;
                    }
                    if (SDL_PointInRect(&p, &btnRet1.rect)) state = 0;
                }
            }
        }

        SDL_GetMouseState(&mouse.x, &mouse.y);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bg, NULL, NULL);

        if (state == 0) {
            Button* s0[] = {&btnMono, &btnMulti, &btnRet0};
            drawButtons(renderer, mouse, s0, 3);

        } else {
            /* ---- Always shown: Player 1 section ---- */

            /* Section label area (simple colored rect as bg) */
            SDL_SetRenderDrawColor(renderer, 80, 40, 10, 180);
            SDL_Rect p1bg = {260, 170, 680, 240};
            SDL_RenderFillRect(renderer, &p1bg);

            /* Player 1 input row */
            Button* inputRow1[] = {&btnWASD, &btnARROWS};
            drawButtons(renderer, mouse, inputRow1, 2);

            /* Player 1 avatar row */
            Button* avatarRow1[] = {&btnAvatar1P1, &btnAvatar2P1};
            drawButtons(renderer, mouse, avatarRow1, 2);

            /* Portrait previews for P1 */
            SDL_Rect portraitOrangeRect = {btnAvatar1P1.rect.x + 10,
                                           btnAvatar1P1.rect.y + btnAvatar1P1.rect.h + 8,
                                           80, 80};
            SDL_Rect portraitYellowRect = {btnAvatar2P1.rect.x + 10,
                                           btnAvatar2P1.rect.y + btnAvatar2P1.rect.h + 8,
                                           80, 80};
            if (portraitOrange) SDL_RenderCopy(renderer, portraitOrange, NULL, &portraitOrangeRect);
            if (portraitYellow) SDL_RenderCopy(renderer, portraitYellow, NULL, &portraitYellowRect);

            /* Highlight selected input P1 */
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_Rect hlInput1 = (sel1 == 0) ? btnWASD.rect : btnARROWS.rect;
            SDL_RenderDrawRect(renderer, &hlInput1);

            /* Highlight selected avatar P1 */
            SDL_Rect hlAv1 = (av1 == 0) ? btnAvatar1P1.rect : btnAvatar2P1.rect;
            SDL_RenderDrawRect(renderer, &hlAv1);

            /* ---- Multi-only: Player 2 section ---- */
            if (pending == MODE_MULTI) {
                SDL_SetRenderDrawColor(renderer, 10, 40, 80, 180);
                SDL_Rect p2bg = {260, 460, 680, 175};
                SDL_RenderFillRect(renderer, &p2bg);

                Button* inputRow2[] = {&btnWASD2, &btnARROWS2};
                drawButtons(renderer, mouse, inputRow2, 2);

                Button* avatarRow2[] = {&btnAvatar1P2, &btnAvatar2P2};
                drawButtons(renderer, mouse, avatarRow2, 2);

                /* Highlight selected input P2 */
                SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
                SDL_Rect hlInput2 = (sel2 == 0) ? btnWASD2.rect : btnARROWS2.rect;
                SDL_RenderDrawRect(renderer, &hlInput2);

                /* Highlight selected avatar P2 */
                SDL_Rect hlAv2 = (av2 == 0) ? btnAvatar1P2.rect : btnAvatar2P2.rect;
                SDL_RenderDrawRect(renderer, &hlAv2);
            }

            /* Validate + back */
            Button* footer[] = {&btnValid, &btnRet1};
            drawButtons(renderer, mouse, footer, 2);
        }

        SDL_RenderPresent(renderer);
    }

    /* Cleanup */
    if (portraitOrange) SDL_DestroyTexture(portraitOrange);
    if (portraitYellow) SDL_DestroyTexture(portraitYellow);
    Mix_FreeMusic(bgm);
    if (click) Mix_FreeChunk(click);
    SDL_DestroyTexture(bg);
    SDL_DestroyTexture(btnMono.texOff);      SDL_DestroyTexture(btnMono.texOn);
    SDL_DestroyTexture(btnMulti.texOff);     SDL_DestroyTexture(btnMulti.texOn);
    SDL_DestroyTexture(btnWASD.texOff);      SDL_DestroyTexture(btnWASD.texOn);
    SDL_DestroyTexture(btnARROWS.texOff);    SDL_DestroyTexture(btnARROWS.texOn);
    SDL_DestroyTexture(btnWASD2.texOff);     SDL_DestroyTexture(btnWASD2.texOn);
    SDL_DestroyTexture(btnARROWS2.texOff);   SDL_DestroyTexture(btnARROWS2.texOn);
    SDL_DestroyTexture(btnAvatar1P1.texOff); SDL_DestroyTexture(btnAvatar1P1.texOn);
    SDL_DestroyTexture(btnAvatar2P1.texOff); SDL_DestroyTexture(btnAvatar2P1.texOn);
    SDL_DestroyTexture(btnAvatar1P2.texOff); SDL_DestroyTexture(btnAvatar1P2.texOn);
    SDL_DestroyTexture(btnAvatar2P2.texOff); SDL_DestroyTexture(btnAvatar2P2.texOn);
    SDL_DestroyTexture(btnValid.texOff);     SDL_DestroyTexture(btnValid.texOn);
    SDL_DestroyTexture(btnRet0.texOff);      SDL_DestroyTexture(btnRet0.texOn);
    SDL_DestroyTexture(btnRet1.texOff);      SDL_DestroyTexture(btnRet1.texOn);
    return result;
}
