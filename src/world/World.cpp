#include "World.h"
#include "Matrix4d.h"

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

	for(int i = 0; i<(sizeof(mesh)/sizeof(mesh[0])); i++) {
		tris.push_back(new Tri3d(mesh[i]));
	}
}

World::~World(){}

void World::tick()
{
    timer++;
}

void World::draw(SDL_Renderer* r)
{
	Matrix4d matRotZ; matRotZ.setToZRotMatrix(timer/100.0);
	Matrix4d matRotX; matRotX.setToXRotMatrix(timer/100.0);
	Matrix4d matProj; matProj.setToProjMatrix(1000, 1000);

    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);

	for(Tri3d* triOriginal : tris)
	{
		//Copy of the original tris to transform
		Tri3d tri = *triOriginal;
		
		//Set tri's color
		double avgX = (tri.pts[0].x+tri.pts[1].x+tri.pts[2].x)/3;
    	SDL_SetRenderDrawColor(r, 100+avgX*123, 200+avgX*456, 300+avgX*789, 255);

		//Transform tri
		tri = tri.multiply4d(matRotZ);
		tri = tri.multiply4d(matRotX);
		tri = tri.translate(Vec3d(0, 0, 3));
		tri = tri.multiply4d(matProj);
		tri = tri.translate(Vec3d(10, 10, 0));
		tri = tri.scale(50, 50, 1);

		//Draw the tri
		SDL_RenderDrawLine(r, tri.pts[0].x, tri.pts[0].y, tri.pts[1].x, tri.pts[1].y);
		SDL_RenderDrawLine(r, tri.pts[0].x, tri.pts[0].y, tri.pts[2].x, tri.pts[2].y);
		SDL_RenderDrawLine(r, tri.pts[1].x, tri.pts[1].y, tri.pts[2].x, tri.pts[2].y);
	}
}