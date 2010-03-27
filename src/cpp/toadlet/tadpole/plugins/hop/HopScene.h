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

#ifndef TOADLET_TADPOLE_HOPSCENE_H
#define TOADLET_TADPOLE_HOPSCENE_H

#include <toadlet/hop/Simulator.h>
#include <toadlet/tadpole/Collision.h>
#include <toadlet/tadpole/Traceable.h>
#include <toadlet/tadpole/node/Scene.h>

namespace toadlet{
namespace tadpole{

class HopEntity;

// I'm not 100% sure that the decorator pattern was the correct hammer for this nail, but thats to be seen.
// Because we really have several subsystems we want to tie together in the Scene
//	- Scene Management	(bsp)
//	- Physics			(hop)
//	- Networking		(knot)
class TOADLET_API HopScene:public node::Scene,public hop::Manager{
public:
	TOADLET_INTRUSIVE_POINTERS(HopScene);

	HopScene(node::Scene::ptr scene);
	virtual ~HopScene();

	virtual void destroy(){mScene->destroy();}

	virtual void setChildScene(node::Scene *scene){mChildScene=scene;}
	virtual node::Scene *getRootScene(){return mScene;}

	virtual Engine *getEngine(){return mScene->getEngine();}
	virtual node::ParentNode *getBackground(){return mScene->getBackground();}
	virtual node::SceneNode *getRootNode(){return mScene->getRootNode();}
	virtual int getScope(){return mScene->getScope();}

	virtual node::Node *findNodeByName(const egg::String &name,node::Node *node=NULL){return mScene->findNodeByName(name,node);}
	virtual node::Node *findNodeByHandle(int handle){return mScene->findNodeByHandle(handle);}

	virtual void setAmbientColor(peeper::Color ambientColor){mScene->setAmbientColor(ambientColor);}
	virtual const peeper::Color &getAmbientColor() const{return mScene->getAmbientColor();}

	virtual void setExcessiveDT(int dt){mScene->setExcessiveDT(dt);}
	virtual int getExcessiveDT() const{return mScene->getExcessiveDT();}
	virtual void setRangeLogicDT(int minDT,int maxDT){mScene->setRangeLogicDT(minDT,maxDT);}
	virtual int getMinLogicDT() const{return mScene->getMinLogicDT();}
	virtual int getMaxLogicDT() const{return mScene->getMaxLogicDT();}
	virtual void setLogicTimeAndFrame(int time,int frame){mScene->setLogicTimeAndFrame(time,frame);}
	virtual int getLogicTime() const{return mScene->getLogicTime();}
	virtual int getLogicFrame() const{return mScene->getLogicFrame();}
	virtual scalar getLogicFraction() const{return mScene->getLogicFraction();}
	virtual int getRenderTime() const{return mScene->getRenderTime();}
	virtual int getRenderFrame() const{return mScene->getRenderFrame();}

	virtual void update(int dt){mScene->update(dt);}
	virtual void render(peeper::Renderer *renderer,node::CameraNode *cameraNode,node::Node *node){mScene->render(renderer,cameraNode,node);}

	virtual void setUpdateListener(UpdateListener *updateListener){mScene->setUpdateListener(updateListener);}
	virtual UpdateListener *getUpdateListener() const{return mScene->getUpdateListener();}

	virtual void setFluidVelocity(const Vector3 &fluidVelocity){mSimulator->setFluidVelocity(fluidVelocity);}
	virtual const Vector3 &getFluidVelocity() const{return mSimulator->getFluidVelocity();}

	virtual void setGravity(const Vector3 &gravity){mSimulator->setGravity(gravity);}
	virtual const Vector3 &getGravity() const{return mSimulator->getGravity();}

	#if defined(TOADLET_FIXED_POINT)
		virtual void setEpsilonBits(int epsilonBits){mSimulator->setEpsilonBits(epsilonBits);}
		virtual int getEpsilonBits() const{return mSimulator->getEpsilonBits();}
	#else
		virtual void setEpsilon(scalar epsilon){mSimulator->setEpsilon(epsilon);}
		virtual scalar getEpsilon() const{return mSimulator->getEpsilon();}
	#endif

	virtual void setTraceable(Traceable *traceable){mTraceable=traceable;}

	virtual void traceSegment(Collision &result,const Segment &segment,int collideWithBits,HopEntity *ignore);
	virtual void traceEntity(Collision &result,HopEntity *entity,const Segment &segment,int collideWithBits);
	virtual void testEntity(Collision &result,HopEntity *entity1,const Segment &segment,HopEntity *entity2);

	virtual void showCollisionVolumes(bool show,bool interpolate);

	inline hop::Simulator *getSimulator(){return mSimulator;}

	virtual int nodeCreated(node::Node *node){return mScene->nodeCreated(node);}
	virtual void nodeDestroyed(node::Node *node){mScene->nodeDestroyed(node);}
	virtual int nodeCreated(HopEntity *entity);
	virtual void nodeDestroyed(HopEntity *entity);

	virtual void preLogicUpdateLoop(int dt);
	virtual void preLogicUpdate(int dt);
	virtual void logicUpdate(int dt);
	virtual void postLogicUpdate(int dt);
	virtual void postLogicUpdateLoop(int dt);
	virtual void intraUpdate(int dt);

	virtual bool performAABoxQuery(SpacialQuery *query,const AABox &box,bool exact){return mScene->performAABoxQuery(query,box,exact);}

	virtual int findSolidsInAABox(const AABox &box,hop::Solid *solids[],int maxSolids);
	virtual void traceSegment(hop::Collision &result,const Segment &segment);
	virtual void traceSolid(hop::Collision &result,const Segment &segment,const hop::Solid *solid);
	virtual void preUpdate(int dt,scalar fdt){}
	virtual void postUpdate(int dt,scalar fdt){}
	virtual void preUpdate(hop::Solid *solid,int dt,scalar fdt){}
	virtual void intraUpdate(hop::Solid *solid,int dt,scalar fdt){}
	virtual bool collisionResponse(hop::Solid *solid,Vector3 &position,Vector3 &remainder,hop::Collision &collision){return false;}
	virtual void postUpdate(hop::Solid *solid,int dt,scalar fdt){}

	virtual void updateRenderTransformsToRoot(node::Node *node){mScene->updateRenderTransformsToRoot(node);}

	virtual egg::PointerCounter *pointerCounter() const{return mCounter;}

	static void set(tadpole::Collision &r,const hop::Collision &c);
	static void set(hop::Collision &r,const tadpole::Collision &c,HopEntity *collider);

	hop::Collision cache_traceSegment_collision;

protected:
	void castShadow(peeper::Renderer *renderer,HopEntity *entity);

	egg::PointerCounter *mCounter;
	node::Scene *mChildScene;
	node::Scene::ptr mScene;

	egg::Collection<HopEntity*> mHopEntities;
	bool mShowCollisionVolumes;
	bool mInterpolateCollisionVolumes;

	int mExcessiveDT;
	hop::Simulator *mSimulator;
	Traceable *mTraceable;

	friend class HopEntity;
};

}
}

#endif
