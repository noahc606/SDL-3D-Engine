#include "Line3d.h"

Line3d::Line3d(Vec3d v1, Vec3d v2) {
    pts.push_back(v1);
    pts.push_back(v2);
}
Line3d::Line3d(double v1x, double v1y, double v1z, double v2x, double v2y, double v2z)
{
    pts.push_back(Vec3d(v1x, v1y, v1z));
    pts.push_back(Vec3d(v2x, v2y, v2z));
}

void Line3d::draw(SDL_Renderer* r)
{

}

Line3d& Line3d::operator=(const Poly3d& other)
{
    if(other.pts.size()==2) {
        Poly3d::set(other);
    } else {
        printf("Error: Can't set a Line3d equal to a Poly3d with %d points.\n", other.pts.size());
    }
    
    return *this;
}