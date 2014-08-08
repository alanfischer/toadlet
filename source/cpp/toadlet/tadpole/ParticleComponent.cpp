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

#include <toadlet/egg/System.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/RenderableSet.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/ParticleComponent.h>

namespace toadlet{
namespace tadpole{

ParticleComponent::ParticleComponent(Scene *scene):
	//mParticles,

	mParticleType(0),
	mParticleScale(Math::ONE),
	mBeamLength(0),
	mBeamCount(0),
	mBeamType(0),
	mNumIndexesPerBeam(0),mNumVertexesPerBeam(0),
	mWorldSpace(false),
	mManualUpdating(false),
	mVelocityAligned(false),
	mSorted(false),
	mRendered(true)
	//mMaterial,
	//mSharedRenderState,
	//mVertexData,
	//mIndexData,
{
	mScene=scene;
	mEngine=scene->getEngine();

	mTransform=new Transform();
	mTransform->setTransformListener(this);
	mBound=new Bound();
	mWorldTransform=new Transform();
	mWorldBound=new Bound();
}

void ParticleComponent::destroy(){
	mParticles.clear();
	mDistances.clear();
	mOrders.clear();

	mMaterial=NULL;

	if(mVertexData!=NULL){
		mVertexData->destroy();
		mVertexData=NULL;
	}
	
	if(mIndexData!=NULL){
		mIndexData->destroy();
		mIndexData=NULL;
	}

	if(mSharedRenderState!=NULL){
		mSharedRenderState->destroy();
		mSharedRenderState=NULL;
	}

	mUpdateCamera=NULL;

	BaseComponent::destroy();
}

void ParticleComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->spacialRemoved(this);
		mParent->visibleRemoved(this);
	}

	BaseComponent::parentChanged(node);

	if(mParent!=NULL){
		mParent->spacialAttached(this);
		mParent->visibleAttached(this);
	}
}

bool ParticleComponent::setNumParticles(int numParticles,int particleType,scalar scale,const Vector3 positions[]){
	if(particleType>=ParticleType_BEAM && (particleType&ParticleType_FLAGS)==0 && (numParticles%particleType)!=0){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"setNumParticles: Must specify a numParticles divisible by particleType");
		return false;
	}

	mParticles.resize(numParticles);

	mParticleType=particleType;
	mParticleScale=scale;
	mBeamLength=particleType&~ParticleType_FLAGS;
	if((particleType&ParticleType_BIT_SERIES)!=0){
		if((particleType&ParticleType_BIT_LOOP)!=0){
			mBeamCount=(numParticles-1)/(mBeamLength-1)+1;
		}
		else{
			mBeamCount=(numParticles-1)/(mBeamLength-1);
		}
	}
	else{
		mBeamCount=numParticles/Math::maxVal(mBeamLength,1);
	}

	if(mBeamLength>=2){
		if((particleType&ParticleType_BIT_SERIES)!=0 || (particleType&ParticleType_BIT_LOOP)==0){
			if((particleType&ParticleType_BIT_UNCAPPED)==0){
				mBeamType=0;
			}
			else{
				mBeamType=ParticleType_BIT_UNCAPPED;
			}
		}
		else{
			mBeamType=ParticleType_BIT_LOOP;
		}
	}

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

	createVertexData();
	createIndexData();

	updateBound();

	return true;
}

void ParticleComponent::setWorldSpace(bool worldSpace){
	if(mWorldSpace==worldSpace){
		return;
	}

	mWorldSpace=worldSpace;

	if(mParent!=NULL){
		Vector3 position;
		int i;
		if(mWorldSpace){
			for(i=0;i<mParticles.size();++i){
				Particle &p=mParticles[i];
				position.set(p.x,p.y,p.z);
				mParent->getWorldTransform()->transform(position);
				p.x=position.x;p.y=position.y;p.z=position.z;
			}
		}
		else{
			for(i=0;i<mParticles.size();++i){
				Particle &p=mParticles[i];
				position.set(p.x,p.y,p.z);
				mParent->getWorldTransform()->inverseTransform(position);
				p.x=position.x;p.y=position.y;p.z=position.z;
			}
		}
	}

	if(mWorldSpace && mParent!=NULL){
		mBound->inverseTransform(mBound,mParent->getWorldTransform());
	}

	transformChanged(mTransform);
}

