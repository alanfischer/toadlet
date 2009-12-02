#ifndef PEEPERTEST_H
#define PEEPERTEST_H

#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/pad/Application.h>

using namespace toadlet::peeper;
using namespace toadlet::pad;

class PeeperTest:public Application{
public:
	PeeperTest();
	virtual ~PeeperTest();

	void create();
	void destroy();

	void render(Renderer *renderer);
	void resized(int width,int height);

protected:
	VertexData::ptr triVertexData;
	IndexData::ptr triIndexData;

	VertexData::ptr quadVertexData;
	IndexData::ptr quadIndexData;

	toadlet::peeper::Matrix4x4 projectionMatrix;
	toadlet::peeper::Matrix4x4 viewMatrix;
	toadlet::peeper::Matrix4x4 modelMatrix;
};

#endif
