/**
 * @file logic.c
 * @brief Implementation file for game mechanics, UI logic, and asset management.
 */

#include "header.h"
#include <stdio.h>
#include <string.h>

GameState currentState = STATE_PLAYING;
SDL_Window *win = NULL;
SDL_Renderer *ren = NULL;

SDL_Texture *playerTex = NULL;
SDL_Texture *bg[4];
int bg1 = 0, bg2 = 0;

int p1X = 150, p1Y = 300, frame1 = 0, dir1 = 0;
int p2X = W / 2 + 150, p2Y = 300, frame2 = 0, dir2 = 0;

TTF_Font *font = NULL;
bool timer1Started = false, timer2Started = false;
Uint32 startTime1 = 0, startTime2 = 0;
bool showGuide = false;
bool ghostMode = false; 

SDL_Rect kitchenTopCounter = { 0, 0, 900, 250 };   
SDL_Rect kitchenIsland     = { 580, 500, 150, 300 }; 
SDL_Rect kitchenLeftDesk   = { 0, 250, 150, 200 };

/**
 * @brief Helper function to quickly load a texture from a file path.
 */
SDL_Texture* loadTexture(char *path, SDL_Renderer *renderer) {
    SDL_Surface *surface = IMG_Load(path);
    if(!surface) return NULL;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

/**
 * @brief Initializes the minimap struct with images and coordinates.
 */
void InitMinimap(Minimap* m, SDL_Renderer* renderer, char* mapPath, char* dotPath, SDL_Rect minimapPos) {
    m->backgroundTexture = loadTexture(mapPath, renderer);
    m->playerDot = loadTexture(dotPath, renderer);
    m->minimapPosition = minimapPos;
    m->playerDotPosition = (SDL_Rect){minimapPos.x, minimapPos.y, 20, 20};
}

/**
 * @brief Updates the player indicator dot position relative to camera and scale.
 */
void MAJMinimap(Minimap* m, SDL_Rect playerPos, SDL_Rect camera) {
    int absoluteX = playerPos.x + camera.x;
    int absoluteY = playerPos.y + camera.y;
    m->playerDotPosition.x = m->minimapPosition.x + (absoluteX * SCALE_FACTOR) / 100;
    m->playerDotPosition.y = m->minimapPosition.y + (absoluteY * SCALE_FACTOR) / 100;
}

/**
 * @brief Renders the minimap and the player dot to the screen.
 */
void AfficherMinimap(SDL_Renderer* renderer, Minimap m) {
    if(m.backgroundTexture) SDL_RenderCopy(renderer, m.backgroundTexture, NULL, &m.minimapPosition);
    if(m.playerDot) SDL_RenderCopy(renderer, m.playerDot, NULL, &m.playerDotPosition);
}

/**
 * @brief Frees the textures associated with the minimap to prevent memory leaks.
 */
void LibererMinimap(Minimap* m) {
    if(m->backgroundTexture) SDL_DestroyTexture(m->backgroundTexture);
    if(m->playerDot) SDL_DestroyTexture(m->playerDot);
}

/**
 * @brief Evaluates an Axis-Aligned Bounding Box (AABB) collision between two rectangles.
 */
int CollisionAABB(SDL_Rect rect1, SDL_Rect rect2) {
    return !(rect1.x + rect1.w < rect2.x || rect1.x > rect2.x + rect2.w ||
             rect1.y + rect1.h < rect2.y || rect1.y > rect2.y + rect2.h);
}

/**
 * @brief Retrieves the color data of a specific pixel from a surface.
 */
SDL_Color GetPixel(SDL_Surface* surface, int x, int y) {
    SDL_Color color = {255, 255, 255, 255};
    if (!surface || x < 0 || x >= surface->w || y < 0 || y >= surface->h) return color;
    Uint8* p = (Uint8*)surface->pixels + (y * surface->pitch) + (x * surface->format->BytesPerPixel);
    Uint32 val = 0;
    memcpy(&val, p, surface->format->BytesPerPixel);
    SDL_GetRGBA(val, surface->format, &color.r, &color.g, &color.b, &color.a);
    return color;
}

/**
 * @brief Checks for a perfect pixel collision against a mask, implementing doorway safe zones and carpet punches.
 */
int CollisionParfaite(SDL_Surface* mask, SDL_Rect player, int roomIndex) {
    if(!mask) return 0;
    SDL_Point pts[8] = {
        {player.x, player.y}, {player.x+player.w/2, player.y}, {player.x+player.w, player.y},
        {player.x, player.y+player.h/2}, {player.x+player.w, player.y+player.h/2},
        {player.x, player.y+player.h}, {player.x+player.w/2, player.y+player.h}, {player.x+player.w, player.y+player.h}
    };
    for(int i=0; i<8; i++) {
        int px = pts[i].x;
        int py = pts[i].y;

        if (px < 40 || px > (W/2) - 40 || py < 40 || py > H - 40) continue; 
        if (px > 150 && px < 750 && py > 350 && py < 950) continue; 

        SDL_Color c = GetPixel(mask, px, py);
        if(c.r == 0 && c.g == 0 && c.b == 0 && c.a > 128) return 1;
    }
    return 0;
}

/**
 * @brief Initializes a moving barrier obstacle.
 */
void InitBarrier(Barrier* b, SDL_Rect pos, SDL_Texture* tex, int spd, int dir) {
    b->position = pos; b->sprite = tex; b->speed = spd; b->direction = dir;
}

/**
 * @brief Updates the barrier's position and reverses direction upon hitting limits.
 */
void MoveBarrier(Barrier* b) {
    b->position.x += b->speed * b->direction;
    if (b->position.x > 800 || b->position.x < 400) b->direction *= -1;
}

/**
 * @brief Renders the barrier as a colored rectangle.
 */
void AfficherBarrier(SDL_Renderer* renderer, Barrier b) {
    SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255);
    SDL_RenderFillRect(renderer, &b.position);
}

