#pragma once
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
    void draw(SDL_Renderer* r);
    void drawMesh(SDL_Renderer* r);
    void drawRasteredTris(SDL_Renderer* r, std::vector<Tri3d>& rasteredTris, bool drawWireFrame);
    void drawInfo(SDL_Renderer* r);

    void assignTriColor(Tri3d& tri, nch::Vec3<double>& normal);
private:
    bool focused = true;
    bool debug = true;

    Mesh mesh;

    double timer = 0;
    double camYaw = 180; //Unit circle in X/Z plane
    double camPch = 999;  //Unit circle in Y/Z plane
    nch::Vec3<double> vCamPos = nch::Vec3<double>(0, 30, -10);
    nch::Vec3<double> vCamDir = nch::Vec3<double>(0, 0, 0);
};