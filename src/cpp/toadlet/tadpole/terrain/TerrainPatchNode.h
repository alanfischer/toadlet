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

#ifndef TOADLET_TADPOLE_TERRAIN_TERRAINPATCH_H
#define TOADLET_TADPOLE_TERRAIN_TERRAINPATCH_H

#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Traceable.h>
#include <toadlet/tadpole/node/Node.h>

namespace toadlet{
namespace tadpole{
namespace terrain{

class TOADLET_API TerrainPatchNode:public node::Node,public Renderable,public Traceable{
public:
	TOADLET_NODE(TerrainPatchNode,node::Node);

	class Vertex{
	public:
		Vertex():
			height(0),
			dependent0(NULL),
			dependent1(NULL),
			enabled(false)
		{}
		
		Vector3 normal;
		scalar height;
		Vertex *dependent0;
		Vertex *dependent1;
		bool enabled;
	};

	class Block{
	public:
		Block(){
			children[0]=0;
			children[1]=0;
			children[2]=0;
			children[3]=0;
			visibility=false;
		}

		Block *children[4];
		
		int xOrigin;
		int yOrigin;
		int stride;
		scalar delta0;
		scalar delta1;
		scalar deltaMax;
		bool even;
		bool processed;
		Vector3 mins;
		Vector3 maxs;
		bool visibility;
	};

	TerrainPatchNode();
	~TerrainPatchNode();

	node::Node *create(Scene *scene);
	void destroy();

	bool setData(scalar *data,int rowPitch,int width,int height,bool water);
	bool setMaterial(Material::ptr material);
	bool setWaterMaterial(Material::ptr material);

	int getSize() const{return mSize;}
	inline Vertex *vertexAt(int x,int y){return &mVertexes[y*(mSize+1)+x];}
	inline const Vertex *vertexAt(int x,int y) const{return &mVertexes[y*(mSize+1)+x];}
	inline int indexOf(int x,int y) const{return y*(mSize+1)+x;}

	bool stitchToRight(TerrainPatchNode *terrain,bool restitchDependents=true);
	void updateBlockBoundsRight(Block *block,int q,int x,int y,int s);
	bool unstitchFromRight(TerrainPatchNode *terrain);
	bool stitchToBottom(TerrainPatchNode *terrain,bool restitchDependents=true);
	void updateBlockBoundsBottom(Block *block,int q,int x,int y,int s);
	bool unstitchFromBottom(TerrainPatchNode *terrain);
	void setTolerance(scalar tolerance){mTolerance=tolerance;}
	void setScale1(float s1){mS1=s1;}
	void setScale2(float s2){mS2=s2;}

	void queueRenderables(node::CameraNode *camera,RenderQueue *queue);
	void updateBlocks(node::CameraNode *camera);
	void updateVertexes();
	void updateIndexBuffers(node::CameraNode *camera);
	void updateWaterIndexBuffers(node::CameraNode *camera);
	Material *getRenderMaterial() const{return mMaterial;}
	Transform *getRenderTransform() const{return super::getWorldTransform();}
	void render(peeper::Renderer *renderer) const;

	Bound *getBound() const{return super::getBound();}
	void traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);
	void traceLocalSegment(Collision &result,const Segment &segment,scalar epsilon,scalar cellEpsilon);
	bool traceCell(Collision &result,int x,int y,const Segment &segment,scalar epsilon,scalar cellEpsilon);

	inline Renderable *internal_getWaterRenderable(){return mWaterRenderable;}

	peeper::VertexBufferAccessor vba;

protected:
	TOADLET_GIB_DEFINE(TerrainPatchNode);

	/// @todo: Could be replaced by a MeshNode for the Water & the Terrain
	class WaterRenderable:public Renderable{
	public:
		TOADLET_SHARED_POINTERS(WaterRenderable);

		WaterRenderable(TerrainPatchNode *terrain){mTerrain=terrain;}

		Material *getRenderMaterial() const{return mTerrain->mWaterMaterial;}
		Transform *getRenderTransform() const{return mTerrain->getWorldTransform();}
		void render(peeper::Renderer *renderer) const{renderer->renderPrimitive(mTerrain->mWaterVertexData,mTerrain->mWaterIndexData);}

