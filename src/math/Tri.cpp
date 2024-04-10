#include "Tri.h"

Tri::Tri()
{


}

Tri::Tri(double coords[9])
{
    for(int i = 0; i<3; i++) {
        points[i].set(coords[0+3*i], coords[1+3*i], coords[2+3*i]);
    }
}

Tri& Tri::multiply4d(Matrix4d b)
{
    points[0].multiply4d(b);
    points[1].multiply4d(b);
    points[2].multiply4d(b);
    return (*this);
}

Tri& Tri::translate(const Point3d& p)
{
    points[0].translate(p);
    points[1].translate(p);
    points[2].translate(p);
    return (*this);
}

Tri& Tri::scale(double xFactor, double yFactor, double zFactor)
{
    for(int i = 0; i<3; i++) {
        points[i].x *= xFactor;
        points[i].y *= yFactor;
        points[i].z *= zFactor;
    }
    return (*this);
}