#pragma once
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "World.h"

class MainLoop {
public:
	MainLoop();

	static int getCurrentFPS();
	static int getCurrentTPS();

private:
	uint64_t getCurrentTimeNS();
	uint64_t getDeltaTime(uint64_t now, uint64_t last);
	
	void tick();
	void draw();
	void event();

	World world;

	bool running = true;
	uint64_t timer = 0;
	uint64_t secLast;
	static int currentFPS;
	static int currentTPS;
};