/**
 * @brief Renders the secondary save state confirmation window.
 */
void fenetre2(SDL_Renderer *renderer) {
    SDL_Texture *bgTex = loadTexture("backgroundtemp.png", renderer);
    SDL_Texture *bt1 = loadTexture("bt1.png", renderer);
    SDL_Texture *bt2 = loadTexture("bt2.png", renderer);
    SDL_Rect pos1={555,500,200,80}, pos2={300,500,200,80};
    int quit = 0; SDL_Event e;
    while(!quit) {
        while(SDL_PollEvent(&e)) if(e.type == SDL_QUIT) quit = 1;
        SDL_RenderClear(renderer);
        if(bgTex) SDL_RenderCopy(renderer, bgTex, NULL, NULL);
        if(bt1) SDL_RenderCopy(renderer, bt1, NULL, &pos1);
        if(bt2) SDL_RenderCopy(renderer, bt2, NULL, &pos2);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(bgTex); SDL_DestroyTexture(bt1); SDL_DestroyTexture(bt2);
}

/**
 * @brief Triggers the primary save game prompt interface loop.
 */
void fenetre1(SDL_Renderer *renderer) {
    SDL_Texture *bg = loadTexture("background.png", renderer);
    TTF_Font *font_local = TTF_OpenFont("font.ttf", 70);
    SDL_Color white = {255, 255, 255};
    SDL_Texture *txt = NULL;
    SDL_Rect txtPos = {0,0,0,0};
    if(font_local) {
        SDL_Surface *surf = TTF_RenderText_Blended(font_local, "Save Game?", white);
        if(surf) {
            txt = SDL_CreateTextureFromSurface(renderer, surf);
            txtPos = (SDL_Rect){(1024-surf->w)/2, 400, surf->w, surf->h};
            SDL_FreeSurface(surf);
        }
    }
    int quit = 0; SDL_Event e;
    while(!quit) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) quit = 1;
            if(e.type == SDL_MOUSEBUTTONDOWN) {
                if(e.button.x > 300 && e.button.x < 500) fenetre2(renderer);
                else quit = 1;
            }
        }
        SDL_RenderClear(renderer);
        if(bg) SDL_RenderCopy(renderer, bg, NULL, NULL);
        if(txt) SDL_RenderCopy(renderer, txt, NULL, &txtPos);
        SDL_RenderPresent(renderer);
    }
    if(bg) SDL_DestroyTexture(bg); 
    if(txt) SDL_DestroyTexture(txt); 
    if(font_local) TTF_CloseFont(font_local);
}

void init() { }

