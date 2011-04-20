/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_TADPOLE_SCENE_H
#define TOADLET_TADPOLE_SCENE_H

#include <toadlet/peeper/RenderTarget.h>
#include <toadlet/tadpole/Collision.h>
#include <toadlet/tadpole/ContextListener.h>
#include <toadlet/tadpole/Mesh.h>
#include <toadlet/tadpole/RenderListener.h>
#include <toadlet/tadpole/RenderableSet.h>
#include <toadlet/tadpole/SceneRenderer.h>
#include <toadlet/tadpole/UpdateListener.h>
#include <toadlet/tadpole/node/PartitionNode.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Scene:public ContextListener{
public:
	TOADLET_SHARED_POINTERS(Scene);

	Scene(Engine *engine);
	virtual ~Scene();
	virtual void destroy();

	virtual Engine *getEngine(){return mEngine;}

	virtual node::PartitionNode *getBackground(){return mBackground;}

	virtual void setRoot(node::PartitionNode *root);
	virtual node::PartitionNode *getRoot(){return mRoot;}

	virtual void setExcessiveDT(int dt){mExcessiveDT=dt;}
	virtual int getExcessiveDT() const{return mExcessiveDT;}
	virtual void setRangeLogicDT(int minDT,int maxDT);
	virtual int getMinLogicDT() const{return mMinLogicDT;}
	virtual int getMaxLogicDT() const{return mMaxLogicDT;}
	virtual void setLogicTimeAndFrame(int time,int frame);
	virtual int getLogicTime() const{return mLogicTime;}
	virtual int getLogicFrame() const{return mLogicFrame;}
	virtual scalar getLogicFraction() const{return (mMinLogicDT==0 || mMinLogicDT!=mMaxLogicDT)?0:Math::div(Math::fromInt(mAccumulatedDT),Math::fromInt(mMinLogicDT));}
	virtual int getTime() const{return mLogicTime+mAccumulatedDT;}
	virtual int getFrame() const{return mFrame;}
	virtual bool getResetFrame(){return mResetFrame;}

	virtual void setUpdateListener(UpdateListener *updateListener){mUpdateListener=updateListener;}
	virtual UpdateListener *getUpdateListener() const{return mUpdateListener;}

	virtual void setRenderListener(RenderListener *renderListener){mRenderListener=renderListener;}
	virtual RenderListener *getRenderListener() const{return mRenderListener;}

	virtual void setSceneRenderer(SceneRenderer::ptr sceneRenderer){mSceneRenderer=sceneRenderer;}
	virtual SceneRenderer::ptr getSceneRenderer() const{return mSceneRenderer;}

	virtual scalar getEpsilon() const{return Math::fromMilli(1);}

	virtual void update(int dt);
	virtual void preLogicUpdate(int dt){}
	virtual void logicUpdate(int dt){logicUpdate(dt,-1);}
	virtual void logicUpdate(int dt,int scope);
	virtual void postLogicUpdate(int dt){}
	virtual void preFrameUpdate(int dt){}
	virtual void frameUpdate(int dt){frameUpdate(dt,-1);}
	virtual void frameUpdate(int dt,int scope);
	virtual void postFrameUpdate(int dt){}
	virtual void queueDependent(node::Node *dependent);

	virtual void render(peeper::Renderer *renderer,node::CameraNode *camera,node::Node *node);

	virtual void traceSegment(Collision &result,const Segment &segment,int collideWithBits=-1,node::Node *ignore=NULL){result.time=Math::ONE;}

	void setAmbientColor(const Vector4 &ambientColor){mAmbientColor.set(ambientColor);}
	inline const Vector4 &getAmbientColor() const{return mAmbientColor;}
	void setFogState(const peeper::FogState &state){mFogState.set(state);}
	inline const peeper::FogState &getFogState() const{return mFogState;}

	int countActiveNodes(node::Node *node=NULL);
	void renderBoundingVolumes(peeper::Renderer *renderer,node::Node *node=NULL);

	virtual void preContextReset(peeper::Renderer *renderer){}
	virtual void postContextReset(peeper::Renderer *renderer){postContextActivate(renderer);}
	virtual void preContextActivate(peeper::Renderer *renderer){}
	virtual void postContextActivate(peeper::Renderer *renderer);
	virtual void preContextDeactivate(peeper::Renderer *renderer){}
	virtual void postContextDeactivate(peeper::Renderer *renderer){}

protected:
	UpdateListener *mUpdateListener;
	RenderListener *mRenderListener;

	int mExcessiveDT;
	int mMinLogicDT;
	int mMaxLogicDT;
	int mLogicTime;
	int mLogicFrame;
	int mAccumulatedDT;
	int mFrame;
	egg::Collection<node::Node::ptr> mDependents;
	bool mResetFrame;

	Engine *mEngine;
	node::PartitionNode::ptr mBackground;
	node::PartitionNode::ptr mRoot;

	Vector4 mAmbientColor;
	peeper::FogState mFogState;
	SceneRenderer::ptr mSceneRenderer;

	Mesh::ptr mSphereMesh,mAABoxMesh;
};

}
}

#endif
