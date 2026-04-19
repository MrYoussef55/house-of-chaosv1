#ifndef MENU_H
#define MENU_H

#include "app.h"

void MainMenu_Init(App* app);
void MainMenu_HandleEvent(App* app, SDL_Event* e, SDL_Renderer* renderer);
void MainMenu_Update(App* app);
void MainMenu_Render(App* app);
void MainMenu_Destroy();
void fade(SDL_Renderer* renderer, int fadeIn);
#endif
