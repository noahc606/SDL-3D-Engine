#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "Line3d.h"
#include "Tri3d.h"

class World {
public:
    World();
    void init();
    ~World();

    void tick();
    void draw(SDL_Renderer* r);
private:
    std::vector<Tri3d> tris;

    double timer = 0;
    Vec3d camPos;
    double camPitch = 0, camRoll = 0, camYaw = 0;
};