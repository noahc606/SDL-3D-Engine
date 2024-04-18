#include "Main.h"
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include "MainLoop.h"

SDL_Renderer* renderer = nullptr;

int main()
{
    SDL_Init(0);
    TTF_Init();

    SDL_Window* win = SDL_CreateWindow("SDL 3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 960, 0);
    renderer = SDL_CreateRenderer(win, -1, 0);
    SDL_ShowWindow(win);

    MainLoop ml;

    SDL_Quit();



    printf("asdf\n");
    return 0;
}

int WinMain()
{
    return main();
}

SDL_Renderer* Main::getRenderer()
{
    return renderer;
}

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
    SDL_RenderCopy(r, txtTex, NULL, &txtRect );
    
    SDL_FreeSurface(txtSurf);
    SDL_DestroyTexture(txtTex);
    TTF_CloseFont(monofont);
}