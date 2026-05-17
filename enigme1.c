/**
 * @file enigme1.c
 * @brief Fichier source contenant la logique métier du quiz et du memory (fonctions d'affichage, chargement, logique du jeu).
 * @author Brahim Riahi
 * @version 1.0
 * @date 2 Mai 2026
 */

#include "enigme1.h"

/**
 * @brief Affiche du texte centré horizontalement à l'écran.
 * @param r Pointeur vers le renderer SDL.
 * @param f Pointeur vers la police TTF.
 * @param txt Chaîne de caractères à afficher.
 * @param cx Coordonnée X du centre du texte.
 * @param y Coordonnée Y du texte.
 * @param c Couleur SDL_Color du texte.
 */
void afficher_texte(SDL_Renderer* r, TTF_Font* f, const char* txt, int cx, int y, SDL_Color c) {
    SDL_Surface* s = TTF_RenderText_Solid(f, txt, c);
    if (!s) return;
    SDL_Texture* t = SDL_CreateTextureFromSurface(r, s);
    SDL_Rect pos = {cx - s->w/2, y, s->w, s->h};
    SDL_RenderCopy(r, t, NULL, &pos);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);
}

/**
 * @brief Met à jour l'état d'un bouton au survol de la souris.
 * @param b Pointeur vers le Bouton à vérifier.
 * @param mx Position X de la souris.
 * @param my Position Y de la souris.
 * @param son Pointeur vers l'effet sonore à jouer lors du survol.
 */
void maj_bouton(Bouton* b, int mx, int my, Mix_Chunk* son) {
    int col = (mx >= b->rect.x && mx <= b->rect.x + b->rect.w &&
               my >= b->rect.y && my <= b->rect.y + b->rect.h);
    if (col && !b->is_hovered && son)
        Mix_PlayChannel(-1, son, 0);
    b->is_hovered = col;
}

/**
 * @brief Charge les questions, réponses et textures depuis un fichier texte.
 * @param fichier Chemin vers le fichier texte contenant les questions.
 * @param tab Tableau de structures Enigme à remplir.
 * @param r Pointeur vers le renderer SDL.
 * @param font Police TTF utilisée pour générer les textures de texte.
 * @param c Couleur du texte des questions.
 * @return Le nombre total d'énigmes correctement chargées.
 */
int charger_enigmes(const char* fichier, Enigme tab[], SDL_Renderer* r, TTF_Font* font, SDL_Color c) {
    FILE* fp = fopen(fichier, "r");
    if (!fp) { printf("Error: cannot open %s\n", fichier); return 0; }
    int nb = 0;
    char ligne[MAX_TEXTE];
    SDL_Color blanc = {255, 255, 255, 255};
    while (nb < MAX_ENIGMES) {
        if (!fgets(tab[nb].question, MAX_TEXTE, fp)) break;
        tab[nb].question[strcspn(tab[nb].question, "\n")] = '\0';
        if (!fgets(tab[nb].reponse1, MAX_TEXTE, fp)) break;
        tab[nb].reponse1[strcspn(tab[nb].reponse1, "\n")] = '\0';
        if (!fgets(tab[nb].reponse2, MAX_TEXTE, fp)) break;
        tab[nb].reponse2[strcspn(tab[nb].reponse2, "\n")] = '\0';
        if (!fgets(tab[nb].reponse3, MAX_TEXTE, fp)) break;
        tab[nb].reponse3[strcspn(tab[nb].reponse3, "\n")] = '\0';
        if (!fgets(ligne, MAX_TEXTE, fp)) break;
        tab[nb].numbr = atoi(ligne) + 1;
        fgets(ligne, MAX_TEXTE, fp);
        tab[nb].font           = font;
        tab[nb].couleur        = c;
        tab[nb].etat           = 0;
        tab[nb].deja_vu        = 0;
        tab[nb].numQuestSelect = nb + 1;
        SDL_Surface* sq = TTF_RenderText_Solid(font, tab[nb].question, c);
        if (sq) {
            tab[nb].texQuestion = SDL_CreateTextureFromSurface(r, sq);
            tab[nb].posQuestion.x = WINDOW_WIDTH/2 - sq->w/2;
            tab[nb].posQuestion.y = 380;
            tab[nb].posQuestion.w = sq->w;
            tab[nb].posQuestion.h = sq->h;
            SDL_FreeSurface(sq);
        }
        SDL_Surface* s1 = TTF_RenderText_Solid(font, tab[nb].reponse1, blanc);
        if (s1) {
            tab[nb].texRep1 = SDL_CreateTextureFromSurface(r, s1);
            tab[nb].posRep1.x = 565 - s1->w/2;
            tab[nb].posRep1.y = 682;
            tab[nb].posRep1.w = s1->w;
            tab[nb].posRep1.h = s1->h;
            SDL_FreeSurface(s1);
        }
        SDL_Surface* s2 = TTF_RenderText_Solid(font, tab[nb].reponse2, blanc);
        if (s2) {
            tab[nb].texRep2 = SDL_CreateTextureFromSurface(r, s2);
            tab[nb].posRep2.x = 935 - s2->w/2;
            tab[nb].posRep2.y = 682;
            tab[nb].posRep2.w = s2->w;
            tab[nb].posRep2.h = s2->h;
            SDL_FreeSurface(s2);
        }
        SDL_Surface* s3 = TTF_RenderText_Solid(font, tab[nb].reponse3, blanc);
        if (s3) {
            tab[nb].texRep3 = SDL_CreateTextureFromSurface(r, s3);
            tab[nb].posRep3.x = 1305 - s3->w/2;
            tab[nb].posRep3.y = 682;
            tab[nb].posRep3.w = s3->w;
            tab[nb].posRep3.h = s3->h;
            SDL_FreeSurface(s3);
        }
        nb++;
    }
    fclose(fp);
    return nb;
}