	protected:
		TerrainPatchNode *mTerrain;
	};

	/*
		Block vertexes ordered as follows:
		
		Odd:
		T0-0,T1-1   T1-0
		   **********
		   **       *
		   *  *     *
		   *    *   *
		   *      * *
		   **********
		T0-1        T0-2,T1-2
		
		Even:
		T0-0        T1-0,T0-2
		   **********
		   *       **
		   *     *  *
		   *   *    *
		   * *      *
		   **********
		T0-1,T1-1   T1-2
	*/

	void initBlocks(Block *block,int q,int x,int y,int s,bool e);
	void resetBlocks();
	void enableVertex(Vertex *v);
	void disableVertex(Vertex *v);
	void simplifyBlocks(const Vector3 &cameraTranslate);
	bool blockShouldSubdivide(Block *block,const Vector3 &cameraTranslate);
	void computeDelta(Block *block,const Vector3 &cameraTranslate,float tolerance);
	bool blockIntersectsCamera(const Block *block,node::CameraNode *camera,bool water) const;
	bool blockVisibleByWater(const Block *block,const Vector3 &cameraTranslate,bool water) const;
	int gatherBlocks(peeper::IndexBuffer *indexBuffer,node::CameraNode *camera,bool water) const;
	int gatherTriangle(peeper::IndexBufferAccessor &iba,int indexCount,int x0,int y0,int x1,int y1,int x2,int y2) const;

	// BlockQueue methods
	inline int getNumBlocksInQueue(){return mNumBlocksInQueue;}

	inline Block *getBlockNumber(int t){return &mBlocks[getBlockNumberInQueue(t)];}
	inline const Block *getBlockNumber(int t) const{return &mBlocks[getBlockNumberInQueue(t)];}

	inline int getBlockNumberInQueue(int blockNum) const{
		blockNum=blockNum+mBlockQueueStart;
		if(blockNum>=mBlockQueueSize)
			blockNum=blockNum-mBlockQueueSize;
		return mBlockQueue[blockNum];
	}

	inline int popBlockFromFront(){
		int block=mBlockQueue[mBlockQueueStart];
		mBlockQueueStart++;
		if(mBlockQueueStart>=mBlockQueueSize)
			mBlockQueueStart=mBlockQueueStart-mBlockQueueSize;
		mNumBlocksInQueue--;
		return block;
	}

	inline void addBlockToBack(int block){
		mBlockQueue[mBlockQueueEnd]=block;
		mBlockQueueEnd++;
		if(mBlockQueueEnd>=mBlockQueueSize)
			mBlockQueueEnd=mBlockQueueEnd-mBlockQueueSize;
		mNumBlocksInQueue++;
	}

	int mSize;
	egg::Collection<Vertex> mVertexes;
	egg::Collection<Block> mBlocks;
	egg::Collection<int> mBlockQueue;
	int mNumBlocks;
	int mInitialStride;
	int mBlockQueueSize;
	int mBlockQueueStart;
	int mBlockQueueEnd;
	int mNumBlocksInQueue;
	int mNumUnprocessedBlocks;
	int mLastBlockUpdateFrame;
	int mLastVertexesUpdateFrame;
	scalar mEpsilon;
	scalar mCellEpsilon;

	TerrainPatchNode::ptr mLeftDependent;
	TerrainPatchNode::ptr mTopDependent;

	scalar mTolerance;
	scalar mS1,mS2;

	Material::ptr mMaterial;
	peeper::VertexBuffer::ptr mVertexBuffer;
	peeper::IndexBuffer::ptr mIndexBuffer;
	peeper::VertexData::ptr mVertexData;
	peeper::IndexData::ptr mIndexData;

	Material::ptr mWaterMaterial;
	peeper::VertexBuffer::ptr mWaterVertexBuffer;
	peeper::IndexBuffer::ptr mWaterIndexBuffer;
	peeper::VertexData::ptr mWaterVertexData;
	peeper::IndexData::ptr mWaterIndexData;
	WaterRenderable::ptr mWaterRenderable;
	friend class WaterRenderable;
};

}
}
}

#endif

