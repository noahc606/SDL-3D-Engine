#include "World.h"
#include <algorithm>
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/timer.h>
#include <nch/math-utils/mat4x4.h>
#include <nch/math-utils/matrixops.h>
#include <nch/math-utils/vec3.h>
#include <nch/sdl-utils/input.h>
#include <nch/sdl-utils/texture-utils.h>
#include <nch/sdl-utils/main-loop-driver.h>
#include <omp.h>
#include <sstream>
#include "Main.h"
#include "MainLoop.h"

using namespace nch;

World::World(){}

void World::init()
{
	//mesh.loadFromPreset(1);
	mesh.loadFromObj("res/jet.obj");
}

World::~World(){}

void World::tick()
{
	//Update camera direction and target based on current yaw and pitch
	double the = camPch*M_PI/180.;
	double phi = camYaw*M_PI/180.;
	vCamDir = Vec3<double>(
		std::sin(the)*std::cos(phi),
		std::cos(the),
		std::sin(the)*std::sin(phi)
	);

	if(focused) {
		SDL_ShowCursor(SDL_DISABLE);
		tickMovement();
	} else {
		SDL_ShowCursor(SDL_ENABLE);
	}
	if(Input::keyDownTime(SDLK_ESCAPE)==1) {
		if(focused) {
			focused = false;
		}
		else {
			SDL_WarpMouseInWindow(Main::getWindow(), Main::getWidth()/2, Main::getHeight()/2);
			focused = true;
		}
	}

	if(Input::keyDownTime(SDLK_F12)==1) {
		if(debug) 	{ debug = false; }
		else 		{ debug = true; }
	}

}

void World::tickMovement()
{
	MatrixOps<double> mops;
    timer++;

	double inc = 0.4;
	if(Input::isKeyDown(SDLK_LCTRL)) {
		inc = 0.05;
	}

	//Movement vectors
	Vec3<double> vForward = vCamDir; vForward.y = 0; vForward = vForward.normalized();
	Vec3<double> vRight = mops.multiply4d(vForward, mops.getYRotMatrix(90.*M_PI/180.));

	//Update camera position based on keyboard presses
	if(Input::isKeyDown(SDLK_d)) 		vCamPos += vRight*inc;
	if(Input::isKeyDown(SDLK_a)) 		vCamPos -= vRight*inc;
	if(Input::isKeyDown(SDLK_SPACE)) 	vCamPos.y += inc;
	if(Input::isKeyDown(SDLK_LSHIFT))	vCamPos.y -= inc;
	if(Input::isKeyDown(SDLK_w)) 		vCamPos += vForward*inc;
	if(Input::isKeyDown(SDLK_s)) 		vCamPos -= vForward*inc;

	//Update camera direction based on mouse movements
	int mx = Input::getMouseX();
	int my = Input::getMouseY();
	if(mx!=Main::getWidth()/2 || my!=Main::getHeight()/2) {
		int lmx = mx;
		int lmy = my;
		SDL_WarpMouseInWindow(Main::getWindow(), Main::getWidth()/2, Main::getHeight()/2);
		int dmx = Main::getWidth()/2-lmx;
		int dmy = Main::getHeight()/2-lmy;

		camYaw -= dmx/4.;
		camPch -= dmy/4.;

		while(camYaw<0) camYaw += 360;
		while(camYaw>360) camYaw -= 360;
		if(camPch>179.9) camPch = 179.9;
		if(camPch<0.01) camPch = 0.01;
	}
}

void World::draw(SDL_Renderer* rend)
{
	//Helper variables
	int mw = Main::getWidth();
	int mh = Main::getHeight();
	uint32_t pixelFormat = Main::getPixelFormat()->format;
	int texAccess = SDL_TEXTUREACCESS_STREAMING;

	//Rebuild frame buffer
	frameBuffer.build(mw, mh);
	//Setup world texture layers (1 on top of 0)
	SDL_Texture* worldTex0 = SDL_CreateTexture(rend, pixelFormat, texAccess, mw, mh);
	SDL_Texture* worldTex1 = SDL_CreateTexture(rend, pixelFormat, texAccess, mw, mh);
	SDL_SetTextureBlendMode(worldTex0, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(worldTex1, SDL_BLENDMODE_BLEND);

	//Draw onto framebuffer within world
	mesh.draw(frameBuffer, vCamPos, vCamDir);
	//Update and draw world textures with new data from framebuffer
	SDL_UpdateTexture(worldTex0, NULL, frameBuffer.getPixelColorData(), frameBuffer.getPitch());
	SDL_RenderCopy(rend, worldTex0, NULL, NULL);
	//Draw UIs
	drawUIs(rend);

	//Cleanup
	SDL_DestroyTexture(worldTex0);
	SDL_DestroyTexture(worldTex1);
	frameBuffer.free();
}

void World::drawWorld(SDL_Renderer* rend)
{
	
}

void World::drawUIs(SDL_Renderer* rend)
{
	//Debug info
	if(debug) {
		std::stringstream ss;
		
		ss << "(FPS, TPS) = (" << MainLoopDriver::getCurrentFPS() << ", " << MainLoopDriver::getCurrentTPS() << ")";
		Main::drawText(rend, ss.str(), 8, 8);
		
		ss.str("");
		ss << "Tris: " << mesh.tris.size();
		Main::drawText(rend, ss.str(), 8, 30);
		
		ss.str("");
		ss << "Camera[(xyz), (yaw/pitch)] = [" << vCamPos.toString() << ", (" << camYaw << ", " << camPch << ")]";
		Main::drawText(rend, ss.str(), 8, 54);
	}

	//Paused
	if(!focused) {
		SDL_SetRenderTarget(rend, NULL);
		SDL_SetRenderDrawColor(rend, 0, 0, 0, 192);
		SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
		SDL_Rect rect;
		rect.x = Main::getWidth()/2-84;  rect.w = 84*2;
		rect.y = Main::getHeight()/2-24; rect.h = 32*2;
		SDL_RenderFillRect(rend, &rect);

		Main::drawText(rend, "Paused", Main::getWidth()/2-36, Main::getHeight()/2);
	}
}

void World::assignTriColor(Tri3d& tri, Vec3<double>& normal)
{
	//Find tri's brightness
	Vec3<double> ld(0, 1, 0);	//Light Direction
	ld = ld.normalized();		//Normalize
	double b = normal.dot(ld);	//Get brightness, store into 'b'.
	
	//Set tri's color based on brightness (dark purple-ish)
	nch::Color c(96, 96, 96);
	c.brighten(30.*b);
	tri.col = c;
}