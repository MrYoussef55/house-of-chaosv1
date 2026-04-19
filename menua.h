#ifndef MENUA_H
#define MENUA_H

#include "app.h"

void MenuA_Init(App* app);
void MenuA_HandleEvent(App* app, SDL_Event* e);
void MenuA_Update(App* app);
void MenuA_Render(App* app);
void MenuA_Destroy(void);

#endif
