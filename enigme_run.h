#ifndef ENIGME_RUN_H
#define ENIGME_RUN_H

#include "app.h"

/*
 * Runs the quiz/puzzle screen for the current room.
 *
 * Returns:
 *   1  = player passed (go to next room)
 *   0  = player failed (restart current room)
 */
int Enigme_Run(App* app, int roomIndex);

#endif
