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

namespace toadlet{
namespace tadpole{
namespace node{

class Scene{
public:
	virtual ~Scene(){}

	virtual void destroy()=0;

	// Get information about the decorator chain, NOT the scene graph
	virtual Scene *getRootScene()=0;
	
	virtual const ParentNode::ptr &getBackground() const=0;

	virtual void setAmbientColor(peeper::Color ambientColor)=0;
	virtual const peeper::Color &getAmbientColor() const=0;

	virtual void setExcessiveDT(int dt)=0;
	virtual int getExcessiveDT() const=0;
	virtual void setLogicDT(int dt)=0;
	virtual int getLogicDT() const=0;
	virtual void setLogicTimeAndFrame(int time,int frame)=0;
	virtual int getLogicTime() const=0;
	virtual int getLogicFrame() const=0;
	virtual int getRenderTime() const=0;
	virtual int getRenderFrame() const=0;

	virtual void update(int dt)=0;
	virtual void render(peeper::Renderer *renderer,CameraNode *cameraNode,Node *node)=0;

	virtual void preLogicUpdateLoop(int dt);
	virtual void logicUpdate(int dt);
	virtual void postLogicUpdateLoop(int dt);
	virtual void renderUpdate(int dt);

	virtual void setUpdateListener(UpdateListener *updateListener)=0;
	virtual UpdateListener *getUpdateListener() const=0;
};

}
}
}

#endif
