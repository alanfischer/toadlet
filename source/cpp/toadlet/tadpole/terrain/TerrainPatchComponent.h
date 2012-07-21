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

#ifndef TOADLET_TADPOLE_TERRAIN_TERRAINPATCHCOMPONENT_H
#define TOADLET_TADPOLE_TERRAIN_TERRAINPATCHCOMPONENT_H

#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Traceable.h>
#include <toadlet/tadpole/Visible.h>
#include <toadlet/tadpole/RenderManager.h>
#include <toadlet/tadpole/BaseComponent.h>

namespace toadlet{
namespace tadpole{
namespace terrain{

class TOADLET_API TerrainPatchComponent:public BaseComponent,public Visible,Renderable,public Traceable{
public:
	TOADLET_OBJECT(TerrainPatchComponent);

	class Vertex{
	public:
		Vertex():
			height(0),
			layer(0),
			dependent0(NULL),
			dependent1(NULL),
			enabled(false)
		{}
		
		Vector3 normal;
		scalar height;
		int layer;
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

	TerrainPatchComponent(Scene *scene);
	virtual ~TerrainPatchComponent();
	void destroy();

	bool parentChanged(Node *node);

	bool setHeightData(scalar *data,int rowPitch,int width,int height,bool water);
	bool setLayerData(tbyte *data,int rowPitch,int width,int height);
	bool setMaterial(Material *material);
	bool setWaterMaterial(Material *material);

	int getSize() const{return mSize;}
	inline Vertex *vertexAt(int x,int y){return &mVertexes[y*(mSize+1)+x];}
	inline const Vertex *vertexAt(int x,int y) const{return &mVertexes[y*(mSize+1)+x];}
	inline int indexOf(int x,int y) const{return y*(mSize+1)+x;}

	bool stitchToRight(TerrainPatchComponent *terrain,bool restitchDependents=true);
	void updateBlockBoundsRight(Block *block,int q,int x,int y,int s);
	bool unstitchFromRight(TerrainPatchComponent *terrain);
	bool stitchToBottom(TerrainPatchComponent *terrain,bool restitchDependents=true);
	void updateBlockBoundsBottom(Block *block,int q,int x,int y,int s);
	bool unstitchFromBottom(TerrainPatchComponent *terrain);

	void setCameraUpdateScope(int scope){mCameraUpdateScope=scope;}
	void setTerrainScope(int scope){mTerrainScope=scope;}
	void setWaterScope(int scope){mWaterScope=scope;}
	void setTolerance(scalar tolerance){mTolerance=tolerance;}
	void setScale1(float s1){mS1=s1;}
	void setScale2(float s2){mS2=s2;}

	void updateBlocks(Camera *camera);
	void updateVertexes();
	void updateIndexBuffers(Camera *camera);
	void updateWaterIndexBuffers(Camera *camera);
	
	int getNumLayers(){return mLayerTextures.size();}
	Texture::ptr getLayerTexture(int i){return mLayerTextures[i];}

	Bound *getBound() const{return mBound;}

	// Visible
	bool getRendered() const{return true;}
	void setRendered(bool rendered){}
	RenderState::ptr getSharedRenderState(){return NULL;}
	void gatherRenderables(Camera *camera,RenderableSet *set);

	// Renderable
	Material *getRenderMaterial() const{return mMaterial;}
	const Transform &getRenderTransform() const{return mParent->getWorldTransform();}
	Bound *getRenderBound() const{return mParent->getWorldBound();}
	void render(RenderManager *manager) const;

	// Traceable
	Bound *getTraceableBound() const{return mBound;}
	void traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);
	void traceLocalSegment(Collision &result,const Segment &segment,scalar epsilon,scalar cellEpsilon);
	bool traceCell(Collision &result,int x,int y,const Segment &segment,scalar epsilon,scalar cellEpsilon);

protected:
	/// @todo: Could be replaced by a Mesh for the Water & the Terrain
	class WaterRenderable:public Object,public Renderable{
	public:
		TOADLET_OBJECT(WaterRenderable);

		WaterRenderable(TerrainPatchComponent *terrain){mTerrain=terrain;}

		Material *getRenderMaterial() const{return mTerrain->mWaterMaterial;}
		const Transform &getRenderTransform() const{return mTerrain->getRenderTransform();}
		Bound *getRenderBound() const{return mTerrain->getRenderBound();}
		void render(RenderManager *manager) const{manager->getDevice()->renderPrimitive(mTerrain->mWaterVertexData,mTerrain->mWaterIndexData);}

	protected:
		TerrainPatchComponent *mTerrain;
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
	void simplifyBlocks(const Vector3 &cameraTranslate,Camera *camera);
	bool blockShouldSubdivide(Block *block,const Vector3 &cameraTranslate,Camera *camera);
	void computeDelta(Block *block,const Vector3 &cameraTranslate,float tolerance);
	bool blockIntersectsCamera(const Block *block,Camera *camera,bool water) const;
	bool blockVisibleByWater(const Block *block,const Vector3 &cameraTranslate,bool water) const;
	int gatherBlocks(IndexBuffer *indexBuffer,Camera *camera,bool water) const;
	int gatherTriangle(IndexBufferAccessor &iba,int indexCount,int x0,int y0,int x1,int y1,int x2,int y2) const;
	scalar calculateLayerWeight(int layer,int i,int j,scalar io,scalar jo);

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

	Engine *mEngine;
	Scene *mScene;
	Bound::ptr mBound;

	int mSize;
	Collection<Vertex> mVertexes;
	Collection<Block> mBlocks;
	Collection<int> mBlockQueue;
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

	TerrainPatchComponent::ptr mLeftDependent;
	TerrainPatchComponent::ptr mTopDependent;

	int mCameraUpdateScope;
	int mTerrainScope;
	int mWaterScope;
	scalar mTolerance;
	scalar mS1,mS2;

	Material::ptr mMaterial;
	VertexBuffer::ptr mVertexBuffer;
	IndexBuffer::ptr mIndexBuffer;
	VertexData::ptr mVertexData;
	IndexData::ptr mIndexData;
	Collection<Texture::ptr> mLayerTextures;

	Material::ptr mWaterMaterial;
	VertexBuffer::ptr mWaterVertexBuffer;
	IndexBuffer::ptr mWaterIndexBuffer;
	VertexData::ptr mWaterVertexData;
	IndexData::ptr mWaterIndexData;
	WaterRenderable::ptr mWaterRenderable;
	friend class WaterRenderable;
};

}
}
}

#endif

