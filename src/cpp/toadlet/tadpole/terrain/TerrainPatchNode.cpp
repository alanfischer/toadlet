/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine 
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 * 
 * All source code for the Toadlet Engine, including 
 * this file, is the sole property of Lightning Toads 
 * Productions, LLC. It has been developed on our own 
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed 
 * without our explicit permission. 
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#include <toadlet/egg/Extents.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/RenderQueue.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/terrain/TerrainPatchNode.h>

#include <toadlet/egg/System.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{
namespace terrain{

TOADLET_NODE_IMPLEMENT(TerrainPatchNode,Categories::TOADLET_TADPOLE_TERRAIN+".TerrainPatchNode");

TerrainPatchNode::TerrainPatchNode():Node(),
	TOADLET_GIB_IMPLEMENT()

	mSize(0),
	//mVertexes,
	//mBlocks,
	//mBlockQueue,
	mBlockQueueSize(0),
	mBlockQueueStart(0),
	mBlockQueueEnd(0),
	mNumBlocksInQueue(0),
	mNumUnprocessedBlocks(0),
	mLastBlockUpdateFrame(0),
	mLastVertexesUpdateFrame(0),

	//mLeftDependent,
	//mTopDependent,
	mEpsilon(0),
	mCellEpsilon(0),

	mTolerance(0),
	mS1(0),mS2(0)
{
}

TerrainPatchNode::~TerrainPatchNode(){
}

Node *TerrainPatchNode::create(Scene *scene){
	super::create(scene);

	mSize=0;
	mVertexes.clear();
	mBlocks.clear();
	mBlockQueue.clear();
	mBlockQueueSize=0;
	mBlockQueueStart=0;
	mBlockQueueEnd=0;
	mNumBlocksInQueue=0;
	mNumUnprocessedBlocks=0;
	mLastBlockUpdateFrame=0;
	mEpsilon=0.03125/16; // Epsilon for tracing against planes
	// Epsilon for checking neighboring cells.  Must be decently large since we need to catch a plane 
	//  that, offset via epsilon at a shallow angle, would be interested outside of the cell 
	mCellEpsilon=0.03125*4;

	mLeftDependent=NULL;
	mTopDependent=NULL;

	mTolerance=0.00001f;
	mS1=Math::HALF;mS2=Math::ONE;

	return this;
}

void TerrainPatchNode::destroy(){
	if(mMaterial!=NULL){
		mMaterial->release();
		mMaterial=NULL;
	} 
	if(mVertexBuffer!=NULL){
		mVertexBuffer->destroy();
		mVertexBuffer=NULL;
	}
	if(mIndexBuffer!=NULL){
		mIndexBuffer->destroy();
		mIndexBuffer=NULL;
	}
	if(mVertexData!=NULL){
		mVertexData->destroy();
		mVertexData=NULL;
	}
	if(mIndexData!=NULL){
		mIndexData->destroy();
		mIndexData=NULL;
	}

	if(mWaterMaterial!=NULL){
		mWaterMaterial->release();
		mWaterMaterial=NULL;
	} 
	if(mWaterVertexBuffer!=NULL){
		mWaterVertexBuffer->destroy();
		mWaterVertexBuffer=NULL;
	}
	if(mWaterIndexBuffer!=NULL){
		mWaterIndexBuffer->destroy();
		mWaterIndexBuffer=NULL;
	}
	if(mWaterVertexData!=NULL){
		mWaterVertexData->destroy();
		mWaterVertexData=NULL;
	}
	if(mWaterIndexData!=NULL){
		mWaterIndexData->destroy();
		mWaterIndexData=NULL;
	}

	super::destroy();
}

bool TerrainPatchNode::setData(scalar *data,int rowPitch,int width,int height,bool water){
	int sizeN=0;
	for(sizeN=0;(1<<(sizeN+1))<=width;sizeN++);

	if(width!=height){
		Error::invalidParameters(Categories::TOADLET_TADPOLE_TERRAIN,"width!=height");
		return false;
	}

	if(width!=(1<<sizeN)){
		Error::invalidParameters(Categories::TOADLET_TADPOLE_TERRAIN,"width & height not a power of 2");
		return false;
	}

	mSize=width;
	int numVertexes=Math::square(mSize+1);
	int numIndexes=Math::square(mSize)*6;
	VertexFormat::ptr vertexFormat=mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD;
	IndexBuffer::IndexFormat indexFormat=IndexBuffer::IndexFormat_UINT16;

	mVertexes.resize(numVertexes);

	// Terrain buffers
	if(mVertexBuffer==NULL || mVertexBuffer->getSize()!=numVertexes){
		if(mVertexBuffer!=NULL){
			mVertexBuffer->destroy();
		}
		mVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_BIT_WRITE,vertexFormat,numVertexes);
		mVertexData=VertexData::ptr(new VertexData(mVertexBuffer));
	}
	if(mIndexBuffer==NULL || mIndexBuffer->getSize()!=numIndexes){
		if(mIndexBuffer!=NULL){
			mIndexBuffer->destroy();
		}
		mIndexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,indexFormat,numIndexes);
		mIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,mIndexBuffer));
	}

	vba.lock(mVertexBuffer,Buffer::Access_BIT_WRITE);
	{
		int i,j;
		// Iterate backwards so verts needed for normals are set
		for(j=mSize-1;j>=0;--j){
			for(i=mSize-1;i>=0;--i){
				int index=indexOf(i,j);
				Vertex *vertex=vertexAt(i,j);

				vertex->height=data[j*rowPitch+i];
				vertex->normal.set(vertex->height-vertexAt(i+1,j)->height,vertex->height-vertexAt(i,j+1)->height,Math::ONE);
				Math::normalize(vertex->normal);

				vba.set3(index,0,i,j,vertex->height);
				vba.set3(index,1,vertex->normal);
				vba.set2(index,2,Math::div(Math::fromInt(i)+Math::HALF,mSize),Math::div(Math::fromInt(j)+Math::HALF,mSize));
			}
		}

		j=mSize;
		for(i=0;i<mSize+1;i++){
			int si=indexOf(i,j);

			vertexAt(i,j)->height=vertexAt(i,j-1)->height;

			vba.set3(si,0,i,j,vertexAt(i,j)->height);
			vba.set3(si,1,0,0,Math::ONE);
			vba.set2(si,2,Math::div(Math::fromInt(i)+Math::HALF,mSize),Math::div(Math::fromInt(j)+Math::HALF,mSize));
		}

		i=mSize;
		for(j=0;j<mSize+1;j++){
			int si=indexOf(i,j);

			vertexAt(i,j)->height=vertexAt(i-1,j)->height;

			vba.set3(si,0,i,j,vertexAt(i,j)->height);
			vba.set3(si,1,0,0,Math::ONE);
			vba.set2(si,2,Math::div(Math::fromInt(i)+Math::HALF,mSize),Math::div(Math::fromInt(j)+Math::HALF,mSize));
		}
	}
	vba.unlock();

	// Build blocks
	mNumBlocks=1<<(2*sizeN);
	mNumBlocks=(int)((mNumBlocks-1)/3) + 1;

	mBlocks.resize(mNumBlocks);
	mBlockQueueSize=mNumBlocks;
	mBlockQueue.resize(mBlockQueueSize);

	mNumBlocksInQueue=0;
	mBlockQueueStart=0;
	mBlockQueueEnd=0;

	mInitialStride=1<<(sizeN-1);
	initBlocks(&mBlocks[0],0,0,0,mInitialStride,true);

	addBlockToBack(0);

	mBound->set(mBlocks[0].mins,mBlocks[0].maxs);

	// Water buffers
	if(water){
		if(mWaterVertexBuffer==NULL || mWaterVertexBuffer->getSize()!=numVertexes){
			if(mWaterVertexBuffer!=NULL){
				mWaterVertexBuffer->destroy();
			}
			mWaterVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_BIT_WRITE,vertexFormat,numVertexes);
			mWaterVertexData=VertexData::ptr(new VertexData(mWaterVertexBuffer));
		}
		if(mWaterIndexBuffer==NULL || mWaterIndexBuffer->getSize()!=numIndexes){
			if(mWaterIndexBuffer!=NULL){
				mWaterIndexBuffer->destroy();
			}
			mWaterIndexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,indexFormat,numIndexes);
			mWaterIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,mWaterIndexBuffer));
		}

		vba.lock(mWaterVertexBuffer,Buffer::Access_BIT_WRITE);
		{
			int i,j;
			// Iterate backwards so verts needed for normals are set
			for(j=mSize;j>=0;--j){
				for(i=mSize;i>=0;--i){
					int index=indexOf(i,j);
					vba.set3(index,0,i,j,0);
					vba.set3(index,1,Math::Z_UNIT_VECTOR3);
					vba.set2(index,2,Math::div(Math::fromInt(i)+Math::HALF,mSize),Math::div(Math::fromInt(j)+Math::HALF,mSize));
				}
			}
		}
		vba.unlock();

		mWaterRenderable=WaterRenderable::ptr(new WaterRenderable(this));
	}

	return true;
}

