#ifndef CONVEXTEST_H
#define CONVEXTEST_H 

#include <toadlet/tadpole/plugins/hop/HopEntity.h>
#include <toadlet/tadpole/plugins/hop/HopScene.h>
#include <toadlet/pad/Application.h>

class ConvexTest:public toadlet::pad::Application{
public:
	ConvexTest();
	virtual ~ConvexTest();

	void create();
	void update(int dt);
	void render(toadlet::peeper::Renderer *renderer);
	void mousePressed(int x,int y,int button);
	void resized(int width,int height);

protected:
	void setupTest();

	toadlet::tadpole::HopScene::ptr mScene;
	toadlet::tadpole::entity::CameraEntity::ptr mCamera;
	toadlet::tadpole::entity::ParentEntity::ptr mNode;
};

#endif