void ParticleComponent::setSorted(bool sorted){
	mSorted=sorted;

	createIndexData();
}

void ParticleComponent::setMaterial(Material *material){
	mMaterial=material;

	if(mSharedRenderState!=NULL){
		mMaterial=mEngine->getMaterialManager()->createSharedMaterial(mMaterial,mSharedRenderState);
	}
}

void ParticleComponent::setTransform(Transform::ptr transform){
	if(transform==NULL){
		mTransform->reset();
	}
	else{
		mTransform->set(transform);
	}
}

void ParticleComponent::updateParticles(Camera *camera){
	mUpdateCamera=camera;

	updateBound();
	
	if(camera!=NULL){
		updateVertexData(camera);
	}
}

void ParticleComponent::updateBound(){
	bool dynamic=false;
	AABox box;

	if(mParticles.size()>0){
		const Particle &p=mParticles[0];
		box.set(p.x,p.y,p.z,p.x,p.y,p.z);
	}

	int i;
	for(i=0;i<mParticles.size() && !dynamic;++i){
		const Particle &p=mParticles[i];
		box.merge(Vector3(p.x,p.y,p.z));
		dynamic |= (p.vx!=0 || p.vy!=0 || p.vz!=0);
	}

	if(dynamic){
		mBound->setInfinite();
	}
	else{
		mBound->set(box);
	}

	if(mWorldSpace){
		mWorldBound->set(mBound);
		if(mParent!=NULL){
			mBound->inverseTransform(mBound,mParent->getWorldTransform());
		}
	}
	else{
		mWorldBound->transform(mBound,mWorldTransform);
	}

	if(mParent!=NULL){
		mParent->boundChanged();
	}
}

void ParticleComponent::frameUpdate(int dt,int scope){
	if(mUpdateCamera!=NULL && mParent->getScene()->getResetFrame()){
		updateVertexData(mUpdateCamera);
	}
}

void ParticleComponent::transformChanged(Transform *transform){
	if(mParent==NULL){
		return;
	}

	if(mWorldSpace){
		mWorldTransform->set(mTransform);
	}
	else{
		mWorldTransform->setTransform(mParent->getWorldTransform(),mTransform);
	}

	mWorldBound->transform(mBound,mWorldTransform);

	if(transform==mTransform){
		mParent->boundChanged();
	}
}

void ParticleComponent::setSharedRenderState(RenderState::ptr renderState){
	if(renderState==NULL){
		renderState=mEngine->getMaterialManager()->createRenderState();
		if(mMaterial!=NULL){
			mEngine->getMaterialManager()->modifyRenderState(renderState,mMaterial->getRenderState());
		}
	}

	mSharedRenderState=renderState;
	if(mMaterial!=NULL){
		mMaterial=mEngine->getMaterialManager()->createSharedMaterial(mMaterial,mSharedRenderState);
	}
}

void ParticleComponent::gatherRenderables(Camera *camera,RenderableSet *set){
	if(mRendered==false){
		return;
	}

	if(mManualUpdating==false){
		updateVertexData(camera);
	}

	if(mSorted){
		updateIndexData(camera);
	}

	set->queueRenderable(this);
}

void ParticleComponent::render(RenderManager *manager) const{
	if(mVertexData!=NULL && mIndexData!=NULL){
		manager->getDevice()->renderPrimitive(mVertexData,mIndexData);
	}
}

