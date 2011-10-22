#ifndef PARTICLES_H
#define PARTICLES_H

#include <toadlet/toadlet.h>
#include <LFX2.h>

class Particles:public Applet{
public:
	Particles(Application *app);
	virtual ~Particles();

	void create();
	void destroy();
	void resized(int width,int height);
	void render(RenderDevice *device);
	void update(int dt);
	
	void focusGained(){}
	void focusLost(){}

	void keyPressed(int key);
	void keyReleased(int){}

	void mousePressed(int x,int y,int button);
	void mouseMoved(int x,int y){}
	void mouseReleased(int x,int y,int button){}
	void mouseScrolled(int x,int y,int scroll){}

	void joyPressed(int button){}
	void joyMoved(scalar x,scalar y,scalar z,scalar r,scalar u,scalar v){}
	void joyReleased(int button){}

	Application *app;
	Engine *engine;
	Scene::ptr scene;
	CameraNode::ptr cameraNode;
	ParticleNode::ptr pointNode,spriteNode,beamNode;
	Random random;
	Collection<ParticleNode::ptr> particles;

	#if defined(LFX_DLL_NAME)
		int lfxTime;
		DynamicLibrary lfxLibrary;
		LFX2INITIALIZE lfxInitialize;
		LFX2RELEASE lfxRelease;
		LFX2RESET lfxReset;
		LFX2UPDATE lfxUpdate;
		LFX2LIGHT lfxLight;
	#endif
};

#endif
