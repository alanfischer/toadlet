#ifndef BEAMBUILDER_H
#define BEAMBUILDER_H

#include <toadlet/pad/Application.h>
#include <toadlet/tadpole/entity/CameraEntity.h>
#include <toadlet/tadpole/entity/ParticleEntity.h>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::entity;
using namespace toadlet::pad;

class BeamBuilder:public Application{
public:
	BeamBuilder();

	void create(int argc,char **argv);

	void update(int dt);
	void render(Renderer *renderer);

	void mousePressed(int x,int y,int b);
	void mouseMoved(int x,int y);
	void mouseReleased(int x,int y,int b);
	void focusLost();

protected:
	void updateCamera();

	void loadBeam();
	void loadAxe();

 	int mMouseX,mMouseY;
	bool mDrag;
	scalar mDistance;
	Matrix3x3 mRotate;
	bool mZoom;

	CameraEntity::ptr mCamera;
	ParticleEntity::ptr mParticles;
};

#endif
