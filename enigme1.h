#ifndef HEADER_H
#define HEADER_H

/**
 * @file enigme1.h
 * @brief Fichier d'en-tête contenant les bibliothèques, constantes et structures du jeu House of Chaos.
 * @author Brahim Riahi
 * @version 1.0
 * @date 2 Mai 2026
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define WINDOW_WIDTH   1920
#define WINDOW_HEIGHT  1080
#define MAX_ENIGMES    10
#define MAX_TEXTE      256

#define ETAT_SOUS_MENU 0
#define ETAT_QUIZ      1
#define ETAT_RESULTAT  2
#define ETAT_GAME_OVER 3
#define ETAT_FIN       4
#define ETAT_PUZZLE    5

#define ROWS           2
#define COLS           4
#define TOTAL_CARDS    8
#define PREVIEW_FRAMES 125

/**
 * @struct Joueur
 * @brief Structure représentant les statistiques du joueur.
 */
typedef struct {
    int score;  /*!< Le score actuel du joueur */
    int vies;   /*!< Le nombre de vies restantes */
    int level;  /*!< Le niveau de difficulté actuel */
    int bonnes; /*!< Le nombre de bonnes réponses consécutives */
} Joueur;

/**
 * @struct Enigme
 * @brief Structure stockant les données d'une question de quiz.
 */
typedef struct {
    char question[MAX_TEXTE]; /*!< Texte de la question */
    char reponse1[MAX_TEXTE]; /*!< Texte de la proposition 1 */
    char reponse2[MAX_TEXTE]; /*!< Texte de la proposition 2 */
    char reponse3[MAX_TEXTE]; /*!< Texte de la proposition 3 */
    int  numQuestSelect;      /*!< Numéro de la question sélectionnée */
    int  numbr;               /*!< Indice de la bonne réponse (1, 2 ou 3) */
    SDL_Texture* texQuestion; /*!< Texture contenant la question */
    SDL_Texture* texRep1;     /*!< Texture contenant la réponse 1 */
    SDL_Texture* texRep2;     /*!< Texture contenant la réponse 2 */
    SDL_Texture* texRep3;     /*!< Texture contenant la réponse 3 */
    SDL_Rect posQuestion;     /*!< Position de la question à l'écran */
    SDL_Rect posRep1;         /*!< Position de la réponse 1 à l'écran */
    SDL_Rect posRep2;         /*!< Position de la réponse 2 à l'écran */
    SDL_Rect posRep3;         /*!< Position de la réponse 3 à l'écran */
    int etat;                 /*!< État de l'énigme (1=gagnée, -1=perdue, 0=en cours) */
    SDL_Color couleur;        /*!< Couleur du texte */
    TTF_Font* font;           /*!< Police utilisée pour le texte */
    int deja_vu;              /*!< Indicateur si la question a déjà été posée (1=oui, 0=non) */
} Enigme;

/**
 * @struct Bouton
 * @brief Structure pour gérer les zones cliquables (boutons).
 */
typedef struct {
    SDL_Rect rect;  /*!< Zone rectangulaire du bouton */
    int is_hovered; /*!< Indicateur si la souris survole le bouton (1=oui, 0=non) */
} Bouton;

/**
 * @struct Timer
 * @brief Structure pour gérer le temps imparti lors du quiz.
 */
typedef struct {
    int debut; /*!< Frame de début du timer */
    int duree; /*!< Durée totale autorisée en frames */
} Timer;

/**
 * @struct Card
 * @brief Structure représentant une carte pour le jeu du Puzzle (Memory).
 */
typedef struct {
    SDL_Rect rect;      /*!< Position et taille de la carte */
    int textureIndex;   /*!< Indice de la texture associée à cette carte */
    int isFlipped;      /*!< État de la carte (1=retournée, 0=cachée) */
    int isMatched;      /*!< Indicateur si la paire a été trouvée (1=oui, 0=non) */
} Card;

void afficher_texte(SDL_Renderer* r, TTF_Font* f, const char* txt, int cx, int y, SDL_Color c);
void maj_bouton(Bouton* b, int mx, int my, Mix_Chunk* son);
int  charger_enigmes(const char* fichier, Enigme tab[], SDL_Renderer* r, TTF_Font* font, SDL_Color c);
int  choisir_enigme(Enigme tab[], int nb);
void liberer_enigmes(Enigme tab[], int nb);
void afficher_barre_timer(SDL_Renderer* r, Timer* t, int frame);

void initCards(Card deck[], int screenW, int screenH);
void shuffleDeck(Card deck[]);
int  getClickedCard(int mouseX, int mouseY, Card deck[]);
void renderPuzzle(SDL_Renderer* renderer, Card deck[], SDL_Texture* textures[], SDL_Texture* background);
void handleEventsPuzzle(SDL_Event* e, Card deck[], int* flippedCount, int* firstIdx, int* secondIdx);

#endif
