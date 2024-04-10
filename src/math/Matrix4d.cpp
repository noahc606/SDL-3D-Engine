#include "Matrix4d.h"
#include <iostream>
#include <math.h>
#include <sstream>

Matrix4d::Matrix4d()
{
    reset();
}

/*
    Return mat[row][col] which represents the value at that spot of the matrix.
*/
double Matrix4d::get(int row, int col)
{
    return mat[row][col];
}

std::string Matrix4d::toString()
{
    std::stringstream ss;
    ss << "Matrix:\n";
    for(int x = 0; x<4; x++) {
        ss << "[ ";
        for(int y = 0; y<4; y++) {
            ss << mat[x][y] << " ";
        }
        ss << "]\n";
    }

    return ss.str();
}

void Matrix4d::reset()
{
    //Set matrix to all 0's
    for(int x = 0; x<4; x++) {
        for(int y = 0; y<4; y++) {
            mat[x][y] = 0;
        }
    }
}

/*
    Projection matrix:
    
    | a 0 0 0 |
    | 0 b 0 0 |
    | 0 0 c d |
    | 0 0 1 0 |
    
    a: aspectRation*fovRad
    b: fovRad
    c: far/(far-near)
    d: (-far*near)/(far-near)

*/
void Matrix4d::setToProjMatrix(int screenWidth, int screenHeight)
{
    //Matrix taken from https://www.youtube.com/watch?v=ih20l3pJoeU
    double near = 0.1;                                              //Near plane (distance from camera/eyes to screen)
    double far = 1000.0;                                            //Far plane
    double fov = 90.0;                                              //Field of View
    double aspectRatio = (double)screenWidth/(double)screenHeight;  //Screen aspect ratio
    double fovRad = 1.0/std::tan( fov*0.5/180.0*M_PI );             //modified fov value, in radians
    
    reset();
    mat[0][0] = aspectRatio*fovRad;
    mat[1][1] = fovRad;
    mat[2][2] = far/(far-near);
    mat[3][2] = 1.0; 
    mat[2][3] = (-far*near)/(far-near);
}

void Matrix4d::setToXRotMatrix(double theta)
{
    reset();
    mat[0][0] = 1;
    mat[1][1] = std::cos(theta);
    mat[1][2] = std::sin(theta);
    mat[2][1] = -std::sin(theta);
    mat[2][2] = std::cos(theta);
    mat[3][3] = 1;
}

void Matrix4d::setToYRotMatrix(double theta)
{
    reset();
    mat[0][0] = std::cos(theta);
    mat[0][2] = std::sin(theta);
    mat[1][1] = 1;
    mat[2][0] = -std::sin(theta);
    mat[2][2] = std::cos(theta);
    mat[3][3] = 1;
}

void Matrix4d::setToZRotMatrix(double theta)
{
    reset();
    mat[0][0] = std::cos(theta);
    mat[0][1] = std::sin(theta);
    mat[1][0] = -std::sin(theta);
    mat[1][1] = std::cos(theta);
    mat[2][2] = 1;
    mat[3][3] = 1;
}