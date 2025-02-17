#pragma once
#include <vector>
#include <nch/cpp-utils/color.h>
#include <nch/math-utils/mat4x4.h>
#include <nch/math-utils/vec2.h>
#include <nch/math-utils/vec3.h>


class Tri3d {
public:
    typedef nch::Vec2<double> t_vec2d;
    typedef nch::Vec3<double> t_vec3d;
    typedef nch::Vec4<double> t_vec4d;

    Tri3d(t_vec3d p1, t_vec3d p2, t_vec3d p3);
    Tri3d(double a[9]);
    Tri3d();
    ~Tri3d();

    Tri3d stretch(const t_vec3d& tv);
    Tri3d translate(const t_vec3d& tv);
    Tri3d multiplyVertices(const nch::Mat4x4<double>& m);

    static int clipAgainstPlane(t_vec3d planeP, t_vec3d planeN, Tri3d& inTri, Tri3d& outTri1, Tri3d& outTri2);

    std::vector<t_vec4d> p; //Points
    std::vector<t_vec3d> t; //Texture location (u, v) + depth buffer value (w)
    nch::Color col = nch::Color(255, 255, 255);
private:
};