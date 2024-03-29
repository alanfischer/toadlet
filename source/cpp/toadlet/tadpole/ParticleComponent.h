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

#ifndef TOADLET_TADPOLE_PARTICLECOMPONENT_H
#define TOADLET_TADPOLE_PARTICLECOMPONENT_H

#include <toadlet/egg/Random.h>
#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/tadpole/Camera.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Visible.h>
#include <toadlet/tadpole/Spacial.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/material/Material.h>

namespace toadlet{
namespace tadpole{

// ParticleComponent will render a group of screen facing sprites specified by points.
// There are 3 types of particles, POINTSPRITE,SPRITE,BEAM
// POINTSPRITE - particles are rendered using a single point, and the Material used should set its GeometryState
// SPRITE - particles are rendered using 4 points and oriented towards the camera
// BEAM - particles are rendered as beams that take up 2 or more particles
// The vx,vy,vz in the Particle class are only used for alignment, this class provides no simulation of particles.
class TOADLET_API ParticleComponent:public BaseComponent,public TransformListener,public Renderable,public Visible,public Spacial{
public:
	TOADLET_COMPONENT(ParticleComponent);

	enum ParticleType{
		ParticleType_POINTSPRITE=0,
		ParticleType_SPRITE=1,
		ParticleType_BEAM=2,
		// 3 and more longer connected beams

		ParticleType_BIT_UNCAPPED=1<<28, // No caps on beams
		ParticleType_BIT_LOOP=1<<29, // Beams are loops
		ParticleType_BIT_SERIES=1<<30, // Multiple beams share start and end points
		ParticleType_FLAGS=ParticleType_BIT_UNCAPPED | ParticleType_BIT_LOOP | ParticleType_BIT_SERIES
	};

	class Particle{
	public:
		Particle():
		  x(0),y(0),z(0),
		  vx(0),vy(0),vz(0),
		  s(0),t(0),s2(Math::ONE),t2(Math::ONE),
		  scale(Math::ONE),
		  color(0xFFFFFFFF)
		{}

		scalar x,y,z;
		scalar vx,vy,vz;
		scalar s,t,s2,t2;
		scalar scale;
		uint32 color;
	};

	ParticleComponent(Scene *scene);
	void destroy();

	void parentChanged(Node *node);
	
	bool setNumParticles(int numParticles,int particleType,scalar scale=Math::ONE,const Vector3 positions[]=NULL);
	inline int getNumParticles() const{return mParticles.size();}
	inline int getParticleType() const{return mParticleType;}
	inline Particle *getParticle(int i){return &mParticles[i];}

	void setWorldSpace(bool worldSpace);
	inline bool getWorldSpace() const{return mWorldSpace;}

	inline void setVelocityAligned(bool aligned){mVelocityAligned=aligned;}
	inline bool getVelocityAligned() const{return mVelocityAligned;}

	inline void setManualUpdating(bool manual){mManualUpdating=manual;}
	inline bool getManualUpdating() const{return mManualUpdating;}

	void setSorted(bool sorted);
	inline bool getSorted() const{return mSorted;}

	void setMaterial(Material *material);
	Material *getMaterial() const{return mMaterial;}

	void updateParticles(Camera *camera);

	void updateBound();

	void frameUpdate(int dt,int scope);

	// Spacial
	void setTransform(Transform::ptr transform);
	Transform *getTransform() const{return mTransform;}
	Bound *getBound() const{return mBound;}
	Transform *getWorldTransform() const{return mWorldTransform;}
	Bound *getWorldBound() const{return mWorldBound;}
	void transformChanged(Transform *transform);

	// Visible
	bool getRendered() const{return mRendered;}
	void setRendered(bool rendered){mRendered=rendered;}
	void setSharedRenderState(RenderState::ptr renderState);
	RenderState *getSharedRenderState() const{return mSharedRenderState;}
	void gatherRenderables(Camera *camera,RenderableSet *set);

	// Renderable
	Material *getRenderMaterial() const{return mMaterial;}
	Transform *getRenderTransform() const{return mWorldTransform;}
	Bound *getRenderBound() const{return mWorldBound;}
	void render(RenderManager *manager) const;

	void updateVertexData(Camera *camera);

protected:
	void createVertexData();
	void createIndexData();

	void updateIndexData(Camera *camera);

	Collection<Particle> mParticles;
	Collection<scalar> mDistances;
	Collection<int> mOrders;

	Engine *mEngine;
	Scene *mScene;
	int mParticleType;
	scalar mParticleScale;
	int mBeamLength;
	int mBeamCount;
	int mBeamType;
	int mNumIndexesPerBeam,mNumVertexesPerBeam;
	bool mWorldSpace;
	bool mManualUpdating;
	bool mVelocityAligned;
	bool mSorted;
	bool mRendered;
	Transform::ptr mTransform;
	Bound::ptr mBound;
	Transform::ptr mWorldTransform;
	Bound::ptr mWorldBound;
	Material::ptr mMaterial;
	RenderState::ptr mSharedRenderState;
	VertexData::ptr mVertexData;
	IndexData::ptr mIndexData;
	Camera::ptr mUpdateCamera;
};

}
}

#endif

