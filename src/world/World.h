#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "Line3d.h"
#include "Tri.h"

class World {
public:
    World();
    void init();
    ~World();

    void tick();
    void draw(SDL_Renderer* r);
private:
    std::vector<Tri*> tris;

    double timer = 0;
    double camX = 0, camY = 0, camZ = 0;
    double camPitch = 0, camRoll = 0, camYaw = 0;
};