void ParticleComponent::createVertexData(){
	if(mVertexData!=NULL){
		mVertexData->destroy();
		mVertexData=NULL;
	}

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
		if(mBeamType==0){
			// Use enough vertexes for 4 on each beam end and 2 in between
			mNumVertexesPerBeam=((mBeamLength-2)*2+8);
		}
		else if(mBeamType==ParticleType_BIT_UNCAPPED){
			// Use enough vertexes for 2 on each beam end and 2 in between
			mNumVertexesPerBeam=((mBeamLength-2)*2+4);
		}
		else{
			// Loop
			mNumVertexesPerBeam=(mBeamLength*2);
		}
		numVertexes=mBeamCount*mNumVertexesPerBeam;
	}

	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_BIT_WRITE,vertexFormat,numVertexes);
	mVertexData=new VertexData(vertexBuffer);
}

void ParticleComponent::createIndexData(){
	int i,j,ii,vi,io,vo;

	if(mIndexData!=NULL){
		mIndexData->destroy();
		mIndexData=NULL;
	}

	int numParticles=mParticles.size();
	int numIndexes=0;
	if(mParticleType==ParticleType_POINTSPRITE){
		numIndexes=numParticles;
	}
	else if(mParticleType==ParticleType_SPRITE){
		numIndexes=numParticles*6;
	}
	else{
		if(mBeamType==0){
			// Use enough indexes for 6 on each beam end and 6 in between
			mNumIndexesPerBeam=((mBeamLength-2)*2+6)*3;
		}
		else if(mBeamType==ParticleType_BIT_UNCAPPED){
			// Use enough indexes for 3 on each beam end and 6 in between
			mNumIndexesPerBeam=((mBeamLength-2)*2+3)*3;
		}
		else{
			// Loop
			mNumIndexesPerBeam=(mBeamLength*2)*3;
		}
		numIndexes=mBeamCount*mNumIndexesPerBeam;
	}

	IndexBuffer::ptr indexBuffer=NULL;
	if(mParticleType!=ParticleType_POINTSPRITE || mSorted==true){
		indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mVertexData->getVertexBuffer(0)->getSize()<256?IndexBuffer::IndexFormat_UINT8:IndexBuffer::IndexFormat_UINT16,numIndexes);

		IndexBufferAccessor iba(indexBuffer,Buffer::Access_BIT_WRITE);

		if(mParticleType==ParticleType_POINTSPRITE){
			for(i=0;i<numParticles;++i){
				iba.set(i,i);
			}
		}
		else if(mParticleType==ParticleType_SPRITE){
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
		else{
			for(i=0;i<mBeamCount;i++){
				ii=i*mNumIndexesPerBeam;
				vi=i*mNumVertexesPerBeam;
				io=0;
				vo=0;

				if(mBeamType==0){
					iba.set(ii+0,vi+0);
					iba.set(ii+1,vi+1);
					iba.set(ii+2,vi+2);
					iba.set(ii+3,vi+3);
					iba.set(ii+4,vi+2);
					iba.set(ii+5,vi+1);
					io=6;
					vo=2;
				}

				for(j=0;j<mBeamLength-1;++j){
					iba.set(ii+j*6+io+0 ,vi+j*2+vo+0);
					iba.set(ii+j*6+io+1 ,vi+j*2+vo+1);
					iba.set(ii+j*6+io+2 ,vi+j*2+vo+2);
					iba.set(ii+j*6+io+3 ,vi+j*2+vo+3);
					iba.set(ii+j*6+io+4 ,vi+j*2+vo+2);
					iba.set(ii+j*6+io+5 ,vi+j*2+vo+1);
				}

				if(mBeamType==0){
					iba.set(ii+mNumIndexesPerBeam-6,vi+mNumVertexesPerBeam-4);
					iba.set(ii+mNumIndexesPerBeam-5,vi+mNumVertexesPerBeam-3);
					iba.set(ii+mNumIndexesPerBeam-4,vi+mNumVertexesPerBeam-2);
					iba.set(ii+mNumIndexesPerBeam-3,vi+mNumVertexesPerBeam-3);
					iba.set(ii+mNumIndexesPerBeam-2,vi+mNumVertexesPerBeam-2);
					iba.set(ii+mNumIndexesPerBeam-1,vi+mNumVertexesPerBeam-1);
				}
				else if(mBeamType==ParticleType_BIT_LOOP){
					iba.set(ii+mNumIndexesPerBeam-6,vi+mNumVertexesPerBeam-2);
					iba.set(ii+mNumIndexesPerBeam-5,vi+mNumVertexesPerBeam-1);
					iba.set(ii+mNumIndexesPerBeam-4,vi+0);
					iba.set(ii+mNumIndexesPerBeam-3,vi+mNumVertexesPerBeam-1);
					iba.set(ii+mNumIndexesPerBeam-2,vi+0);
					iba.set(ii+mNumIndexesPerBeam-1,vi+1);
				}
			}
		}

		iba.unlock();
	}

	mIndexData=new IndexData(mParticleType==ParticleType_POINTSPRITE?IndexData::Primitive_POINTS:IndexData::Primitive_TRIS,indexBuffer,0,numIndexes);

	if(mSorted){
		mDistances.resize(numParticles);
		mOrders.resize(numParticles);

		for(i=0;i<numParticles;++i){
			mOrders[i]=i;
		}
	}
	else{
		mDistances.resize(0);
		mOrders.resize(0);
	}
}

