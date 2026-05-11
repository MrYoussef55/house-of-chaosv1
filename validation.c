#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

#define W 1800
#define H 1000

#define FRAME_W 300
#define FRAME_H 449

#define SPEED 10
#define SCALE 0.5

SDL_Window *win = NULL;
SDL_Renderer *ren = NULL;

SDL_Texture *playerTex;
SDL_Texture *bg[4];

int bg1 = 0;
int bg2 = 0;

// ================= LEFT PLAYER
int p1X = W / 4;
int p1Y = H / 2;
int frame1 = 0;
int dir1 = 0;

// ================= RIGHT PLAYER
int p2X = 3 * W / 4;
int p2Y = H / 2;
int frame2 = 0;
int dir2 = 0;

// ================= TIMER
TTF_Font *font = NULL;

bool timer1Started = false;
bool timer2Started = false;

Uint32 startTime1 = 0;
Uint32 startTime2 = 0;

// ================= GUIDE
bool showGuide = false;

void init() {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    win = SDL_CreateWindow("Split Screen",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        W, H, 0);

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
}

void load() {
    playerTex = IMG_LoadTexture(ren, "player.png");

    bg[0] = IMG_LoadTexture(ren, "1.png");
    bg[1] = IMG_LoadTexture(ren, "2.png");
    bg[2] = IMG_LoadTexture(ren, "3.png");
    bg[3] = IMG_LoadTexture(ren, "4.png");

    font = TTF_OpenFont("/home/hadir/hadir validation 2/Beautiful.otf", 24);
}

// ================= TIMER
void renderTime(bool started, Uint32 start, int x, int y) {
    if (!started || !font) return;

    Uint32 elapsed = (SDL_GetTicks() - start) / 1000;

    int m = elapsed / 60;
    int s = elapsed % 60;

    char txt[50];
    sprintf(txt, "TIME : %02d:%02d", m, s);

    SDL_Color c = {255,255,255};

    SDL_Surface *surf = TTF_RenderText_Solid(font, txt, c);
    if (!surf) return;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);

    SDL_Rect dst = {x, y, surf->w, surf->h};

    SDL_FreeSurface(surf); 

    if (tex) {
        SDL_RenderCopy(ren, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
    }
}

// ================= GUIDE
void renderGuide() {
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND); 
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 200);

    // SMALL WINDOW
    SDL_Rect box = {W/2 - 150, H/2 - 100, 300, 200};
    SDL_RenderFillRect(ren, &box);

    SDL_Color c = {255,255,255};

    const char *lines[] = {
        "GUIDE",
        "LEFT : arrows",
        "RIGHT : Q S D Z",
        "ESC : menu"
    };

    for (int i = 0; i < 4; i++) {
        SDL_Surface *surf = TTF_RenderText_Solid(font, lines[i], c);
        if (!surf) continue;

        SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);

        SDL_Rect dst = {
            box.x + 20,
            box.y + 20 + i*30, // spacing smaller
            surf->w,
            surf->h
        };

        SDL_FreeSurface(surf);

        if (tex) {
            SDL_RenderCopy(ren, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
        }
    }
}

// ================= INPUT
void input(bool *running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            *running = false;

        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                showGuide = !showGuide;
            }
        }
    }

    const Uint8 *k = SDL_GetKeyboardState(NULL);

    int w = FRAME_W * SCALE;
    int h = FRAME_H * SCALE;

    // LEFT PLAYER
    bool move1 = false;

    if (k[SDL_SCANCODE_UP])    { dir1 = 1; p1Y -= SPEED; move1 = true; }
    if (k[SDL_SCANCODE_DOWN])  { dir1 = 0; p1Y += SPEED; move1 = true; }
    if (k[SDL_SCANCODE_LEFT])  { dir1 = 2; p1X -= SPEED; move1 = true; }
    if (k[SDL_SCANCODE_RIGHT]) { dir1 = 3; p1X += SPEED; move1 = true; }

    if (move1) {
        frame1 = (frame1 + 1) % 4; //animation
        if (!timer1Started) {
            timer1Started = true;
            startTime1 = SDL_GetTicks();
        }
    }

    if (p1X < 0) { bg1 = (bg1 + 1) % 4; p1X = 0; } //gauche
    if (p1X + w > W/2) { bg1 = (bg1 + 1) % 4; p1X = W/2 - w; } //depasse la moitie de lecran
    if (p1Y < 0) { bg1 = (bg1 + 1) % 4; p1Y = 0; }//haut
    if (p1Y + h > H) { bg1 = (bg1 + 1) % 4; p1Y = H - h; }//bas

    // RIGHT PLAYER
    bool move2 = false;

    if (k[SDL_SCANCODE_Z]) { dir2 = 1; p2Y -= SPEED; move2 = true; }
    if (k[SDL_SCANCODE_S]) { dir2 = 0; p2Y += SPEED; move2 = true; }
    if (k[SDL_SCANCODE_Q]) { dir2 = 2; p2X -= SPEED; move2 = true; }
    if (k[SDL_SCANCODE_D]) { dir2 = 3; p2X += SPEED; move2 = true; }

    if (move2) {
        frame2 = (frame2 + 1) % 4;
        if (!timer2Started) {
            timer2Started = true;
            startTime2 = SDL_GetTicks();
        }
    }

    if (p2X < W/2) { bg2 = (bg2 + 1) % 4; p2X = W/2; } // non gauche 
    if (p2X + w > W) { bg2 = (bg2 + 1) % 4; p2X = W - w; } //droit 
    if (p2Y < 0) { bg2 = (bg2 + 1) % 4; p2Y = 0; }// bloque en haut 
    if (p2Y + h > H) { bg2 = (bg2 + 1) % 4; p2Y = H - h; }//limite en bas
}

// ================= RENDER
void render() {
    SDL_RenderClear(ren);

    int w = FRAME_W * SCALE;
    int h = FRAME_H * SCALE;

    SDL_Rect leftView = {0, 0, W/2, H};
    SDL_RenderSetViewport(ren, &leftView);

    SDL_RenderCopy(ren, bg[bg1], NULL, NULL);

    SDL_Rect src1 = {frame1 * FRAME_W, dir1 * FRAME_H, FRAME_W, FRAME_H};
    SDL_Rect dst1 = {p1X, p1Y, w, h}; //position du joueur a lecran

    SDL_RenderCopy(ren, playerTex, &src1, &dst1); //affichage du joueur 1
    renderTime(timer1Started, startTime1, 10, 10);//affiche temps en haut gauche de la zone

    SDL_Rect rightView = {W/2, 0, W/2, H};
    SDL_RenderSetViewport(ren, &rightView);

    SDL_RenderCopy(ren, bg[bg2], NULL, NULL);

    SDL_Rect src2 = {frame2 * FRAME_W, dir2 * FRAME_H, FRAME_W, FRAME_H};//sprite joueur 2
    SDL_Rect dst2 = {p2X - W/2, p2Y, w, h};

    SDL_RenderCopy(ren, playerTex, &src2, &dst2);
    renderTime(timer2Started, startTime2, 10, 10);

    // GUIDE
    SDL_RenderSetViewport(ren, NULL);
    if (showGuide) renderGuide();

    SDL_RenderPresent(ren);
}

// ================= CLEAN
void clean() {
    SDL_DestroyTexture(playerTex);
    for (int i = 0; i < 4; i++)
        SDL_DestroyTexture(bg[i]);

    TTF_CloseFont(font);

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main() {
    bool running = true;

    init();
    load();

    while (running) {
        input(&running);
        render();
        SDL_Delay(40);
    }

    clean();
    return 0;
}
