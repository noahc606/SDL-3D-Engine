#pragma once
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "World.h"

class MainLoop {
public:
	MainLoop();

private:
	bool running = true;
	uint64_t timer = 0;
	uint64_t secLast;
	
	uint64_t getCurrentTimeNS();
	uint64_t getDeltaTime(uint64_t now, uint64_t last);
	
	void tick();
	void draw();
	void event();

	World world;
};