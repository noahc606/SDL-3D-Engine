#pragma once
#include <SDL2/SDL.h>
#include "Vec3d.h"
#include "Poly3d.h"

class Line3d : public Poly3d {
public:
    Line3d(Vec3d v1, Vec3d v2);
    Line3d(double v1x, double v1y, double v1z, double v2x, double v2y, double v2z);

    void draw(SDL_Renderer* r);

    Line3d& operator=(const Poly3d& other);
private:
};