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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/Profile.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/RenderQueue.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/node/ParticleNode.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/node/CameraNode.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(ParticleNode,Categories::TOADLET_TADPOLE_NODE+".ParticleNode");

ParticleNode::ParticleNode():super(),
	TOADLET_GIB_IMPLEMENT()

	//mParticles,

	mParticleType(0),
	mWorldSpace(false),
	mManualUpdating(false),
	mVelocityAligned(false),
	mRendered(false)
	//mMaterial,
	//mVertexData,
	//mIndexData,
{}

Node *ParticleNode::create(Scene *scene){
	super::create(scene);

	mParticles.clear();

	mParticleType=0;
	mWorldSpace=false;
	mManualUpdating=false;
	mVelocityAligned=false;
	mRendered=true;

	return this;
}

void ParticleNode::destroy(){
	mParticles.clear();

	if(mVertexData!=NULL){
		mVertexData->destroy();
		mVertexData=NULL;
	}
	
	if(mIndexData!=NULL){
		mIndexData->destroy();
		mIndexData=NULL;
	}

	if(mMaterial!=NULL){
		mMaterial->release();
		mMaterial=NULL;
	}

	super::destroy();
}

Node *ParticleNode::set(Node *node){
	super::set(node);

	ParticleNode *particleNode=(ParticleNode*)node;
	setNumParticles(particleNode->getNumParticles(),particleNode->getParticleType());
	setWorldSpace(particleNode->getWorldSpace());
	setMaterial(particleNode->getMaterial());

	return NULL;
}

void *ParticleNode::hasInterface(int type){
	switch(type){
		case InterfaceType_RENDERABLE:
			return (Renderable*)this;
		case InterfaceType_VISIBLE:
			return (Visible*)this;
		default:
			return NULL;
	}
}

bool ParticleNode::setNumParticles(int numParticles,int particleType,scalar scale,const Vector3 positions[]){
	if(particleType>=ParticleType_BEAM && (numParticles%particleType)!=0){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"setNumParticles: Must specify a numParticles divisible by particleType");
		return false;
	}

	mParticles.resize(numParticles);

	mParticleType=particleType;

	int i;
	if(positions!=NULL){
		for(i=0;i<numParticles;++i){
			Particle &p=mParticles[i];
			const Vector3 &position=positions[i];
			p.x=position.x;
			p.y=position.y;
			p.z=position.z;
		}
	}
	for(i=0;i<numParticles;++i){
		mParticles[i].scale=scale;
	}

	createVertexBuffer();

	return true;
}

void ParticleNode::setWorldSpace(bool worldSpace){
	if(mWorldSpace==worldSpace){
		return;
	}

	mWorldSpace=worldSpace;

	Vector3 position;
	int i;
	if(mWorldSpace){
		for(i=0;i<mParticles.size();++i){
			Particle &p=mParticles[i];
			position.set(p.x,p.y,p.z);
			mWorldTransform->transform(position);
			p.x=position.x;p.y=position.y;p.z=position.z;
		}
	}
	else{
		for(i=0;i<mParticles.size();++i){
			Particle &p=mParticles[i];
			position.set(p.x,p.y,p.z);
			mWorldTransform->inverseTransform(position);
			p.x=position.x;p.y=position.y;p.z=position.z;
		}
	}
}

void ParticleNode::setMaterial(Material::ptr material){
	if(mMaterial!=NULL){
		mMaterial->release();
	}

	mMaterial=material;

	if(mMaterial!=NULL){
		mMaterial->retain();
	}
}

void ParticleNode::modifyMaterial(Material::ptr material){
	if(mMaterial!=NULL){
		if(mMaterial->getManaged()){
			mMaterial=mMaterial->clone();
		}

		mMaterial->modifyWith(material);
	}
}

void ParticleNode::queueRenderables(CameraNode *camera,RenderQueue *queue){
	super::queueRenderables(camera,queue);

	if(mRendered==false){
		return;
	}

	if(mManualUpdating==false){
		updateVertexBuffer(camera);
	}

#if defined(TOADLET_GCC_INHERITANCE_BUG)
	queue->queueRenderable(&renderable);
#else
	queue->queueRenderable(this);
#endif
}

void ParticleNode::render(Renderer *renderer) const{
	if(mVertexData!=NULL && mIndexData!=NULL){
		renderer->renderPrimitive(mVertexData,mIndexData);
	}
}

