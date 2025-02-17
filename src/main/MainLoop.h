#pragma once
#include <mutex>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "World.h"

class MainLoop {
public:
	MainLoop();

private:
	static void tick();
	static void draw(SDL_Renderer* rend);

	static World world;
};