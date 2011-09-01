#ifndef MYPLANET_H
#define MYPLANET_H

#include <toadlet/toadlet.h>

class Planet;
class Orbit;

class MyPlanet:public Applet{
public:
	MyPlanet(Application *app);
	virtual ~MyPlanet();

	void create();
	void destroy();
	void resized(int width,int height);
	void render(RenderDevice *device);
	void update(int dt);

	void focusGained(){}
	void focusLost(){}

	void keyPressed(int key){}
	void keyReleased(int key){}

	void mousePressed(int x,int y,int button);
	void mouseMoved(int x,int y);
	void mouseReleased(int x,int y,int button);
	void mouseScrolled(int x,int y,int scroll){}

	void joyPressed(int button){}
	void joyMoved(scalar x,scalar y,scalar z,scalar r,scalar u,scalar v){}
	void joyReleased(int button){}

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

	inline Engine *getEngine(){return mEngine;}

protected:
	void getSolarSystemPoint(Vector3 &result,int x,int y,toadlet::scalar height);
	ParentNode::ptr createSun(toadlet::scalar size);
	Node::ptr createBackground();

	Application *mApp;
	Engine *mEngine;
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
