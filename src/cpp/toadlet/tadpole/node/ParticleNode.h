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
//#include <toadlet/peeper/TextureSection.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/RenderableNode.h>

namespace toadlet{
namespace tadpole{
namespace node{

class Scene;
class CameraNode;

class TOADLET_API ParticleNode:public RenderableNode,public Renderable{
public:
	TOADLET_NODE(ParticleNode,RenderableNode);

	class Particle{
	public:
		scalar x,y,z; // local space
		scalar ox,oy,oz; // orientation vector
		scalar vx,vy,vz; // velocity vector
		uint8 frame;
		uint32 startColor,endColor;
		scalar startScale,endScale;
		scalar age;
		bool visible;
	};

	class ParticleSimulator{
	public:
		TOADLET_SHARED_POINTERS(ParticleSimulator);

		virtual ~ParticleSimulator(){}

		virtual void setParticleLife(int life)=0;
		virtual void setParticleInitialVelocity(const Vector3 &velocity,const Vector3 &variance)=0;
		virtual void setSpawnAABox(const AABox &box)=0;
		virtual void setCoefficientOfGravity(scalar cog)=0;

		virtual void updateParticles(int dt,const Matrix4x4 &transform)=0;
	};

	ParticleNode();
	virtual Node *create(Engine *engine);
	virtual void destroy();

	virtual void setScale(scalar x,scalar y,scalar z){super::setScale(x,y,z);}
	virtual void setScale(const Vector3 &scale){super::setScale(scale);}

	virtual bool start(int particlesPerBeam,int numParticles,bool hasColor,const Vector3 points[],const scalar ages[],scalar defaultAge=0,bool visible=true);

	inline void setRandomStartColor(bool random){mRandomStartColor=random;}
	virtual void setStartColor(const peeper::Color &startColor);
	inline const peeper::Color getStartColor() const{return mStartColor;}
	virtual void setEndColor(const peeper::Color &endColor);
	inline const peeper::Color getEndColor() const{return mEndColor;}
	virtual void setColor(const peeper::Color &startColor,const peeper::Color &endColor);
	virtual void setColor(const peeper::Color &color);

	virtual void setStartScale(scalar startScale);
	inline scalar getStartScale() const{return mStartScale;}
	virtual void setEndScale(scalar endScale);
	inline scalar getEndScale() const{return mEndScale;}
	virtual void setScale(scalar startScale,scalar endScale);
	virtual void setScale(scalar scale);

	virtual void setWorldSpace(bool worldSpace);
	inline bool getWorldSpace() const{return mWorldSpace;}

	inline void setOrientation(bool vector){mOrientation=vector;}
	inline bool getOrientation() const{return mOrientation;}

//	inline void setTextureSection(peeper::TextureSection::ptr textureSection){mTextureSection=textureSection;}
//	inline peeper::TextureSection::ptr getTextureSection() const{return mTextureSection;}

	inline void setIndividualAttributes(bool individual){mIndividualAttributes=individual;}
	inline bool getIndividualAttributes() const{return mIndividualAttributes;}

	inline int getNumParticles() const{return mParticles.size();}
	inline Particle &getParticle(int i){return mParticles[i];}

	inline bool hadPoints() const{return mHadPoints;}
	inline bool hadAges() const{return mHadAges;}

	virtual void setRenderMaterial(const Material::ptr &material);

	inline void setDestroyOnFinish(bool destroy){mDestroyOnFinish=destroy;}
	inline bool getDestroyOnFinish() const{return mDestroyOnFinish;}

	virtual void setIdealViewTransform(const Matrix4x4 &transform){mIdealViewTransform.set(transform);mHasIdealViewTransform=true;}

	virtual void startSimulating(ParticleSimulator::ptr particleSimulator=ParticleSimulator::ptr());
	virtual void stopSimulating();
	inline ParticleSimulator *getParticleSimulator() const{return mParticleSimulator;}
	inline void destroyNextLogicFrame(){mDestroyNextLogicFrame=true;}
	
	virtual void logicUpdate(int dt);
	virtual void visualUpdate(int dt);
	virtual void queueRenderables(Scene *scene);

	virtual const Material::ptr &getRenderMaterial() const{return mMaterial;}
	virtual const Matrix4x4 &getRenderTransform() const;
	virtual void render(peeper::Renderer *renderer) const;

	peeper::VertexBufferAccessor vba;
	peeper::IndexBufferAccessor iba;

protected:
	TOADLET_GIB_DEFINE(ParticleNode);

	int randomColor();

	void updateWorldTransform(Node *node);

	void createVertexBuffer();
	void updateVertexBuffer(const Matrix4x4 &viewTransform);

	bool mDestroyNextLogicFrame;
	
	egg::Random mRandom;

	bool mWorldSpace;
	bool mOrientation;
	bool mIndividualAttributes;
	int mParticlesPerBeam;
	bool mDestroyOnFinish;
	bool mHadPoints;
	bool mHadAges;
	bool mHasColor;
	bool mHasScale;
	bool mLineBeams;

	egg::Collection<Particle> mParticles;

	bool mRandomStartColor;
	peeper::Color mStartColor;
	peeper::Color mEndColor;
	scalar mStartScale;
	scalar mEndScale;

//	peeper::TextureSection::ptr mTextureSection;
	Material::ptr mMaterial;
	peeper::VertexBuffer::ptr mVertexBuffer;
	peeper::VertexData::ptr mVertexData;
	peeper::IndexBuffer::ptr mIndexBuffer;
	peeper::IndexData::ptr mIndexData;
	peeper::IndexData::ptr mLineIndexData;

	Matrix4x4 mWorldTransform;

	bool mUpdateParticles;
	ParticleSimulator::ptr mParticleSimulator;

	bool mHasIdealViewTransform;
	Matrix4x4 mIdealViewTransform;

	scalar mEpsilon;

	Vector3 cache_updateVertexBuffer_viewUp;
	Vector3 cache_updateVertexBuffer_viewRight;
	Vector3 cache_updateVertexBuffer_viewForward;
	Vector3 cache_updateVertexBuffer_up;
	Vector3 cache_updateVertexBuffer_right;
	Vector3 cache_updateVertexBuffer_forward;
	Matrix4x4 cache_updateWorldTransform_temp1;
	Matrix4x4 cache_updateWorldTransform_temp2;
};

}
}
}

#endif

