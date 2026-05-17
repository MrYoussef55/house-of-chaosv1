#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

typedef enum {
    STATE_MAIN_MENU,
    STATE_OPTIONS,
    STATE_SCORE,
    STATE_HISTORY,
    STATE_ENIGUME,   /* quiz/puzzle screen - shown between rooms */
    STATE_SAUVGARD,
    STATE_AVATAR,
    STATE_GAME,       /* singleplayer game loop */
    STATE_GAME_MULTI, /* multiplayer game loop  */
    STATE_ENIGME      /* alias used internally by game/room logic */
} AppState;

typedef struct {
    SDL_Window*   window;
    SDL_Renderer* renderer;

    Mix_Chunk* clickSound;
    Mix_Music* sauvgardemusic;
    Mix_Music* mainMenuMusic;
    Mix_Music* optionsMusic;

    int      volume;
    AppState state;
    AppState previousState;
    bool     running;
} App;

#endif