bool TerrainPatchNode::setMaterial(Material::ptr material){
	if(mMaterial!=NULL){
		mMaterial->release();
	}

	mMaterial=material;

	if(mMaterial!=NULL){
		mMaterial->retain();
	}

	return true;
}

bool TerrainPatchNode::setWaterMaterial(Material::ptr material){
	if(mWaterMaterial!=NULL){
		mWaterMaterial->release();
	}

	mWaterMaterial=material;

	if(mWaterMaterial!=NULL){
		mWaterMaterial->retain();
	}

	return true;
}

bool TerrainPatchNode::stitchToRight(TerrainPatchNode *terrain,bool restitchDependents){
	TerrainPatchNode *lt=this;
	TerrainPatchNode *rt=terrain;
	int y=0;

	if(restitchDependents){
		rt->mLeftDependent=this;

		if(lt->getSize()!=rt->getSize()){
			Error::unknown("lt->getSize()!=rt->getSize()");
			return false;
		}
	}

	vba.lock(lt->mVertexBuffer,Buffer::Access_BIT_WRITE);

	for(y=(restitchDependents?0:mSize-1);y<mSize+1;y++){
		Vertex *lv=lt->vertexAt(mSize,y),*rv=rt->vertexAt(0,y);
		rv->dependent0=lv;
		lv->dependent1=rv;
		lv->height=rv->height;
		lv->normal.set(rv->normal);

		Vertex *llv=lt->vertexAt(mSize-1,y);
		if(y<mSize){
			llv->normal.set(llv->height-lt->vertexAt(mSize,y)->height,llv->height-lt->vertexAt(mSize-1,y+1)->height,Math::ONE);
			Math::normalize(llv->normal);
		}

		vba.set3(indexOf(mSize,y),0,mSize,y,lv->height);
		vba.set3(indexOf(mSize,y),1,lv->normal);
		vba.set3(indexOf(mSize-1,y),1,llv->normal);
	}

	vba.unlock();

	if(restitchDependents){
		updateBlockBoundsRight(&mBlocks[0],0,0,0,mInitialStride);
		if(mTopDependent!=NULL){
			// This fixes corner issues, allowing you to stitch in any order
			mTopDependent->stitchToBottom(this,false);
		}
	}

	return true;
}

