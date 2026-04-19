#ifndef VALIDATION_H
#define VALIDATION_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "app.h"     /* CRITICAL: Tells the compiler what 'App' is */
#include "button.h"  /* CRITICAL: Tells the compiler what 'Button' is */

/* Function prototypes match the ones in validation.c exactly */
void Validation_Init(App* app);
void Validation_HandleEvent(App* app, SDL_Event* e);
void Validation_Update(App* app);
void Validation_Render(App* app);
void Validation_Destroy(void);

#endif
