/**
 * @file header.h
 * @brief Main header file for the Home Alone split-screen game.
 */

#ifndef HEADER_H
#define HEADER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <stdbool.h>

#define W 1800                  
#define H 1000                  
#define LARGEUR  800            
#define HAUTEUR  600            
#define SCREEN_WIDTH  1024      
#define SCREEN_HEIGHT 1024      
#define SCALE_FACTOR  19        
#define FRAME_W 300             
#define FRAME_H 449             
#define SPEED 10                
#define SCALE 0.5               

/**
 * @brief Represents the current state of the game application.
 */
typedef enum {
    STATE_PLAYING,  
    STATE_NOM,      
    STATE_SCORES,   
    STATE_QUITTER   
} GameState;

extern GameState currentState;  
extern SDL_Window *win;         
extern SDL_Renderer *ren;       

extern int p1X;                 
extern int p1Y;                 

/**
 * @brief Structure representing a generic player entity.
 */
typedef struct {
    SDL_Rect position;    
    SDL_Texture* sprite;  
} Player;

/**
 * @brief Structure for managing and rendering the minimap.
 */
typedef struct {
    SDL_Texture* backgroundTexture;  
    SDL_Rect minimapPosition;        
    SDL_Texture* playerDot;          
    SDL_Rect playerDotPosition;      
} Minimap;

/**
 * @brief Structure representing a moving obstacle or barrier.
 */
typedef struct {
    SDL_Rect position;    
    SDL_Texture* sprite;  
    int speed;            
    int direction;        
} Barrier;

/**
 * @brief Structure grouping the main menu background textures.
 */
typedef struct {
    SDL_Texture *bg_nom;      
    SDL_Texture *bg_scores;   
    SDL_Texture *bg_quitter;  
} Fonds;

/**
 * @brief Structure defining an interactive UI button.
 */
typedef struct {
    SDL_Texture *tex;  
    SDL_Rect     rect; 
    int          survole; 
} Bouton;

/**
 * @brief Structure managing all game audio assets.
 */
typedef struct {
    Mix_Chunk *survol;    
    Mix_Chunk *clic;      
    Mix_Music *victoire;  
} Sons;

/**
 * @brief Structure managing typography assets.
 */
typedef struct {
    TTF_Font *police; 
} Texte;

void InitMinimap(Minimap* m, SDL_Renderer* renderer, char* mapPath, char* dotPath, SDL_Rect minimapPos);
void MAJMinimap(Minimap* m, SDL_Rect playerPos, SDL_Rect camera);
void AfficherMinimap(SDL_Renderer* renderer, Minimap m);
void LibererMinimap(Minimap* m);
int CollisionAABB(SDL_Rect rect1, SDL_Rect rect2);
SDL_Color GetPixel(SDL_Surface* surface, int x, int y);
int CollisionParfaite(SDL_Surface* mask, SDL_Rect player, int roomIndex);

void InitBarrier(Barrier* b, SDL_Rect pos, SDL_Texture* tex, int spd, int dir);
void MoveBarrier(Barrier* b);
void AfficherBarrier(SDL_Renderer* renderer, Barrier b);

SDL_Texture* loadTexture(char *path, SDL_Renderer *renderer);
void fenetre1(SDL_Renderer *renderer);
void fenetre2(SDL_Renderer *renderer);

void init();
void load();
void handleInput(SDL_Event *e, bool *running);
void updateGame(SDL_Surface* maskSurf, Barrier* b); 
void render();
void renderGuide();
void renderTime(bool started, Uint32 start, int x, int y);
void clean();

SDL_Texture* charger_image(SDL_Renderer *renderer, const char *chemin);
Mix_Chunk* charger_son(const char *chemin);
Mix_Music* charger_musique(const char *chemin);
TTF_Font* charger_police(const char *chemin, int taille);
int charger_fonds(SDL_Renderer *renderer, Fonds *f);
int charger_boutons(SDL_Renderer *renderer, Bouton *verify, Bouton *retour, Bouton *quitter);
int charger_sons(Sons *s);
int charger_texte(Texte *t);
void liberer_fonds(Fonds *f);
void liberer_boutons(Bouton *verify, Bouton *retour, Bouton *quitter);
void liberer_sons(Sons *s);
void liberer_texte(Texte *t);
int souris_sur_bouton(int mx, int my, Bouton *b);
void afficher_bouton(SDL_Renderer *renderer, Bouton *b);
void verifier_survol(int mx, int my, Bouton *b, Mix_Chunk *son_survol);
void afficher_texte(SDL_Renderer *renderer, TTF_Font *police,const char *texte, SDL_Color couleur, int x, int y);
void sauvegarder_nom(const char *nom);
void afficher_meilleurs_scores(SDL_Renderer *renderer, TTF_Font *police, int start_x, int start_y);

#endif
