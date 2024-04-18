#pragma once
#include <string>

/*
    Implementation of a 4x4 matrix.
    
    Most code in this class was ported from code I wrote for an old team project:
    https://github.com/SledgeThatJackal/SkyGazer/blob/main/app/src/main/java/com/echo/skygazer/gfx/math/Matrix4d.java
*/

class Matrix4d {
public:
    /* Construct/Destruct */
    Matrix4d();
    
    /* Getters */
    double get(int row, int col) const;
    std::string toString();

    /* Setters */
    void reset();
    void setToProjMatrix(int screenWidth, int screenHeight);
    void setToXRotMatrix(double theta);
    void setToYRotMatrix(double theta);
    void setToZRotMatrix(double theta);
private:
    double mat[4][4];
};