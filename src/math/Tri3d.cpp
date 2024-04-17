#include "Tri3d.h"
#include "Line3d.h"

Tri3d::Tri3d(const Vec3d& p0, const Vec3d& p1, const Vec3d& p2)
{
    pts.push_back(p0);
    pts.push_back(p1);
    pts.push_back(p2);
}
Tri3d::Tri3d(double coords[9])
{
    for(int i = 0; i<3; i++) {
        pts.push_back(Vec3d(coords[0+3*i], coords[1+3*i], coords[2+3*i]));
    }
}


Tri3d& Tri3d::operator=(const Poly3d& other)
{
    if(other.pts.size()==3) {
        Poly3d::set(other);
    } else {
        printf("Error: Can't set a Tri3d equal to a Poly3d with %d points.\n", other.pts.size());
    }
    
    return *this;
}

Vec3d Tri3d::normal()
{

    Vec3d res;
    return res;
}