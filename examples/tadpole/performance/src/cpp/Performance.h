#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <toadlet/tadpole/MeshManager.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/pad/Application.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::node;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::pad;

class Performance:public Application{
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
	Node::ptr setupMinimumTest();
	Node::ptr setupFillrateTest();
	Node::ptr setupVertexrateTest();
	Node::ptr setupDynamicTest();

	Scene::ptr scene;
	CameraNode::ptr cameraNode;
	ParentNode::ptr parentNode;
	int test;
};

#endif
