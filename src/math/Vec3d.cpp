#include "Vec3d.h"
#include <cmath>
#include <sstream>


Vec3d::Vec3d(double x, double y, double z) { set(x, y, z); }
Vec3d::Vec3d(const Vec3d& p3d) { (*this) = p3d; }
Vec3d::Vec3d() { set(0, 0, 0); }

void Vec3d::set(double x, double y, double z) 
{
    Vec3d::x = x;
    Vec3d::y = y;
    Vec3d::z = z;
}

Vec3d& Vec3d::operator=(const Vec3d& other)
{
    set(other.x, other.y, other.z);
    return *this;
}

std::string Vec3d::toString()
{
    std::stringstream ss;
    ss << "( " << x << ", " << y << ", " << z << " )";
    return ss.str();
}

Vec3d Vec3d::operator-() const
{
    Vec3d res(-x, -y, -z);
    return res;
}
double Vec3d::operator*(const Vec3d& other) const
{
    return (*this).dot(other);
}

double Vec3d::magnitude() const
{
    return std::sqrt(x*x+y*y+z*z);
}

Vec3d Vec3d::scale(double factor) const
{
    return stretch(Vec3d(factor, factor, factor));
}

Vec3d Vec3d::stretch(const Vec3d& sv) const
{
    Vec3d res(x*sv.x, y*sv.y, z*sv.z);
    return res;
}

Vec3d Vec3d::translate(const Vec3d& tv) const
{
    Vec3d res(x, y, z);
    res.x += tv.x;
    res.y += tv.y;
    res.z += tv.z;
    return res;
}

double Vec3d::dot(const Vec3d& other) const
{
    return x*other.x + y*other.y + z*other.z;
}

/*
    Multiply vector 'a' (represented by this point) by the matrix 'b'
    Here, 'a' is treated as a 4D vector with w=1 and the original 3 coordinates (x,y,z) remaining the same.
    In the result, each of these 3 coordinates are divided by the w that we obtain.
    
    Returns: a 3D point which is a*b (a transformed by b).
*/
Vec3d Vec3d::multiply4d(const Matrix4d& b) const
{
    Vec3d res(0, 0, 0);
    res.x =     x*b.get(0, 0) + y*b.get(1, 0) + z*b.get(2, 0) + b.get(3, 0);
    res.y =     x*b.get(0, 1) + y*b.get(1, 1) + z*b.get(2, 1) + b.get(3, 1);
    res.z =     x*b.get(0, 2) + y*b.get(1, 2) + z*b.get(2, 2) + b.get(3, 2);
    double w =  x*b.get(0, 3) + y*b.get(1, 3) + z*b.get(2, 3) + b.get(3, 3);

    if( w!=0.0 ) {
        res.x /= w;
        res.y /= w;
        res.z /= w;
        return res;
    }
    
    return Vec3d(0, 0, 0);
}