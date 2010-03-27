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

#ifndef TOADLET_TADPOLE_NODE_SCENENODE_H
#define TOADLET_TADPOLE_NODE_SCENENODE_H

#include <toadlet/peeper/Color.h>
#include <toadlet/peeper/RenderTarget.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/tadpole/UpdateListener.h>
#include <toadlet/tadpole/RenderQueue.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/node/LightNode.h>
#include <toadlet/tadpole/node/ParticleNode.h>
#include <toadlet/tadpole/node/Scene.h>

namespace toadlet{
namespace tadpole{
namespace node{

/// @todo  I think a good way to let us use EventKeyFrames with the Controller framework is to:
///   1: Have a way of ensuring that the EventKeyFrames are aligned on LogicDT times.
///   2: Have the AnimationControllerNode not issue a renderUpdate until we have enough logicUpdates to keep up with it.
///  That way, entities without EventKeyFrames should update smoothly if something keeps the logic from flowing, but
///   we will still be able to have any logic dependent stuff happen at the correct times.
///
///  Or perhaps, we could use the above tactic for all the update entities.  There is just 1 function, update
///   But it stops being called if logic updates cease due to networking issues.
///   Down sides are that its harder to do logic based updating, since you'd have to handle the checking to see if its
///    been a logicDT yourself.

// TODO: Force no-scene-graph updates in renderUpdate, to allow us to thread that easily

class TOADLET_API SceneNode:public ParentNode,public Scene{
public:
	TOADLET_NONINSTANCIABLENODE(SceneNode,ParentNode);
	TOADLET_POINTERCOUNTER_PASSTHROUGH(ParentNode);

	SceneNode(Engine *engine);
	virtual ~SceneNode();
	virtual Node *create(Scene *scene);
	virtual void destroy();

	virtual void setChildScene(Scene *scene){mChildScene=scene;}
	virtual Scene *getRootScene(){return this;}

	virtual Engine *getEngine(){return mEngine;}
	virtual ParentNode *getBackground(){return mBackground;}
	virtual SceneNode *getRootNode(){return this;}
	virtual int getScope(){return super::getScope();}

	virtual Node *findNodeByName(const egg::String &name,Node *node=NULL);
	virtual Node *findNodeByHandle(int handle);

	virtual void setAmbientColor(peeper::Color ambientColor){mAmbientColor.set(ambientColor);}
	virtual const peeper::Color &getAmbientColor() const{return mAmbientColor;}

	virtual void setExcessiveDT(int dt){mExcessiveDT=dt;}
	virtual int getExcessiveDT() const{return mExcessiveDT;}
	virtual void setRangeLogicDT(int minDT,int maxDT);
	virtual int getMinLogicDT() const{return mMinLogicDT;}
	virtual int getMaxLogicDT() const{return mMaxLogicDT;}
	virtual void setLogicTimeAndFrame(int time,int frame);
	virtual int getLogicTime() const{return mLogicTime;}
	virtual int getLogicFrame() const{return mLogicFrame;}
	virtual scalar getLogicFraction() const{return (mMinLogicDT==0 || mMinLogicDT!=mMaxLogicDT)?0:Math::div(Math::fromInt(mAccumulatedDT),Math::fromInt(mMinLogicDT));}
	virtual int getRenderTime() const{return mLogicTime+mAccumulatedDT;}
	virtual int getRenderFrame() const{return mRenderFrame;}
	virtual int getNumLastUpdatedNodes() const{return mNumLastUpdatedNodes;}

	virtual void update(int dt);
	virtual void preLogicUpdateLoop(int dt){}
	virtual void preLogicUpdate(int dt){}
	virtual void logicUpdate(int dt);
	virtual void logicUpdate(int dt,int scope);
	virtual void logicUpdate(Node *node,int dt,int scope);
	virtual void postLogicUpdate(int dt){}
	virtual void postLogicUpdateLoop(int dt){}
	virtual void intraUpdate(int dt){}

	virtual void render(peeper::Renderer *renderer,CameraNode *cameraNode,Node *node);
	virtual void renderUpdate(CameraNode *camera,RenderQueue *queue);
	virtual void renderUpdate(Node *node,CameraNode *camera,RenderQueue *queue);

	virtual void setUpdateListener(UpdateListener *updateListener){mUpdateListener=updateListener;}
	virtual UpdateListener *getUpdateListener() const{return mUpdateListener;}

	virtual bool performAABoxQuery(SpacialQuery *query,const AABox &box,bool exact);

	virtual void updateRenderTransformsToRoot(Node *node);
	virtual bool culled(Node *node,CameraNode *camera);
	virtual int nodeCreated(Node *node);
	virtual void nodeDestroyed(Node *node);

protected:
	virtual void renderRenderables(peeper::Renderer *renderer,CameraNode *camera,RenderQueue *queue);
	virtual bool preLayerRender(peeper::Renderer *renderer,CameraNode *camera,int layer){return false;}
	virtual bool postLayerRender(peeper::Renderer *renderer,CameraNode *camera,int layer){return false;}

	Scene *mChildScene;
	egg::Collection<int> mFreeHandles;
	egg::Collection<Node*> mNodesFromHandles;

	int mExcessiveDT;
	int mMinLogicDT;
	int mMaxLogicDT;
	int mLogicTime;
	int mLogicFrame;
	int mAccumulatedDT;
	int mRenderFrame;

	ParentNode::ptr mBackground;
	peeper::Color mAmbientColor;

	UpdateListener *mUpdateListener;
	int mNumLastUpdatedNodes;

	RenderQueue::ptr mRenderQueue;
	Material *mPreviousMaterial;

	peeper::Viewport cache_render_viewport;
};

}
}
}

#endif