void ParticleComponent::updateVertexData(Camera *camera){
	if(mVertexData==NULL){
		return;
	}

	int i=0,j=0;
	Vector3 viewRight,viewUp,viewForward;
	Quaternion invRot;
	Math::invert(invRot,mParent->getWorldTransform()->getRotate());
	bool useOrigin=camera->getAlignmentCalculationsUseOrigin();
	if(useOrigin==false){
		Matrix4x4 invViewMatrix;
		Math::invert(invViewMatrix,camera->getViewMatrix());
		Math::setAxesFromMatrix4x4(invViewMatrix,viewRight,viewUp,viewForward);

		if(mWorldSpace==false){
			Math::mul(viewRight,invRot);
			Math::mul(viewUp,invRot);
			Math::mul(viewForward,invRot);
		}
	}

	int numParticles=mParticles.size();
	scalar epsilon=mScene->getEpsilon();
	Vector3 right,up,up2;
	{
		VertexBufferAccessor vba(mVertexData->getVertexBuffer(0),Buffer::Access_BIT_WRITE);

		if(mParticleType==ParticleType_POINTSPRITE){
			for(i=0;i<numParticles;++i){
				const Particle &p=mParticles[i];

				vba.set3(i+0,0,		p.x,p.y,p.z);
				vba.setRGBA(i+0,1,	p.color);
				vba.set2(i+0,2,		p.s,p.y);
			}
		}
		else if(mParticleType==ParticleType_SPRITE){
			for(i=0;i<numParticles;++i){
				const Particle &p=mParticles[i];
				int vi=i*4;
				scalar scale=p.scale/2;

				if(useOrigin){
					viewForward.set(p.x,p.y,p.z);
 					Math::sub(viewForward,camera->getPosition());
					Math::normalizeCarefully(viewForward,0);
					Math::cross(viewUp,camera->getRight(),viewForward);
					Math::normalizeCarefully(viewUp,0);
					Math::cross(viewRight,viewForward,viewUp);
				}

				if(mVelocityAligned){
					up.set(p.vx,p.vy,p.vz);
					Math::normalizeCarefully(up,0);
					if(mWorldSpace==false){
						Math::mul(up,invRot);
					}
					Math::mul(up,scale);
					Math::mul(right,viewRight,scale);
				}
				else{
					Math::mul(up,viewUp,scale);
					Math::mul(right,viewRight,scale);
				}

				vba.set3(vi+0,0,	p.x+up.x-right.x, p.y+up.y-right.y, p.z+up.z-right.z);
				vba.setRGBA(vi+0,1,	p.color);
				vba.set2(vi+0,2,	p.s,p.t);

				vba.set3(vi+1,0,	p.x-up.x-right.x, p.y-up.y-right.y, p.z-up.z-right.z);
				vba.setRGBA(vi+1,1,	p.color);
				vba.set2(vi+1,2,	p.s,p.t2);

				vba.set3(vi+2,0,	p.x+up.x+right.x, p.y+up.y+right.y, p.z+up.z+right.z);
				vba.setRGBA(vi+2,1,	p.color);
				vba.set2(vi+2,2,	p.s2,p.t);

				vba.set3(vi+3,0,	p.x-up.x+right.x, p.y-up.y+right.y, p.z-up.z+right.z);
				vba.setRGBA(vi+3,1,	p.color);
				vba.set2(vi+3,2,	p.s2,p.t2);
			}
		}
		else{
			for(i=0;i<mBeamCount;i++){
				int pi=i*(((mParticleType&ParticleType_BIT_SERIES)!=0) ? (mBeamLength-1) : mBeamLength);
				int vi=i*mNumVertexesPerBeam;
				int vo=0;
				Particle &p=mParticles[pi];
				Particle &p1=mParticles[(pi+1)%mParticles.size()];
				Particle &pn=mParticles[(pi+mBeamLength-1)%mParticles.size()];
				scalar scale=p.scale/2;

				right.x=p1.x-p.x; right.y=p1.y-p.y; right.z=p1.z-p.z;

				Math::normalizeCarefully(right,epsilon);
				Math::cross(up,viewForward,right);
				if(Math::normalizeCarefully(up,epsilon)==false){
					up.set(Math::X_UNIT_VECTOR3);
				}
				Math::mul(up,scale);
				Math::mul(right,scale);

				if(mBeamType==0){
					vba.set3(vi+0,0,		p.x+up.x-right.x, p.y+up.y-right.y, p.z+up.z-right.z);
					vba.setRGBA(vi+0,1,		p.color);
					vba.set2(vi+0,2,		0,0);

					vba.set3(vi+1,0,		p.x-up.x-right.x, p.y-up.y-right.y, p.z-up.z-right.z);
					vba.setRGBA(vi+1,1,		p.color);
					vba.set2(vi+1,2,		0,Math::ONE);

					vo=2;
				}

				vba.set3(vi+vo+0,0,		p.x+up.x, p.y+up.y, p.z+up.z);
				vba.setRGBA(vi+vo+0,1,	p.color);
				vba.set2(vi+vo+0,2,		Math::QUARTER,0);

				vba.set3(vi+vo+1,0,		p.x-up.x, p.y-up.y, p.z-up.z);
				vba.setRGBA(vi+vo+1,1,	p.color);
				vba.set2(vi+vo+1,2,		Math::QUARTER,Math::ONE);

				for(j=0;j<mBeamLength-2;++j){
					int ipi=pi+j+1;
					int ivi=vi+j*2+vo+2;
					Particle &ip=mParticles[ipi];
					Particle &ip1=mParticles[ipi+1];

					right.x=ip1.x-ip.x; right.y=ip1.y-ip.y; right.z=ip1.z-ip.z;

					Math::cross(up2,viewForward,right);
					if(Math::normalizeCarefully(up2,epsilon)==false){
						up2.set(Math::X_UNIT_VECTOR3);
					}
					Math::mul(up2,scale);

					Math::add(up,up2);
					if(Math::normalizeCarefully(up,epsilon)==false){
						up.set(Math::X_UNIT_VECTOR3);
					}
					Math::mul(up,scale);

					vba.set3(ivi+0,0,		ip.x+up.x, ip.y+up.y, ip.z+up.z);
					vba.setRGBA(ivi+0,1,	p.color);
					vba.set2(ivi+0,2,		Math::QUARTER,0);

					vba.set3(ivi+1,0,		ip.x-up.x, ip.y-up.y, ip.z-up.z);
					vba.setRGBA(ivi+1,1,	p.color);
					vba.set2(ivi+1,2,		Math::QUARTER,Math::ONE);

					up.set(up2);
				}

				if(mBeamType==ParticleType_BIT_LOOP){
					right.x=p.x-pn.x; right.y=p.y-pn.y; right.z=p.z-pn.z;

					Math::cross(up,viewForward,right);
					if(Math::normalizeCarefully(up,epsilon)==false){
						up.set(Math::X_UNIT_VECTOR3);
					}
					Math::mul(up,scale);
				}
				else{
					// We re-use the up from the previous particle pair,
					//  since we always use particle(n) and particle(n+1), on the final beam cap we
					//  only need to calculate the right, and only if we drew middle particles
					if(j>0){
						Math::normalizeCarefully(right,epsilon);
						Math::mul(right,scale);
					}
				}

				vi=vi+mNumVertexesPerBeam-(vo+2);

				vba.set3(vi+0,0,		pn.x+up.x, pn.y+up.y, pn.z+up.z);
				vba.setRGBA(vi+0,1,		p.color);
				vba.set2(vi+0,2,		Math::THREE_QUARTERS,0);

				vba.set3(vi+1,0,		pn.x-up.x, pn.y-up.y, pn.z-up.z);
				vba.setRGBA(vi+1,1,		p.color);
				vba.set2(vi+1,2,		Math::THREE_QUARTERS,Math::ONE);

				if(mBeamType==0){
					vba.set3(vi+2,0,		pn.x+up.x+right.x, pn.y+up.y+right.y, pn.z+up.z+right.z);
					vba.setRGBA(vi+2,1,		p.color);
					vba.set2(vi+2,2,		Math::ONE,0);

					vba.set3(vi+3,0,		pn.x-up.x+right.x, pn.y-up.y+right.y, pn.z-up.z+right.z);
					vba.setRGBA(vi+3,1,		p.color);
					vba.set2(vi+3,2,		Math::ONE,Math::ONE);
				}
			}
		}

		vba.unlock();
	}
}

