#pragma once
#include <SDL2/SDL.h>
#include <string>

class Main {
public:
    static SDL_Renderer* getRenderer();
    static SDL_Window* getWindow();
    static int getWidth();
    static int getHeight();

    static void drawText(SDL_Renderer* r, std::string text, int x, int y);
};