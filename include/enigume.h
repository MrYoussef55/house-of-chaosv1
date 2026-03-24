#ifndef HEADER_H
#define HEADER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h> // Ajout pour gérer le texte
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 650

typedef enum {
    ETAT_SOUS_MENU,
    ETAT_QUIZ,
    ETAT_RESULTAT // Nouvel état pour afficher la réponse
} EtatJeu;

typedef struct {
    SDL_Rect rect;
    bool is_hovered;
} ZoneCliquable;

// Prototypes
void maj_survol_bouton(ZoneCliquable* zone, int mx, int my, Mix_Chunk* son_survol);
void afficher_texte_centre(SDL_Renderer* renderer, TTF_Font* police, const char* texte, int x, int y, SDL_Color couleur);

#endif
