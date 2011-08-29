#ifndef PARTICLES_H
#define PARTICLES_H

#include <toadlet/toadlet.h>

class Particles:public ApplicationActivity{
public:
	Particles(Application *application);
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

	virtual void mousePressed(int x,int y,int button){}
	virtual void mouseMoved(int x,int y){}
	virtual void mouseReleased(int x,int y,int button){}
	virtual void mouseScrolled(int x,int y,int scroll){}

	virtual void joyPressed(int button){}
	virtual void joyMoved(scalar x,scalar y,scalar z,scalar r,scalar u,scalar v){}
	virtual void joyReleased(int button){}

	void addSimulatedParticles(ParticleNode::ptr particles);

	Application *app;
	Engine *engine;
	Scene::ptr scene;
	CameraNode::ptr cameraNode;
	ParticleNode::ptr pointNode,spriteNode,beamNode;
	Random random;
	Collection<ParticleNode::ptr> simulatedParticles;
};

#endif
