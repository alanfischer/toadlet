#include "BSPObject.h"

#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/LightEffect.h>
#include <toadlet/peeper/BlendFunction.h>
#include <toadlet/tadpole/sg/NodeEntity.h>

using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace bsp;

BSPObject::BSPObject():Super(){
	mNode=-1;
	
	mPlaneIndexes.resize(8);
	mPlaneIndexes.index(0)=0;
	mPlaneIndexes.index(1)=1;
	mPlaneIndexes.index(2)=2;
	mPlaneIndexes.index(3)=3;

	mPlaneVertexes.resize(VertexBuffer::VT_POSITION,4);
	mPlaneVertexes.position(0)=Vector3(0,-100,-100);
	mPlaneVertexes.position(1)=Vector3(0,100,-100);
	mPlaneVertexes.position(2)=Vector3(0,100,100);
	mPlaneVertexes.position(3)=Vector3(0,-100,100);
}

void BSPObject::update(float dt){
	Super::update(dt);

	setWorldBound(mLocalBound+getWorldTranslate());
}

void BSPObject::render(RenderQueue *queue,Frustum *frustum) const{
	queue->addRenderable(this);
}

void BSPObject::render(Renderer *renderer) const{
	renderer->setFillType(FT_SOLID);
	renderer->setLighting(false);

	int i;
	for(i=0;i<mPrimitives.size();++i){
		renderer->renderPrimitive(mPrimitives[i].type,&mVertexes,&mPrimitives[i].indexes);
	}

	renderer->setDepthFunction(DF_NONE);
	renderer->setColor(Vector4(1,0,0,1));
	LightEffect le;
	le.trackColor=true;
	renderer->setLightEffect(le);
	renderer->setFillType(FT_LINE);

	for(i=0;i<mPrimitives.size();++i){
		renderer->renderPrimitive(mPrimitives[i].type,&mVertexes,&mPrimitives[i].indexes);
	}

	if(mNode!=-1){
		renderer->setFillType(FT_SOLID);
		renderer->setColor(Vector4(0,0.8f,0.8f,0.9f));
		renderer->setBlendFunction(BC_ALPHA);
		renderer->setFaceCulling(FC_NONE);

		renderer->setModelMatrix(mPlaneTransform);
		renderer->renderPrimitive(PRIMITIVE_TRIFAN,&mPlaneVertexes,&mPlaneIndexes);
	}
}

void BSPObject::setup(const List<bsp::Polygon> &polygons,const Collection<Vector3> &vertexes){
	int i,j;

	AABox bound;

	mVertexes.resize(VertexBuffer::VT_POSITION,vertexes.size());
	for(i=0;i<vertexes.size();++i){
		mVertexes.position(i)=vertexes[i];

		if(bound.mins.x<vertexes[i].x){
			bound.mins.x=vertexes[i].x;
		}
		if(bound.mins.y<vertexes[i].y){
			bound.mins.y=vertexes[i].y;
		}
		if(bound.mins.z<vertexes[i].z){
			bound.mins.z=vertexes[i].z;
		}
		if(bound.maxs.x>vertexes[i].x){
			bound.maxs.x=vertexes[i].x;
		}
		if(bound.maxs.y>vertexes[i].y){
			bound.maxs.y=vertexes[i].y;
		}
		if(bound.maxs.z>vertexes[i].z){
			bound.maxs.z=vertexes[i].z;
		}
	}

	mLocalBound=bound;

	mPrimitives.resize(polygons.size());
	List<bsp::Polygon>::const_iterator it;
	for(i=0,it=polygons.begin();it!=polygons.end();++it,++i){
		mPrimitives[i].normal=it->plane.normal;
		mPrimitives[i].indexes.resize(it->indexes.size());
		if(mPrimitives[i].indexes.getNumIndexes()==3){
			mPrimitives[i].type=PRIMITIVE_TRIS;
		}
		else if(mPrimitives[i].indexes.getNumIndexes()==4){
			mPrimitives[i].type=PRIMITIVE_TRIFAN;
		}

		for(j=0;j<it->indexes.size();j++){
			mPrimitives[i].indexes.index(j)=it->indexes[j];
		}
	}
}

void BSPObject::showNextPlane(){
	mNode++;
	Plane plane=mOutputData->nodes[mNode].plane;
	showPlane(plane);
}

void BSPObject::showPlane(const Plane &plane){
	Vector3 trans=plane.normal*plane.d;

	Vector3 right=Vector3(0.2,0.467,0.235);
	normalize(right);

	Vector3 out=plane.normal;
	Vector3 up=cross(out,right);
	normalize(up);
	right=cross(out,up);
	normalize(right);

	Matrix3x3 rot=makeMatrix3x3FromAxis(out,right,up);

	setMatrix4x4(mPlaneTransform,rot,trans);
}