/**
 * @brief Loads the sprites, backgrounds, and fonts specifically for gameplay.
 */
void load() {
    playerTex = IMG_LoadTexture(ren, "player.png");
    bg[0] = IMG_LoadTexture(ren, "1.png");
    bg[1] = IMG_LoadTexture(ren, "2.png");
    bg[2] = IMG_LoadTexture(ren, "3.png");
    bg[3] = IMG_LoadTexture(ren, "4.png");
    font = TTF_OpenFont("Beautiful.otf", 24);
}

/**
 * @brief Calculates and renders the elapsed time string onto the screen.
 */
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

/**
 * @brief Parses global SDL events specific to the gameplay loop.
 */
void handleInput(SDL_Event *e, bool *running) {
    if (e->type == SDL_QUIT) *running = false;
    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_ESCAPE) showGuide = !showGuide;
        if (e->key.keysym.sym == SDLK_g) {
            ghostMode = !ghostMode; 
            printf("Ghost Mode is now %s\n", ghostMode ? "ON" : "OFF");
        }
        if (e->key.keysym.sym == SDLK_r) {
            p1X = W / 4; p1Y = H / 2;
            p2X = (3 * W) / 4; p2Y = H / 2;
        }
    }
}

/**
 * @brief Core game mechanics: Reads keyboard states, manages collisions, and handles screen wrapping.
 */
