#include "Main.h"
#include <iostream>
#include "MainLoop.h"

SDL_Renderer* renderer = nullptr;

int main()
{
    SDL_Init(0);

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