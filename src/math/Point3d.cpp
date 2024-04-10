#include "Point3d.h"

Point3d::Point3d(double x, double y, double z) { set(x, y, z); }
Point3d::Point3d(Point3d& p3d) { (*this) = p3d; }
Point3d::Point3d() { set(0, 0, 0); }

void Point3d::set(double x, double y, double z) 
{
    Point3d::x = x;
    Point3d::y = y;
    Point3d::z = z;
}

Point3d& Point3d::operator=(const Point3d& other)
{
    set(other.x, other.y, other.z);
    return *this;
}

/*
    Multiply vector 'a' (represented by this point) by the matrix 'b'
    Here, 'a' is treated as a 4D vector with w=1 and the original 3 coordinates (x,y,z) remaining the same.
    In the result, each of these 3 coordinates are divided by the w that we obtain.
    
    Returns: a 3D point which is a*b (a transformed by b).
*/

Point3d& Point3d::multiply4d(Matrix4d b)
{
    Point3d res(0, 0, 0);
    res.x =     x*b.get(0, 0) + y*b.get(1, 0) + z*b.get(2, 0) + b.get(3, 0);
    res.y =     x*b.get(0, 1) + y*b.get(1, 1) + z*b.get(2, 1) + b.get(3, 1);
    res.z =     x*b.get(0, 2) + y*b.get(1, 2) + z*b.get(2, 2) + b.get(3, 2);
    double w =  x*b.get(0, 3) + y*b.get(1, 3) + z*b.get(2, 3) + b.get(3, 3);

    if( w!=0.0 ) {
        res.x /= w;
        res.y /= w;
        res.z /= w;
        set(res.x, res.y, res.z);
        return (*this);
    }
    
    //printf("Matrix4d.multiply3d() - w shouldn't be zero!\n");
    //res.x /= 100;
    //res.y /= 100;
    //res.z /= 100;
    //set(res.x, res.y, res.z);
    return (*this);
}
Point3d& Point3d::translate(const Point3d& p)
{
    x += p.x;
    y += p.y;
    z += p.z;
    return (*this);
}