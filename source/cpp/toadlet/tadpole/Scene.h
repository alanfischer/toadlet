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
#include <toadlet/tadpole/ContextListener.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Mesh.h>
#include <toadlet/tadpole/NodeManager.h>
#include <toadlet/tadpole/RenderableSet.h>
#include <toadlet/tadpole/RenderManager.h>
#include <toadlet/tadpole/PhysicsCollision.h>
#include <toadlet/tadpole/PhysicsManager.h>
#include <toadlet/tadpole/UpdateListener.h>
#include <toadlet/tadpole/PartitionNode.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Scene:public Object,public ContextListener{
public:
	TOADLET_OBJECT(Scene);

	Scene(Engine *engine,scalar epsilon=Math::fromMilli(1));
	void destroy();

	Engine *getEngine(){return mEngine;}

	PartitionNode *getBackground(){return mBackground;}

	virtual void setRoot(PartitionNode *root);
	PartitionNode *getRoot(){return mRoot;}

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

	virtual void setNodeManager(NodeManager *nodeManager){mNodeManager=nodeManager;}
	virtual NodeManager *getNodeManager() const{return mNodeManager;}

	virtual void setRenderManager(RenderManager *renderManager){mRenderManager=renderManager;}
	virtual RenderManager *getRenderManager() const{return mRenderManager;}

	virtual void setPhysicsManager(PhysicsManager *physicsManager){mPhysicsManager=physicsManager;}
	virtual PhysicsManager *getPhysicsManager() const{return mPhysicsManager;}

	// In lieu of an AudioManager here, we want a way of specifying if AudioComponents should create Audios
	virtual void setCreateAudio(bool audio){mCreateAudio=audio;}
	virtual bool getCreateAudio() const{return mCreateAudio;}

	virtual void setEpsilon(scalar epsilon){mEpsilon=epsilon;}
	virtual scalar getEpsilon() const{return mEpsilon;}

	virtual void update(int dt);
	virtual void preLogicUpdate(int dt){}
	virtual void logicUpdate(int dt,int scope=-1,Node *node=NULL);
	virtual void postLogicUpdate(int dt){}
	virtual void preFrameUpdate(int dt){}
	virtual void frameUpdate(int dt,int scope=-1,Node *node=NULL);
	virtual void postFrameUpdate(int dt){}

	virtual void render(RenderDevice *renderDevice,Camera *camera,Node *node=NULL);

	virtual void traceSegment(PhysicsCollision &result,const Segment &segment,int collideWithBits=-1,Node *ignore=NULL);

	virtual void destroy(Component *component);

	void setAmbientColor(const Vector4 &ambientColor){mAmbientColor.set(ambientColor);}
	inline const Vector4 &getAmbientColor() const{return mAmbientColor;}

	void renderBoundingVolumes(RenderDevice *renderDevice,Node *node=NULL);

	virtual void preContextReset(RenderDevice *renderDevice){}
	virtual void postContextReset(RenderDevice *renderDevice);
	virtual void preContextActivate(RenderDevice *renderDevice){}
	virtual void postContextActivate(RenderDevice *renderDevice){}
	virtual void preContextDeactivate(RenderDevice *renderDevice){}
	virtual void postContextDeactivate(RenderDevice *renderDevice){}

protected:
	UpdateListener *mUpdateListener;

	int mExcessiveDT;
	int mMinLogicDT;
	int mMaxLogicDT;
	int mLogicTime;
	int mLogicFrame;
	int mAccumulatedDT;
	int mFrame;
	bool mResetFrame;
	scalar mEpsilon;

	Engine::ptr mEngine;
	PartitionNode::ptr mBackground;
	PartitionNode::ptr mRoot;
	Vector4 mAmbientColor;

	NodeManager::ptr mNodeManager;
	RenderManager::ptr mRenderManager;
	PhysicsManager::ptr mPhysicsManager;
	bool mCreateAudio;

	Collection<Component::ptr> mDestroyComponents;
};

}
}

#endif
