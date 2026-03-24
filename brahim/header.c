#include "header.h"

// Détection de collision (Hitbox) et effet sonore unique au survol
void maj_survol_bouton(ZoneCliquable* zone, int mx, int my, Mix_Chunk* son_survol) {
    bool en_collision = (mx >= zone->rect.x && mx <= zone->rect.x + zone->rect.w &&
                         my >= zone->rect.y && my <= zone->rect.y + zone->rect.h);
    
    // Joue le son uniquement si la souris VIENT d'entrer dans la zone
    if (en_collision && !zone->is_hovered) {
        if (son_survol != NULL) {
            Mix_PlayChannel(-1, son_survol, 0);
        }
    }
    zone->is_hovered = en_collision;
}

// Génération et affichage dynamique de texte avec la fonction SOLID du cours
void afficher_texte_centre(SDL_Renderer* renderer, TTF_Font* police, const char* texte, int x, int y, SDL_Color couleur) {
    if (police == NULL || texte == NULL) return;
    
    // ICI : Remplacement par TTF_RenderText_Solid exactement comme à la Page 4 de ton TP
    SDL_Surface* surface = TTF_RenderText_Solid(police, texte, couleur);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = {x - (surface->w / 2), y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}