/**
 * @brief Sélectionne aléatoirement une question qui n'a pas encore été posée.
 * @param tab Tableau des énigmes disponibles.
 * @param nb Nombre total d'énigmes dans le tableau.
 * @return L'indice de l'énigme choisie dans le tableau.
 */
int choisir_enigme(Enigme tab[], int nb) {
    int i, tous = 1;
    for (i = 0; i < nb; i++) if (!tab[i].deja_vu) { tous = 0; break; }
    if (tous) for (i = 0; i < nb; i++) tab[i].deja_vu = 0;
    int idx;
    do { idx = rand() % nb; } while (tab[idx].deja_vu);
    tab[idx].deja_vu = 1;
    tab[idx].etat    = 0;
    return idx;
}

/**
 * @brief Libère la mémoire allouée pour les textures des énigmes.
 * @param tab Tableau des énigmes.
 * @param nb Nombre d'énigmes dans le tableau.
 */
void liberer_enigmes(Enigme tab[], int nb) {
    int i;
    for (i = 0; i < nb; i++) {
        if (tab[i].texQuestion) SDL_DestroyTexture(tab[i].texQuestion);
        if (tab[i].texRep1)     SDL_DestroyTexture(tab[i].texRep1);
        if (tab[i].texRep2)     SDL_DestroyTexture(tab[i].texRep2);
        if (tab[i].texRep3)     SDL_DestroyTexture(tab[i].texRep3);
    }
}

/**
 * @brief Affiche une barre de progression graphique pour le chronomètre.
 * @param r Pointeur vers le renderer SDL.
 * @param t Pointeur vers la structure Timer.
 * @param frame Numéro de la frame actuelle d'exécution.
 */
void afficher_barre_timer(SDL_Renderer* r, Timer* t, int frame) {
    int winW = 0, winH = 0;
    SDL_GetRendererOutputSize(r, &winW, &winH);
    float ratio = 1.0f - (float)(frame - t->debut) / (float)t->duree;
    if (ratio < 0.0f) ratio = 0.0f;
    int lmax = winW - 40;
    SDL_SetRenderDrawColor(r, 40, 40, 40, 255);
    SDL_Rect fond = {20, 46, lmax, 28};
    SDL_RenderFillRect(r, &fond);
    if      (ratio > 0.5f)  SDL_SetRenderDrawColor(r, 40, 200, 50, 255);
    else if (ratio > 0.25f) SDL_SetRenderDrawColor(r, 250, 190, 0, 255);
    else                    SDL_SetRenderDrawColor(r, 220, 40, 40, 255);
    SDL_Rect barre = {20, 46, (int)(lmax * ratio), 28};
    SDL_RenderFillRect(r, &barre);
    SDL_SetRenderDrawColor(r, 180, 140, 30, 255);
    SDL_RenderDrawRect(r, &fond);
}