void TerrainPatchNode::updateBlockBoundsRight(Block *block,int q,int x,int y,int s){
	int i,j;
	if(s>1){
		updateBlockBoundsRight(block->children[1],4*q+2,x+s,y,s/2);
		updateBlockBoundsRight(block->children[3],4*q+4,x+s,y+s,s/2);

		block->mins.z=block->children[0]->mins.z;
		block->maxs.z=block->children[0]->maxs.z;

		for(i=1;i<4;i++){
			if(block->mins.z>block->children[i]->mins.z)
				block->mins.z=block->children[i]->mins.z;
			if(block->maxs.z<block->children[i]->maxs.z)
				block->maxs.z=block->children[i]->maxs.z;
		}
	}
	else{
		block->maxs.z=block->mins.z=vertexAt(x,y)->height;

		for(i=x;i<=x+2;i++){
			for(j=y;j<=y+2;j++){
				Vertex *vertex=vertexAt(i,j);
				if(vertex->height<block->mins.z)
					block->mins.z=vertex->height;
				if(vertex->height>block->maxs.z)
					block->maxs.z=vertex->height;
			}
		}
	}
}

bool TerrainPatchNode::unstitchFromRight(TerrainPatchNode *terrain){
	TerrainPatchNode *lt=this;
	TerrainPatchNode *rt=terrain;
	int y;

	if(rt->mLeftDependent==NULL){
		return false;
	}
	else if(rt->mLeftDependent!=lt){
		Error::unknown("right is not stitched to this");
		return false;
	}

	for(y=0;y<mSize+1;y++){
		rt->vertexAt(0,y)->dependent0=NULL;
		lt->vertexAt(mSize,y)->dependent1=NULL;
	}

	rt->mLeftDependent=NULL;

	return true;
}

bool TerrainPatchNode::stitchToBottom(TerrainPatchNode *terrain,bool restitchDependents){
	TerrainPatchNode *tt=this;
	TerrainPatchNode *bt=terrain;
	int x;

	if(restitchDependents){
		bt->mTopDependent=this;

		if(tt->getSize()!=bt->getSize()){
			Error::unknown("tt->getSize()!=bt->getSize()");
			return false;
		}
	}

	vba.lock(tt->mVertexBuffer,Buffer::Access_BIT_WRITE);

	for(x=(restitchDependents?0:mSize-1);x<mSize+1;x++){
		Vertex *bv=bt->vertexAt(x,0),*tv=tt->vertexAt(x,mSize);
		bv->dependent1=tv;
		tv->dependent0=bv;
		tv->height=bv->height;
		tv->normal.set(bv->normal);

		Vertex *ttv=tt->vertexAt(x,mSize-1);
		if(x<mSize){
			ttv->normal.set(ttv->height-tt->vertexAt(x,mSize)->height,ttv->height-tt->vertexAt(x+1,mSize-1)->height,Math::ONE);
			Math::normalize(ttv->normal);
		}

		vba.set3(indexOf(x,mSize),0,x,mSize,tv->height);
		vba.set3(indexOf(x,mSize),1,tv->normal);
		vba.set3(indexOf(x,mSize-1),1,ttv->normal);
	}

	vba.unlock();

	if(restitchDependents){
		updateBlockBoundsBottom(&mBlocks[0],0,0,0,mInitialStride);
		if(mLeftDependent!=NULL){
			// This fixes corner issues, allowing you to stitch in any order
			mLeftDependent->stitchToRight(this,false);
		}
	}

	return true;
}

void TerrainPatchNode::updateBlockBoundsBottom(Block *block,int q,int x,int y,int s){
	int i,j;
	if(s>1){
		updateBlockBoundsBottom(block->children[2],4*q+3,x,y+s,s/2);
		updateBlockBoundsBottom(block->children[3],4*q+4,x+s,y+s,s/2);

		block->mins.z=block->children[0]->mins.z;
		block->maxs.z=block->children[0]->maxs.z;

		for(i=1;i<4;i++){
			if(block->mins.z>block->children[i]->mins.z)
				block->mins.z=block->children[i]->mins.z;
			if(block->maxs.z<block->children[i]->maxs.z)
				block->maxs.z=block->children[i]->maxs.z;
		}
	}
	else{
		block->maxs.z=block->mins.z=vertexAt(x,y)->height;

		for(i=x;i<=x+2;i++){
			for(j=y;j<=y+2;j++){
				Vertex *vertex=vertexAt(i,j);
				if(vertex->height<block->mins.z)
					block->mins.z=vertex->height;
				if(vertex->height>block->maxs.z)
					block->maxs.z=vertex->height;
			}
		}
	}
}

