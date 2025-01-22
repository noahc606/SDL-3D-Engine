#pragma once
#include <vector>
#include "Tri3d.h"

class Mesh {
public:
    Mesh();
    ~Mesh();

    void loadFromObj(std::string path);
    void loadFromPreset(int id);

    std::vector<Tri3d> tris;
private:
};