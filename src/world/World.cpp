#include "World.h"
#include <algorithm>
#include <nch/cpp-utils/log.h>
#include <nch/math-utils/mat4x4.h>
#include <nch/math-utils/matrixops.h>
#include <nch/math-utils/vec3.h>
#include <nch/sdl-utils/texture-utils.h>
#include <nch/sdl-utils/input.h>
#include <omp.h>
#include <list>
#include <sstream>
#include "Main.h"
#include "MainLoop.h"

using namespace nch;

World::World(){}

void World::init()
{
	mesh.loadFromObj("res/human.obj");

	int numMaxThreads = omp_get_max_threads();
	Log::log("Max threads: %d", numMaxThreads);
}

World::~World(){}

void World::tick()
{
	if(focused) {
		SDL_ShowCursor(SDL_DISABLE);
		tickMovement();
	} else {
		SDL_ShowCursor(SDL_ENABLE);
	}
	if(Input::keyDownTime(SDLK_ESCAPE)==1) {
		if(focused) {
			focused = false;
		}
		else {
			SDL_WarpMouseInWindow(Main::getWindow(), Main::getWidth()/2, Main::getHeight()/2);
			focused = true;
		}
	}

	if(Input::keyDownTime(SDLK_F12)==1) {
		if(debug) 	{ debug = false; }
		else 		{ debug = true; }
	}

}

void World::tickMovement()
{
	MatrixOps<double> mops;
    timer++;

	double inc = 0.4;
	if(Input::isKeyDown(SDLK_LCTRL)) {
		inc = 0.05;
	}

	//Movement vectors
	Vec3<double> vForward = vCamDir; vForward.y = 0; vForward = vForward.normalized();
	Vec3<double> vRight = mops.multiply4d(vForward, mops.getYRotMatrix(90.*M_PI/180.));

	//Update camera position based on keyboard presses
	if(Input::isKeyDown(SDLK_d)) 		vCamPos += vRight*inc;
	if(Input::isKeyDown(SDLK_a)) 		vCamPos -= vRight*inc;
	if(Input::isKeyDown(SDLK_SPACE)) 	vCamPos.y += inc;
	if(Input::isKeyDown(SDLK_LSHIFT))	vCamPos.y -= inc;
	if(Input::isKeyDown(SDLK_w)) 		vCamPos += vForward*inc;
	if(Input::isKeyDown(SDLK_s)) 		vCamPos -= vForward*inc;

	//Update camera direction based on mouse movements
	int mx = Input::getMouseX();
	int my = Input::getMouseY();
	if(mx!=Main::getWidth()/2 || my!=Main::getHeight()/2) {
		int lmx = mx;
		int lmy = my;
		SDL_WarpMouseInWindow(Main::getWindow(), Main::getWidth()/2, Main::getHeight()/2);
		int dmx = Main::getWidth()/2-lmx;
		int dmy = Main::getHeight()/2-lmy;

		camYaw -= dmx/4.;
		camPch -= dmy/4.;

		while(camYaw<0) camYaw += 360;
		while(camYaw>360) camYaw -= 360;
		if(camPch>179.9) camPch = 179.9;
		if(camPch<0.01) camPch = 0.01;
	}
}


void World::draw(SDL_Renderer* r)
{
	if(true) {
		drawMesh(r);
	}

	if(debug) {
		drawInfo(r);
	}

	if(!focused) {
		SDL_SetRenderTarget(r, NULL);
		SDL_SetRenderDrawColor(r, 0, 0, 0, 192);
		SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
		SDL_Rect rect;
		rect.x = Main::getWidth()/2-84;  rect.w = 84*2;
		rect.y = Main::getHeight()/2-24; rect.h = 32*2;
		SDL_RenderFillRect(r, &rect);

		Main::drawText(r, "Paused", Main::getWidth()/2-36, Main::getHeight()/2);
	}
}