bool TerrainPatchNode::unstitchFromBottom(TerrainPatchNode *terrain){
	TerrainPatchNode *tt=this;
	TerrainPatchNode *bt=terrain;
	int x;

	if(bt->mTopDependent==NULL){
		return false;
	}
	else if(bt->mTopDependent!=tt){
		Error::unknown("bottom is not stitched to this");
		return false;
	}

	for(x=0;x<mSize+1;x++){
		bt->vertexAt(x,0)->dependent1=NULL;
		tt->vertexAt(x,mSize)->dependent0=NULL;
	}

	bt->mTopDependent=NULL;

	return true;
}

void TerrainPatchNode::queueRenderables(CameraNode *camera,RenderQueue *queue){
	super::queueRenderables(camera,queue);

	updateBlocks(camera);
	updateVertexes();
	updateIndexBuffers(camera);

	if(mIndexData->getCount()>0){
		#if defined(TOADLET_GCC_INHERITANCE_BUG)
			queue->queueRenderable(&renderable);
		#else
			queue->queueRenderable(this);
		#endif
	}

	if(mWaterRenderable!=NULL){
		updateWaterIndexBuffers(camera);

		if(mWaterIndexData->getCount()>0){
			queue->queueRenderable(mWaterRenderable);
		}
	}
}

void TerrainPatchNode::updateBlocks(CameraNode *camera){
	if(mScene->getFrame()==mLastBlockUpdateFrame){
		return;
	}

	Vector3 cameraTranslate;
	mWorldTransform->inverseTransform(cameraTranslate,camera->getWorldTranslate());

	resetBlocks();
	simplifyBlocks(cameraTranslate);

	mLastBlockUpdateFrame=mScene->getFrame();
}

void TerrainPatchNode::updateIndexBuffers(CameraNode *camera){
	int indexCount=gatherBlocks(mIndexBuffer,camera,false);
	mIndexData->setCount(indexCount);
}

void TerrainPatchNode::updateWaterIndexBuffers(CameraNode *camera){
	int indexCount=gatherBlocks(mWaterIndexBuffer,camera,true);
	mWaterIndexData->setCount(indexCount);
}

void TerrainPatchNode::render(Renderer *renderer) const{
	renderer->renderPrimitive(mVertexData,mIndexData);
}

void TerrainPatchNode::traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	Segment localSegment;
	scalar sizeAdjust=Math::div(size.z,mWorldTransform->getScale().z);
	Transform::inverseTransform(localSegment,segment,position,mWorldTransform->getScale(),mWorldTransform->getRotate());
	localSegment.origin.z-=sizeAdjust;

	result.time=Math::ONE;

	Block *block=&mBlocks[0];
	const Vector3 &mins1=block->mins;
	const Vector3 &maxs1=block->maxs;

	scalar mins2x=Math::minVal(localSegment.origin.x,localSegment.origin.x+localSegment.direction.x);
	scalar mins2y=Math::minVal(localSegment.origin.y,localSegment.origin.y+localSegment.direction.y);
	scalar maxs2x=Math::maxVal(localSegment.origin.x,localSegment.origin.x+localSegment.direction.x);
	scalar maxs2y=Math::maxVal(localSegment.origin.y,localSegment.origin.y+localSegment.direction.y);
	if(!(mins1.x>maxs2x || mins1.y>maxs2y || mins2x>maxs1.x || mins2y>maxs1.y)){
		traceLocalSegment(result,localSegment,mEpsilon,mCellEpsilon);
	}

	result.point.z+=sizeAdjust;
	if(result.time<Math::ONE){
		mWorldTransform->transformNormal(result.normal);
		Transform::transform(result.point,position,mWorldTransform->getScale(),mWorldTransform->getRotate());
	}
}

void TerrainPatchNode::traceLocalSegment(Collision &result,const Segment &segment,scalar epsilon,scalar cellEpsilon){
	scalar x0=segment.origin.x,y0=segment.origin.y;
	scalar x1=segment.origin.x+segment.direction.x,y1=segment.origin.y+segment.direction.y;

	scalar stepX=segment.direction.x>0?Math::ONE:-Math::ONE;
	scalar stepY=segment.direction.y>0?Math::ONE:-Math::ONE;

	scalar fractionX=x0-Math::floor(x0);
	scalar fractionY=y0-Math::floor(y0);
	scalar tMaxX=0;
	scalar tMaxY=0;
	scalar tDeltaX=0;
	scalar tDeltaY=0;
	
	scalar x=x0,y=y0;

	if(segment.direction.x>epsilon){
		tMaxX=Math::div(Math::ONE-fractionX,segment.direction.x);
		tDeltaX=Math::div(stepX,segment.direction.x);
	}
	else if(segment.direction.x<-epsilon){
		tMaxX=Math::div(-fractionX,segment.direction.x);
		tDeltaX=Math::div(stepX,segment.direction.x);
	}
	else{
		tMaxX=9999999;
	}

	if(segment.direction.y>epsilon){
		tMaxY=Math::div(Math::ONE-fractionY,segment.direction.y);
		tDeltaY=Math::div(stepY,segment.direction.y);
	}
	else if(segment.direction.y<-epsilon){
		tMaxY=Math::div(-fractionY,segment.direction.y);
		tDeltaY=Math::div(stepY,segment.direction.y);
	}
	else{
		tMaxY=9999999;
	}

	int count=0;
	do{
		if(count++>mSize){
			break;
		}

		if(x>=0 && y>=0 && x<mSize && y<mSize){
			if(traceCell(result,x,y,segment,epsilon,cellEpsilon)){
				break;
			}
		}
		if(tMaxX<tMaxY){
			tMaxX+=tDeltaX;
			x+=stepX;
		}
		else{
			tMaxY+=tDeltaY;
			y+=stepY;
		}
	}while(((stepX>0 && (int)(x-cellEpsilon)<=(int)x1) || (stepX<0 && (int)(x+cellEpsilon)>=(int)x1)) && ((stepY>0 && (int)(y-cellEpsilon)<=(int)y1) || (stepY<0 && (int)(y+cellEpsilon)>=(int)y1)));
}