void ParticleNode::createVertexBuffer(){
	int i,j,ii,vi;

	VertexFormat::ptr vertexFormat=mEngine->getVertexFormats().POSITION_COLOR_TEX_COORD;

	int numParticles=mParticles.size();
	int numVertexes=0;
	if(mParticleType==ParticleType_POINTSPRITE){
		numVertexes=numParticles;
	}
	else if(mParticleType==ParticleType_SPRITE){
		numVertexes=numParticles*4;
	}
	else{
		// Use enough vertexes for 4 on each beam end and 2 in between
		numVertexes=(numParticles/mParticleType)*((mParticleType-2)*2+8);
	}

	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_BIT_WRITE,vertexFormat,numVertexes);
	mVertexData=VertexData::ptr(new VertexData(vertexBuffer));

	int numIndexes=0;
	if(mParticleType==ParticleType_SPRITE){
		numIndexes=numParticles*6;
	}
	else if(mParticleType>=ParticleType_BEAM){
		// Use enough indexes for a 6 on each beam end and 6 in between
		numIndexes=(numParticles/mParticleType)*((mParticleType-2)*2+6)*3;
	}

	if(numIndexes>0){
		IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,numVertexes<256?IndexBuffer::IndexFormat_UINT8:IndexBuffer::IndexFormat_UINT16,numIndexes);
		iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);

		if(mParticleType==ParticleType_SPRITE){
			for(i=0;i<numParticles;++i){
				ii=i*6;

				iba.set(ii+0,i*4+0);
				iba.set(ii+1,i*4+1);
				iba.set(ii+2,i*4+2);
				iba.set(ii+3,i*4+3);
				iba.set(ii+4,i*4+2);
				iba.set(ii+5,i*4+1);
			}
		}
		else if(mParticleType>=ParticleType_BEAM){
			int indexesPerBeam=((mParticleType-2)*2+6)*3;
			int vertexesPerBeam=((mParticleType-2)*2+8);
			for(i=0;i<numParticles/mParticleType;i++){
				ii=i*indexesPerBeam;
				vi=i*vertexesPerBeam;

				iba.set(ii+0,vi+0);
				iba.set(ii+1,vi+1);
				iba.set(ii+2,vi+2);
				iba.set(ii+3,vi+3);
				iba.set(ii+4,vi+2);
				iba.set(ii+5,vi+1);

				for(j=0;j<mParticleType-1;++j){
					iba.set(ii+j*6+6 ,vi+j*2+2);
					iba.set(ii+j*6+7 ,vi+j*2+3);
					iba.set(ii+j*6+8 ,vi+j*2+4);
					iba.set(ii+j*6+9 ,vi+j*2+5);
					iba.set(ii+j*6+10,vi+j*2+4);
					iba.set(ii+j*6+11,vi+j*2+3);
					
				}

				iba.set(ii+indexesPerBeam-6,vi+vertexesPerBeam-4);
				iba.set(ii+indexesPerBeam-5,vi+vertexesPerBeam-3);
				iba.set(ii+indexesPerBeam-4,vi+vertexesPerBeam-2);
				iba.set(ii+indexesPerBeam-3,vi+vertexesPerBeam-3);
				iba.set(ii+indexesPerBeam-2,vi+vertexesPerBeam-2);
				iba.set(ii+indexesPerBeam-1,vi+vertexesPerBeam-1);
			}
		}

		iba.unlock();

		mIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,numIndexes));
	}
	else{
		mIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_POINTS,NULL,0,numParticles));
	}
}

