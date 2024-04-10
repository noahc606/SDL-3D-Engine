#include "Line3d.h"

Line3d::Line3d(Point3d p1, Point3d p2) {
    Line3d::p1 = p1;
    Line3d::p2 = p2;
}

Line3d::Line3d(double p1x, double p1y, double p1z, double p2x, double p2y, double p2z)
{
    p1 = Point3d(p1x, p1y, p1z);
    p2 = Point3d(p2x, p2y, p2z);
}

Line3d& Line3d::multiply4d(Matrix4d b)
{
    p1.multiply4d(b);
    p2.multiply4d(b);
    return (*this);
}

Line3d& Line3d::translate(const Point3d& p)
{
    p1.translate(p);
    p2.translate(p);
    return (*this);
}