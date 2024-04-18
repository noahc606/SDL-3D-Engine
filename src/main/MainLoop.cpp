#include "MainLoop.h"
#include <iostream>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include "Main.h"

int MainLoop::currentFPS = 0;
int MainLoop::currentTPS = 0;

MainLoop::MainLoop()
{
	//Init world
	world.init();

	//While game running, run gameLoop
	uint64_t nsPerTick = 1000000000/60;
	uint64_t nsPerFrame = 1000000000/120;
	int fps = 0;
	int tps = 0;

	//All units below are in nanoseconds.
	uint64_t tickLast = 0;
	uint64_t frameLast = 0;
	
	//Game loop
	while(running) {
		/* Calculate delta time (time difference between the last iteration and the current iteration of this loop).
		 * This is used to measure how long draw() and tick() are taking.
		 * We need to keep tick rate at 60 ticks per second (TPS). When TPS<60, we reduce the FPS temporarily to improve performance in terms of TPS.
		 */
		if( getCurrentTimeNS()>=tickLast ) {
			tickLast = getCurrentTimeNS()+nsPerTick;
			
			uint64_t tickT0 = getCurrentTimeNS();
			tick();
			tps++;
			uint64_t tickT1 = getCurrentTimeNS();
			uint64_t tickDelta = getDeltaTime(tickT1, tickT0);
			
			//Modify framerate based on how fast the tick ran.
			if( tickDelta>nsPerTick ) {
				nsPerFrame += (tickDelta-nsPerTick);
			} else {
				nsPerFrame -= (nsPerTick-tickDelta);
			}
		} 
		
		if( getCurrentTimeNS()>=frameLast ) {
			frameLast = getCurrentTimeNS()+nsPerFrame;
			draw();
			fps++;
		}
		
		//Events
		event();
		
		//Run this block every second.
		if( SDL_GetTicks()>=secLast ) {
			secLast = SDL_GetTicks()+1000;
			currentTPS = tps;
			currentFPS = fps;
			tps = 0;
			fps = 0;
		}	
	}
}

int MainLoop::getCurrentFPS() { return currentFPS; }
int MainLoop::getCurrentTPS() { return currentTPS; }

/**
 * Get the time since startup in nanoseconds, according to SDL_GetPerformanceCounter() and SDL_GetPerformanceFrequency().
 */
uint64_t MainLoop::getCurrentTimeNS()
{	
	//Prevent overflow: multiply performance counter (which is huge) by 10k, divide by performance frequency, then multiply by 100k.
	//Effectively we have multiplied by 1B.
	//Related: https://github.com/libsdl-org/SDL/issues/6762
	return SDL_GetPerformanceCounter()*10000/SDL_GetPerformanceFrequency()*100000;
}

uint64_t MainLoop::getDeltaTime(uint64_t now, uint64_t last) { return now-last; }

void MainLoop::tick()
{
	timer++;
	world.tick();
}

void MainLoop::draw()
{
	SDL_Renderer* rend = Main::getRenderer();
	
	SDL_SetRenderTarget(rend, NULL);
	
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
	SDL_RenderFillRect(rend, NULL);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
	SDL_Rect r; r.x = 32+timer/5; r.y = 32+timer/5; r.w = 32; r.h = 32;
	//SDL_RenderDrawRect(rend, &r);
	
	SDL_SetRenderDrawColor(rend, 255, 255, 0, 255);
	SDL_Rect r2; r2.x = 32+timer/5+1; r2.y = 32+timer/5+1; r2.w = 32-2; r2.h = 32-2;
	//SDL_RenderDrawRect(rend, &r2);

	world.draw(rend);
	//SDL_Quit();



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	SDL_RenderPresent(rend);
}

void MainLoop::event() {
	SDL_Event e;
	while( SDL_PollEvent(&e)!=0 ) {
		switch(e.type) {
			case SDL_QUIT: {
				running = false;
				SDL_Quit();
			} break;
			
			case SDL_KEYDOWN:
			case SDL_KEYUP: {
			} break;
		}
	}
}