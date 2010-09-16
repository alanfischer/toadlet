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

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{
namespace terrain{

scalar epsilon=0.001;

TOADLET_NODE_IMPLEMENT(TerrainPatchNode,Categories::TOADLET_TADPOLE_TERRAIN+".TerrainPatchNode");

TerrainPatchNode::TerrainPatchNode():Node(),
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

	//mLeftDependent,
	//mTopDependent,

	mMinTolerance(0),
	mMaxTolerance(0),
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

	mLeftDependent=NULL;
	mTopDependent=NULL;

	mMinTolerance=0;
	mMaxTolerance=0;//0.0001;
	mTolerance=0;

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

	super::destroy();
}

bool TerrainPatchNode::setData(scalar *data,int rowPitch,int width,int height){
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

	mVertexes.resize(numVertexes);
	mVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,numVertexes);
	mIndexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT_16,numIndexes);

	vba.lock(mVertexBuffer,Buffer::Access_BIT_WRITE);

	int i,j;
	// Iterate backwards so verts needed for normals are set
	for(j=mSize-1;j>=0;--j){
		for(i=mSize-1;i>=0;--i){
			int index=indexOf(i,j);
			Vertex *vertex=vertexAt(i,j);

			vertex->index=index;
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
		int gi=indexOf(i-1,j);
		int si=indexOf(i,j);

		vertexAt(i,j)->height=vertexAt(i-1,j)->height;

		vba.set3(si,0,i,j,vertexAt(i,j)->height);
		vba.set3(si,1,0,0,Math::ONE);
		vba.set2(si,2,Math::div(Math::fromInt(i)+Math::HALF,mSize),Math::div(Math::fromInt(j)+Math::HALF,mSize));
	}

	vba.unlock();

	mVertexData=VertexData::ptr(new VertexData(mVertexBuffer));
	mIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,mIndexBuffer));

	// Build blocks
	int numBlocks=1<<(2*sizeN);
	numBlocks=(int)((numBlocks-1)/3) + 1;

	mBlocks.resize(numBlocks);
	mBlockQueueSize=numBlocks;
	mBlockQueue.resize(mBlockQueueSize);

	mNumBlocksInQueue=0;
	mBlockQueueStart=0;
	mBlockQueueEnd=0;

	int numInitBlocks=1<<(sizeN-1);
	initBlocks(&mBlocks[0],0,0,0,numInitBlocks,true);

	addBlockToBack(0);

	set(mLocalBound,AABox(mBlocks[0].mins,mBlocks[0].maxs));

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

bool TerrainPatchNode::stitchToRight(TerrainPatchNode *terrain,bool restitchDependents){
	TerrainPatchNode *lt=this;
	TerrainPatchNode *rt=terrain;
	int y=0;

	if(lt->getSize()!=rt->getSize()){
		Error::unknown("lt->getSize()!=rt->getSize()");
		return false;
	}

	vba.lock(lt->mVertexBuffer,Buffer::Access_BIT_WRITE);

	for(y=0;y<mSize+1;y++){
		Vertex *lv=lt->vertexAt(mSize,y),*rv=rt->vertexAt(0,y);
		rv->dependent0=lv;
		lv->dependent1=rv;
		lv->height=rv->height;
		lv->normal.set(rv->normal);

		vba.set3(indexOf(mSize,y),0,mSize,y,lv->height);
		vba.set3(indexOf(mSize,y),1,lv->normal);
	}

	vba.unlock();

	// This fixes corner issues, allowing you to stitch in any order
	if(restitchDependents && mTopDependent!=NULL){
		mTopDependent->stitchToBottom(this,false);
	}

	return true;
}

