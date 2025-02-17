#include "Mesh.h"
#include <list>
#include <nch/cpp-utils/file-utils.h>
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/string-utils.h>
#include <nch/cpp-utils/timer.h>
#include <nch/math-utils/matrixops.h>
#include <nch/math-utils/vec2.h>
#include <nch/sdl-utils/texture-utils.h>
#include <SDL2/SDL_image.h>
#include "Main.h"
#include "World.h"

using namespace nch;

double Mesh::cubeMesh[][9] = {
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

Mesh::Mesh(){}
Mesh::~Mesh(){ delete[] srcTex; }

void Mesh::loadFromObj(std::string path)
{
    initTex();

    nch::Timer t(".obj loading", true);
    tris.clear();

    //Load file
    FILE* fp = fopen((Main::getBinDirPath()+path).c_str(), "r");
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
    initTex();

    switch(id) {
        case 0: {
            setCube(0, 0, 0);
            int x = 0;
            int y = 123;
        } break;

        case 1: {
            for(int ix = 0; ix<16; ix++)
            for(int iy = 0; iy<1; iy++)
            for(int iz = 0; iz<3; iz++) {
                setCube(ix, iy, iz);
            }
        }
    }
}

void Mesh::draw(FrameBuffer& fb, Vec3<double> vCamPos, Vec3<double> vCamDir)
{
	MatrixOps<double> mops;
	typedef Vec3<double> t_vec3d;
	typedef Mat4x4<double> t_mat4d;

	//Rotation matrices
	t_mat4d matTrans = mops.getTranslationMatrix(t_vec3d(0, 0, 16));
	
	//World and camera matrices
	t_mat4d matWorld = mops.getIdentityMatrix();
	matWorld = mops.multiply(matWorld, matTrans);
	
	//Camera, view, projection matrices
	t_vec3d vUp(0, 1, 0);
	t_vec3d vTarget = vCamPos+vCamDir;
	t_mat4d matCam = mops.getPointAtMatrix(vCamPos, vTarget, vUp);
	t_mat4d matView = mops.invSpecialMatrix(matCam);
	t_mat4d matProj = mops.getProjectionMatrix();

    //Build final list of screen tris
	std::vector<Tri3d> screenTris;
	//#pragma omp parallel for num_threads(8)
	for(int i = 0; i<tris.size(); i++) {
		Tri3d triMod, triViewed, triProj;

		triMod = tris.at(i);
		triMod = triMod.multiplyVertices(matWorld);

		//Store normal vector
		t_vec3d norm, line1, line2;
		line1 = triMod.p[1].vec3()-triMod.p[0].vec3();
		line2 = triMod.p[2].vec3()-triMod.p[0].vec3();
		norm = line1.cross(line2).normalized();
		t_vec3d vCamRay = triMod.p[0].vec3()-vCamPos;

		//If the triangle is visible based on normal vector...
		if(norm.dot(vCamRay)<0) {
			//Assign color
			World::assignTriColor(triMod, norm);

			//Convert from world space to view space
			triViewed = triMod;
			triViewed = triViewed.multiplyVertices(matView);
			triViewed = triViewed.stretch(t_vec3d(-1, -1, 1));

			//Apply world clipping
			int numClippedTris = 0;
			Tri3d trisClipped[2];
			t_vec3d nearPlane(0, 0, 0.1);
			numClippedTris = Tri3d::clipAgainstPlane(nearPlane, t_vec3d(0, 0, 1), triViewed, trisClipped[0], trisClipped[1]);

			for(int n = 0; n<numClippedTris; n++) {
				//Apply Projection
				triProj = trisClipped[n];
				triProj = triProj.multiplyVertices(matProj);

				triProj.t[0].x = triProj.t[0].x/triProj.p[0][3];
				triProj.t[1].x = triProj.t[1].x/triProj.p[1][3];
				triProj.t[2].x = triProj.t[2].x/triProj.p[2][3];
				triProj.t[0].y = triProj.t[0].y/triProj.p[0][3];
				triProj.t[1].y = triProj.t[1].y/triProj.p[1][3];
				triProj.t[2].y = triProj.t[2].y/triProj.p[2][3];
				triProj.t[0].z = 1./triProj.p[0][3];
				triProj.t[1].z = 1./triProj.p[1][3];
				triProj.t[2].z = 1./triProj.p[2][3];

				for(int i = 0; i<3; i++) {
					triProj.p[i][0] = triProj.p[i][0]/triProj.p[i][3];
					triProj.p[i][1] = triProj.p[i][1]/triProj.p[i][3];
					triProj.p[i][2] = triProj.p[i][2]/triProj.p[i][3];
				}

				//Add triangle to final list of screen tris.
				//#pragma omp critical
				screenTris.push_back(triProj);
			}
		}
	}

	//Draw list of screen tris
	drawScreenTris(fb, screenTris);
}

void Mesh::initTex()
{
    srcTex = new uint32_t[32*32]();

	SDL_Surface* img = IMG_Load((Main::getBinDirPath()+"res/cubetest.png").c_str());
	SDL_Surface* surf = SDL_ConvertSurface(img, Main::getPixelFormat(), 0);
	SDL_FreeSurface(img);
    if(surf==NULL) {
        Log::error(__PRETTY_FUNCTION__, "SDL Error: %s (path: %s)", SDL_GetError(), (Main::getBinDirPath()+"res/cubetest.png").c_str());
    } else {
		Timer t("getting color array of texture", true);
		for(int ix = 0; ix<32; ix++)
		for(int iy = 0; iy<32; iy++) {
			srcTex[32*iy+ix] = TexUtils::getPixelColor(surf, ix, iy).getRGBA();
		}
    }

	SDL_FreeSurface(surf);
}

void Mesh::drawScreenTris(FrameBuffer& fb, std::vector<Tri3d>& screenTris)
{
	typedef Vec3<double> t_vec3d;
	double w = Main::getWidth()-1;
	double h = Main::getHeight()-1;
	
	for(int i = 0; i<screenTris.size(); i++) {
		Tri3d sTri = screenTris[i];
		//Final transformations into screen
		sTri = sTri.stretch(Vec3<double>(512, 512, 1));
		sTri = sTri.translate(Vec3<double>(Main::getWidth()/2, Main::getHeight()/2, 1));

		//Prepare final clippings against screen boundaries
		Tri3d trisClipped[2];
		std::list<Tri3d> triList;
		triList.push_back(sTri);
		int numNewTris = 1;

		//Perform final clippings against screen boundaries
		for(int p = 0; p<4; p++) {
			int numToAddTris = 0;
			while(numNewTris>0) {
				Tri3d test = triList.front();
				triList.pop_front();
				numNewTris--;

				switch(p) {
					case 0: numToAddTris = Tri3d::clipAgainstPlane(t_vec3d(0,0,0), t_vec3d( 0, 1, 0), test, trisClipped[0], trisClipped[1] );	break;
					case 1: numToAddTris = Tri3d::clipAgainstPlane(t_vec3d(0,h,0), t_vec3d( 0,-1, 0), test, trisClipped[0], trisClipped[1] );	break;
					case 2: numToAddTris = Tri3d::clipAgainstPlane(t_vec3d(0,0,0), t_vec3d( 1, 0, 0), test, trisClipped[0], trisClipped[1] );	break;
					case 3: numToAddTris = Tri3d::clipAgainstPlane(t_vec3d(w,0,0), t_vec3d(-1, 0, 0), test, trisClipped[0], trisClipped[1] );	break;
				}
				for(int t = 0; t<numToAddTris; t++) {
					triList.push_back(trisClipped[t]);
				}
			}

			numNewTris = triList.size();
		}

		for(Tri3d& tri : triList) {
			nch::Color c = tri.col;

			std::vector<SDL_FPoint> pts = {
				SDL_FPoint{ (float)(tri.p[0][0]), (float)(tri.p[0][1]) },
				SDL_FPoint{ (float)(tri.p[1][0]), (float)(tri.p[1][1]) },
				SDL_FPoint{ (float)(tri.p[2][0]), (float)(tri.p[2][1]) }
			};

            TexUtils::renderTexturedTri(fb, srcTex, pts[0], pts[1], pts[2], tri.t[0].tuple(), tri.t[1].tuple(), tri.t[2].tuple());

			// Draw triangle wireframe
			if(drawWireFrame) {
				// SDL_SetRenderTarget(rend, NULL);
				// SDL_SetRenderDrawColor(rend, 96, 160, 96, 255);
				// SDL_RenderDrawLine(rend, tri.p[0][0], tri.p[0][1], tri.p[1][0], tri.p[1][1]);
				// SDL_RenderDrawLine(rend, tri.p[0][0], tri.p[0][1], tri.p[2][0], tri.p[2][1]);
				// SDL_RenderDrawLine(rend, tri.p[1][0], tri.p[1][1], tri.p[2][0], tri.p[2][1]);
				// SDL_SetRenderTarget(rend, NULL);
			}
		}
	}
}



void Mesh::setCube(int x, int y, int z)
{
    int size = (sizeof(cubeMesh)/sizeof(cubeMesh[0]));

    for(int i = 0; i<size; i++) {
        Tri3d t(
            Vec3(cubeMesh[i][0]+x, cubeMesh[i][1]+y, cubeMesh[i][2]+z),
            Vec3(cubeMesh[i][3]+x, cubeMesh[i][4]+y, cubeMesh[i][5]+z),
            Vec3(cubeMesh[i][6]+x, cubeMesh[i][7]+y, cubeMesh[i][8]+z)
        );

        if(i%2==0) {
            t.t[0] = (Vec3<double>(0, 1, 1));
            t.t[1] = (Vec3<double>(0, 0, 1));
            t.t[2] = (Vec3<double>(1, 0, 1));
        } else {
            t.t[0] = (Vec3<double>(0, 1, 1));
            t.t[1] = (Vec3<double>(1, 0, 1));
            t.t[2] = (Vec3<double>(1, 1, 1));
        }

        tris.push_back(t);
    }
}