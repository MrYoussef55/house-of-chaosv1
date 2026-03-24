#include "outils.h"
#include <stdio.h>

int init(SDL_Window **window, SDL_Renderer **renderer)
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        return 0;

    if(IMG_Init(IMG_INIT_PNG) == 0)
        return 0;

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        return 0;

    *window = SDL_CreateWindow("Sous Menu",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               1000, 600,
                               0);

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

    return 1;
}

SDL_Texture* loadTexture(char* path, SDL_Renderer* renderer)
{
    SDL_Surface* surface = IMG_Load(path);
    if(!surface) return NULL;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}

void cleanup(SDL_Window *window, SDL_Renderer *renderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
}
void initButton(Button *b, char* normalPath, char* hoverPath,
                int x, int y, SDL_Renderer *renderer)
{
    b->normal = loadTexture(normalPath, renderer);
    b->hover = loadTexture(hoverPath, renderer);

    b->rect.x = x;
    b->rect.y = y;
    b->rect.w = 200;
    b->rect.h = 70;

    b->isHovered = 0;
}

void renderButton(Button *b, SDL_Renderer *renderer)
{
    if(b->isHovered)
        SDL_RenderCopy(renderer, b->hover, NULL, &b->rect);
    else
        SDL_RenderCopy(renderer, b->normal, NULL, &b->rect);
}

void handleHover(Button *b, int mouseX, int mouseY)
{
    if(mouseX >= b->rect.x &&
       mouseX <= b->rect.x + b->rect.w &&
       mouseY >= b->rect.y &&
       mouseY <= b->rect.y + b->rect.h)
        b->isHovered = 1;
    else
        b->isHovered = 0;
}

int runSubMenu(SDL_Window *window, SDL_Renderer *renderer)
{
    SDL_Event event;
    int running = 1;

    SDL_Texture *background = loadTexture("background.png", renderer);

    Mix_Music *bgMusic = Mix_LoadMUS("backgroundsound.mp3");
    Mix_Chunk *clickSound = Mix_LoadWAV("clicksound.mp3");

    Mix_PlayMusic(bgMusic, -1);

    Button avatar1, avatar2, input1, input2, valider, retour;

    initButton(&avatar1, "avatar1off.png", "avatar1on.png", 350, 150, renderer);
    initButton(&avatar2, "avatar2off.png", "avatar2on.png", 600, 150, renderer);
    initButton(&input1, "input1off.png", "input1on.png", 350, 250, renderer);
    initButton(&input2, "input2off.png", "input2on.png", 600, 250, renderer);
    initButton(&valider, "valideroff.png", "valideron.png", 475, 350, renderer);
    initButton(&retour, "retouroff.png", "retouron.png", 475, 450, renderer);

    while(running)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                running = 0;

            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(retour.isHovered)
                {
                    Mix_PlayChannel(-1, clickSound, 0);
                    SDL_Delay(200);
                    running = 0;
                }
                else if(avatar1.isHovered || avatar2.isHovered ||
                        input1.isHovered || input2.isHovered ||
                        valider.isHovered)
                {
                    Mix_PlayChannel(-1, clickSound, 0);
                }
            }
        }

        handleHover(&avatar1, mouseX, mouseY);
        handleHover(&avatar2, mouseX, mouseY);
        handleHover(&input1, mouseX, mouseY);
        handleHover(&input2, mouseX, mouseY);
        handleHover(&valider, mouseX, mouseY);
        handleHover(&retour, mouseX, mouseY);

        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, background, NULL, NULL);

        renderButton(&avatar1, renderer);
        renderButton(&avatar2, renderer);
        renderButton(&input1, renderer);
        renderButton(&input2, renderer);
        renderButton(&valider, renderer);
        renderButton(&retour, renderer);

        SDL_RenderPresent(renderer);
    }

    Mix_FreeMusic(bgMusic);
    Mix_FreeChunk(clickSound);
    SDL_DestroyTexture(background);

    return 0;
}