bool TerrainPatchNode::unstitchFromRight(TerrainPatchNode *terrain){
	TerrainPatchNode *lt=this;
	TerrainPatchNode *rt=terrain;
	int y;

	if(rt->mTopDependent!=lt){
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

	bt->mTopDependent=this;

	if(tt->getSize()!=bt->getSize()){
		Error::unknown("tt->getSize()!=bt->getSize()");
		return false;
	}

	vba.lock(tt->mVertexBuffer,Buffer::Access_BIT_WRITE);

	for(x=0;x<mSize+1;x++){
		Vertex *bv=bt->vertexAt(x,0),*tv=tt->vertexAt(x,mSize);
		bv->dependent1=tv;
		tv->dependent0=bv;
		tv->height=bv->height;
		tv->normal.set(bv->normal);

		vba.set3(indexOf(x,mSize),0,x,mSize,tv->height);
		vba.set3(indexOf(x,mSize),1,tv->normal);
	}

	vba.unlock();

	// This fixes corner issues, allowing you to stitch in any order
	if(restitchDependents && mLeftDependent){
		mLeftDependent->stitchToRight(this,false);
	}

	return true;
}

bool TerrainPatchNode::unstitchFromBottom(TerrainPatchNode *terrain){
	TerrainPatchNode *tt=this;
	TerrainPatchNode *bt=terrain;
	int x;

	if(bt->mTopDependent=tt){
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
	updateIndexBuffers(camera);

#if defined(TOADLET_GCC_INHERITANCE_BUG)
	queue->queueRenderable(&renderable);
#else
	queue->queueRenderable(this);
#endif
}

void TerrainPatchNode::updateBlocks(CameraNode *camera){
	if(mScene->getFrame()==mLastBlockUpdateFrame){
		return;
	}

	Vector3 cameraTranslate;
	inverseTransform(cameraTranslate,camera->getWorldTranslate(),mWorldTranslate,mWorldScale,mWorldRotate);

	mTolerance=Math::lerp(mMaxTolerance,mMinTolerance,0.5f);

	resetBlocks();
	simplifyBlocks(cameraTranslate);
	simplifyVertexes();

	mLastBlockUpdateFrame=mScene->getFrame();
}

void TerrainPatchNode::updateIndexBuffers(CameraNode *camera){
	int indexCount=gatherBlocks(mIndexBuffer,camera);
	mIndexData->setCount(indexCount);
}

void TerrainPatchNode::render(Renderer *renderer) const{
	renderer->renderPrimitive(mVertexData,mIndexData);
}

void TerrainPatchNode::traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	Segment localSegment;
	inverseTransform(localSegment,segment,position,mWorldScale,mWorldRotate);

	result.time=Math::ONE;

	Block *block=&mBlocks[0];
	const Vector3 &mins1=block->mins;
	const Vector3 &maxs1=block->maxs;
	scalar mins2x=Math::minVal(localSegment.origin.x,localSegment.origin.x+localSegment.direction.x);
	scalar mins2y=Math::minVal(localSegment.origin.y,localSegment.origin.y+localSegment.direction.y);
	scalar maxs2x=Math::maxVal(localSegment.origin.x,localSegment.origin.x+localSegment.direction.x);
	scalar maxs2y=Math::maxVal(localSegment.origin.y,localSegment.origin.y+localSegment.direction.y);
	if(!(mins1.x>=maxs2x || mins1.y>=maxs2y || mins2x>=maxs1.x || mins2y>=maxs1.y)){
		traceLocalSegment(result,localSegment);
	}

	if(result.time<Math::ONE){
		Math::div(result.normal,mWorldScale);
		Math::mul(result.normal,mWorldRotate);
		Math::normalize(result.normal);
		transform(result.point,result.point,position,mWorldScale,mWorldRotate);
	}
}

void TerrainPatchNode::traceLocalSegment(Collision &result,const Segment &segment){
	scalar t=0;
	scalar x0=segment.origin.x,y0=segment.origin.y,z0=segment.origin.z;
	scalar x1=segment.origin.x+segment.direction.x,y1=segment.origin.y+segment.direction.y,z1=segment.origin.z+segment.direction.z;

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

	do{
		if(x>=0 && y>=0 && x<mSize && y<mSize){
			if(traceCell(result,x,y,segment)){
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
	}while(((stepX>0 && (int)x<=(int)x1) || (stepX<0 && (int)x>=(int)x1)) && ((stepY>0 && (int)y<=(int)y1) || (stepY<0 && (int)y>=(int)y1)));
}

bool TerrainPatchNode::traceCell(Collision &result,int x,int y,const Segment &segment){
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
		//return false;
		if(d1o<=0 && (d1o>d2o || d2o>0)){
			result.normal.set(p1.normal);
		}
		else{
			result.normal.set(p2.normal);
		}
		if(Math::dot(segment.direction,result.normal)<-epsilon){
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
			if(result2.time<Math::ONE && Math::length(p1,result2.point)<0){
				result.set(result2);
			}
			else if(!(result.time<Math::ONE && Math::length(p2,result.point)<0)){
				result.reset();
			}
		}
	}

	if(result.time<Math::ONE){
		if(result.point.x<x || result.point.y<y || result.point.x>x+1 || result.point.y>y+1){
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
		initBlocks(block->children[0],4*q+1,x,y,(int)s/2,true);
		initBlocks(block->children[1],4*q+2,x+s,y,(int)s/2,false);
		initBlocks(block->children[2],4*q+3,x,y+s,(int)s/2,false);
		initBlocks(block->children[3],4*q+4,x+s,y+s,(int)s/2,true);

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
		block->mins.z=vertexAt(x,y)->height;
		block->maxs.z=vertexAt(x,y)->height;

		for(i=x;i<=x+2;i++){
			for(j=y;j<=y+2;j++){
				if(vertexAt(i,j)->height<block->mins.z)
					block->mins.z=vertexAt(i,j)->height;
				if(vertexAt(i,j)->height>block->maxs.z)
					block->maxs.z=vertexAt(i,j)->height;
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

			//Is block a first child?
			if(blockNum>0 && ((blockNum-1)%4)==0){

				//Does block have a sibling?
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
bool TerrainPatchNode::blockShouldSubdivide(Block *block,const Vector3 &cameraTranslate){
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

void TerrainPatchNode::simplifyVertexes(){
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
}

bool TerrainPatchNode::blockIntersectsCamera(const Block *block,CameraNode *camera) const{
	AABox box;

	transform(box.mins,block->mins,mWorldTranslate,mWorldScale,mWorldRotate);
	transform(box.maxs,block->maxs,mWorldTranslate,mWorldScale,mWorldRotate);

	return camera->culled(box)==false;
}

int TerrainPatchNode::gatherBlocks(IndexBuffer *indexBuffer,CameraNode *camera) const{
	int indexCount=0;
	const Block *block=NULL;

	IndexBufferAccessor iba(indexBuffer,Buffer::Access_BIT_WRITE);

	int i;
	for(i=0;i<mNumBlocksInQueue;i++){
		block=getBlockNumber(i);

		if(blockIntersectsCamera(block,camera)){
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
