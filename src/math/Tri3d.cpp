#include "Tri3d.h"
#include "Line3d.h"
#include <math.h>

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
    Vec3d v1( pts[1].translate(-pts[0]) );
    Vec3d v2( pts[2].translate(-pts[0]) );
    Line3d li(v1, v2);

    //printf("%s ...... %s\n", v1.toString().c_str(), v2.toString().c_str());


    Vec3d res;
    res.x = li.pts[0].y*li.pts[1].z - li.pts[0].z*li.pts[1].y;  // y1z2 - z1y2
    res.y = li.pts[0].z*li.pts[1].x - li.pts[0].x*li.pts[1].z;  // z1x2 - x1z2
    res.z = li.pts[0].x*li.pts[1].y - li.pts[0].y*li.pts[1].x;  // x1y2 - y1x2

    double len = std::sqrt(res.x*res.x + res.y*res.y + res.z*res.z);
    res = res.scale(1.0/len);
    
    //printf("%s\n", res.toString().c_str());
    return res;
}