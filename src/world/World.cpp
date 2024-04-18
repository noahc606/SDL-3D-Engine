#include "World.h"
#include "Matrix4d.h"
#include <algorithm>

World::World(){}

void World::init()
{
	double mesh[][9] = {
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

	for(int j = 0; j<10; j++)
	for(int i = 0; i<(sizeof(mesh)/sizeof(mesh[0])); i++) {
		Tri3d t2 = Tri3d(mesh[i]); t2 = t2.translate(Vec3d(j*2, 0, 0));
		tris.push_back(t2);
	}

	//tris.push_back(new Tri3d(mesh[4]));
}

World::~World(){}

void World::tick()
{
    timer++;

}

void World::draw(SDL_Renderer* r)
{
	Matrix4d matRotZ; matRotZ.setToZRotMatrix(timer/100.0);
	Matrix4d matRotY; matRotY.setToXRotMatrix(timer/100.0);
	Matrix4d matRotX; matRotX.setToXRotMatrix(timer/100.0);
	Matrix4d matProj; matProj.setToProjMatrix(1000, 1000);
	Vec3d vecProjOffset(0, 0, 3);
	Vec3d vecProjStretch(50, 50, 1);
	int tx = 640;
	int ty = 480;

    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);

	/** Build list of triangles to raster **/
	std::vector<Tri3d> trisToRaster;
	for(Tri3d triOriginal : tris) {
		//Copy of the original tris to transform
		Tri3d tri = triOriginal;
		
		//Set tri's color
		double avgX = (tri.pts[0].x+tri.pts[1].x+tri.pts[2].x)/3;
    	SDL_SetRenderDrawColor(r, 100+avgX*123, 200+avgX*456, 300+avgX*789, 255);

		//Get transformed tri
		tri = tri.multiply4d(matRotZ);
		tri = tri.multiply4d(matRotX);
		tri = tri.translate(vecProjOffset);

		Vec3d nl = tri.normal();
		Vec3d triNP = tri.pts[0];

		tri = tri.multiply4d(matProj);
		tri = tri.stretch(vecProjStretch);

		//Get normal of the tri to determine if the triangle should be drawn
		//if(true) {
		if( nl.dot(triNP)<0 ) {

			//Find tri's brightness
			Vec3d ld(0, 0, -1.0);		//Light Direction
			double ls = ld.magnitude();	//Magnitude of light vector
			ld.scale((double)(1)/ls);	//Scale light direction vector so that its mag. == 1.
			double b = nl.dot(ld);		//Get brightness, store into 'b'.
			
			//Set tri's color based on brightness (dark purple-ish)
			SDL_Color c;
			c.r = 255*b*0.25;
			c.g = 255*b*0.0;
			c.b = 255*b*1.0;
			c.a = 255;
			tri.col = c;

			trisToRaster.push_back(tri);
		}
	}

	/** Sort trisToRaster so that triangles with a higher avg. Z are put in front (drawn last) **/
	std::sort(trisToRaster.begin(), trisToRaster.end(), [](Tri3d& t1, Tri3d& t2) {
		double z1 = (t1.pts[0].z+t1.pts[1].z+t1.pts[2].z)/3.0;	//Avg. Z in points of t1
		double z2 = (t2.pts[0].z+t2.pts[1].z+t2.pts[2].z)/3.0;	//Avg. Z in points of t2
		return z1>z2;
	});

	/** Raster triangles **/
	for(Tri3d tri : trisToRaster) {
		SDL_Color c = tri.col;
		std::vector<SDL_Vertex> verts = {
			{ SDL_FPoint{ (float)tri.pts[0].x+tx, (float)tri.pts[0].y+ty }, c, SDL_FPoint{ 0 }, },
			{ SDL_FPoint{ (float)tri.pts[1].x+tx, (float)tri.pts[1].y+ty }, c, SDL_FPoint{ 0 }, },
			{ SDL_FPoint{ (float)tri.pts[2].x+tx, (float)tri.pts[2].y+ty }, c, SDL_FPoint{ 0 }, },
		};

		SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
		SDL_RenderGeometry(r, nullptr, verts.data(), verts.size(), nullptr, 0 );

		if(0) {
			SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
			SDL_RenderDrawLine(r, tri.pts[0].x+tx, tri.pts[0].y+ty, tri.pts[1].x+tx, tri.pts[1].y+ty);
			SDL_RenderDrawLine(r, tri.pts[0].x+tx, tri.pts[0].y+ty, tri.pts[2].x+tx, tri.pts[2].y+ty);
			SDL_RenderDrawLine(r, tri.pts[1].x+tx, tri.pts[1].y+ty, tri.pts[2].x+tx, tri.pts[2].y+ty);
		}
	}
}