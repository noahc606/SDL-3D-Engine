#include "Main.h"
#include <iostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "MainLoop.h"

using namespace nch;

SDL_Window* win = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_PixelFormat* pxFmt = nullptr;
std::string binDirPath = "???null???";

int main()
{
    //Initializing libs
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    //Create window, renderer, pixelFormat, dir path
    win = SDL_CreateWindow("Out-of-this-World Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 960, SDL_WINDOW_VULKAN);
    SDL_SetWindowResizable(win, SDL_TRUE);
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_ShowWindow(win);
    pxFmt = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    binDirPath = SDL_GetBasePath();

	//Set application icon
	SDL_Surface* tempico = IMG_Load("res/icon.png");
	SDL_SetWindowIcon(win, tempico);
	SDL_FreeSurface(tempico);

    //Do main loop, then quit
    MainLoop ml;    
    SDL_Quit();
    return 0;
}

int WinMain() { return main(); }

SDL_Window* Main::getWindow() { return win; }
SDL_Renderer* Main::getRenderer() { return renderer; }
SDL_PixelFormat* Main::getPixelFormat() { return pxFmt; }
std::string Main::getBinDirPath() { return binDirPath; }
int Main::getWidth() { int res; SDL_GetWindowSize(win, &res, NULL); return res; }
int Main::getHeight() { int res; SDL_GetWindowSize(win, NULL, &res); return res; }

void Main::drawText(SDL_Renderer* r, std::string text, int x, int y)
{
    TTF_Font* monofont = TTF_OpenFont((getBinDirPath()+"res/FreeMono.ttf").c_str(), 20);
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