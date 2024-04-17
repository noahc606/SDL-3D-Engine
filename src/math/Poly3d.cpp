#include "Poly3d.h"

Poly3d::Poly3d(){}

Poly3d& Poly3d::set(const Poly3d& other)
{
    pts.clear();
    for(int i = 0; i<other.pts.size(); i++) {
        pts.push_back(other.pts[i]);
    }
    
    return *this;
}
Poly3d& Poly3d::operator=(const Poly3d& other) { return set(other); }

Poly3d Poly3d::translate(const Vec3d& v)
{
    Poly3d res = (*this);
    for(int i = 0; i<pts.size(); i++) {
        res.pts[i] = res.pts[i].translate(v);
    }
    return res;
}

Poly3d Poly3d::scale(double xFactor, double yFactor, double zFactor)
{
    Poly3d res = (*this);
    for(int i = 0; i<pts.size(); i++) {
        res.pts[i].x *= xFactor;
        res.pts[i].y *= yFactor;
        res.pts[i].z *= zFactor;
    }
    return res;
}

Poly3d Poly3d::multiply4d(Matrix4d b)
{
    Poly3d res = (*this);
    for(int i = 0; i<pts.size(); i++) {
        res.pts[i] = res.pts[i].multiply4d(b);        
    }
    return res;
}