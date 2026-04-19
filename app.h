#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

typedef enum {
    STATE_MAIN_MENU, //youssef mehri
    STATE_OPTIONS,   // youssef trabelsi
    STATE_SCORE,     // yahya
    STATE_HISTORY,
    STATE_ENIGUME,   //brahim
    STATE_SAUVGARD,  //hadir
    STATE_AVATAR,    //jed
    STATE_GAME       // <--- ADD THIS LINE HERE!
} AppState;
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;

    Mix_Chunk* clickSound;
    Mix_Music* sauvgardemusic;
    Mix_Music* mainMenuMusic;
    Mix_Music* optionsMusic;

    int volume;

    AppState state;
    AppState previousState;

    bool running;
} App;

#endif


