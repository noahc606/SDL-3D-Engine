#include "MainLoop.h"
#include <iostream>
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/timer.h>
#include <nch/sdl-utils/input.h>
#include <nch/sdl-utils/main-loop-driver.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>
#include "Main.h"

#if (defined(_WIN32) || defined(WIN32))
#include <windows.h>
#endif

World MainLoop::world;

using namespace nch;

MainLoop::MainLoop()
{
	//Init world
	world.init();
	Log::log("Finished world initialization");

	MainLoopDriver mld(Main::getRenderer(), tick, 60, draw, 300);
}

void MainLoop::tick()
{
	world.tick();
}

void MainLoop::draw(SDL_Renderer* rend)
{	
	SDL_SetRenderTarget(rend, NULL);
	
	SDL_SetRenderDrawColor(rend, 0, 16, 48, 255);
	SDL_RenderFillRect(rend, NULL);
	
	world.draw(rend);
	SDL_RenderPresent(rend);
}