void updateGame(SDL_Surface* maskSurf, Barrier* b) {
    const Uint8 *k = SDL_GetKeyboardState(NULL);
    int w = (int)(FRAME_W * SCALE);
    int h = (int)(FRAME_H * SCALE);

    bool move1 = false;
    int prevX1 = p1X; 
    int prevY1 = p1Y; 

    if (k[SDL_SCANCODE_UP])    { dir1 = 1; p1Y -= SPEED; move1 = true; }
    if (k[SDL_SCANCODE_DOWN])  { dir1 = 0; p1Y += SPEED; move1 = true; }
    if (k[SDL_SCANCODE_LEFT])  { dir1 = 2; p1X -= SPEED; move1 = true; }
    if (k[SDL_SCANCODE_RIGHT]) { dir1 = 3; p1X += SPEED; move1 = true; }

    if (move1) {
        frame1 = (frame1 + 1) % 4;
        if (!timer1Started) { timer1Started = true; startTime1 = SDL_GetTicks(); }
    }

    SDL_Rect p1Hitbox = { p1X + 35, p1Y + h - 40, w - 70, 40 };
    bool collided1 = false;
    
    if (bg1 == 0) { 
        if (CollisionAABB(p1Hitbox, kitchenTopCounter)) collided1 = true;
        if (CollisionAABB(p1Hitbox, kitchenIsland)) collided1 = true;
        if (CollisionAABB(p1Hitbox, kitchenLeftDesk)) collided1 = true;
        if (b && CollisionAABB(p1Hitbox, b->position)) collided1 = true;
    } else {
        if (maskSurf && CollisionParfaite(maskSurf, p1Hitbox, bg1)) collided1 = true;
        if (b && CollisionAABB(p1Hitbox, b->position)) collided1 = true;
    }

    if (!ghostMode && collided1) {
        p1X = prevX1;
        p1Y = prevY1;
    }

    if (p1X < 0) { bg1 = (bg1 + 1) % 4; p1X = (W/2) - w - 20; }
    else if (p1X + w > W/2) { bg1 = (bg1 + 1) % 4; p1X = 160; }
    
    if (p1Y < 0) { bg1 = (bg1 + 1) % 4; p1Y = H - h - 20; }
    else if (p1Y + h > H) { bg1 = (bg1 + 1) % 4; p1Y = 260; }

    if (!ghostMode) {
        SDL_Rect currentBox1 = { p1X + 35, p1Y + h - 40, w - 70, 40 };
        bool currentlyStuck1 = false;
        if (bg1 == 0) { 
            if (CollisionAABB(currentBox1, kitchenTopCounter) || CollisionAABB(currentBox1, kitchenIsland) || CollisionAABB(currentBox1, kitchenLeftDesk) || (b && CollisionAABB(currentBox1, b->position))) currentlyStuck1 = true;
        } else {
            if ((maskSurf && CollisionParfaite(maskSurf, currentBox1, bg1)) || (b && CollisionAABB(currentBox1, b->position))) currentlyStuck1 = true;
        }
        
        if (currentlyStuck1) {
            if (p1X < W / 4) p1X += 5; else p1X -= 5;
            if (p1Y < H / 2) p1Y += 5; else p1Y -= 5;
        }
    }

    bool move2 = false;
    int prevX2 = p2X;
    int prevY2 = p2Y;

    if (k[SDL_SCANCODE_Z]) { dir2 = 1; p2Y -= SPEED; move2 = true; }
    if (k[SDL_SCANCODE_S]) { dir2 = 0; p2Y += SPEED; move2 = true; }
    if (k[SDL_SCANCODE_Q]) { dir2 = 2; p2X -= SPEED; move2 = true; }
    if (k[SDL_SCANCODE_D]) { dir2 = 3; p2X += SPEED; move2 = true; }

    if (move2) {
        frame2 = (frame2 + 1) % 4;
        if (!timer2Started) { timer2Started = true; startTime2 = SDL_GetTicks(); }
    }

    SDL_Rect p2Hitbox = { (p2X - W/2) + 35, p2Y + h - 40, w - 70, 40 };
    SDL_Rect p2HitboxRelative = { p2Hitbox.x, p2Hitbox.y, p2Hitbox.w, p2Hitbox.h };
    bool collided2 = false;

    if (bg2 == 0) { 
        if (CollisionAABB(p2HitboxRelative, kitchenTopCounter)) collided2 = true;
        if (CollisionAABB(p2HitboxRelative, kitchenIsland)) collided2 = true;
        if (CollisionAABB(p2HitboxRelative, kitchenLeftDesk)) collided2 = true;
        if (b && CollisionAABB(p2HitboxRelative, b->position)) collided2 = true;
    } else {
        if (maskSurf && CollisionParfaite(maskSurf, p2HitboxRelative, bg2)) collided2 = true;
        if (b && CollisionAABB(p2HitboxRelative, b->position)) collided2 = true;
    }

    if (!ghostMode && collided2) {
        p2X = prevX2;
        p2Y = prevY2;
    }

    if (p2X < W/2) { bg2 = (bg2 + 1) % 4; p2X = W - w - 20; }
    else if (p2X + w > W) { bg2 = (bg2 + 1) % 4; p2X = W/2 + 160; } 
    
    if (p2Y < 0) { bg2 = (bg2 + 1) % 4; p2Y = H - h - 20; }
    else if (p2Y + h > H) { bg2 = (bg2 + 1) % 4; p2Y = 260; }

    if (!ghostMode) {
        SDL_Rect currentBox2Relative = { (p2X - W/2) + 35, p2Y + h - 40, w - 70, 40 };
        bool currentlyStuck2 = false;
        if (bg2 == 0) { 
            if (CollisionAABB(currentBox2Relative, kitchenTopCounter) || CollisionAABB(currentBox2Relative, kitchenIsland) || CollisionAABB(currentBox2Relative, kitchenLeftDesk) || (b && CollisionAABB(currentBox2Relative, b->position))) currentlyStuck2 = true;
        } else {
            if ((maskSurf && CollisionParfaite(maskSurf, currentBox2Relative, bg2)) || (b && CollisionAABB(currentBox2Relative, b->position))) currentlyStuck2 = true;
        }
        
        if (currentlyStuck2) {
            if (p2X < (3 * W) / 4) p2X += 5; else p2X -= 5;
            if (p2Y < H / 2) p2Y += 5; else p2Y -= 5;
        }
    }
}

/**
 * @brief Renders the visual control guide overlay.
 */
void renderGuide() {
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 200);
    SDL_Rect box = {W/2 - 150, H/2 - 100, 300, 200};
    SDL_RenderFillRect(ren, &box);

    SDL_Color c = {255,255,255};
    const char *lines[] = {"GUIDE", "LEFT : arrows", "RIGHT : Q S D Z", "G: Ghost | R: Respawn", "F : Menu"};

    for (int i = 0; i < 5; i++) {
        SDL_Surface *surf = TTF_RenderText_Solid(font, lines[i], c);
        if (!surf) continue;
        SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
        SDL_Rect dst = { box.x + 20, box.y + 20 + i*30, surf->w, surf->h };
        SDL_FreeSurface(surf);
        if (tex) {
            SDL_RenderCopy(ren, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
        }
    }
}

