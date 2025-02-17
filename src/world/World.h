#pragma once
#include <nch/sdl-utils/z/gfx/FrameBuffer.h>
#include <SDL2/SDL.h>
#include <vector>
#include "Mesh.h"

class World {
public:
    World();
    void init();
    ~World();

    
    void tick();
    void tickMovement();
    void draw(SDL_Renderer* rend);
    void drawWorld(SDL_Renderer* rend);
    void drawUIs(SDL_Renderer* rend);

    void drawInfo(SDL_Renderer* rend);

    static void assignTriColor(Tri3d& tri, nch::Vec3<double>& normal);

private:
    bool focused = true;
    bool debug = true;

    Mesh mesh;
    nch::FrameBuffer frameBuffer;

    double timer = 0;
    double camYaw = 0; //Unit circle in X/Z plane
    double camPch = 0;  //Unit circle in Y/Z plane
    nch::Vec3<double> vCamPos = nch::Vec3<double>(0, 4, 16);
    nch::Vec3<double> vCamDir = nch::Vec3<double>(0, 0, 0);
};