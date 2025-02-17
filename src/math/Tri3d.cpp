#include "Tri3d.h"
#include <nch/math-utils/matrixops.h>
#include <nch/math-utils/vectorops.h>

using namespace nch;

Tri3d::Tri3d(t_vec3d p1, t_vec3d p2, t_vec3d p3)
{
    p.clear();
    p.push_back(p1);
    p.push_back(p2);
    p.push_back(p3);
    t.clear();
    t.push_back(t_vec3d(0, 1, 1));
    t.push_back(t_vec3d(0, 0, 1));
    t.push_back(t_vec3d(1, 0, 1));
}
Tri3d::Tri3d(double a[9]): Tri3d(t_vec3d(a[0],a[1],a[2]), t_vec3d(a[3],a[4],a[5]), t_vec3d(a[6],a[7],a[8])){}
Tri3d::Tri3d(): Tri3d(t_vec3d(0), t_vec3d(0), t_vec3d(0)){}

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


    t_vec4d* inPts[3];   int numIPs = 0;
    t_vec4d* outPts[3];  int numOPs = 0;
    t_vec3d* inTex[3];   int numITs = 0;
    t_vec3d* outTex[3];  int numOTs = 0;


    double d0 = dist(inTri.p[0]);
    double d1 = dist(inTri.p[1]);
    double d2 = dist(inTri.p[2]);
    if(d0>=0)   { inPts [numIPs++] = &inTri.p[0];   inTex [numITs++] = &inTri.t[0]; }
    else        { outPts[numOPs++] = &inTri.p[0];   outTex[numOTs++] = &inTri.t[0]; }
    if(d1>=0)   { inPts [numIPs++] = &inTri.p[1];   inTex [numITs++] = &inTri.t[1]; }
    else        { outPts[numOPs++] = &inTri.p[1];   outTex[numOTs++] = &inTri.t[1]; }
    if(d2>=0)   { inPts [numIPs++] = &inTri.p[2];   inTex [numITs++] = &inTri.t[2]; }
    else        { outPts[numOPs++] = &inTri.p[2];   outTex[numOTs++] = &inTri.t[2]; }

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

        outTri1.p[0] = *inPts[0];
        outTri1.t[0] = *inTex[0];

        double t;
        outTri1.p[1] = vops.intersectPlane(planeP, planeN, (*inPts[0]).vec3(), (*outPts[0]).vec3(), t);
        outTri1.t[1].x = t*(outTex[0]->x-inTex[0]->x) + inTex[0]->x;
        outTri1.t[1].y = t*(outTex[0]->y-inTex[0]->y) + inTex[0]->y;
        outTri1.t[1].z = t*(outTex[0]->z-inTex[0]->z) + inTex[0]->z;
        
        outTri1.p[2] = vops.intersectPlane(planeP, planeN, (*inPts[0]).vec3(), (*outPts[1]).vec3(), t);
        outTri1.t[2].x = t*(outTex[1]->x-inTex[0]->x) + inTex[0]->x;
        outTri1.t[2].y = t*(outTex[1]->y-inTex[0]->y) + inTex[0]->y;
        outTri1.t[2].z = t*(outTex[1]->z-inTex[0]->z) + inTex[0]->z;

        return 1;   //Clipping creates single triangle
    }
    if(numIPs==2 && numOPs==1) {
        //Out tri 1
        outTri1.col = inTri.col;
        outTri1.p[0] = *inPts[0];   outTri1.t[0] = *inTex[0];
        outTri1.p[1] = *inPts[1];   outTri1.t[1] = *inTex[1];

        double tx;
        outTri1.p[2] = vops.intersectPlane(planeP, planeN, (*inPts[0]).vec3(), (*outPts[0]).vec3(), tx);
        outTri1.t[2].x = tx*(outTex[0]->x-inTex[0]->x) + inTex[0]->x;
        outTri1.t[2].y = tx*(outTex[0]->y-inTex[0]->y) + inTex[0]->y;
        outTri1.t[2].z = tx*(outTex[0]->z-inTex[0]->z) + inTex[0]->z;

        //Out tri 2
        outTri2.col = inTri.col;
        outTri2.p[0] = *inPts[1];       outTri2.t[0] = *inTex[1];
        outTri2.p[1] = outTri1.p[2];    outTri2.t[1] = outTri1.t[2];

        outTri2.p[2] = vops.intersectPlane(planeP, planeN, (*inPts[1]).vec3(), (*outPts[0]).vec3(), tx);
        outTri2.t[2].x = tx*(outTex[0]->x-inTex[1]->x) + inTex[1]->x;
        outTri2.t[2].y = tx*(outTex[0]->y-inTex[1]->y) + inTex[1]->y;
        outTri2.t[2].z = tx*(outTex[0]->z-inTex[1]->z) + inTex[1]->z;

        return 2;   //Clipping creates two triangles (quad)
    }

    return 0;
}