/**
 * @brief Manipulates viewports to render the split-screen view.
 */
void render() {
    int w = (int)(FRAME_W * SCALE);
    int h = (int)(FRAME_H * SCALE);

    SDL_Rect leftView = {0, 0, W/2, H};
    SDL_RenderSetViewport(ren, &leftView);
    if(bg[bg1]) SDL_RenderCopy(ren, bg[bg1], NULL, NULL);
    
    SDL_Rect src1 = {frame1 * FRAME_W, dir1 * FRAME_H, FRAME_W, FRAME_H};
    SDL_Rect dst1 = {p1X, p1Y, w, h};
    if(playerTex) SDL_RenderCopy(ren, playerTex, &src1, &dst1);

    renderTime(timer1Started, startTime1, 10, 10);

    SDL_Rect rightView = {W/2, 0, W/2, H};
    SDL_RenderSetViewport(ren, &rightView);
    if(bg[bg2]) SDL_RenderCopy(ren, bg[bg2], NULL, NULL);
    
    SDL_Rect src2 = {frame2 * FRAME_W, dir2 * FRAME_H, FRAME_W, FRAME_H};
    SDL_Rect dst2 = {p2X - W/2, p2Y, w, h};
    if(playerTex) SDL_RenderCopy(ren, playerTex, &src2, &dst2);
    
    renderTime(timer2Started, startTime2, 10, 10);

    SDL_RenderSetViewport(ren, NULL);
    if (showGuide) renderGuide();
}

/**
 * @brief Destroys all textures and fonts related to the gameplay loop upon exit.
 */
void clean() {
    if(playerTex) SDL_DestroyTexture(playerTex);
    for (int i = 0; i < 4; i++) if(bg[i]) SDL_DestroyTexture(bg[i]);
    if(font) TTF_CloseFont(font);
    if(ren) SDL_DestroyRenderer(ren);
    if(win) SDL_DestroyWindow(win);
    TTF_Quit();
    IMG_Quit();
    Mix_CloseAudio();
    SDL_Quit();
}

/**
 * @brief Safely loads an image texture and prints an error via console on failure.
 */
SDL_Texture* charger_image(SDL_Renderer *renderer, const char *chemin) {
    SDL_Texture *tex = IMG_LoadTexture(renderer, chemin);
    if (!tex) printf("Erreur image %s : %s\n", chemin, IMG_GetError());
    return tex;
}

/**
 * @brief Safely loads a .WAV audio chunk for sound effects.
 */
Mix_Chunk* charger_son(const char *chemin) {
    Mix_Chunk *son = Mix_LoadWAV(chemin);
    if (!son) printf("Erreur son %s : %s\n", chemin, Mix_GetError());
    return son;
}

/**
 * @brief Safely loads a .MP3 file for continuous background music.
 */
Mix_Music* charger_musique(const char *chemin) {
    Mix_Music *mus = Mix_LoadMUS(chemin);
    if (!mus) printf("Erreur musique %s : %s\n", chemin, Mix_GetError());
    return mus;
}

/**
 * @brief Safely loads a TrueType font.
 */
TTF_Font* charger_police(const char *chemin, int taille) {
    TTF_Font *police = TTF_OpenFont(chemin, taille);
    if (!police) printf("Erreur police %s : %s\n", chemin, TTF_GetError());
    return police;
}

/**
 * @brief Populates the Fonds struct with UI menu backgrounds.
 */
int charger_fonds(SDL_Renderer *renderer, Fonds *f) {
    f->bg_nom     = charger_image(renderer, "playername.png");
    f->bg_scores  = charger_image(renderer, "bestrunmen.png");
    f->bg_quitter = charger_image(renderer, "menuplaceholder.png");
    return (f->bg_nom && f->bg_scores && f->bg_quitter);
}

/**
 * @brief Populates Button structs with their corresponding image textures.
 */
int charger_boutons(SDL_Renderer *renderer, Bouton *verify, Bouton *retour, Bouton *quitter) {
    verify->tex  = charger_image(renderer, "varifysmal.png");
    retour->tex  = charger_image(renderer, "returnsmal.png");
    if (quitter) {
        quitter->tex = charger_image(renderer, "quitsmal.png");
        if (!quitter->tex) return 0;
    }
    return (verify->tex && retour->tex);
}

