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