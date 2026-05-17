#include "video.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Plays an MP4 using the OS default video player.
 *
 * Windows : starts with "start /wait"  (uses Windows Media Player or default)
 * Linux   : tries mpv, then vlc, then xdg-open
 * macOS   : uses "open -W" (QuickTime or default)
 *
 * The call BLOCKS until the video window is closed,
 * so the game resumes right after the player exits.
 *
 * To change the video file just edit INTRO_VIDEO in video.h
 * or pass a different filename when calling play_video().
 */
void play_video(SDL_Window* window, const char* filename)
{
    if (!filename || strlen(filename) == 0) return;

    /* Check file exists */
    FILE* f = fopen(filename, "rb");
    if (!f) {
        printf("[video] File not found: %s — skipping intro.\n", filename);
        return;
    }
    fclose(f);

    /* Pause audio so it doesn't clash with video sound */
    Mix_PauseMusic();
    Mix_Pause(-1);

    /* Hide the game window while video plays */
    SDL_HideWindow(window);

    /* Build the OS command */
    char cmd[512];

#if defined(_WIN32) || defined(_WIN64)
    /* Windows: "start /wait" blocks until the player closes */
    snprintf(cmd, sizeof(cmd), "start /wait \"\" \"%s\"", filename);
#elif defined(__APPLE__)
    /* macOS: open -W blocks until QuickTime closes */
    snprintf(cmd, sizeof(cmd), "open -W \"%s\"", filename);
#else
    /* Linux: try mpv first (best), then vlc, then xdg-open */
    snprintf(cmd, sizeof(cmd),
        "mpv --fs --no-terminal \"%s\" 2>/dev/null || "
        "vlc --play-and-exit --fullscreen \"%s\" 2>/dev/null || "
        "xdg-open \"%s\"",
        filename, filename, filename);
#endif

    printf("[video] Playing: %s\n", filename);
    system(cmd);
    printf("[video] Done.\n");

    /* Show game window again and resume audio */
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    Mix_ResumeMusic();
    Mix_Resume(-1);
}
