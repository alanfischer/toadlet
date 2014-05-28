#ifndef PARTICLES_H
#define PARTICLES_H

#include <toadlet/toadlet.h>
#include <LFX2.h>

class Particles:public Object,public Applet{
public:
	TOADLET_IOBJECT(Particles);

	Particles(Application *app);
	virtual ~Particles();

	void create();
	void destroy();
	void resized(int width,int height){}
	void render();
	void update(int dt);
	
	void focusGained(){}
	void focusLost(){}

	void keyPressed(int key);
	void keyReleased(int){}

	void mousePressed(int x,int y,int button);
	void mouseMoved(int x,int y){}
	void mouseReleased(int x,int y,int button){}
	void mouseScrolled(int x,int y,int scroll){}

	Application *app;
	Engine::ptr engine;
	Scene::ptr scene;
	Camera::ptr camera;
	Random random;
	ParticleComponent::ptr points,sprites,beams;
	Collection<ParticleComponent::ptr> particles;

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
