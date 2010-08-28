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
#include <toadlet/tadpole/terrain/TerrainPatchNode.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{
namespace terrain{

TOADLET_NODE_IMPLEMENT(TerrainPatchNode,Categories::TOADLET_TADPOLE_TERRAIN+".TerrainPatchNode");

TerrainPatchNode::TerrainPatchNode():Node(),
	mMinTolerance(0),
	mMaxTolerance(0),

	mS1(0),
	mS2(0),

	mSize(0)
{
}

TerrainPatchNode::~TerrainPatchNode(){
}

Node *TerrainPatchNode::create(Scene *scene){
	super::create(scene);

	mMinTolerance=0;
	mMaxTolerance=1;

	mS1=0.5;
	mS2=1;

	mSize=0;

	return this;
}

void TerrainPatchNode::destroy(){
	if(mMaterial!=NULL){
		mMaterial->release();
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

	Vector3 position,normal;
	int i,j;
	for(j=0;j<mSize;++j){
		for(i=0;i<mSize;++i){
			int index=indexOf(i,j);
			Vertex *vertex=vertexAt(i,j);

			scalar height=data[j*rowPitch+i];
			vertex->index=index;
			vertex->height=height;

			vba.set3(index,0,i,j,height);

			if(i>0 && j>0){
				scalar heightl=vertexAt(i-1,j)->height;
				scalar heightu=vertexAt(i,j-1)->height;

				// Result of cross(height(i-1,j)-height(i,j),height(i,j-1)-height(i,j))
				normal.set(heightl-height,heightu-height,Math::ONE);
				Math::normalize(normal);
				vba.set3(index,1,normal);
			}

			vba.set2(index,2,Math::div(Math::fromInt(i)+Math::HALF,mSize+1),Math::div(Math::fromInt(j)+Math::HALF,mSize+1));
		}
	}

	j=mSize;
	for(i=0;i<mSize+1;i++){
		int gi=indexOf(i,j-1);
		int si=indexOf(i,j);

		vertexAt(i,j)->height=vertexAt(i,j-1)->height;

		vba.set3(si,0,i,j,vertexAt(i,j)->height);

		vba.get3(gi,1,normal);
		vba.set3(si,1,normal);

		vba.set2(si,2,Math::div(Math::fromInt(i)+Math::HALF,mSize+1),Math::div(Math::fromInt(j)+Math::HALF,mSize+1));
	}

	i=mSize;
	for(j=0;j<mSize+1;j++){
		int gi=indexOf(i-1,j);
		int si=indexOf(i,j);

		vertexAt(i,j)->height=vertexAt(i-1,j)->height;

		vba.set3(si,0,i,j,vertexAt(i,j)->height);

		vba.get3(gi,1,normal);
		vba.set3(si,1,normal);

		vba.set2(si,2,Math::div(Math::fromInt(i)+Math::HALF,mSize+1),Math::div(Math::fromInt(j)+Math::HALF,mSize+1));
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

bool TerrainPatchNode::stitchToRight(TerrainPatchNode *terrain){
	TerrainPatchNode *tLeft=this;
	TerrainPatchNode *tRight=terrain;
	int y=0;

	if(tLeft->getSize()!=tRight->getSize()){
		Error::unknown("left->size!=right->size");
		return false;
	}

	for(y=0;y<mSize+1;y++){
		tRight->vertexAt(0,y)->dependent0=tLeft->vertexAt(mSize,y);
		tLeft->vertexAt(mSize,y)->dependent1=tRight->vertexAt(0,y);
	}

	vba.lock(tRight->mVertexBuffer,Buffer::Access_BIT_WRITE);

	for(y=0;y<mSize+1;y++){
//		tRight->normal(0,y)=tLeft->normal(mSize,x);
		vba.set3(indexOf(0,y),0,0,y,tLeft->vertexAt(mSize,y)->height);
	}

	vba.unlock();

	return true;
}

bool TerrainPatchNode::unstitchFromRight(TerrainPatchNode *terrain){
	TerrainPatchNode *tLeft=this;
	TerrainPatchNode *tRight=terrain;
	int y;

	if(tLeft->getSize()!=tRight->getSize()){
		Error::unknown("left->size!=right->size");
		return false;
	}

	for(y=0;y<mSize+1;y++){
		tRight->vertexAt(0,y)->dependent0=NULL;
		tLeft->vertexAt(mSize,y)->dependent1=NULL;
	}

	return true;
}

bool TerrainPatchNode::stitchToBottom(TerrainPatchNode *terrain){
	TerrainPatchNode *tTop=this;
	TerrainPatchNode *tBottom=terrain;
	int x;

	if(tTop->getSize()!=tBottom->getSize()){
		Error::unknown("top->size!=bottom->size");
		return false;
	}

	for(x=0;x<mSize+1;x++){
		tBottom->vertexAt(x,mSize)->dependent0=tTop->vertexAt(x,0);
		tTop->vertexAt(x,0)->dependent1=tBottom->vertexAt(x,mSize);
	}

	vba.lock(tBottom->mVertexBuffer,Buffer::Access_BIT_WRITE);

	for(x=0;x<mSize+1;x++){
//		tBottom->normal(0,y)=tTop->normal(mSize,x);
		vba.set3(indexOf(x,mSize),0,x,mSize,tTop->vertexAt(x,0)->height);
	}

	vba.unlock();

	return true;
}

bool TerrainPatchNode::unstitchFromBottom(TerrainPatchNode *terrain){
	TerrainPatchNode *tTop=this;
	TerrainPatchNode *tBottom=terrain;
	int x;

	if(tTop->getSize()!=tBottom->getSize()){
		Error::unknown("top->size!=bottom->size");
		return false;
	}

	for(x=0;x<mSize+1;x++){
		tTop->vertexAt(x,mSize)->dependent0=NULL;
		tBottom->vertexAt(x,0)->dependent1=NULL;
	}

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
	Vector3 cameraTranslate;
	inverseTransform(cameraTranslate,camera->getWorldTranslate(),mWorldTranslate,mWorldScale,mWorldRotate);

	mTolerance=Math::lerp(mMaxTolerance,mMinTolerance,0.5f);

	resetBlocks();
	simplifyBlocks(cameraTranslate);
	simplifyVertexes();
}

void TerrainPatchNode::updateIndexBuffers(CameraNode *camera){
	int indexCount=gatherBlocks(mIndexBuffer,camera);
	mIndexData->setCount(indexCount);
}

void TerrainPatchNode::render(Renderer *renderer) const{
	renderer->renderPrimitive(mVertexData,mIndexData);
}

void TerrainPatchNode::traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	result.time=Math::ONE;
	return; // dont do a thing
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

#if 1
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
