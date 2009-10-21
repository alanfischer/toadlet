#ifndef BSPOBJECT_H
#define BSPOBJECT_H

#include <toadlet/egg/List.h>
#include <toadlet/peeper/Constants.h>
#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/tadpole/sg/Entity.h>
#include "../../experimental/bsp/Polygon.h"
#include "../../experimental/bsp/OutputData.h"

class BSPObject:public toadlet::tadpole::sg::Entity,public toadlet::tadpole::Renderable{
public:
	TOADLET_ENTITY(BSPObject,toadlet::tadpole::sg::Entity);

	BSPObject();

	void update(float dt);

	void render(toadlet::tadpole::RenderQueue *queue,toadlet::peeper::Frustum *frustum) const;

	void render(toadlet::peeper::Renderer *renderer) const;

	void setup(const toadlet::egg::List<bsp::Polygon> &polygons,const toadlet::egg::Collection<toadlet::egg::math::Vector3> &vertexes);

	void showNextPlane();

	void showPlane(const toadlet::egg::math::Plane &plane);

	void setOutputData(bsp::OutputData *output){mOutputData=output;}

	const toadlet::tadpole::Transform &getTransform() const{return mWorldTransform;}
	const toadlet::tadpole::material::Material *getMaterial() const{return 0;}
	bool isTransparent() const{return false;}
	bool castsShadows() const{return false;}

private:
	class Primitive{
	public:
		toadlet::peeper::Primitive type;
		toadlet::egg::math::Vector3 normal;
		toadlet::peeper::IndexBuffer indexes;
	};

	int mNode;
	toadlet::peeper::VertexBuffer mVertexes;
	toadlet::egg::Collection<Primitive> mPrimitives;
	bsp::OutputData *mOutputData;
	toadlet::peeper::VertexBuffer mPlaneVertexes;
	toadlet::peeper::IndexBuffer mPlaneIndexes;
	toadlet::egg::math::Matrix4x4 mPlaneTransform;
	toadlet::egg::math::AABox mLocalBound;
};

#endif
