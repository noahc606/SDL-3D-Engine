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


		for(int j = 0; j<9; j++) {
			double* temp = mesh[i];
			temp[0] += 1;
			temp[3] += 1;
			temp[6] += 1;
			tris.push_back(new Tri(temp));
		}
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
	for(Tri* triOriginal : tris)
	{
		//Create copy of the original tris to transform
		Tri tri = *triOriginal;
		
		//Transform tri
		tri = tri.multiply4d(matRotZ);
		tri = tri.multiply4d(matRotX);
		tri = tri.translate(Point3d(0, 0, 3));
		tri = tri.multiply4d(matProj);
		tri = tri.translate(Point3d(5, 5, 0));
		tri = tri.scale(100, 100, 1);

		//Draw the tri
		SDL_RenderDrawLine(r, tri.points[0].x, tri.points[0].y, tri.points[1].x, tri.points[1].y);
		SDL_RenderDrawLine(r, tri.points[0].x, tri.points[0].y, tri.points[2].x, tri.points[2].y);
		SDL_RenderDrawLine(r, tri.points[1].x, tri.points[1].y, tri.points[2].x, tri.points[2].y);
	}
}