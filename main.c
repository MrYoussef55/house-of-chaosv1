#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include <stdlib.h>
#include "enemie.h"

int main(int argc, char *argv[]) {
    srand(time(NULL));

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("House Of Chaos", 100, 100, 1280, 720, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);  
    
    SDL_Surface *bgSurf = IMG_Load("assets/back.png"); 
    SDL_Texture *backgroundVisual = SDL_CreateTextureFromSurface(renderer, bgSurf);
    SDL_FreeSurface(bgSurf);
    
    // Create and initialize two NPCs
    Ennemi e1;
    Ennemi e2;
    initEnnemi(&e1, renderer); 
    initEnnemi(&e2, renderer); 

    e2.destRect.x = 800; 
    e2.destRect.y = 400;

    // Define the hazard zone
    SDL_Rect hazardZone = { 540, 260, 200, 200 }; 

    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }       
        
        // Move them and pass the hazard
        deplacerAleatoire(&e1, hazardZone);
        deplacerAleatoire(&e2, hazardZone);

        animerEnnemi(&e1);
        animerEnnemi(&e2);

        SDL_RenderClear(renderer);
        
        // Draw the background
        SDL_RenderCopy(renderer, backgroundVisual, NULL, NULL); 
        
        // Draw the hazard zone
        SDL_SetRenderDrawColor(renderer, 150, 0, 0, 150); 
        SDL_RenderFillRect(renderer, &hazardZone);
        
        SDL_Rect cam = {0, 0, 1280, 720};
        
        // Draw both enemies
        afficherEnnemi(e1, renderer, cam);
        afficherEnnemi(e2, renderer, cam);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); 
    }
    
    SDL_DestroyTexture(backgroundVisual);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
