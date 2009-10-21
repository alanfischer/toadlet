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

#ifndef TOADLET_UTIL_TERRAIN_TERRAINPATCH_H
#define TOADLET_UTIL_TERRAIN_TERRAINPATCH_H

#include "../../egg/math/Math.h"
#include "../../tadpole/Engine.h"
#include "../../tadpole/RendererListener.h"
#include "TerrainData.h"

namespace toadlet{
namespace util{
namespace terrain{

class TOADLET_API TerrainPatch:public tadpole::RendererListener{
public:
	class Vertex{
	public:
		Vertex(){
			index=0;
			dependent0=0;
			dependent1=0;
			enabled=false;
		}
		
		int index;
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
		float delta0;
		float delta1;
		float deltaMax;
		bool even;
		bool processed;
		egg::math::Vector3 mins;
		egg::math::Vector3 maxs;
		bool visibility;
	};

	typedef unsigned int BlockQueueType;

	TerrainPatch(tadpole::Engine *engine);
	~TerrainPatch();

	void render(peeper::Renderer *renderer) const;

	void load(TerrainData *data);

	void updateBlocks(const egg::math::Vector3 &position,const egg::math::Vector3 &direction);
	void updateIndexBuffers(peeper::Renderer *renderer,peeper::Frustum *frustum);

	float getHeightAt(float x,float y);

	void stitchToRight(TerrainPatch *terrain);
	void unstitchFromRight(TerrainPatch *terrain);
	void stitchToBottom(TerrainPatch *terrain);
	void unstitchFromBottom(TerrainPatch *terrain);
	void setMinTolerance(float minTol){mMinTolerance=minTol;}
	void setMaxTolerance(float maxTol){mMaxTolerance=maxTol;}

	void rendererRegistered(peeper::Renderer *r);
	void rendererUnregistered(peeper::Renderer *r);

	inline egg::math::Vector3 &position(int x,int y){return mVertexBuffer->position(y*mSize+x);}
	inline egg::math::Vector3 &normal(int x,int y){return mVertexBuffer->normal(y*mSize+x);}
	inline egg::math::Vector2 &texCoord(int x,int y){return mVertexBuffer->texCoord2d(y*mSize+x);}
	inline Vertex *vertex(int x,int y){return &mTerrainVertexes[y*mSize+x];}
	inline const Vertex *vertex(int x,int y) const{return &mTerrainVertexes[y*mSize+x];}
	inline char &vertexType(int x,int y){return mTerrainVertexTypes[y*mSize+x];}

	inline void setScale1(float s1){mS1=s1;}
	inline void setScale2(float s2){mS2=s2;}

protected:
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

	class RendererData{
	public:
		peeper::IndexBuffer::Ptr indexBuffer;
		peeper::IndexBuffer::Ptr detailBuffer;
	};

	int getSizeN(){return mSizeN;}
	void initBlocks(Block *block,int q,int x,int y,int s,bool e);
	void resetBlocks();
	void enableVertex(Vertex *v);
	void disableVertex(Vertex *v);
	void simplifyBlocks();
	bool blockShouldSubdivide(Block *block);
	void computeDelta(Block *block);
	void simplifyVertexes();
	bool blockIntersectsFrustum(const Block *block,peeper::Frustum *f) const;
	int gatherBlocks(peeper::Frustum *f,peeper::IndexBuffer *indexes) const;
	int gatherTriangle(peeper::IndexBuffer *indexes,int indexCount,int x0,int y0,int x1,int y1,int x2,int y2) const;

	// BlockQueue methods
	inline int getNumBlocksInQueue(){return mNumBlocksInQueue;}

	inline Block *getBlockNumber(BlockQueueType t){return &mBlocks[getBlockNumberInQueue(t)];}
	inline const Block *getBlockNumber(BlockQueueType t) const{return &mBlocks[getBlockNumberInQueue(t)];}

	inline BlockQueueType getBlockNumberInQueue(int blockNum) const{
		blockNum=blockNum+mBlockQueueStart;
		if(blockNum>=mBlockQueueSize)
			blockNum=blockNum-mBlockQueueSize;
		return mBlockQueue[blockNum];
	}

	inline BlockQueueType popBlockFromFront(){
		int block=mBlockQueue[mBlockQueueStart];
		mBlockQueueStart++;
		if(mBlockQueueStart>=mBlockQueueSize)
			mBlockQueueStart=mBlockQueueStart-mBlockQueueSize;
		mNumBlocksInQueue--;
		return block;
	}

	inline void addBlockToBack(BlockQueueType block){
		mBlockQueue[mBlockQueueEnd]=block;
		mBlockQueueEnd++;
		if(mBlockQueueEnd>=mBlockQueueSize)
			mBlockQueueEnd=mBlockQueueEnd-mBlockQueueSize;
		mNumBlocksInQueue++;
	}

	tadpole::Engine *mEngine;

	egg::Collection<Block> mBlocks;
	egg::Collection<BlockQueueType> mBlockQueue;
	unsigned int mBlockQueueSize;
	unsigned int mBlockQueueStart;
	unsigned int mBlockQueueEnd;
	int mNumBlocksInQueue;
	int mNumUnprocessedBlocks;

	int mSize;
	int mSizeN;
	egg::Collection<Vertex> mTerrainVertexes;
	egg::Collection<char> mTerrainVertexTypes;
	egg::Collection<int> mTerrainVertexIndexes;
	egg::Collection<int> mDetailVertexIndexes;

	egg::math::Vector3 mPosition;
	egg::math::Vector3 mDirection;

	float mMinTolerance;
	float mMaxTolerance;
	float mTolerance;

	peeper::VertexBuffer::Ptr mVertexBuffer;
	egg::Map<peeper::Renderer*,RendererData> mRendererData;
	float mS1,mS2;
};

}
}
}

#endif