/**
 * @brief Initialise les positions et les propriétés des cartes du Puzzle.
 * @param deck Tableau contenant les cartes à initialiser.
 * @param screenW Largeur de la fenêtre de l'application.
 * @param screenH Hauteur de la fenêtre de l'application.
 */
void initCards(Card deck[], int screenW, int screenH) {
    int cardW = 200;
    int cardH = 200;
    int padding = 20;
    int totalWidth  = COLS * cardW + (COLS - 1) * padding;
    int totalHeight = ROWS * cardH + (ROWS - 1) * padding;
    int startX = (screenW - totalWidth)  / 2;
    int startY = (screenH - totalHeight) / 2;
    int count = 0;
    int i, j;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            deck[count].rect.x     = startX + j * (cardW + padding);
            deck[count].rect.y     = startY + i * (cardH + padding);
            deck[count].rect.w     = cardW;
            deck[count].rect.h     = cardH;
            deck[count].isFlipped  = 0;
            deck[count].isMatched  = 0;
            deck[count].textureIndex = count / 2;
            count++;
        }
    }
}

/**
 * @brief Mélange aléatoirement les textures attribuées aux cartes.
 * @param deck Tableau des cartes à mélanger.
 */
void shuffleDeck(Card deck[]) {
    int i, j, temp;
    for (i = TOTAL_CARDS - 1; i > 0; i--) {
        j = rand() % (i + 1);
        temp = deck[i].textureIndex;
        deck[i].textureIndex = deck[j].textureIndex;
        deck[j].textureIndex = temp;
    }
}

/**
 * @brief Vérifie quelle carte a été cliquée en fonction des coordonnées de la souris.
 * @param mouseX Position X du clic.
 * @param mouseY Position Y du clic.
 * @param deck Tableau des cartes.
 * @return L'indice de la carte cliquée, ou -1 si aucune carte n'a été touchée.
 */
int getClickedCard(int mouseX, int mouseY, Card deck[]) {
    int i;
    for (i = 0; i < TOTAL_CARDS; i++) {
        if (mouseX >= deck[i].rect.x &&
            mouseX <= deck[i].rect.x + deck[i].rect.w &&
            mouseY >= deck[i].rect.y &&
            mouseY <= deck[i].rect.y + deck[i].rect.h) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Gère l'affichage graphique du jeu de Puzzle (Memory).
 * @param renderer Pointeur vers le renderer SDL.
 * @param deck Tableau des cartes à dessiner.
 * @param textures Tableau contenant les différentes images des paires.
 * @param background Texture d'arrière-plan du jeu.
 */
void renderPuzzle(SDL_Renderer* renderer, Card deck[], SDL_Texture* textures[], SDL_Texture* background) {
    int i;
    SDL_RenderCopy(renderer, background, NULL, NULL);
    for (i = 0; i < TOTAL_CARDS; i++) {
        if (deck[i].isFlipped || deck[i].isMatched) {
            SDL_RenderCopy(renderer, textures[deck[i].textureIndex], NULL, &deck[i].rect);
        } else {
            SDL_SetRenderDrawColor(renderer, 30, 60, 120, 255);
            SDL_RenderFillRect(renderer, &deck[i].rect);
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
            SDL_RenderDrawRect(renderer, &deck[i].rect);
        }
    }
}

/**
 * @brief Traite les événements de clics sur les cartes du Memory.
 * @param e Pointeur vers l'événement SDL courant.
 * @param deck Tableau des cartes.
 * @param flippedCount Pointeur vers le compteur de cartes actuellement retournées.
 * @param firstIdx Pointeur vers l'indice de la première carte cliquée.
 * @param secondIdx Pointeur vers l'indice de la deuxième carte cliquée.
 */
void handleEventsPuzzle(SDL_Event* e, Card deck[], int* flippedCount, int* firstIdx, int* secondIdx) {
    int clickedIdx;
    if (e->type == SDL_MOUSEBUTTONDOWN && *flippedCount < 2) {
        clickedIdx = getClickedCard(e->button.x, e->button.y, deck);
        if (clickedIdx != -1 && !deck[clickedIdx].isFlipped && !deck[clickedIdx].isMatched) {
            deck[clickedIdx].isFlipped = 1;
            if (*flippedCount == 0) {
                *firstIdx     = clickedIdx;
                *flippedCount = 1;
            } else {
                *secondIdx    = clickedIdx;
                *flippedCount = 2;
            }
        }
    }
}