bool TerrainPatchNode::traceCell(Collision &result,int x,int y,const Segment &segment,scalar epsilon,scalar cellEpsilon){
	Vertex *vxy=vertexAt(x,y),*vx1y=vertexAt(x+1,y),*vxy1=vertexAt(x,y+1),*vx1y1=vertexAt(x+1,y+1);

	Plane p1,p2;
	bool even=(((x+y)%2)==0);
	if(even){
		p1.set(Vector3(x,y,vxy->height),Vector3(x+1,y,vx1y->height),Vector3(x+1,y+1,vx1y1->height));
		p2.set(Vector3(x,y+1,vxy1->height),Vector3(x,y,vxy->height),Vector3(x+1,y+1,vx1y1->height));
	}
	else{
		p1.set(Vector3(x,y,vxy->height),Vector3(x+1,y,vx1y->height),Vector3(x,y+1,vxy1->height));
		p2.set(Vector3(x+1,y,vx1y->height),Vector3(x+1,y+1,vx1y1->height),Vector3(x,y+1,vxy1->height));
	}

	Vector3 nc;
	Math::cross(nc,p1.normal,p2.normal);
	bool concave=(nc.x>0);

	Math::add(result.point,segment.origin,segment.direction);
	scalar d1o=Math::length(p1,segment.origin),d2o=Math::length(p2,segment.origin);
	scalar d1e=Math::length(p1,result.point),d2e=Math::length(p2,result.point);

	if((concave && (d1o<=0 || d2o<=0)) || (!concave && (d1o<=0 && d2o<=0))){
		// If a trace gets into the terrain, we just try to force it out.
		if(d1o<=0 && (d1o>d2o || d2o>0)){
			result.normal.set(p1.normal);
		}
		else{
			result.normal.set(p2.normal);
		}
		if(Math::dot(segment.direction,result.normal)<0){
			result.point.set(segment.origin);
			result.time=0;
		}
	}
	else{
		result.time=Math::ONE;
		Collision result2;

		if(d1o>0 && d1e<0){
			if(d1o<0){
				result.time=Math::div(d1o+epsilon,d1o-d1e);
			}
			else{
				result.time=Math::div(d1o-epsilon,d1o-d1e);
			}
			result.time=Math::clamp(0,Math::ONE,result.time);
			result.normal.set(p1.normal);
		}

		if(d2o>0 && d2e<0){
			if(d2o<0){
				result2.time=Math::div(d2o+epsilon,d2o-d2e);
			}
			else{
				result2.time=Math::div(d2o-epsilon,d2o-d2e);
			}
			result2.time=Math::clamp(0,Math::ONE,result2.time);
			result2.normal.set(p2.normal);
		}

		Math::madd(result.point,segment.direction,result.time,segment.origin);
		Math::madd(result2.point,segment.direction,result2.time,segment.origin);

		if(concave){
			if(result.time>result2.time){
				result.set(result2);
			}
		}
		else{
			// Use epsilon*2 here since our point is already an epsilon off the plane
			if(result2.time<Math::ONE && Math::length(p1,result2.point)<epsilon*2){
				result.set(result2);
			}
			else if(!(result.time<Math::ONE && Math::length(p2,result.point)<epsilon*2)){
				result.reset();
			}
		}
	}

	if(result.time<Math::ONE){
		if(result.point.x+cellEpsilon<x || result.point.y+cellEpsilon<y || result.point.x-cellEpsilon>x+1 || result.point.y-cellEpsilon>y+1){
			result.reset();
		}
	}

	return result.time<Math::ONE;
}

