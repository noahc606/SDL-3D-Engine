#pragma once
#include "Matrix4d.h"

class Vec3d {
public:
    Vec3d(double x, double y, double z);
    Vec3d(const Vec3d& v);
    Vec3d();

    void set(double x, double y, double z);
    Vec3d& operator= (const Vec3d& other);
    
    std::string toString();
    Vec3d operator- () const;
    double operator* (const Vec3d& other) const;
    
    Vec3d scale(double factor) const;
    Vec3d stretch(const Vec3d& sv) const;
    Vec3d translate(const Vec3d& tv) const;
    double dot(const Vec3d& other) const;
    Vec3d multiply4d(const Matrix4d& b) const;

    double x = 0; double y = 0; double z = 0;
private:
};