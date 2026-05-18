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

static void drawSection(SDL_Renderer* r, SDL_Rect rect,
                        Uint8 rr, Uint8 gg, Uint8 bb) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, rr, gg, bb, 160);
    SDL_RenderFillRect(r, &rect);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}

MenuResult runMenu(SDL_Renderer* renderer) {
    MenuResult result = { MODE_NONE, INPUT_WASD, INPUT_ARROWS, 0, 1 };

    Mix_Music*  bgm   = Mix_LoadMUS("backgroundsound.mp3");
    Mix_Chunk*  click = Mix_LoadWAV("clicksound.mp3");
    if (bgm) Mix_PlayMusic(bgm, -1);

    SDL_Texture* bg             = load_t(renderer, "background.png");
    SDL_Texture* portraitOrange = load_t(renderer, "down1.png");
    SDL_Texture* portraitYellow = load_t(renderer, "yd1.png");

    /* ── State 0 ── */
    Button btnMono  = {{540, 400, 200, 80},
                       load_t(renderer, "monooff.png"),
                       load_t(renderer, "monoon.png")};
    Button btnMulti = {{540, 500, 200, 80},
                       load_t(renderer, "multioff.png"),
                       load_t(renderer, "multion.png")};
    Button btnRet0  = {{20,  650, 120, 50},
                       load_t(renderer, "retouroff.png"),
                       load_t(renderer, "retouron.png")};

    /*
     * ── State 1 layout ──
     * SOLO   : P1 section centered  (x=390..890)
     * MULTI  : P1 left (x=280..630), P2 right (x=650..1000)
     *
     * Each section has:
     *   Row A  y=240  – Input1 / Input2
     *   Row B  y=320  – Avatar1 / Avatar2
     *   Row C  y=390  – portraits (64×64 drawn in code)
     */

    /* P1 – solo positions (centered) */
    Button btnWASD_p1   = {{410, 240, 170, 55},
                            load_t(renderer, "input1off.png"),
                            load_t(renderer, "input1on.png")};
    Button btnARROWS_p1 = {{620, 240, 170, 55},
                            load_t(renderer, "input2off.png"),
                            load_t(renderer, "input2on.png")};
    Button btnAv1_p1    = {{410, 320, 170, 55},
                            load_t(renderer, "avatar1off.png"),
                            load_t(renderer, "avatar1on.png")};
    Button btnAv2_p1    = {{620, 320, 170, 55},
                            load_t(renderer, "avatar2off.png"),
                            load_t(renderer, "avatar2on.png")};

    /* P1 – multi positions (left half) */
    Button btnWASD_p1m   = {{295, 240, 150, 55},
                             load_t(renderer, "input1off.png"),
                             load_t(renderer, "input1on.png")};
    Button btnARROWS_p1m = {{470, 240, 150, 55},
                             load_t(renderer, "input2off.png"),
                             load_t(renderer, "input2on.png")};
    Button btnAv1_p1m    = {{295, 320, 150, 55},
                             load_t(renderer, "avatar1off.png"),
                             load_t(renderer, "avatar1on.png")};
    Button btnAv2_p1m    = {{470, 320, 150, 55},
                             load_t(renderer, "avatar2off.png"),
                             load_t(renderer, "avatar2on.png")};

    /* P2 – multi positions (right half) */
    Button btnWASD_p2   = {{660, 240, 150, 55},
                            load_t(renderer, "input1off.png"),
                            load_t(renderer, "input1on.png")};
    Button btnARROWS_p2 = {{835, 240, 150, 55},
                            load_t(renderer, "input2off.png"),
                            load_t(renderer, "input2on.png")};
    Button btnAv1_p2    = {{660, 320, 150, 55},
                            load_t(renderer, "avatar1off.png"),
                            load_t(renderer, "avatar1on.png")};
    Button btnAv2_p2    = {{835, 320, 150, 55},
                            load_t(renderer, "avatar2off.png"),
                            load_t(renderer, "avatar2on.png")};

    /* Footer */
    Button btnValid = {{540, 590, 200, 60},
                       load_t(renderer, "valideroff.png"),
                       load_t(renderer, "valideron.png")};
    Button btnRet1  = {{20,  650, 120, 50},
                       load_t(renderer, "retouroff.png"),
                       load_t(renderer, "retouron.png")};

    int      state   = 0;
    int      sel1    = 0;
    int      sel2    = 1;
    int      av1     = 0;
    int      av2     = 1;
    GameMode pending = MODE_NONE;
    int      running = 1;
    SDL_Event ev;
    SDL_Point mouse = {0, 0};

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
                    if (pending == MODE_SOLO) {
                        if (SDL_PointInRect(&p, &btnWASD_p1.rect))   sel1 = 0;
                        if (SDL_PointInRect(&p, &btnARROWS_p1.rect)) sel1 = 1;
                        if (SDL_PointInRect(&p, &btnAv1_p1.rect))    av1  = 0;
                        if (SDL_PointInRect(&p, &btnAv2_p1.rect))    av1  = 1;
                    } else {
                        /* P1 left */
                        if (SDL_PointInRect(&p, &btnWASD_p1m.rect))   sel1 = 0;
                        if (SDL_PointInRect(&p, &btnARROWS_p1m.rect)) sel1 = 1;
                        if (SDL_PointInRect(&p, &btnAv1_p1m.rect))    av1  = 0;
                        if (SDL_PointInRect(&p, &btnAv2_p1m.rect))    av1  = 1;
                        /* P2 right */
                        if (SDL_PointInRect(&p, &btnWASD_p2.rect))   sel2 = 0;
                        if (SDL_PointInRect(&p, &btnARROWS_p2.rect)) sel2 = 1;
                        if (SDL_PointInRect(&p, &btnAv1_p2.rect))    av2  = 0;
                        if (SDL_PointInRect(&p, &btnAv2_p2.rect))    av2  = 1;
                    }

                    if (SDL_PointInRect(&p, &btnValid.rect)) {
                        result.mode    = pending;
                        result.input1  = (sel1 == 0) ? INPUT_WASD : INPUT_ARROWS;
                        result.input2  = (sel2 == 0) ? INPUT_WASD : INPUT_ARROWS;
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
            if (pending == MODE_SOLO) {
                /* Single centered section */
                SDL_Rect sec = {385, 210, 425, 290};
                drawSection(renderer, sec, 80, 40, 10);

                Button* row[] = {&btnWASD_p1, &btnARROWS_p1,
                                 &btnAv1_p1,  &btnAv2_p1};
                drawButtons(renderer, mouse, row, 4);

                /* Portraits */
                SDL_Rect pOr = {btnAv1_p1.rect.x + (btnAv1_p1.rect.w-64)/2,
                                btnAv1_p1.rect.y + btnAv1_p1.rect.h + 6, 64, 64};
                SDL_Rect pYe = {btnAv2_p1.rect.x + (btnAv2_p1.rect.w-64)/2,
                                btnAv2_p1.rect.y + btnAv2_p1.rect.h + 6, 64, 64};
                if (portraitOrange) SDL_RenderCopy(renderer, portraitOrange, NULL, &pOr);
                if (portraitYellow) SDL_RenderCopy(renderer, portraitYellow, NULL, &pYe);

                /* Highlights */
                SDL_SetRenderDrawColor(renderer, 255, 220, 0, 255);
                SDL_Rect hlI = (sel1==0) ? btnWASD_p1.rect : btnARROWS_p1.rect;
                SDL_RenderDrawRect(renderer, &hlI);
                SDL_Rect hlA = (av1==0) ? btnAv1_p1.rect : btnAv2_p1.rect;
                SDL_RenderDrawRect(renderer, &hlA);

            } else {
                /* P1 left section */
                SDL_Rect secP1 = {278, 210, 360, 290};
                drawSection(renderer, secP1, 80, 40, 10);

                Button* rP1[] = {&btnWASD_p1m, &btnARROWS_p1m,
                                 &btnAv1_p1m,  &btnAv2_p1m};
                drawButtons(renderer, mouse, rP1, 4);

                SDL_Rect pOr1 = {btnAv1_p1m.rect.x + (btnAv1_p1m.rect.w-56)/2,
                                 btnAv1_p1m.rect.y + btnAv1_p1m.rect.h + 6, 56, 56};
                SDL_Rect pYe1 = {btnAv2_p1m.rect.x + (btnAv2_p1m.rect.w-56)/2,
                                 btnAv2_p1m.rect.y + btnAv2_p1m.rect.h + 6, 56, 56};
                if (portraitOrange) SDL_RenderCopy(renderer, portraitOrange, NULL, &pOr1);
                if (portraitYellow) SDL_RenderCopy(renderer, portraitYellow, NULL, &pYe1);

                SDL_SetRenderDrawColor(renderer, 255, 220, 0, 255);
                SDL_Rect hlI1 = (sel1==0) ? btnWASD_p1m.rect : btnARROWS_p1m.rect;
                SDL_RenderDrawRect(renderer, &hlI1);
                SDL_Rect hlA1 = (av1==0) ? btnAv1_p1m.rect : btnAv2_p1m.rect;
                SDL_RenderDrawRect(renderer, &hlA1);

                /* P2 right section */
                SDL_Rect secP2 = {648, 210, 360, 290};
                drawSection(renderer, secP2, 10, 40, 80);

                Button* rP2[] = {&btnWASD_p2, &btnARROWS_p2,
                                 &btnAv1_p2,  &btnAv2_p2};
                drawButtons(renderer, mouse, rP2, 4);

                SDL_Rect pOr2 = {btnAv1_p2.rect.x + (btnAv1_p2.rect.w-56)/2,
                                 btnAv1_p2.rect.y + btnAv1_p2.rect.h + 6, 56, 56};
                SDL_Rect pYe2 = {btnAv2_p2.rect.x + (btnAv2_p2.rect.w-56)/2,
                                 btnAv2_p2.rect.y + btnAv2_p2.rect.h + 6, 56, 56};
                if (portraitOrange) SDL_RenderCopy(renderer, portraitOrange, NULL, &pOr2);
                if (portraitYellow) SDL_RenderCopy(renderer, portraitYellow, NULL, &pYe2);

                SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
                SDL_Rect hlI2 = (sel2==0) ? btnWASD_p2.rect : btnARROWS_p2.rect;
                SDL_RenderDrawRect(renderer, &hlI2);
                SDL_Rect hlA2 = (av2==0) ? btnAv1_p2.rect : btnAv2_p2.rect;
                SDL_RenderDrawRect(renderer, &hlA2);
            }

            Button* footer[] = {&btnValid, &btnRet1};
            drawButtons(renderer, mouse, footer, 2);
        }

        SDL_RenderPresent(renderer);
    }

    /* ── Cleanup ── */
    if (portraitOrange) SDL_DestroyTexture(portraitOrange);
    if (portraitYellow) SDL_DestroyTexture(portraitYellow);
    Mix_FreeMusic(bgm);
    if (click) Mix_FreeChunk(click);
    SDL_DestroyTexture(bg);
    SDL_DestroyTexture(btnMono.texOff);       SDL_DestroyTexture(btnMono.texOn);
    SDL_DestroyTexture(btnMulti.texOff);      SDL_DestroyTexture(btnMulti.texOn);
    SDL_DestroyTexture(btnWASD_p1.texOff);    SDL_DestroyTexture(btnWASD_p1.texOn);
    SDL_DestroyTexture(btnARROWS_p1.texOff);  SDL_DestroyTexture(btnARROWS_p1.texOn);
    SDL_DestroyTexture(btnAv1_p1.texOff);     SDL_DestroyTexture(btnAv1_p1.texOn);
    SDL_DestroyTexture(btnAv2_p1.texOff);     SDL_DestroyTexture(btnAv2_p1.texOn);
    SDL_DestroyTexture(btnWASD_p1m.texOff);   SDL_DestroyTexture(btnWASD_p1m.texOn);
    SDL_DestroyTexture(btnARROWS_p1m.texOff); SDL_DestroyTexture(btnARROWS_p1m.texOn);
    SDL_DestroyTexture(btnAv1_p1m.texOff);    SDL_DestroyTexture(btnAv1_p1m.texOn);
    SDL_DestroyTexture(btnAv2_p1m.texOff);    SDL_DestroyTexture(btnAv2_p1m.texOn);
    SDL_DestroyTexture(btnWASD_p2.texOff);    SDL_DestroyTexture(btnWASD_p2.texOn);
    SDL_DestroyTexture(btnARROWS_p2.texOff);  SDL_DestroyTexture(btnARROWS_p2.texOn);
    SDL_DestroyTexture(btnAv1_p2.texOff);     SDL_DestroyTexture(btnAv1_p2.texOn);
    SDL_DestroyTexture(btnAv2_p2.texOff);     SDL_DestroyTexture(btnAv2_p2.texOn);
    SDL_DestroyTexture(btnValid.texOff);      SDL_DestroyTexture(btnValid.texOn);
    SDL_DestroyTexture(btnRet0.texOff);       SDL_DestroyTexture(btnRet0.texOn);
    SDL_DestroyTexture(btnRet1.texOff);       SDL_DestroyTexture(btnRet1.texOn);
    return result;
}