void TerrainPatchNode::initBlocks(Block *block,int q,int x,int y,int s,bool e){
	int i=0;
	int j=0;
	scalar delta[5];
	
	block->xOrigin=x;
	block->yOrigin=y;
	block->stride=s;
	block->even=e;
	block->delta0=0;
	block->delta1=Extents::MAX_SCALAR;
	block->processed=false;

	delta[0]=(vertexAt(x,y)->height+vertexAt(x+2*s,y)->height)/2-vertexAt(x+s,y)->height;
	delta[0]=Math::square(delta[0]);
	delta[1]=(vertexAt(x+2*s,y)->height+vertexAt(x+2*s,y+2*s)->height)/2-vertexAt(x+2*s,y+s)->height;
	delta[1]=Math::square(delta[1]);
	delta[2]=(vertexAt(x,y+2*s)->height+vertexAt(x+2*s,y+2*s)->height)/2-vertexAt(x+s,y+2*s)->height;
	delta[2]=Math::square(delta[2]);
	delta[3]=(vertexAt(x,y)->height+vertexAt(x,y+2*s)->height)/2-vertexAt(x,y+s)->height;
	delta[3]=Math::square(delta[3]);
	if(block->even){
		delta[4]=(vertexAt(x,y+2*s)->height+vertexAt(x+2*s,y)->height)/2-vertexAt(x+s,y+s)->height;
	}
	else{
		delta[4]=(vertexAt(x,y)->height+vertexAt(x+2*s,y+2*s)->height)/2-vertexAt(x+s,y+s)->height;
	}
	delta[4]=Math::square(delta[4]);

	block->deltaMax=delta[0];
	for(i=1;i<5;i++){
		if(delta[i]>block->deltaMax){
			block->deltaMax=delta[i];
		}
	}

	block->mins.x=x;
	block->maxs.x=(x+s*2);
	block->mins.y=y;
	block->maxs.y=(y+s*2);

	// Dependents set correctly! :)
	vertexAt(x+s,y)->dependent0=vertexAt(x+s,y+s);
	vertexAt(x,y+s)->dependent1=vertexAt(x+s,y+s);
	vertexAt(x+2*s,y+s)->dependent0=vertexAt(x+s,y+s);
	vertexAt(x+s,y+2*s)->dependent1=vertexAt(x+s,y+s);

	if(block->even==true){
		vertexAt(x+s,y+s)->dependent0=vertexAt(x,y+2*s);
		vertexAt(x+s,y+s)->dependent1=vertexAt(x+2*s,y);
	}
	else{
		vertexAt(x+s,y+s)->dependent0=vertexAt(x,y);
		vertexAt(x+s,y+s)->dependent1=vertexAt(x+2*s,y+2*s);
	}

	// Initialization set correctly! :)
	if(s>1){
		block->children[0]=&mBlocks[4*q+1];
		block->children[1]=&mBlocks[4*q+2];
		block->children[2]=&mBlocks[4*q+3];
		block->children[3]=&mBlocks[4*q+4];
		initBlocks(block->children[0],4*q+1,x,y,s/2,true);
		initBlocks(block->children[1],4*q+2,x+s,y,s/2,false);
		initBlocks(block->children[2],4*q+3,x,y+s,s/2,false);
		initBlocks(block->children[3],4*q+4,x+s,y+s,s/2,true);

		block->mins.z=block->children[0]->mins.z;
		block->maxs.z=block->children[0]->maxs.z;

		for(i=1;i<4;i++){
			if(block->mins.z>block->children[i]->mins.z)
				block->mins.z=block->children[i]->mins.z;
			if(block->maxs.z<block->children[i]->maxs.z)
				block->maxs.z=block->children[i]->maxs.z;

			if(block->deltaMax<block->children[i]->deltaMax)
				block->deltaMax=block->children[i]->deltaMax;
		}
	}
	else{
		block->maxs.z=block->mins.z=vertexAt(x,y)->height;

		for(i=x;i<=x+2;i++){
			for(j=y;j<=y+2;j++){
				Vertex *vertex=vertexAt(i,j);
				if(vertex->height<block->mins.z)
					block->mins.z=vertex->height;
				if(vertex->height>block->maxs.z)
					block->maxs.z=vertex->height;
			}
		}
	}
}

void TerrainPatchNode::resetBlocks(){
	int i;
	int x,y,s;

	mNumUnprocessedBlocks=mNumBlocksInQueue;

	for(i=0;i<mNumBlocksInQueue;i++){
		Block *b=getBlockNumber(i);

		x=b->xOrigin;
		y=b->yOrigin;
		s=b->stride;

		b->processed=false;

		disableVertex(vertexAt(x+s,y));
		disableVertex(vertexAt(x,y+s));
		disableVertex(vertexAt(x+s,y+s));
		disableVertex(vertexAt(x+2*s,y+s));
		disableVertex(vertexAt(x+s,y+s*2));

		if(b->even==true){
			disableVertex(vertexAt(x,y+s*2));
			disableVertex(vertexAt(x+2*s,y));
		}
		else{
			disableVertex(vertexAt(x,y));
			disableVertex(vertexAt(x+2*s,y+s*2));
		}
	}
}

void TerrainPatchNode::enableVertex(Vertex *v){
	if(v->enabled==false){
		v->enabled=true;
		if(v->dependent0){
			enableVertex(v->dependent0);
		}
		if(v->dependent1){
			enableVertex(v->dependent1);
		}
	}
}

void TerrainPatchNode::disableVertex(Vertex *v){
	if(v->enabled==true){
		v->enabled=false;
		if(v->dependent0){
			disableVertex(v->dependent0);
		}
		if(v->dependent1){
			disableVertex(v->dependent1);
		}
	}
}

