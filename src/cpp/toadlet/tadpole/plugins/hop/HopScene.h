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
#include <toadlet/tadpole/plugins/hop/HopEntityFactory.h>
#include <toadlet/tadpole/node/Scene.h>
#include <toadlet/tadpole/node/PhysicallyTraceable.h>

namespace toadlet{
namespace tadpole{

class HopNode;
class HopEntityMessage;

// I'm not 100% sure that the decorator pattern was the correct hammer for this nail, but thats to be seen.
class TOADLET_API HopScene:public node::Scene,public hop::Manager{
public:
	TOADLET_INTRUSIVE_POINTERS(HopScene);

	HopScene(node::Scene::ptr scene);
	virtual ~HopScene();

	virtual void destroy(){mScene->destroy();}

	virtual void setChildScene(node::Scene *scene){mChildScene=scene;}
	virtual node::Scene *getRootScene(){return mScene;}

	virtual node::ParentNode *getBackground(){return mScene->getBackground();}
	virtual node::SceneNode *getRootNode(){return mScene->getRootNode();}

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

	virtual void findHopEntitiesInAABox(const AABox &box,egg::Collection<egg::IntrusivePointer<HopEntity> > &entities);
	virtual void findHopEntitiesInSphere(const Sphere &sphere,egg::Collection<egg::IntrusivePointer<HopEntity> > &entities);
	virtual void findHopEntitiesInSolids(egg::Collection<egg::IntrusivePointer<HopEntity> > &entities,hop::Solid *solids[],int numSolids);

	virtual void traceSegment(Collision &result,const Segment &segment,int collideWithBits,HopEntity *ignore);

	inline int getNumHopEntities() const{return mHopEntities.size();}
	inline HopEntity *getHopEntity(int i) const{return mHopEntities[i];}
	HopEntity *getHopEntityFromNetworkID(int id) const;
	void setHopEntityNetworkID(HopEntity *entity,int id);
	inline void addFreeNetworkID(int id){mFreeNetworkIDs.add(id);}
	virtual void resetNetworkIDs();

	virtual void setHopEntityFactory(HopEntityFactory *factory);

	virtual void showCollisionVolumes(bool show,bool interpolate);

	inline hop::Simulator *getSimulator(){return mSimulator;}

//	virtual node::ParticleNode::ParticleSimulator::ptr newParticleSimulator(node::ParticleNode *particleNode);
	virtual void registerHopEntity(HopEntity *entity);
	virtual void unregisterHopEntity(HopEntity *entity);

	virtual void preLogicUpdateLoop(int dt);
	virtual void preLogicUpdate(int dt);
	virtual void logicUpdate(int dt);
	virtual void postLogicUpdate(int dt);
	virtual void postLogicUpdateLoop(int dt);
	virtual void intraUpdate(int dt);
	virtual void preRenderUpdate(int dt);
	virtual void renderUpdate(int dt);
	virtual void postRenderUpdate(int dt);

	virtual bool performAABoxQuery(SpacialQuery *query,const AABox &box,bool exact){return mScene->performAABoxQuery(query,box,exact);}
	virtual int getContents(const Vector3 &point){return mScene->getContents(point);}

	void traceSegment(hop::Collision &result,const Segment &segment);
	void traceSolid(hop::Collision &result,const Segment &segment,const hop::Solid *solid);
	void preUpdate(int,scalar){}
	void preUpdate(hop::Solid*,int,scalar){}
	void intraUpdate(hop::Solid*,int,scalar){}
	bool collisionResponse(hop::Solid*,Vector3&,Vector3&,hop::Collision&){return false;}
	void postUpdate(hop::Solid*,int,scalar){}
	void postUpdate(int,scalar){}

	virtual egg::PointerCounter *getCounter() const{return mCounter;}

	hop::Collision cache_traceSegment_collision;

protected:
	void set(tadpole::Collision &r,hop::Collision &c);
	void set(hop::Collision &r,tadpole::Collision &c);

	virtual void defaultRegisterHopEntity(HopEntity *entity);
	virtual void defaultUnregisterHopEntity(HopEntity *entity);

	void castShadow(peeper::Renderer *renderer,HopEntity *entity);

	egg::PointerCounter *mCounter;
	node::Scene *mChildScene;
	node::Scene::ptr mScene;
	node::PhysicallyTraceable *mTraceable;

	egg::Collection<HopEntity*> mHopEntities;
	bool mShowCollisionVolumes;
	bool mInterpolateCollisionVolumes;

	int mExcessiveDT;
	hop::Simulator *mSimulator;
	hop::Solid::ptr mWorld;

	egg::Collection<int> mFreeNetworkIDs;
	egg::Collection<egg::IntrusivePointer<HopEntity> > mNetworkIDMap;
	HopEntityFactory *mHopEntityFactory;

	egg::Collection<hop::Solid*> mSolidCollection;

	friend class HopEntity;
};

}
}

#endif
