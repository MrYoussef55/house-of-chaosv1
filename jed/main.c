#include "submenu.h"

int main()
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if(!init(&window, &renderer))
        return -1;

    runSubMenu(window, renderer);

    cleanup(window, renderer);

    return 0;
}