void TerrainPatchNode::simplifyBlocks(const Vector3 &cameraTranslate){
	int blockNum;
	int i;

	while(mNumUnprocessedBlocks>0){
		blockNum=popBlockFromFront();

		if(mBlocks[blockNum].processed==false){
			mNumUnprocessedBlocks--;

			// Is block a first child?
			if(blockNum>0 && ((blockNum-1)%4)==0){

				// Does block have a sibling?
				if(mNumBlocksInQueue>2 && (blockNum-1)>>2==(getBlockNumberInQueue(2)-1)>>2){
					bool replaceParent=true;
					
					for(i=0;i<4;i++){
						if(blockShouldSubdivide(&mBlocks[blockNum+i],cameraTranslate)){
							replaceParent=false;
						}
					}

					if(replaceParent){
						for(i=1;i<4;i++){
							popBlockFromFront();
							
							if(mBlocks[blockNum+i].processed==false){
								mNumUnprocessedBlocks--;
							}
						}

						mBlocks[(blockNum-1)/4].processed=false;

						addBlockToBack((blockNum-1)/4);
						mNumUnprocessedBlocks++;

						continue;
					}
				}
			}

			// Is block interrior node?
			if(mBlocks[blockNum].stride>1){
				// TODO: Put back in optional blockIntersectsCamera test here if on single pipe machine
				bool shouldSubdivide=false;
					
				for(i=0;i<4;i++){
					if(blockShouldSubdivide(mBlocks[blockNum].children[i],cameraTranslate)){
						shouldSubdivide=true;
					}
				}

				if(shouldSubdivide){
					for(i=0;i<4;i++){
						mBlocks[blockNum].children[i]->processed=false;
						addBlockToBack(4*blockNum+i+1);
					}
					continue;
				}
			}

			mBlocks[blockNum].processed=true;
		}

		addBlockToBack(blockNum);
	}
}

#if 0
// A simple distance calculation
bool TerrainPatchNode::blockShouldSubdivide(Block *block,const Vector3 &cameraTranslate){
	Vector3 bo=(block->mins+block->maxs)/2.0f;
	scalar size=(block->maxs.z-block->mins.z);
	scalar distx=Math::abs(bo.x-cameraTranslate.x);
	scalar disty=Math::abs(bo.y-cameraTranslate.y);
	scalar dist=Math::maxVal(distx,disty);

	dist=Math::mul(dist,Math::square(mS1));
	size=Math::mul(size,Math::square(mS2));

	if(Math::mul(dist,mS1)-Math::mul(size,mS2)<=0){
		return true;
	}
	else{
		return false;
	}
}
#else
/// @todo: Work blockIntersectsCamera in here, but that would require a local space camera transform.  Maybe a transform object with some CameraNode modifications to pass in a transform would work?
bool TerrainPatchNode::blockShouldSubdivide(Block *block,const Vector3 &cameraTranslate){
	if(blockVisibleByWater(block,cameraTranslate,false)==false){
		return false;
	}

	computeDelta(block,cameraTranslate,mTolerance);

	if(block->deltaMax>block->delta0){
		return true;
	}

	return false;
}

/// @todo: Document this section some, it's from the geometric tools engine books.
void TerrainPatchNode::computeDelta(Block *block,const Vector3 &cameraTranslate,float tolerance){
	float fHMin;

	if(cameraTranslate.z<block->mins.z){
		fHMin = cameraTranslate.z - block->mins.z;
		fHMin *= fHMin;
	}
	else if(cameraTranslate.z>block->maxs.z){
		fHMin = cameraTranslate.z - block->maxs.z;
		fHMin *= fHMin;
	}
	else{
		fHMin = 0.0;
	}

	// compute hmax = max{|eye.z - bmin.z|,|eye.z - bmax.z|}
	float fHMax = cameraTranslate.z - block->mins.z;
	fHMax *= fHMax;
	float fTmp = cameraTranslate.z - block->maxs.z;
	fTmp *= fTmp;
	if ( fTmp > fHMax )
		fHMax = fTmp;

	// compute rmin and rmax
	float fDxMin = cameraTranslate.x - block->mins.x;
	float fDxMax = cameraTranslate.x - block->maxs.x;
	float fDyMin = cameraTranslate.y - block->mins.y;
	float fDyMax = cameraTranslate.y - block->maxs.y;
	fDxMin *= fDxMin;
	fDxMax *= fDxMax;
	fDyMin *= fDyMin;
	fDyMax *= fDyMax;
	float fRMin = 0.0, fRMax = 0.0;

	if(cameraTranslate.x < block->mins.x){
		fRMin += fDxMin;
		fRMax += fDxMax;
	}
	else if(cameraTranslate.x <= block->maxs.x){
		fRMax += ( fDxMax >= fDxMin ? fDxMax : fDxMin );
	}
	else{
		fRMin += fDxMax;
		fRMax += fDxMin;
	}

	if(cameraTranslate.y < block->mins.y){
		fRMin += fDyMin;
		fRMax += fDyMax;
	}
	else if(cameraTranslate.y <= block->maxs.y){
		fRMax += ( fDyMax >= fDyMin ? fDyMax : fDyMin );
	}
	else{
		fRMin += fDyMax;
		fRMax += fDyMin;
	}

	// compute fmin
	float fDenom = fRMin + fHMax;
	float fFMin =
		(fDenom > 0.0 ? fRMin/(fDenom*fDenom) : 9999999);
	fDenom = fRMax + fHMax;
	fTmp = (fDenom > 0.0 ? fRMax/(fDenom*fDenom) : 9999999);
	if(fTmp < fFMin)
		fFMin = fTmp;

	// compute fmax
	float fFMax;
	if(fRMin >= fHMin){
		fDenom = fRMin + fHMin;
		fFMax = (fDenom > 0.0 ? fRMin/(fDenom*fDenom) : 9999999);
	}
	else if(fRMax <= fHMin){
		fDenom = fRMax + fHMin;
		fFMax = (fDenom > 0.0 ? fRMax/(fDenom*fDenom) : 9999999);
	}
	else{
		fFMax = (fHMin > 0.0 ? 0.25/fHMin : 9999999);
	}

	block->delta0=tolerance/fFMax;
	block->delta1=(fFMin > 0.0 ? tolerance/fFMin : 9999999);
}
#endif

