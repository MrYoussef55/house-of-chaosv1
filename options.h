#ifndef OPTIONS_H
#define OPTIONS_H

#include "app.h"

void Options_Init(App* app);
void Options_HandleEvent(App* app, SDL_Event* e);
void Options_Update(App* app);
void Options_Render(App* app);
void Options_Destroy();

#endif
