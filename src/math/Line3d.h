#pragma once
#include "Point3d.h"

class Line3d {
public:
    Line3d(Point3d p1, Point3d p2);
    Line3d(double p1x, double p1y, double p1z, double p2x, double p2y, double p2z);
    
    Line3d& multiply4d(Matrix4d b);
    Line3d& translate(const Point3d& p);

    Point3d p1;
    Point3d p2;
private:
};