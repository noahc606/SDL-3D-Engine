#pragma once
#include <vector>
#include "Vec3d.h"

class Poly3d {
public:
    Poly3d();

    Poly3d translate(const Vec3d& v);
    Poly3d scale(double xFactor, double yFactor, double zFactor);
    Poly3d multiply4d(Matrix4d b);
    
    Poly3d& set(const Poly3d& other);
    Poly3d& operator= (const Poly3d& other);

    std::vector<Vec3d> pts;
private:
};