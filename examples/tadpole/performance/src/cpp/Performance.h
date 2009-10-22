#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <toadlet/pad/Application.h>

class Performance:public toadlet::pad::Application{
public:
	Performance();
	virtual ~Performance();

	void create();
	void update(int dt);
	void render(toadlet::peeper::Renderer *renderer);
	void mousePressed(int x,int y,int button);
	void resized(int width,int height);

protected:
	bool setupTest(int test,int intensity);
	toadlet::tadpole::entity::Entity::ptr setupMinimumTest();
	toadlet::tadpole::entity::Entity::ptr setupFillrateTest();
	toadlet::tadpole::entity::Entity::ptr setupVertexrateTest();
	toadlet::tadpole::entity::Entity::ptr setupDynamicTest();

	toadlet::tadpole::entity::Scene::ptr mScene;
	toadlet::tadpole::entity::CameraEntity::ptr mCamera;
	toadlet::tadpole::entity::ParentEntity::ptr mNode;
	int mTest;
};

#endif
