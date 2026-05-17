#ifndef MENUS_H
#define MENUS_H

#include "app.h"
#include <SDL2/SDL.h>

void Menus_Init(App* app);
void Menus_HandleEvent(App* app, SDL_Event* e);
void Menus_Update(App* app);
void Menus_Render(App* app);
void Menus_Destroy(void);

#endif
