#ifndef MYPLANET_H
#define MYPLANET_H

#include <toadlet/toadlet.h>

class Planet;
class Orbit;

class MyPlanet:public Application{
public:
	MyPlanet();
	virtual ~MyPlanet();

	void create();
	void destroy();
	void resized(int width,int height);
	void render(Renderer *renderer);
	void update(int dt);
	void mousePressed(int x,int y,int button);
	void mouseMoved(int x,int y);
	void mouseReleased(int x,int y,int button);

void keyPressed(int k){if(k=='g'){changeRendererPlugin("gl");}else if(k=='9'){changeRendererPlugin("d3d9");}else if(k=='0'){changeRendererPlugin("d3d10");}}

	void backgroundAnimation();
	void startAnimation();
	void burnupAnimation();
	void createPlanet();
	void focusPlanet();
	void createLand();

	static Texture::ptr createPerlin(Engine *engine,int width,int height,int scale,int seed);
	static Texture::ptr createNebula(Engine *engine,int width,int height,int scale,int seed,float falloff);
	static Texture::ptr createFlare(Engine *engine,int width,int height,int scale,int seed,float falloff);
	static Texture::ptr createPoint(Engine *engine,int width,int height);
	static Mesh::ptr createDisc(Engine *engine,toadlet::scalar size);

protected:
	void getSolarSystemPoint(Vector3 &result,int x,int y,toadlet::scalar height);
	ParentNode::ptr createSun(toadlet::scalar size);
	Node::ptr createBackground();

	Random mRandom;

	int mMode;
	ParentNode::ptr mSun;
	ParticleNode::ptr mPlanetDisk;
	IntrusivePointer<Planet> mPlanet;
	IntrusivePointer<Orbit> mOrbit;

	Scene::ptr mScene;
	ParentNode::ptr mCameraOrbit;
	CameraNode::ptr mCamera;
	AudioNode::ptr mAudio;

	int mMouseX,mMouseY;
	Vector3 mPlanetPoint;
	int mCreatePlanetStartTime;
	int mPlanetDiskFadeOutTime;

	CameraNode::ptr mOverlay;
};

#endif
