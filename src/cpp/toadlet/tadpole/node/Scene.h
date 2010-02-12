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

#ifndef TOADLET_TADPOLE_NODE_SCENE_H
#define TOADLET_TADPOLE_NODE_SCENE_H

#include <toadlet/peeper/Color.h>
#include <toadlet/peeper/RenderTarget.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/tadpole/UpdateListener.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/node/Renderable.h>
#include <toadlet/tadpole/node/Scene.h>
#include <toadlet/tadpole/query/AABoxQuery.h>

namespace toadlet{
namespace tadpole{
namespace node{

class SceneNode;

class Scene{
public:
	TOADLET_INTRUSIVE_POINTERS(Scene);

	virtual ~Scene(){}

	virtual void destroy()=0;

	// Get information about the decorator chain, NOT the scene graph
	virtual void setChildScene(Scene *scene)=0;
	virtual Scene *getRootScene()=0;

	virtual ParentNode *getBackground()=0;
	virtual SceneNode *getRootNode()=0;

	virtual void setAmbientColor(peeper::Color ambientColor)=0;
	virtual const peeper::Color &getAmbientColor() const=0;

	virtual void setExcessiveDT(int dt)=0;
	virtual int getExcessiveDT() const=0;
	virtual void setLogicDT(int dt)=0;
	virtual int getLogicDT() const=0;
	virtual void setLogicTimeAndFrame(int time,int frame)=0;
	virtual int getLogicTime() const=0;
	virtual int getLogicFrame() const=0;
	virtual scalar getLogicFraction() const=0;
	virtual int getRenderTime() const=0;
	virtual int getRenderFrame() const=0;

	virtual void update(int dt)=0;
	virtual void render(peeper::Renderer *renderer,CameraNode *cameraNode,Node *node)=0;

	virtual void preLogicUpdateLoop(int dt)=0;
	virtual void preLogicUpdate(int dt)=0;
	virtual void logicUpdate(int dt)=0;
	virtual void postLogicUpdate(int dt)=0;
	virtual void postLogicUpdateLoop(int dt)=0;
	virtual void intraUpdate(int dt)=0;
	virtual void preRenderUpdate(int dt)=0;
	virtual void renderUpdate(int dt)=0;
	virtual void postRenderUpdate(int dt)=0;

	virtual void setUpdateListener(UpdateListener *updateListener)=0;
	virtual UpdateListener *getUpdateListener() const=0;

	virtual bool performQuery(query::AABoxQuery *query)=0;

	virtual egg::PointerCounter *getCounter() const=0;
};

}
}
}

#endif
