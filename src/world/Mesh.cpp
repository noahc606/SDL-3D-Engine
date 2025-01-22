#include "Mesh.h"
#include <nch/cpp-utils/file-utils.h>
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/string-utils.h>
#include <nch/sdl-utils/timer.h>

using namespace nch;

Mesh::Mesh(){}
Mesh::~Mesh(){}

void Mesh::loadFromObj(std::string path)
{
    nch::Timer t(".obj loading", true);
    tris.clear();

    //Load file
    FILE* fp = fopen(path.c_str(), "r");
    if(fp==NULL) {
        nch::Log::error(__PRETTY_FUNCTION__, "Failed to load file \"%s\"", path.c_str());
        return;
    }

    //Parse OBJ file to build '::tris'.
    std::vector<Vec3<double>> vertices;
    auto linesOBJ = FileUtils::getFileLines(fp);
    for(int i = 0; i<linesOBJ.size(); i++) {
        //Preprocessing
        std::string line = StringUtils::trimmed(linesOBJ[i]);
        if(line.size()==0) continue;
        auto tokens = StringUtils::split(line, ' ');

        try {        
            //Process line by type (first token)
            if(tokens.at(0)=="v") {
                //Add vertex from this line
                vertices.push_back(Vec3<double>(std::stod(tokens.at(1)), std::stod(tokens.at(2)), std::stod(tokens.at(3))));
            } else
            if(tokens.at(0)=="f") {
                //Build tri(s) from this line, add to mesh.
                std::vector<int> ids;
                for(int j = 1; j<tokens.size(); j++) {
                    auto subtokens = StringUtils::split(tokens.at(j), '/');
                    int num = std::stoi(subtokens.at(0))-1;
                    ids.push_back(num);
                }

                if(ids.size()==3) {
                    Tri3d tri(vertices.at(ids.at(0)), vertices.at(ids.at(1)), vertices.at(ids.at(2)));
                    tris.push_back(tri);
                } else {
                    Log::log("Found %d-sided poly at line %d, replacing with multiple triangles", ids.size(), i);
                    tris.push_back(Tri3d(vertices.at(ids.at(0)), vertices.at(ids.at(1)), vertices.at(ids.at(2))));
                    tris.push_back(Tri3d(vertices.at(ids.at(0)), vertices.at(ids.at(2)), vertices.at(ids.at(3))));
                }
            }
        } catch(std::out_of_range e) {
            Log::errorv(__PRETTY_FUNCTION__, e.what(), "Error at line %d", i+1);
            continue;
        } catch(std::exception e) {
            Log::errorv(__PRETTY_FUNCTION__, e.what(), "Error at line %d", i+1);
            continue;
        }
    }

    //Close file
    fclose(fp);
}

void Mesh::loadFromPreset(int id)
{
	double mdef[][9] = {
		{ 0, 0, 0,    0, 1, 0,    1, 1, 0 },
		{ 0, 0, 0,    1, 1, 0,    1, 0, 0 },
		{ 1, 0, 0,    1, 1, 0,    1, 1, 1 },
		{ 1, 0, 0,    1, 1, 1,    1, 0, 1 },
		{ 1, 0, 1,    1, 1, 1,    0, 1, 1 },
		{ 1, 0, 1,    0, 1, 1,    0, 0, 1 },
		{ 0, 0, 1,    0, 1, 1,    0, 1, 0 },
		{ 0, 0, 1,    0, 1, 0,    0, 0, 0 },
		{ 0, 1, 0,    0, 1, 1,    1, 1, 1 },
		{ 0, 1, 0,    1, 1, 1,    1, 1, 0 },
		{ 1, 0, 1,    0, 0, 1,    0, 0, 0 },
		{ 1, 0, 1,    0, 0, 0,    1, 0, 0 },
	};

	for(int i = 0; i<(sizeof(mdef)/sizeof(mdef[0])); i++) {
		Tri3d t(mdef[i]);
        tris.push_back(t);
	}
}