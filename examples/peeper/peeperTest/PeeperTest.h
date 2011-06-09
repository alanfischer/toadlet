#ifndef PEEPERTEST_H
#define PEEPERTEST_H

#include <toadlet/peeper.h>
#include <toadlet/pad.h>

using namespace toadlet::peeper;
using namespace toadlet::pad;

class PeeperTest:public Application{
public:
	PeeperTest();
	virtual ~PeeperTest();

	void create();
	void destroy();

	void render(RenderDevice *device);
	void resized(int width,int height);

protected:
	VertexData::ptr triVertexData;
	IndexData::ptr triIndexData;
	RenderState::ptr triRenderState;

	VertexData::ptr quadVertexData;
	IndexData::ptr quadIndexData;
	RenderState::ptr quadRenderState;

	Matrix4x4 projectionMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 modelMatrix;
};

#endif
