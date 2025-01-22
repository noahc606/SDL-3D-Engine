#include "Main.h"
#include <iostream>
#include <nch/math-utils/mat4x4.h>
#include <nch/sdl-utils/input.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "MainLoop.h"
#include "MatrixOps.h"
#include <nch/cpp-utils/string-utils.h>

using namespace nch;

SDL_Renderer* renderer = nullptr;
SDL_Window* win = nullptr;

int main()
{
    SDL_Init(0);
    TTF_Init();

    win = SDL_CreateWindow("Out-of-this-World Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 960, 0);
    SDL_SetWindowResizable(win, SDL_TRUE);
    renderer = SDL_CreateRenderer(win, -1, 0);
    SDL_ShowWindow(win);
    
	//Set icon
	SDL_Surface* tempico = IMG_Load("res/icon.png");
	SDL_SetWindowIcon(win, tempico);
	SDL_FreeSurface(tempico);

    if(true) {
        MainLoop ml;
    } else {

    }
    

    SDL_Quit();
    return 0;
}

int WinMain() { return main(); }

SDL_Renderer* Main::getRenderer() { return renderer; }
SDL_Window* Main::getWindow() { return win; }
int Main::getWidth() { int res; SDL_GetWindowSize(win, &res, NULL); return res; }
int Main::getHeight() { int res; SDL_GetWindowSize(win, NULL, &res); return res; }

void Main::drawText(SDL_Renderer* r, std::string text, int x, int y)
{
    TTF_Font* monofont = TTF_OpenFont("res/FreeMono.ttf", 20);
    SDL_Color white = {255, 255, 255};

    int textWidth = 0;
    TTF_MeasureText(monofont, text.c_str(), 5000, &textWidth, NULL);
    int textHeight = TTF_FontHeight(monofont);
    
    SDL_Surface* txtSurf = TTF_RenderText_Solid(monofont, text.c_str(), white);
    SDL_Texture* txtTex = SDL_CreateTextureFromSurface(r, txtSurf);
    
    SDL_Rect txtRect; txtRect.x = x; txtRect.y = y; txtRect.w = textWidth; txtRect.h = textHeight;
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderCopy(r, txtTex, NULL, &txtRect);
    
    SDL_FreeSurface(txtSurf);
    SDL_DestroyTexture(txtTex);
    TTF_CloseFont(monofont);
}