void ParticleComponent::updateIndexData(Camera *camera){
	int i,j,ii;

	if(mIndexData==NULL){
		return;
	}

	Vector3 cameraPosition=camera->getPosition();
	if(mWorldSpace==false){
		Math::sub(cameraPosition,mParent->getWorldTransform()->getTranslate());
	}

	for(i=0;i<mParticles.size();++i){
		const Particle &p=mParticles[i];
		mDistances[i]=Math::lengthSquared(cameraPosition,Vector3(p.x,p.y,p.z));
	}

	bool swapped=false;
	int numParticles=mParticles.size();
	for(i=0;i<numParticles;++i){
		swapped=false;
		for(j=i;j<numParticles-1;++j){
			if(mDistances[mOrders[j]] < mDistances[mOrders[j+1]]){
				int temp=mOrders[j+1];
				mOrders[j+1]=mOrders[j];
				mOrders[j]=temp;
				swapped=true;
			}
		}
		if(swapped==false){
			break;
		}
	}

	{
		IndexBufferAccessor iba(mIndexData->getIndexBuffer(),Buffer::Access_BIT_WRITE);

		if(mParticleType==ParticleType_POINTSPRITE){
			for(i=0;i<numParticles;++i){
				iba.set(i,mOrders[i]);
			}
		}
		else if(mParticleType==ParticleType_SPRITE){
			for(i=0;i<numParticles;++i){
				ii=i*6;

				j=mOrders[i];

				iba.set(ii+0,j*4+0);
				iba.set(ii+1,j*4+1);
				iba.set(ii+2,j*4+2);
				iba.set(ii+3,j*4+3);
				iba.set(ii+4,j*4+2);
				iba.set(ii+5,j*4+1);
			}
		}

		iba.unlock();
	}
}

}
}
