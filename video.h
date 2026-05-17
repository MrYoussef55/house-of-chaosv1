#ifndef VIDEO_H
#define VIDEO_H

#include <SDL2/SDL.h>

/*
 * Plays an MP4 video file before starting the game.
 * Pauses SDL audio, launches the system video player,
 * waits for it to finish, then resumes.
 *
 * Change INTRO_VIDEO below to match your actual filename.
 * The player quits early if the file is not found.
 */

#define INTRO_VIDEO "intro.mp4"

void play_video(SDL_Window* window, const char* filename);

#endif
