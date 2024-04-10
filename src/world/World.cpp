#include "World.h"
#include "Matrix4d.h"


World::World(){}

void World::init()
{
	lines.push_back(new Line3d(0, 0, 1, 0, 10, 1));
	lines.push_back(new Line3d(0, 0, 1, 10, 10, 1));
	lines.push_back(new Line3d(0, 10, 1, 10, 10, 1));
}

World::~World(){}

void World::tick()
{
    timer++;
}

void World::draw(SDL_Renderer* r)
{
	Point3d p1(0, 0, -0.9);
	Point3d pT(0, 0, 0.01);
	Matrix4d matP; matP.setToProjMatrix(1000, 1000);
	Matrix4d matX; matX.setToXRotMatrix(0); matX.setToXRotMatrix(timer/40.0);
	Matrix4d matY; matY.setToYRotMatrix(0); matY.setToYRotMatrix(timer/40.0);
	Matrix4d matZ; matZ.setToZRotMatrix(0); matZ.setToZRotMatrix(timer/40.0);

	//Transform and draw all lines
	SDL_SetRenderDrawColor(r, 128, 0, 255, 255);
	for(int i = 0; i<lines.size(); i++) {
		Line3d l = (*lines[i]);

		l.translate(p1).multiply4d(matP).multiply4d(matX).multiply4d(matY).multiply4d(matZ);
		SDL_RenderDrawLine(r, 640+l.p1.x, 480+l.p1.y, 640+l.p2.x, 480+l.p2.y);
	}
}