/**
 * @brief Loads all menu sounds into the Sons struct.
 */
int charger_sons(Sons *s) {
    s->survol   = charger_son("woosh.mp3");
    s->clic     = charger_son("CSE.mp3");
    s->victoire = charger_musique("vic.mp3");
    return (s->survol && s->clic && s->victoire);
}

/**
 * @brief Initializes the primary font into the Texte struct.
 */
int charger_texte(Texte *t) {
    t->police = charger_police("AUGUSTUS.ttf", 28);
    return t->police != NULL;
}

/**
 * @brief Cleans up memory used by the menu backgrounds.
 */
void liberer_fonds(Fonds *f) {
    if(f->bg_nom) SDL_DestroyTexture(f->bg_nom);
    if(f->bg_scores) SDL_DestroyTexture(f->bg_scores);
    if(f->bg_quitter) SDL_DestroyTexture(f->bg_quitter);
}

/**
 * @brief Cleans up memory used by button textures.
 */
void liberer_boutons(Bouton *verify, Bouton *retour, Bouton *quitter) {
    if(verify->tex) SDL_DestroyTexture(verify->tex);
    if(retour->tex) SDL_DestroyTexture(retour->tex);
    if (quitter && quitter->tex) SDL_DestroyTexture(quitter->tex);
}

/**
 * @brief Cleans up memory used by audio assets.
 */
void liberer_sons(Sons *s) {
    if(s->survol) Mix_FreeChunk(s->survol);
    if(s->clic) Mix_FreeChunk(s->clic);
    if(s->victoire) Mix_FreeMusic(s->victoire);
}

/**
 * @brief Closes the TTF font safely.
 */
void liberer_texte(Texte *t) {
    if(t->police) TTF_CloseFont(t->police);
}

/**
 * @brief Detects if the mouse coordinates intersect with a button's defined rectangle.
 */
int souris_sur_bouton(int mx, int my, Bouton *b) {
    if (!b) return 0;
    return (mx >= b->rect.x && mx <= b->rect.x + b->rect.w &&
            my >= b->rect.y && my <= b->rect.y + b->rect.h);
}

/**
 * @brief Draws a button, adjusting transparency based on its hover state.
 */
void afficher_bouton(SDL_Renderer *renderer, Bouton *b) {
    if (!b || !b->tex) return;
    if (b->survole) SDL_SetTextureAlphaMod(b->tex, 255);
    else SDL_SetTextureAlphaMod(b->tex, 0);
    SDL_RenderCopy(renderer, b->tex, NULL, &b->rect);
}

/**
 * @brief Evaluates hover state and plays sound effect upon initial intersection.
 */
void verifier_survol(int mx, int my, Bouton *b, Mix_Chunk *son_survol) {
    if (!b) return;
    int dessus = souris_sur_bouton(mx, my, b);
    if (dessus && !b->survole && son_survol) Mix_PlayChannel(-1, son_survol, 0);
    b->survole = dessus;
}

/**
 * @brief Helper to generate a text surface, convert it to a texture, and render it.
 */
void afficher_texte(SDL_Renderer *renderer, TTF_Font *police,const char *texte, SDL_Color couleur, int x, int y) {
    SDL_Surface *surface = TTF_RenderText_Solid(police, texte, couleur);
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

/**
 * @brief Appends a provided player string to the local high-score text file.
 */
void sauvegarder_nom(const char *nom) {
    FILE *f = fopen("players.txt", "a");
    if (!f) return;
    fprintf(f, "%s\n", nom);
    fclose(f);
}

/**
 * @brief Reads the top 3 lines of the local text file and renders them as the high scores.
 */
void afficher_meilleurs_scores(SDL_Renderer *renderer, TTF_Font *police, int start_x, int start_y) {
    FILE *f = fopen("players.txt", "r");
    if (!f) return;
    SDL_Color blanc = {255, 255, 255, 255};
    char ligne[64];
    int i = 0, y = start_y;
    while (i < 3 && fgets(ligne, sizeof(ligne), f)) {
        ligne[strcspn(ligne, "\n")] = '\0';
        afficher_texte(renderer, police, ligne, blanc, start_x, y);
        y += 70;
        i++;
    }
    fclose(f);
}
