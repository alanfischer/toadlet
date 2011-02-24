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

#ifndef TOADLET_TADPOLE_NODE_PARTICLENODE_H
#define TOADLET_TADPOLE_NODE_PARTICLENODE_H

#include <toadlet/egg/String.h>
#include <toadlet/egg/Random.h>
#include <toadlet/peeper/Color.h>
#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Renderer.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/node/CameraNode.h>

namespace toadlet{
namespace tadpole{
namespace node{

class TOADLET_API ParticleNode:public Node,public Renderable{
public:
	TOADLET_NODE(ParticleNode,Node);

	enum ParticleType{
		ParticleType_POINTSPRITE=0,
		ParticleType_SPRITE=1,
		ParticleType_BEAM=2,
		// 3 and more longer connected beams
	};

	class Particle{
	public:
		Particle():
		  x(0),y(0),z(0),
		  ox(0),oy(0),oz(0),
		  tx(0),ty(0),tw(Math::ONE),th(Math::ONE),
		  size(Math::ONE),
		  color(0xFFFFFFFF)
		{}

		scalar x,y,z;
		scalar ox,oy,oz; // orientation vector
		scalar s,t,s2,t2;
		scalar scale;
		uint32 color;
	};

	ParticleNode();
	Node *create(Scene *scene);
	void destroy();
	Node *set(Node *node);

	bool setNumParticles(int numParticles,int particleType,const Vector3 positions[]=NULL);
	inline int getNumParticles() const{return mParticles.size();}
	inline int getParticleType() const{return mParticleType;}
	inline Particle *getParticle(int i){return &mParticles[i];}

	inline void setWorldSpace(bool worldSpace);
	inline bool getWorldSpace() const{return mWorldSpace;}

	inline void setManualUpdating(bool manual){mManualUpdating=manual;}
	inline bool getManualUpdating() const{return mManualUpdating;}

	void setMaterial(Material::ptr material);
	Material::ptr getMaterial() const{return mMaterial;}

	void frameUpdate(int dt,int scope);
	void queueRenderables(CameraNode *camera,RenderQueue *queue);

	Material *getRenderMaterial() const{return mMaterial;}
	Transform *getRenderTransform() const{return mWorldSpace==false?getWorldTransform():NULL;}
	Bound *getRenderBound() const{return getWorldBound();}
	void render(peeper::Renderer *renderer) const;

	void updateVertexBuffer(CameraNode *camera);

	peeper::VertexBufferAccessor vba;
	peeper::IndexBufferAccessor iba;

protected:
	TOADLET_GIB_DEFINE(ParticleNode);

	void updateWorldTransform(Node *node);

	void createVertexBuffer();

	egg::Collection<Particle> mParticles;

	int mParticleType;
	bool mWorldSpace;
	bool mManualUpdating;
	Material::ptr mMaterial;
	peeper::VertexData::ptr mVertexData;
	peeper::IndexData::ptr mIndexData;
};

}
}
}

#endif