void TerrainPatchNode::updateVertexes(){
	if(mScene->getFrame()==mLastVertexesUpdateFrame){
		return;
	}

	int i;
	Block *b;
	for(i=0;i<mNumBlocksInQueue;i++){
		b=getBlockNumber(i);

		enableVertex(vertexAt(b->xOrigin+b->stride,b->yOrigin));
		enableVertex(vertexAt(b->xOrigin,b->yOrigin+b->stride));
		enableVertex(vertexAt(b->xOrigin+b->stride,b->yOrigin+b->stride));
		enableVertex(vertexAt(b->xOrigin+b->stride*2,b->yOrigin+b->stride));
		enableVertex(vertexAt(b->xOrigin+b->stride,b->yOrigin+b->stride*2));

		if(b->even==true){
			enableVertex(vertexAt(b->xOrigin,b->yOrigin+b->stride*2));
			enableVertex(vertexAt(b->xOrigin+b->stride*2,b->yOrigin));
		}
		else{
			enableVertex(vertexAt(b->xOrigin,b->yOrigin));
			enableVertex(vertexAt(b->xOrigin+b->stride*2,b->yOrigin+b->stride*2));
		}
	}
	
	mLastVertexesUpdateFrame=mScene->getFrame();
}

bool TerrainPatchNode::blockIntersectsCamera(const Block *block,CameraNode *camera,bool water) const{
	AABox box(block->mins,block->maxs);

	if(water){
		box.mins.z=0;
		box.maxs.z=0;
	}

	mWorldTransform->transform(box.mins);
	mWorldTransform->transform(box.maxs);

	return camera->culled(box)==false;
}

bool TerrainPatchNode::blockVisibleByWater(const Block *block,const Vector3 &cameraTranslate,bool water) const{
	scalar waterLevel=0; /// @todo: Make this a class variable, and have it come from the TerrainNodeDataSource
	bool waterOpaque=false; /// @todo: Make this a class variable, and have it come from the TerrainNodeDataSource
	return (water==false && waterOpaque==false) || 
		(cameraTranslate.z>waterLevel && ((water==false && block->maxs.z>waterLevel) || (water==true && block->mins.z<=waterLevel)) ||
		(cameraTranslate.z<waterLevel && block->mins.z<=waterLevel));
}

int TerrainPatchNode::gatherBlocks(IndexBuffer *indexBuffer,CameraNode *camera,bool water) const{
	int indexCount=0;
	const Block *block=NULL;

	IndexBufferAccessor iba(indexBuffer,Buffer::Access_BIT_WRITE);

	int i;
	for(i=0;i<mNumBlocksInQueue;i++){
		block=getBlockNumber(i);

		if(blockIntersectsCamera(block,camera,water) && blockVisibleByWater(block,camera->getWorldTranslate(),water)){
			int x0=block->xOrigin;
			int y0=block->yOrigin;
			int x1=block->xOrigin+block->stride*2;
			int y1=block->yOrigin+block->stride*2;

			if(block->even==true){
				indexCount=gatherTriangle(iba,indexCount,x0,y0,x0,y1,x1,y0);
				indexCount=gatherTriangle(iba,indexCount,x1,y1,x1,y0,x0,y1);
			}
			else{
				indexCount=gatherTriangle(iba,indexCount,x1,y0,x0,y0,x1,y1);
				indexCount=gatherTriangle(iba,indexCount,x0,y1,x1,y1,x0,y0);
			}
		}
	}

	iba.unlock();

	return indexCount;
}

int TerrainPatchNode::gatherTriangle(IndexBufferAccessor &iba,int indexCount,int x0,int y0,int x1,int y1,int x2,int y2) const{
	if(Math::abs(x0-x1)>1 || Math::abs(x2-x1)>1 || Math::abs(y0-y1)>1 || Math::abs(y2-y1)>1){
		int mx,my;

		mx=(x1+x2)/2;
		my=(y1+y2)/2;

		if(vertexAt(mx,my)->enabled){
			indexCount=gatherTriangle(iba,indexCount,mx,my,x0,y0,x1,y1);
			indexCount=gatherTriangle(iba,indexCount,mx,my,x2,y2,x0,y0);
			return indexCount;
		}
	}

	iba.set(indexCount++,indexOf(x0,y0));
	iba.set(indexCount++,indexOf(x2,y2));
	iba.set(indexCount++,indexOf(x1,y1));

	return indexCount;
}

}
}
}