void World::drawMesh(SDL_Renderer* r)
{

	MatrixOps<double> mops;
	typedef Vec3<double> t_vec3d;
	typedef Mat4x4<double> t_mat4d;

	//Rotation matrices
	t_mat4d matRotZ = mops.getZRotMatrix(timer/100.);
	t_mat4d matRotX = mops.getXRotMatrix(timer/200.);
	t_mat4d matTrans = mops.getTranslationMatrix(t_vec3d(0, 0, 16));
	
	//World and camera matrices
	t_mat4d matWorld = mops.getIdentityMatrix();
	matWorld = mops.multiply(matWorld, matTrans);
	
	//Update camera direction and target based on current yaw and pitch
	double the = camPch*M_PI/180.;
	double phi = camYaw*M_PI/180.;
	vCamDir = Vec3<double>(
		std::sin(the)*std::cos(phi),
		std::cos(the),
		std::sin(the)*std::sin(phi)
	);
	t_vec3d vTarget = vCamPos+vCamDir;

	//Camera, view, projection matrices
	t_vec3d vUp(0, 1, 0);
	t_mat4d matCam = mops.getPointAtMatrix(vCamPos, vTarget, vUp);
	t_mat4d matView = mops.invSpecialMatrix(matCam);
	t_mat4d matProj = mops.getProjectionMatrix();

	std::vector<Tri3d> rasteredTris;

	//#pragma omp parallel for num_threads(8)
	for(int i = 0; i<mesh.tris.size(); i++) {
		Tri3d triMod, triViewed, triProj;

		triMod = mesh.tris.at(i);
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
			assignTriColor(triMod, norm);

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

				for(int i = 0; i<3; i++) {
					triProj.p[i][0] = triProj.p[i][0]/triProj.p[i][3];
					triProj.p[i][1] = triProj.p[i][1]/triProj.p[i][3];
					triProj.p[i][2] = triProj.p[i][2]/triProj.p[i][3];
				}

				//Add triangle to final list of rastered tris.
				//#pragma omp critical
				rasteredTris.push_back(triProj);
			}
		}
	}

	//Draw list of rastered tris
	drawRasteredTris(r, rasteredTris, debug);
}

void World::drawRasteredTris(SDL_Renderer* r, std::vector<Tri3d>& rasteredTris, bool drawWireFrame)
{
	typedef Vec3<double> t_vec3d;
	double w = Main::getWidth()-1;
	double h = Main::getHeight()-1;

	//Triangles with a higher avg. Z are put in front (drawn last)
	std::sort(rasteredTris.begin(), rasteredTris.end(), [](Tri3d& t1, Tri3d& t2) {
		double z1 = (t1.p[0][2]+t1.p[1][2]+t1.p[2][2])/3.;	//Avg. Z in points of t1
		double z2 = (t2.p[0][2]+t2.p[1][2]+t2.p[2][2])/3.;	//Avg. Z in points of t2
		return z1>z2;
	});
	
	for(int i = 0; i<rasteredTris.size(); i++) {
		Tri3d rTri = rasteredTris[i];
		//Final transformations into screen
		rTri = rTri.stretch(Vec3<double>(512, 512, 1));
		rTri = rTri.translate(Vec3<double>(Main::getWidth()/2, Main::getHeight()/2, 1));

		//Prepare final clippings against screen boundaries
		Tri3d trisClipped[2];
		std::list<Tri3d> triList;
		triList.push_back(rTri);
		int numNewTris = 1;

		//Perform final clippings
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

			SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
			TexUtils::renderFillTri(r, pts[0], pts[1], pts[2]);

			// Draw triangle wireframe
			if(drawWireFrame) {
				SDL_SetRenderDrawColor(r, 96, 160, 96, 255);
				SDL_RenderDrawLine(r, tri.p[0][0], tri.p[0][1], tri.p[1][0], tri.p[1][1]);
				SDL_RenderDrawLine(r, tri.p[0][0], tri.p[0][1], tri.p[2][0], tri.p[2][1]);
				SDL_RenderDrawLine(r, tri.p[1][0], tri.p[1][1], tri.p[2][0], tri.p[2][1]);
			}
		}
	}


}

void World::drawInfo(SDL_Renderer* r)
{
	std::stringstream ss;
	ss << "(FPS, TPS) = (" << MainLoop::getCurrentFPS() << ", " << MainLoop::getCurrentTPS() << ")";
	Main::drawText(r, ss.str(), 8, 8);
	
	ss.str("");
	ss << "Tris: " << mesh.tris.size();
	Main::drawText(r, ss.str(), 8, 30);
	
	ss.str("");
	ss << "Camera[(xyz), (yaw/pitch)] = [" << vCamPos.toString() << ", (" << camYaw << ", " << camPch << ")]";
	Main::drawText(r, ss.str(), 8, 54);
}

void World::assignTriColor(Tri3d& tri, Vec3<double>& normal)
{
	//Find tri's brightness
	Vec3<double> ld(0, 1, 0);	//Light Direction
	ld = ld.normalized();		//Normalize
	double b = normal.dot(ld);	//Get brightness, store into 'b'.
	
	//Set tri's color based on brightness (dark purple-ish)
	nch::Color c(96, 96, 96);
	c.brighten(30.*b);
	tri.col = c;
}