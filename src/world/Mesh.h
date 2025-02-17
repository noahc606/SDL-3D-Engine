#pragma once
#include <nch/cpp-utils/color.h>
#include <nch/sdl-utils/z/gfx/FrameBuffer.h>
#include <SDL2/SDL.h>
#include <vector>
#include "Tri3d.h"

class Mesh {
public:
    Mesh();
    ~Mesh();
    void loadFromObj(std::string path);
    void loadFromPreset(int id);

    void draw(nch::FrameBuffer& fb, nch::Vec3<double> vCamPos, nch::Vec3<double> vCamDir);

    std::vector<Tri3d> tris;
    bool drawWireFrame = false;
private:
    void initTex();
    void drawScreenTris(nch::FrameBuffer& fb, std::vector<Tri3d>& screenTris);

    void setCube(int x, int y, int z);
    bool cubes[32][32][32];
	static double cubeMesh[][9];

    uint32_t* srcTex = nullptr;
};