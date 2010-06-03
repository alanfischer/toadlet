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

#include <toadlet/peeper/Color.h>
#include <toadlet/peeper/RenderTarget.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/tadpole/UpdateListener.h>
#include <toadlet/tadpole/RenderQueue.h>
#include <toadlet/tadpole/node/PartitionNode.h>
#include <toadlet/tadpole/mesh/Mesh.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Scene{
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

	virtual void setUpdateListener(UpdateListener *updateListener){mUpdateListener=updateListener;}
	virtual UpdateListener *getUpdateListener() const{return mUpdateListener;}

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
	virtual void renderRenderables(peeper::Renderer *renderer,node::CameraNode *camera,RenderQueue *queue);

	/// @todo: replace with just a sensor?
	virtual node::Node *findNodeByName(const egg::String &name,node::Node *node=NULL);
	virtual node::Node *findNodeByHandle(int handle);

	virtual void setAmbientColor(peeper::Color ambientColor){mAmbientColor.set(ambientColor);}
	virtual const peeper::Color &getAmbientColor() const{return mAmbientColor;}

	virtual egg::image::Image::ptr renderToImage(peeper::Renderer *renderer,node::CameraNode *camera,int format,int width,int height);

	int countActiveNodes(node::Node *node=NULL);
	int countLastRendered();
	void renderBoundingVolumes(peeper::Renderer *renderer,node::Node *node=NULL);

	virtual int nodeCreated(node::Node *node);
	virtual void nodeDestroyed(node::Node *node);

protected:
	virtual bool preLayerRender(peeper::Renderer *renderer,node::CameraNode *camera,int layer){return false;}
	virtual bool postLayerRender(peeper::Renderer *renderer,node::CameraNode *camera,int layer){return false;}

	egg::Collection<int> mFreeHandles;
	egg::Collection<node::Node*> mNodesFromHandles;

	UpdateListener *mUpdateListener;
	int mExcessiveDT;
	int mMinLogicDT;
	int mMaxLogicDT;
	int mLogicTime;
	int mLogicFrame;
	int mAccumulatedDT;
	int mFrame;
	egg::Collection<node::Node::ptr> mDependents;

	Engine *mEngine;
	node::PartitionNode::ptr mBackground;
	node::PartitionNode::ptr mRoot;
	peeper::Color mAmbientColor;
	mesh::Mesh::ptr mBoundMesh;

	RenderQueue::ptr mRenderQueue;
	Material *mPreviousMaterial;
	int mCountLastRendered;

	peeper::Viewport cache_render_viewport;
};

}
}

#endif
