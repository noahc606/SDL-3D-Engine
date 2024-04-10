#pragma once
#include "Matrix4d.h"

class Point3d {
public:
    Point3d(double x, double y, double z);
    Point3d(Point3d& p);
    Point3d();

    void set(double x, double y, double z);
    Point3d& operator= ( const Point3d& other );
    Point3d& multiply4d(Matrix4d b);
    Point3d& translate(const Point3d& p);

    double x = 0; double y = 0; double z = 0;
private:
};