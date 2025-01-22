#include "Tri3d.h"
#include <nch/math-utils/matrixops.h>
#include <nch/math-utils/vectorops.h>

using namespace nch;

Tri3d::Tri3d()
{
    for(int i = 0; i<3; i++)
    p.push_back(t_vec3d(0, 0, 0));
}

Tri3d::Tri3d(t_vec3d p1, t_vec3d p2, t_vec3d p3)
{
    p.push_back(p1);
    p.push_back(p2);
    p.push_back(p3);
}

Tri3d::Tri3d(double arr[9])
{
    p.push_back(t_vec3d(arr[0], arr[1], arr[2]));
    p.push_back(t_vec3d(arr[3], arr[4], arr[5]));
    p.push_back(t_vec3d(arr[6], arr[7], arr[8]));
}

Tri3d::~Tri3d(){}

Tri3d Tri3d::stretch(const t_vec3d& tv)
{
    Tri3d res = (*this);
    for(int i = 0; i<3; i++) {
        res.p[i][0] = res.p[i][0]*tv.x;
        res.p[i][1] = res.p[i][1]*tv.y;
        res.p[i][2] = res.p[i][2]*tv.z;
    }
    return res;
}


Tri3d Tri3d::translate(const t_vec3d& tv)
{
    Tri3d res = (*this);
    for(int i = 0; i<3; i++) {
        res.p[i][0] = res.p[i][0]+tv.x;
        res.p[i][1] = res.p[i][1]+tv.y;
        res.p[i][2] = res.p[i][2]+tv.z;
    }
    return res;
}

Tri3d Tri3d::multiplyVertices(const Mat4x4<double>& m)
{
    Tri3d res = (*this);
    res.p[0] = MatrixOps<double>::multiply(p[0], m);
    res.p[1] = MatrixOps<double>::multiply(p[1], m);
    res.p[2] = MatrixOps<double>::multiply(p[2], m);
    return res;
}

int Tri3d::clipAgainstPlane(t_vec3d planeP, t_vec3d planeN, Tri3d& inTri, Tri3d& outTri1, Tri3d& outTri2)
{
    VectorOps<double> vops;

    planeN = planeN.normalized();

    auto dist = [&](t_vec4d& p) {
        t_vec3d n = p.vec3().normalized();
        return (planeN.x*p[0] + planeN.y*p[1] + planeN.z*p[2] - planeN.dot(planeP));
    };


    t_vec4d* inPoints[3];   int numIPs = 0;
    t_vec4d* outPoints[3];  int numOPs = 0;
    double d0 = dist(inTri.p[0]);
    double d1 = dist(inTri.p[1]);
    double d2 = dist(inTri.p[2]);
    if(d0>=0)   { inPoints[numIPs++] = &inTri.p[0]; }
    else        { outPoints[numOPs++] = &inTri.p[0]; }
    if(d1>=0)   { inPoints[numIPs++] = &inTri.p[1]; }
    else        { outPoints[numOPs++] = &inTri.p[1]; }
    if(d2>=0)   { inPoints[numIPs++] = &inTri.p[2]; }
    else        { outPoints[numOPs++] = &inTri.p[2]; }

    /* Determine type of clip */
    //Triangle should not be clipped...
    if(numIPs==0) {
        return 0;   //No triangles onscreen
    }
    if(numIPs==3) {
        outTri1 = inTri;
        return 1;   //Original triangle completely onscreen
    }

    //Triangle should be clipped...
    if(numIPs==1 && numOPs==2) {
        outTri1.col = inTri.col;

        outTri1.p[0] = *inPoints[0];
        outTri1.p[1] = vops.intersectPlane(planeP, planeN, (*inPoints[0]).vec3(), (*outPoints[0]).vec3());
        outTri1.p[2] = vops.intersectPlane(planeP, planeN, (*inPoints[0]).vec3(), (*outPoints[1]).vec3());
    
        return 1;   //Single triangle found
    }
    if(numIPs==2 && numOPs==1) {
        outTri1.col = inTri.col;
        outTri1.p[0] = *inPoints[0];
        outTri1.p[1] = *inPoints[1];
        outTri1.p[2] = vops.intersectPlane(planeP, planeN, (*inPoints[0]).vec3(), (*outPoints[0]).vec3());

        outTri2.col = inTri.col;
        outTri2.p[0] = *inPoints[1];
        outTri2.p[1] = outTri1.p[2];
        outTri2.p[2] = vops.intersectPlane(planeP, planeN, (*inPoints[1]).vec3(), (*outPoints[0]).vec3());

        return 2;
    }

    return 0;
}