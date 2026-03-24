#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>
#include "header.h"
#include "fonction.h"

int main(int argc, char *argv[])
{
    /* ===================== INIT ===================== */

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        printf("SDL Init error %s\n", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("IMG init error %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() != 0)
    {
        printf("TTF_Init Error %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("Mixer error %s\n", Mix_GetError());
        return 1;
    }

    /* ====== WINDOW ============ */

    SDL_Window *window = SDL_CreateWindow(
        "Options Menu",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN);

    if (!window)
    {
        printf("CreateWindow error %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
        printf("Renderer error %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    /* ============ RESOURCES ============== */

    Background background =
    initBackground(renderer, "background_final");

    TTF_Font *textfont = TTF_OpenFont("font.ttf", 28);
    if (!textfont)
    {
        printf("Font error %s\n", TTF_GetError());
        TTF_Quit();
        return 1;
    }

    Mix_Music *music = Mix_LoadMUS("8oneya");
    Mix_PlayMusic(music, -1);

    Mix_Chunk *clickSound = Mix_LoadWAV("click");
    Mix_VolumeChunk(clickSound, 30);

    int volume = 50;
    applyVolume(volume);

    /* ============== BUTTONS =========== */

    button volMinus = {
    	{270,190,60,60},
        IMG_LoadTexture(renderer,"-b_ub"),
        IMG_LoadTexture(renderer,"-d_ub"),
        false};

    button volPlus = {{470,190,60,60},
        IMG_LoadTexture(renderer,"b_ub"),
        IMG_LoadTexture(renderer,"d_ub"),
        false};

    button musOn = {{440,255,60,60},
        IMG_LoadTexture(renderer,"b_ub"),
        IMG_LoadTexture(renderer,"d_ub"),
        false};

    button musOff = {{305,255,60,60},
        IMG_LoadTexture(renderer,"-b_ub"),
        IMG_LoadTexture(renderer,"-d_ub"),
        false};

    button normal = {{280,350,120,60},
        IMG_LoadTexture(renderer,"normal_b_ub"),
        IMG_LoadTexture(renderer,"normal_d_ub"),
        false};

    button fullsc = {{410,350,120,60},
        IMG_LoadTexture(renderer,"fsb_ub"),
        IMG_LoadTexture(renderer,"fsd_ub"),
        false};

    button backbtn = {{350,430,120,60},
        IMG_LoadTexture(renderer,"back_b_ub"),
        IMG_LoadTexture(renderer,"back_d_ub"),
        false};

    button* buttons[] =
        {&volMinus,&volPlus,&musOn,&musOff,&normal,&fullsc,&backbtn};


	/* ================= NAVIGATION SETUP ================= */

/* Order:
0 volMinus
1 volPlus
2 musOn
3 musOff
4 normal
5 fullsc
6 backbtn
*/

		volMinus.up = 6;
		volMinus.down = 3;
		volMinus.left = 0;
		volMinus.right = 1;

		volPlus.up = 6;
		volPlus.down = 2;
		volPlus.left = 0;
		volPlus.right = 1;

		musOn.up = 1;
		musOn.down = 4;
		musOn.left = 3;
		musOn.right = 2;

		musOff.up = 0;
		musOff.down = 4;
		musOff.left = 3;
		musOff.right = 2;

		normal.up = 2;
		normal.down = 6;
		normal.left = 4;
		normal.right = 5;

		fullsc.up = 2;
		fullsc.down = 6;
		fullsc.left = 4;
		fullsc.right = 5;

		backbtn.up = 4;
		backbtn.down = 0;
		backbtn.left = 6;
		backbtn.right = 6;



    /* ============ MAIN LOOP ============ */

    bool running = true;
    SDL_Event event;
    int selectedIndex = 0;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;

            if (event.type == SDL_MOUSEMOTION)
            {
                int mx = event.motion.x;
                int my = event.motion.y;

                for (int i = 0; i < 7; i++)
                {
                    buttons[i]->hover =
                        isInside(buttons[i]->rect, mx, my);

                    if (buttons[i]->hover)
                        selectedIndex = i;
                }
            }

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:    selectedIndex = buttons[selectedIndex]->up; break;
                    case SDLK_DOWN:  selectedIndex = buttons[selectedIndex]->down; break;
                    case SDLK_LEFT:  selectedIndex = buttons[selectedIndex]->left; break;
                    case SDLK_RIGHT: selectedIndex = buttons[selectedIndex]->right; break;

                    case SDLK_RETURN:
                        Mix_PlayChannel(-1, clickSound, 0);

                        switch (selectedIndex)
                        {
                            case 0: volume -= 5; break;
                            case 1: volume += 5; break;
                            case 2: Mix_ResumeMusic(); break;
                            case 3: Mix_PauseMusic(); break;

                            case 4:
    				SDL_SetWindowFullscreen(window,0);
    				SDL_SetWindowSize(window,WIDTH,HEIGHT);
    				SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
   				 break;

			    case 5:
  				  SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN_DESKTOP);
  				  break;

                            case 6:
                                running = false;
                                break;
                        }

                        if (volume < 0) volume = 0;
                        if (volume > 100) volume = 100;

                        applyVolume(volume);
                        break;
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int mx = event.button.x;
                int my = event.button.y;

                if (isInside(volMinus.rect,mx,my))
                {
                    volume -= 5;
                    if (volume < 0) volume = 0;
                    applyVolume(volume);
                    Mix_PlayChannel(-1, clickSound, 0);
                }

                if (isInside(volPlus.rect,mx,my))
                {
                    volume += 5;
                    if (volume > 100) volume = 100;
                    applyVolume(volume);
                    Mix_PlayChannel(-1, clickSound, 0);
                }

                if (isInside(musOff.rect,mx,my))
                {
                    Mix_PauseMusic();
                    Mix_PlayChannel(-1, clickSound, 0);
                }

                if (isInside(musOn.rect,mx,my))
                {
                    Mix_ResumeMusic();
                    Mix_PlayChannel(-1, clickSound, 0);
                }

                if(isInside(normal.rect,mx,my))
		{
 		   SDL_SetWindowFullscreen(window,0);
  		  SDL_SetWindowSize(window,WIDTH,HEIGHT);
  		  SDL_SetWindowPosition(window,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED);
   		 Mix_PlayChannel(-1,clickSound,0);
		}

               if(isInside(fullsc.rect,mx,my))
		{
    		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
   		 Mix_PlayChannel(-1, clickSound, 0);
		}

                if (isInside(backbtn.rect,mx,my))
                {
                    Mix_PlayChannel(-1, clickSound, 0);
                    running = false;
                }
            }
        }

        for (int i = 0; i < 7; i++)
            buttons[i]->hover = (i == selectedIndex);


        /* ============ RENDER ================== */

        SDL_RenderClear(renderer);
	renderBackground(renderer, &background, window);

        for (int i = 0; i < 7; i++)
            renderButton(renderer, buttons[i]);

        char volumeText[32];
        sprintf(volumeText, "Volume: %d", volume);

        SDL_Texture* volTex =
            renderText(renderer, textfont, volumeText);

        if (volTex)
        {
            int w, h;
            SDL_QueryTexture(volTex, NULL, NULL, &w, &h);

            int winW, winH;
		SDL_GetWindowSize(window, &winW, &winH);

		int panelW = 420;
		int panelH = 520;

		int panelX = background.destRect.x + (background.destRect.w - panelW) / 2;
		int panelY = background.destRect.y + (background.destRect.h - panelH) / 2;


		SDL_Rect textRect = {
   			 panelX + (panelW - w) / 2,
    			panelY + 90,
   			 w,
    			h
		};
            SDL_RenderCopy(renderer, volTex, NULL, &textRect);
            SDL_DestroyTexture(volTex);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    /* ============== CLEANUP =================== */

    for (int i = 0; i < 7; i++)
    {
        SDL_DestroyTexture(buttons[i]->normalTex);
        SDL_DestroyTexture(buttons[i]->hoverTex);
    }

    destroyBackground(&background);
    Mix_FreeChunk(clickSound);
    Mix_FreeMusic(music);
    TTF_CloseFont(textfont);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