void ParticleNode::updateVertexBuffer(CameraNode *camera){
	if(mVertexData==NULL){
		return;
	}

	int i=0,j=0;
	Matrix4x4 invViewMatrix;
	Vector3 viewRight,viewUp,viewForward;
	if(camera->getAlignmentCalculationsUseOrigin()){
		Matrix4x4 lookAtCamera; Math::setMatrix4x4FromLookAt(lookAtCamera,camera->getWorldTranslate(),getWorldTranslate(),Math::Z_UNIT_VECTOR3,false);
		Math::invert(invViewMatrix,lookAtCamera);
	}
	else{
		Math::invert(invViewMatrix,camera->getViewMatrix());
	}
	Math::setAxesFromMatrix4x4(invViewMatrix,viewRight,viewUp,viewForward);

	if(mWorldSpace==false){
		mWorldTransform->inverseTransformNormal(viewRight);
		mWorldTransform->inverseTransformNormal(viewUp);
		mWorldTransform->inverseTransformNormal(viewForward);
	}

	scalar epsilon=mScene->getEpsilon();
	Vector3 right,up,forward;
	{
		vba.lock(mVertexData->getVertexBuffer(0),Buffer::Access_BIT_WRITE);

		if(mParticleType==ParticleType_POINTSPRITE){
			for(i=0;i<mParticles.size();++i){
				const Particle &p=mParticles[i];

				vba.set3(i+0,0,	p.x,p.y,p.z);
				vba.setABGR(i+0,1,	p.color);
				vba.set2(i+0,2,	p.s,p.y);
			}
		}
		else if(mParticleType==ParticleType_SPRITE){
			for(i=0;i<mParticles.size();++i){
				const Particle &p=mParticles[i];
				int vi=i*4;

				if(mVelocityAligned){
					up.set(p.vx,p.vy,p.vz);
					Math::normalizeCarefully(up,0);
					if(mWorldSpace==false){
						mWorldTransform->inverseTransformNormal(up);
					}
					Math::mul(up,p.scale);
					Math::mul(right,viewRight,p.scale);
				}
				else{
					Math::mul(up,viewUp,p.scale);
					Math::mul(right,viewRight,p.scale);
				}

				vba.set3(vi+0,0,	p.x+up.x-right.x, p.y+up.y-right.y, p.z+up.z-right.z);
				vba.setABGR(vi+0,1,	p.color);
				vba.set2(vi+0,2,	p.s,p.t);

				vba.set3(vi+1,0,	p.x-up.x-right.x, p.y-up.y-right.y, p.z-up.z-right.z);
				vba.setABGR(vi+1,1,	p.color);
				vba.set2(vi+1,2,	p.s,p.t2);

				vba.set3(vi+2,0,	p.x+up.x+right.x, p.y+up.y+right.y, p.z+up.z+right.z);
				vba.setABGR(vi+2,1,	p.color);
				vba.set2(vi+2,2,	p.s2,p.t);

				vba.set3(vi+3,0,	p.x-up.x+right.x, p.y-up.y+right.y, p.z-up.z+right.z);
				vba.setABGR(vi+3,1,	p.color);
				vba.set2(vi+3,2,	p.s2,p.t2);
			}
		}
		else{
			int vertexesPerBeam=((mParticleType-2)*2+8);
			for(i=0;i<mParticles.size()/mParticleType;i++){
				int pi=i*mParticleType;
				int vi=i*vertexesPerBeam;
				Particle &p=mParticles[pi];
				Particle &p1=mParticles[pi+1];
				Particle &pn=mParticles[pi+mParticleType-1];

				right.x=p1.x-p.x; right.y=p1.y-p.y; right.z=p1.z-p.z;

				Math::normalizeCarefully(right,epsilon);
				Math::cross(up,viewForward,right);
				if(Math::normalizeCarefully(up,epsilon)==false){
					up.set(Math::X_UNIT_VECTOR3);
				}
				Math::mul(up,p.scale);
				Math::mul(right,p.scale);

				vba.set3(vi+0,0,		p.x+up.x-right.x, p.y+up.y-right.y, p.z+up.z-right.z);
				vba.setABGR(vi+0,1,		p.color);
				vba.set2(vi+0,2,		0,0);

				vba.set3(vi+1,0,		p.x-up.x-right.x, p.y-up.y-right.y, p.z-up.z-right.z);
				vba.setABGR(vi+1,1,		p.color);
				vba.set2(vi+1,2,		0,Math::ONE);

				vba.set3(vi+2,0,		p.x+up.x, p.y+up.y, p.z+up.z);
				vba.setABGR(vi+2,1,		p.color);
				vba.set2(vi+2,2,		Math::QUARTER,0);

				vba.set3(vi+3,0,		p.x-up.x, p.y-up.y, p.z-up.z);
				vba.setABGR(vi+3,1,		p.color);
				vba.set2(vi+3,2,		Math::QUARTER,Math::ONE);


				for(j=0;j<mParticleType-2;++j){
					int ipi=pi+j+1;
					int ivi=vi+j*2+4;
					Particle &ip=mParticles[ipi];
					Particle &ip1=mParticles[ipi+1];

					right.x=ip1.x-ip.x; right.y=ip1.y-ip.y; right.z=ip1.z-ip.z;

					Math::cross(up,viewForward,right);
					if(Math::normalizeCarefully(up,epsilon)==false){
						up.set(Math::X_UNIT_VECTOR3);
					}
					Math::mul(up,p.scale);

					vba.set3(ivi+0,0,		ip.x+up.x, ip.y+up.y, ip.z+up.z);
					vba.setABGR(ivi+0,1,	p.color);
					vba.set2(ivi+0,2,		Math::QUARTER,0);

					vba.set3(ivi+1,0,		ip.x-up.x, ip.y-up.y, ip.z-up.z);
					vba.setABGR(ivi+1,1,	p.color);
					vba.set2(ivi+1,2,		Math::QUARTER,Math::ONE);
				}

				// We re-use the up from the previous particle pair,
				//  since we always use particle(n) and particle(n+1), on the final beam cap we
				//  only need to calculate the right, and only if we drew middle particles
				if(j>0){
					Math::normalizeCarefully(right,epsilon);
					Math::mul(right,p.scale);
				}

				vi=vi+vertexesPerBeam-4;
				vba.set3(vi+0,0,		pn.x+up.x, pn.y+up.y, pn.z+up.z);
				vba.setABGR(vi+0,1,		p.color);
				vba.set2(vi+0,2,		Math::THREE_QUARTERS,0);

				vba.set3(vi+1,0,		pn.x-up.x, pn.y-up.y, pn.z-up.z);
				vba.setABGR(vi+1,1,		p.color);
				vba.set2(vi+1,2,		Math::THREE_QUARTERS,Math::ONE);

				vba.set3(vi+2,0,		pn.x+up.x+right.x, pn.y+up.y+right.y, pn.z+up.z+right.z);
				vba.setABGR(vi+2,1,		p.color);
				vba.set2(vi+2,2,		Math::ONE,0);

				vba.set3(vi+3,0,		pn.x-up.x+right.x, pn.y-up.y+right.y, pn.z-up.z+right.z);
				vba.setABGR(vi+3,1,		p.color);
				vba.set2(vi+3,2,		Math::ONE,Math::ONE);
			}
		}

		vba.unlock();
	}
}

}
}
}
