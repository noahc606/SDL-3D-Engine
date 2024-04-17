#pragma once
#include "Matrix4d.h"

class Vec3d {
public:
    Vec3d(double x, double y, double z);
    Vec3d(const Vec3d& v);
    Vec3d();

    void set(double x, double y, double z);
    Vec3d& operator= ( const Vec3d& other );
    Vec3d operator- ( const Vec3d& other ) const;
    Vec3d multiply4d(Matrix4d b);
    Vec3d translate(const Vec3d& v);

    double x = 0; double y = 0; double z = 0;
private:
};