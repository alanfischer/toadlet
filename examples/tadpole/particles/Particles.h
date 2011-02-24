#ifndef PARTICLES_H
#define PARTICLES_H

#include <toadlet/toadlet.h>

class Particles:public Application{
public:
	Particles();
	virtual ~Particles();

	void create();
	void resized(int width,int height);
	void render(Renderer *renderer);
	void update(int dt);
	void keyPressed(int key);
	void addSimulatedParticles(ParticleNode::ptr particles);

	Scene::ptr scene;
	CameraNode::ptr cameraNode;
	ParticleNode::ptr pointNode,spriteNode,beamNode;
	Random random;
	Collection<ParticleNode::ptr> simulatedParticles;
};

#endif
