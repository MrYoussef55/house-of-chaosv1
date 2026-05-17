#ifndef GAME_H
#define GAME_H

#include "app.h"

/*
 * Runs the full game loop (rooms, player, enemies, minimap).
 * Returns when the player quits or the session ends.
 * After returning, app->state is set to STATE_MAIN_MENU.
 */
void Game_Run(App* app);

#endif
