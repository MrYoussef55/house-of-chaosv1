#include "header.h"

int main(int argc, char* argv[]) {
    // --- INITIALISATION ---
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) return -1;
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    TTF_Init(); 
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur audio: %s\n", Mix_GetError());
    }

    SDL_Window* window = SDL_CreateWindow("House of Chaos - Enigmes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // --- CHARGEMENT DES RESSOURCES ---
    SDL_Texture* texMenu = IMG_LoadTexture(renderer, "choice menu.png");
    SDL_Texture* texMenuHover = IMG_LoadTexture(renderer, "image effet.png"); 
    
    SDL_Texture* texQuiz = IMG_LoadTexture(renderer, "quizz reponse.png");
    SDL_Texture* texResultat = IMG_LoadTexture(renderer, "harry feu.png"); 

    TTF_Font* police = TTF_OpenFont("arial.ttf", 60); 

    Mix_Chunk* sonBref = Mix_LoadWAV("son_bref.wav");
    Mix_Chunk* sonCymbale = Mix_LoadWAV("cymbale.wav"); 
    
    Mix_Music* zikSuspense = Mix_LoadMUS("tambour.mp3");
    Mix_Music* zikTheme = Mix_LoadMUS("theme.mp3");

    // Lancement de la musique du menu
    if (zikTheme) Mix_PlayMusic(zikTheme, -1); 

    // --- HITBOXES (Zones cliquables) ---
    ZoneCliquable zoneQuiz   = {{350, 260, 300, 60}, false};
    ZoneCliquable zonePuzzle = {{350, 350, 300, 60}, false};
    ZoneCliquable zoneA = {{310, 410, 120, 100}, false};
    ZoneCliquable zoneB = {{440, 410, 120, 100}, false};
    ZoneCliquable zoneC = {{570, 410, 120, 100}, false};

    EtatJeu etatActuel = ETAT_SOUS_MENU;
    bool est_correct = false; 
    bool running = true;
    SDL_Event event;

    // --- BOUCLE PRINCIPALE ---
    while (running) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            
            // 1. GESTION DU CLAVIER
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;

                if (etatActuel == ETAT_QUIZ) {
                    if (event.key.keysym.sym == SDLK_a) {
                        etatActuel = ETAT_RESULTAT;
                        est_correct = true; 
                        Mix_HaltMusic(); 
                        if (sonCymbale) Mix_PlayChannel(-1, sonCymbale, 0); 
                    }
                    else if (event.key.keysym.sym == SDLK_b || event.key.keysym.sym == SDLK_c) {
                        etatActuel = ETAT_RESULTAT;
                        est_correct = false; 
                        Mix_HaltMusic(); 
                        if (sonCymbale) Mix_PlayChannel(-1, sonCymbale, 0); 
                    }
                }
                else if (etatActuel == ETAT_RESULTAT) {
                    if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN) {
                        etatActuel = ETAT_SOUS_MENU;
                        if (zikTheme) Mix_PlayMusic(zikTheme, -1); 
                    }
                }
            }

            // 2. GESTION DE LA SOURIS
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                if (etatActuel == ETAT_SOUS_MENU) {
                    if (zoneQuiz.is_hovered) {
                        etatActuel = ETAT_QUIZ;
                        if (zikSuspense) Mix_PlayMusic(zikSuspense, -1); 
                    }
                    else if (zonePuzzle.is_hovered) {
                        // Action pour le bouton puzzle à ajouter plus tard si besoin
                    }
                } 
                else if (etatActuel == ETAT_QUIZ) {
                    if (zoneA.is_hovered) {
                        etatActuel = ETAT_RESULTAT;
                        est_correct = true; 
                        Mix_HaltMusic(); 
                        if (sonCymbale) Mix_PlayChannel(-1, sonCymbale, 0); 
                    }
                    else if (zoneB.is_hovered || zoneC.is_hovered) {
                        etatActuel = ETAT_RESULTAT;
                        est_correct = false; 
                        Mix_HaltMusic(); 
                        if (sonCymbale) Mix_PlayChannel(-1, sonCymbale, 0); 
                    }
                }
                else if (etatActuel == ETAT_RESULTAT) {
                    etatActuel = ETAT_SOUS_MENU;
                    if (zikTheme) Mix_PlayMusic(zikTheme, -1); 
                }
            }
        }

        // --- UPDATE (Survols et Sons brefs) ---
        if (etatActuel == ETAT_SOUS_MENU) {
            maj_survol_bouton(&zoneQuiz, mouseX, mouseY, sonBref);
            maj_survol_bouton(&zonePuzzle, mouseX, mouseY, sonBref);
        } else if (etatActuel == ETAT_QUIZ) {
            maj_survol_bouton(&zoneA, mouseX, mouseY, sonBref);
            maj_survol_bouton(&zoneB, mouseX, mouseY, sonBref);
            maj_survol_bouton(&zoneC, mouseX, mouseY, sonBref);
        }

        // --- RENDU GRAPHIQUE ---
        SDL_RenderClear(renderer);

        if (etatActuel == ETAT_SOUS_MENU) {
            // Affichage de l'image lumineuse si la souris survole un bouton
            if ((zoneQuiz.is_hovered || zonePuzzle.is_hovered) && texMenuHover) {
                SDL_RenderCopy(renderer, texMenuHover, NULL, NULL);
            } 
            // Sinon on affiche le menu classique
            else if (texMenu) {
                SDL_RenderCopy(renderer, texMenu, NULL, NULL);
            }
        } 
        else if (etatActuel == ETAT_QUIZ && texQuiz) {
            // Affichage de l'image du quiz (sans aucune bordure rajoutée)
            SDL_RenderCopy(renderer, texQuiz, NULL, NULL);
        }
        else if (etatActuel == ETAT_RESULTAT && texResultat) {
            // Affichage de la photo de Harry avec le feu
            SDL_RenderCopy(renderer, texResultat, NULL, NULL);
            
            // Affichage du texte par-dessus
            if (est_correct) {
                SDL_Color vert = {50, 255, 50, 255}; 
                afficher_texte_centre(renderer, police, "Correct !", WINDOW_WIDTH / 2, 50, vert);
            } else {
                SDL_Color rouge = {255, 50, 50, 255}; 
                afficher_texte_centre(renderer, police, "Incorrect it was harry", WINDOW_WIDTH / 2, 50, rouge);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Environ 60 FPS
    }

    // --- NETTOYAGE (Libération de la mémoire) ---
    if (texMenu) SDL_DestroyTexture(texMenu);
    if (texMenuHover) SDL_DestroyTexture(texMenuHover); 
    if (texQuiz) SDL_DestroyTexture(texQuiz);
    if (texResultat) SDL_DestroyTexture(texResultat);
    if (police) TTF_CloseFont(police);
    if (sonBref) Mix_FreeChunk(sonBref);
    if (sonCymbale) Mix_FreeChunk(sonCymbale);
    if (zikSuspense) Mix_FreeMusic(zikSuspense);
    if (zikTheme) Mix_FreeMusic(zikTheme); 

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit(); 
    IMG_Quit();
    SDL_Quit();

    return 0;
}
