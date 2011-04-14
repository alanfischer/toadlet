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

	void render(Renderer *renderer);
	void resized(int width,int height);

protected:
	VertexData::ptr triVertexData;
	IndexData::ptr triIndexData;
	RenderStateSet::ptr triRenderStateSet;

	VertexData::ptr quadVertexData;
	IndexData::ptr quadIndexData;
	RenderStateSet::ptr quadRenderStateSet;

	Matrix4x4 projectionMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 modelMatrix;
};

#endif
