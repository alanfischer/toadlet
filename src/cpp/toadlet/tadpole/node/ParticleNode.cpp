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
#include <toadlet/tadpole/node/ParticleNode.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/SceneNode.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(ParticleNode,Categories::TOADLET_TADPOLE_NODE+".ParticleNode");

ParticleNode::ParticleNode():super(),
	TOADLET_GIB_IMPLEMENT()

	mDestroyNextLogicFrame(false),
	
	mWorldSpace(false),
	mOrientation(false),
	mIndividualAttributes(false),
	mParticlesPerBeam(0),
	mDestroyOnFinish(false),
	mHadPoints(false),
	mHadAges(false),
	mHasColor(false),
	mHasScale(false),
	mLineBeams(false),

	//mParticles,

	mRandomStartColor(false),
	//mStartColor,
	//mEndColor,
	mStartScale(0),
	mEndScale(0),

	//mTextureSection,
	//mMaterial,
	//mVertexBuffer,
	//mVertexData,
	//mIndexBuffer,
	//mIndexData,
	//mLineIndexData,

	//mWorldTranslate,
	//mWorldRotate,

	mUpdateParticles(false),
	//mParticleSimulator,

	mHasIdealViewTransform(false),
	//mIdealViewTransform,

	mEpsilon(0)
{}

Node *ParticleNode::create(Scene *scene){
	super::create(scene);

	mDestroyNextLogicFrame=false;

	mWorldSpace=false;
	mOrientation=false;
	mIndividualAttributes=false;
	mParticlesPerBeam=0;
	mDestroyOnFinish=false;
	mHadPoints=false;
	mHadAges=false;
	mHasColor=false;
	mHasScale=false;
	mLineBeams=false;

	mRandomStartColor=false;
	mStartColor.reset();
	mEndColor.reset();
	mStartScale=0;
	mEndScale=0;

//	mTextureSection=NULL;
	mMaterial=NULL;
	mVertexBuffer=NULL;
	mVertexData=NULL;
	mIndexBuffer=NULL;
	mIndexData=NULL;
	mLineIndexData=NULL;

	mWorldTransform.reset();

	mUpdateParticles=true;
	mParticleSimulator=NULL;

	mHasIdealViewTransform=false;
	mIdealViewTransform.reset();

	mEpsilon=
		#if defined(TOADLET_FIXED_POINT)
			512;
		#else
			0.01f;
		#endif

	mRandom.setSeed(System::mtime());

	return this;
}

void ParticleNode::destroy(){
	stopSimulating();

	if(mVertexBuffer!=NULL){
		mVertexBuffer->destroy();
		mVertexBuffer=NULL;
	}
	
	if(mIndexBuffer!=NULL){
		mIndexBuffer->destroy();
		mIndexBuffer=NULL;
	}

	if(mMaterial!=NULL){
		mMaterial->release();
		mMaterial=NULL;
	}

	super::destroy();
}

bool ParticleNode::start(int particlesPerBeam,int numParticles,bool hasColor,const Vector3 points[],const scalar ages[],scalar defaultAge,bool visible){
	if(particlesPerBeam>1 && (numParticles%particlesPerBeam)!=0){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"startParticles: Must specify a number of particles divisible by particlesPerBeam");
		return false;
	}

	mParticlesPerBeam=particlesPerBeam;

	int numFrames=1;
//	if(mTextureSection!=NULL){
//		numFrames=mTextureSection->getNumFrames();
//	}

	uint32 startColor=mStartColor.getABGR();
	uint32 endColor=mEndColor.getABGR();
	scalar startScale=mStartScale/2;
	scalar endScale=mEndScale/2;

	mParticles.resize(numParticles);

	int i;
	if(points!=NULL){
		mLocalBound.radius=0;
		if(ages!=NULL){
			for(i=0;i<numParticles;++i){
				Particle &p=mParticles[i];
				const Vector3 &point=points[i];

				p.x=point.x;
				p.y=point.y;
				p.z=point.z;
				p.frame=i%numFrames;
				p.startColor=mRandomStartColor?randomColor():startColor;
				p.endColor=endColor;
				p.startScale=startScale;
				p.endScale=endScale;
				p.age=ages[i];
				p.visible=visible;

				scalar ls=Math::lengthSquared(point);
				if(mLocalBound.radius<ls) mLocalBound.radius=ls;
			}
		}
		else{
			for(i=0;i<numParticles;++i){
				Particle &p=mParticles[i];
				const Vector3 &point=points[i];

				p.x=point.x;
				p.y=point.y;
				p.z=point.z;
				p.frame=i%numFrames;
				p.startColor=mRandomStartColor?randomColor():startColor;
				p.endColor=endColor;
				p.startScale=startScale;
				p.endScale=endScale;
				p.age=defaultAge>=0?defaultAge:Math::div(Math::fromInt(i),Math::fromInt(numParticles));
				p.visible=visible;

				scalar ls=Math::lengthSquared(point);
				if(mLocalBound.radius<ls) mLocalBound.radius=ls;
			}
		}
		mLocalBound.radius=Math::sqrt(mLocalBound.radius);
	}
	else if(ages!=NULL){
		for(i=0;i<numParticles;++i){
			Particle &p=mParticles[i];

			p.x=0;
			p.y=0;
			p.z=0;
			p.frame=i%numFrames;
			p.startColor=mRandomStartColor?randomColor():startColor;
			p.endColor=endColor;
			p.startScale=startScale;
			p.endScale=endScale;
			p.age=ages[i];
			p.visible=visible;
		}
	}
	else{
		for(i=0;i<numParticles;++i){
			Particle &p=mParticles[i];
			p.x=0;
			p.y=0;
			p.z=0;
			p.frame=i%numFrames;
			p.startColor=mRandomStartColor?randomColor():startColor;
			p.endColor=endColor;
			p.startScale=startScale;
			p.endScale=endScale;
			p.age=defaultAge>=0?defaultAge:Math::div(Math::fromInt(i),Math::fromInt(numParticles));
			p.visible=visible;
		}
	}

	mHadPoints=(points!=NULL);
	mHadAges=(ages!=NULL) || defaultAge!=0;
	mHasColor=hasColor;
	mHasScale=false;

	createVertexBuffer();

	mUpdateParticles=true;

	return true;
}

int ParticleNode::randomColor(){
	return 0xFF000000 | (mRandom.nextInt(0xFF)<<16) | (mRandom.nextInt(0xFF)<<8) | mRandom.nextInt(0xFF);
}

void ParticleNode::setStartColor(const Color &startColor){
	mStartColor.set(startColor);

	mUpdateParticles=true;
}

void ParticleNode::setEndColor(const Color &endColor){
	mEndColor.set(endColor);

	mUpdateParticles=true;
}

void ParticleNode::setColor(const Color &startColor,const Color &endColor){
	mStartColor.set(startColor);
	mEndColor.set(endColor);

	mUpdateParticles=true;
}

void ParticleNode::setColor(const Color &color){
	mStartColor.set(color);
	mEndColor.set(color);

	mUpdateParticles=true;
}

void ParticleNode::setStartScale(scalar startScale){
	mStartScale=startScale;

	mUpdateParticles=true;
}

void ParticleNode::setEndScale(scalar endScale){
	mEndScale=endScale;

	mUpdateParticles=true;
}

void ParticleNode::setScale(scalar startScale,scalar endScale){
	mStartScale=startScale;
	mEndScale=endScale;

	mUpdateParticles=true;
}

void ParticleNode::setScale(scalar scale){
	mStartScale=scale;
	mEndScale=scale;

	mUpdateParticles=true;
}

void ParticleNode::setWorldSpace(bool worldSpace){
	mWorldSpace=worldSpace;

	updateWorldTransform(this);
	Matrix4x4 &m=mWorldTransform;
	int i;
	for(i=0;i<mParticles.size();++i){
		Particle &p=mParticles[i];
		scalar x=Math::mul(m.at(0,0),p.x) + Math::mul(m.at(0,1),p.y) + Math::mul(m.at(0,2),p.z) + m.at(0,3);
		scalar y=Math::mul(m.at(1,0),p.x) + Math::mul(m.at(1,1),p.y) + Math::mul(m.at(1,2),p.z) + m.at(1,3);
		scalar z=Math::mul(m.at(2,0),p.x) + Math::mul(m.at(2,1),p.y) + Math::mul(m.at(2,2),p.z) + m.at(2,3);
		p.x=x;
		p.y=y;
		p.z=z;
	}
}

void ParticleNode::setMaterial(const egg::String &name){
	setMaterial(mEngine->getMaterialManager()->findMaterial(name));
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

void ParticleNode::startSimulating(ParticleSimulator::ptr particleSimulator){
	if(particleSimulator==NULL){
//		particleSimulator=mEngine->getScene()->newParticleSimulator(this);
	}
	mParticleSimulator=particleSimulator;

	// Disable the bounding radius, cause they may very well bounce out of it
	mLocalBound.radius=-Math::ONE;

	setReceiveUpdates(true);
}

void ParticleNode::stopSimulating(){
	setReceiveUpdates(false);

	mParticleSimulator=NULL;
}

void ParticleNode::logicUpdate(int dt){
	super::logicUpdate(dt);

	if(mDestroyNextLogicFrame){
		destroy();
	}
}
	
void ParticleNode::renderUpdate(int dt){
	if(mParticleSimulator!=NULL){
		if(mWorldSpace){
			Math::mul(mWorldRenderTransform,mParent->getWorldRenderTransform(),mRenderTransform);
			mParticleSimulator->updateParticles(dt,mWorldRenderTransform);
		}
		else{
			mParticleSimulator->updateParticles(dt,mRenderTransform);
		}
		mUpdateParticles=true;
	}
}
	
void ParticleNode::queueRenderable(SceneNode *scene,CameraNode *camera){
	if(mUpdateParticles){
		if(mHasIdealViewTransform){
			updateVertexBuffer(mIdealViewTransform);
		}
		else{
//			updateVertexBuffer(scene->getIdealParticleViewTransform());
		}
		mUpdateParticles=false;
	}

#if defined(TOADLET_GCC_INHERITANCE_BUG)
	scene->queueRenderable(&renderable);
#else
	scene->queueRenderable(this);
#endif
}

const Matrix4x4 &ParticleNode::getRenderTransform() const{
	if(mWorldSpace){
		return Math::IDENTITY_MATRIX4X4;
	}
	else{
		return super::getWorldRenderTransform();
	}
}

void ParticleNode::render(Renderer *renderer) const{
	renderer->renderPrimitive(mVertexData,mLineBeams?mLineIndexData:mIndexData);
}

void ParticleNode::updateWorldTransform(Node *node){
	if(node->getParent()==NULL){
		Matrix3x3 &temp=cache_updateWorldTransform_temp3;
		Math::setMatrix3x3FromQuaternion(temp,node->getRotate());
		Math::setMatrix4x4FromTranslateRotateScale(mWorldTransform,node->getTranslate(),temp,node->getScale());
	}
	else{
		updateWorldTransform(node->getParent());

		if(node->isIdentityTransform()==false){
			Matrix4x4 &temp1=cache_updateWorldTransform_temp1;
			Matrix4x4 &temp2=cache_updateWorldTransform_temp2;
			Matrix3x3 &temp3=cache_updateWorldTransform_temp3;

			Math::setMatrix3x3FromQuaternion(temp3,node->getRotate());
			Math::setMatrix4x4FromTranslateRotateScale(temp1,node->getTranslate(),temp3,node->getScale());
			Math::mul(temp2,mWorldTransform,temp1);
			mWorldTransform.set(temp2);
		}
	}
}

void ParticleNode::createVertexBuffer(){
	int i,j,ii,vi;

	int numParticles=mParticles.size();

	VertexFormat::ptr vertexFormat=mHasColor?mEngine->getVertexFormats().POSITION_COLOR_TEX_COORD:mEngine->getVertexFormats().POSITION_TEX_COORD;

	int numVertexes=0;
	if(mParticlesPerBeam<2){
		numVertexes=numParticles*4;
	}
	else{
		// Use enough vertexes for 4 on each beam end and 2 in between
		numVertexes=(numParticles/mParticlesPerBeam)*((mParticlesPerBeam-2)*2+8);
	}

	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::UsageFlags_DYNAMIC,Buffer::AccessType_WRITE_ONLY,vertexFormat,numVertexes);
	mVertexData=VertexData::ptr(new VertexData(vertexBuffer));

	int numIndexes=0;
	if(mParticlesPerBeam<2){
		numIndexes=numParticles*6;
	}
	else{
		// Use enough indexes for a 6 on each beam end and 6 in between
		numIndexes=(numParticles/mParticlesPerBeam)*((mParticlesPerBeam-2)*2+6)*3;
	}

	IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_WRITE_ONLY,numVertexes<256?IndexBuffer::IndexFormat_UINT_8:IndexBuffer::IndexFormat_UINT_16,numIndexes);
	mIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,numIndexes));
	mLineIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_LINES,NULL,0,numParticles));

	{
		iba.lock(indexBuffer,Buffer::AccessType_WRITE_ONLY);

		if(mParticlesPerBeam<2){
			for(i=0;i<numParticles;++i){
				ii=i*6;

				iba.set(ii+0,i*4+0);
				iba.set(ii+1,i*4+1);
				iba.set(ii+2,i*4+2);
				iba.set(ii+3,i*4+2);
				iba.set(ii+4,i*4+3);
				iba.set(ii+5,i*4+1);
			}
		}
		else{
			int indexesPerBeam=((mParticlesPerBeam-2)*2+6)*3;
			int vertexesPerBeam=((mParticlesPerBeam-2)*2+8);
			for(i=0;i<numParticles/mParticlesPerBeam;i++){
				ii=i*indexesPerBeam;
				vi=i*vertexesPerBeam;

				iba.set(ii+0,vi+0);
				iba.set(ii+1,vi+1);
				iba.set(ii+2,vi+2);
				iba.set(ii+3,vi+2);
				iba.set(ii+4,vi+3);
				iba.set(ii+5,vi+1);

				for(j=0;j<mParticlesPerBeam-1;++j){
					iba.set(ii+j*6+6 ,vi+j*2+2);
					iba.set(ii+j*6+7 ,vi+j*2+3);
					iba.set(ii+j*6+8 ,vi+j*2+4);
					iba.set(ii+j*6+9 ,vi+j*2+4);
					iba.set(ii+j*6+10,vi+j*2+5);
					iba.set(ii+j*6+11,vi+j*2+3);
					
				}

				iba.set(ii+indexesPerBeam-6,vi+vertexesPerBeam-4);
				iba.set(ii+indexesPerBeam-5,vi+vertexesPerBeam-3);
				iba.set(ii+indexesPerBeam-4,vi+vertexesPerBeam-2);
				iba.set(ii+indexesPerBeam-3,vi+vertexesPerBeam-2);
				iba.set(ii+indexesPerBeam-2,vi+vertexesPerBeam-1);
				iba.set(ii+indexesPerBeam-1,vi+vertexesPerBeam-3);
			}
		}

		iba.unlock();
	}
}

void ParticleNode::updateVertexBuffer(const Matrix4x4 &viewTransform){
	TOADLET_PROFILE_BEGINSECTION(ParticleNode::updateVertexBuffer);

	int i=0,j=0;

	Vector3 &viewUp=cache_updateVertexBuffer_viewUp.reset(),&viewRight=cache_updateVertexBuffer_viewRight.reset(),&viewForward=cache_updateVertexBuffer_viewForward.reset();
	Math::setAxesFromMatrix4x4(viewTransform,viewRight,viewUp,viewForward);

	Vector3 &up=cache_updateVertexBuffer_up.reset(),&right=cache_updateVertexBuffer_right.reset(),&forward=cache_updateVertexBuffer_forward.reset();
	uint32 startColor=mStartColor.getABGR();
	uint32 endColor=mEndColor.getABGR();
	int color=startColor;
	scalar startScale=mStartScale/2;
	scalar endScale=mEndScale/2;
	scalar scale=startScale;
	{
		vba.lock(mVertexData->getVertexBuffer(0),Buffer::AccessType_WRITE_ONLY);

		if(mParticlesPerBeam<2){
			int frameOffset=0,widthFrames=1,heightFrames=1;
//			if(mTextureSection!=NULL){
//				frameOffset=mTextureSection->getFrameOffset();
//				widthFrames=mTextureSection->getWidthFrames();
//				heightFrames=mTextureSection->getHeightFrames();
//			}

			for(i=mParticles.size()-1;i>=0;--i){
				const Particle &p=mParticles[i];

				if(p.visible==false){
					color=0x00000000;
					if(mHasScale) scale=0;
				}
				else if(mIndividualAttributes){
					color=Color::lerp(p.startColor,p.endColor,p.age);
					if(mHasScale) scale=Math::lerp(p.startScale,p.endScale,p.age);
				}
				else{
					color=Color::lerp(startColor,endColor,p.age);
					if(mHasScale) scale=Math::lerp(startScale,endScale,p.age);
				}

				if(mOrientation){
					up.set(p.ox,p.oy,p.oz);
					if(mWorldSpace==false){
						Math::mul(up,mWorldRenderTransform);
					}
					Math::mul(up,scale);
					Math::mul(right,viewRight,scale);
				}
				else{
					Math::mul(up,viewUp,scale);
					Math::mul(right,viewRight,scale);
				}

				scalar tx0,ty0,tx1,ty1;

				int v=(p.frame+frameOffset)%widthFrames;
				scalar s=Math::div(Math::ONE,Math::fromInt(widthFrames));
				tx0=Math::mul(Math::fromInt(v),s);
				tx1=tx0+s;

				v=((p.frame+frameOffset)/widthFrames)%heightFrames;
				s=Math::div(Math::ONE,Math::fromInt(heightFrames));
				ty0=Math::mul(Math::fromInt(v),s);
				ty1=ty0+s;

				int vi=i*4;
				if(mHasColor){
					vba.set3(vi+0,0,	p.x+up.x-right.x, p.y+up.y-right.y, p.z+up.z-right.z);
					vba.setABGR(vi+0,1,	color);
					vba.set2(vi+0,2,	tx0,ty0);

					vba.set3(vi+1,0,	p.x-up.x-right.x, p.y-up.y-right.y, p.z-up.z-right.z);
					vba.setABGR(vi+1,1,	color);
					vba.set2(vi+1,2,	tx0,ty1);

					vba.set3(vi+2,0,	p.x+up.x+right.x, p.y+up.y+right.y, p.z+up.z+right.z);
					vba.setABGR(vi+2,1,	color);
					vba.set2(vi+2,2,	tx1,ty0);

					vba.set3(vi+3,0,	p.x-up.x+right.x, p.y-up.y+right.y, p.z-up.z+right.z);
					vba.setABGR(vi+3,1,	color);
					vba.set2(vi+3,2,	tx1,ty1);
				}
				else{
					vba.set3(vi+0,0,	p.x+up.x-right.x, p.y+up.y-right.y, p.z+up.z-right.z);
					vba.set2(vi+0,1,	tx0,ty0);

					vba.set3(vi+1,0,	p.x-up.x-right.x, p.y-up.y-right.y, p.z-up.z-right.z);
					vba.set2(vi+1,1,	tx0,ty1);

					vba.set3(vi+2,0,	p.x+up.x+right.x, p.y+up.y+right.y, p.z+up.z+right.z);
					vba.set2(vi+2,1,	tx1,ty0);

					vba.set3(vi+3,0,	p.x-up.x+right.x, p.y-up.y+right.y, p.z-up.z+right.z);
					vba.set2(vi+3,1,	tx1,ty1);
				}
			}
		}
		else if(!mLineBeams){
			int vertexesPerBeam=((mParticlesPerBeam-2)*2+8);
			if(mHasColor){
				for(i=0;i<mParticles.size()/mParticlesPerBeam;i++){
					int pi=i*mParticlesPerBeam;
					int vi=i*vertexesPerBeam;
					Particle &p=mParticles[pi];
					Particle &p1=mParticles[pi+1];
					Particle &pn=mParticles[pi+mParticlesPerBeam-1];

					if(p.visible==false){
						color=0x00000000;
						if(mHasScale) scale=0;
					}
					else if(mIndividualAttributes){
						color=Color::lerp(p.startColor,p.endColor,p.age);
						if(mHasScale) scale=Math::lerp(p.startScale,p.endScale,p.age);
					}
					else{
						color=Color::lerp(startColor,endColor,p.age);
						if(mHasScale) scale=Math::lerp(startScale,endScale,p.age);
					}

					right.x=p1.x-p.x; right.y=p1.y-p.y; right.z=p1.z-p.z;

					Math::normalizeCarefully(right,mEpsilon);
					Math::cross(up,viewForward,right);
					if(Math::normalizeCarefully(up,mEpsilon)==false){
						up.set(Math::X_UNIT_VECTOR3);
					}
					Math::mul(up,scale);
					Math::mul(right,scale);


					vba.set3(vi+0,0,		p.x+up.x-right.x, p.y+up.y-right.y, p.z+up.z-right.z);
					vba.setABGR(vi+0,1,		color);
					vba.set2(vi+0,2,		0,0);

					vba.set3(vi+1,0,		p.x-up.x-right.x, p.y-up.y-right.y, p.z-up.z-right.z);
					vba.setABGR(vi+1,1,		color);
					vba.set2(vi+1,2,		0,Math::ONE);

					vba.set3(vi+2,0,		p.x+up.x, p.y+up.y, p.z+up.z);
					vba.setABGR(vi+2,1,		color);
					vba.set2(vi+2,2,		Math::QUARTER,0);

					vba.set3(vi+3,0,		p.x-up.x, p.y-up.y, p.z-up.z);
					vba.setABGR(vi+3,1,		color);
					vba.set2(vi+3,2,		Math::QUARTER,Math::ONE);


					for(j=0;j<mParticlesPerBeam-2;++j){
						int ipi=pi+j+1;
						int ivi=vi+j*2+4;
						Particle &ip=mParticles[ipi];
						Particle &ip1=mParticles[ipi+1];

						if(ip.visible==false){
							color=0x00000000;
							if(mHasScale) scale=0;
						}
						else if(mIndividualAttributes){
							color=Color::lerp(ip.startColor,ip.endColor,ip.age);
							if(mHasScale) scale=Math::lerp(ip.startScale,ip.endScale,ip.age);
						}
						else{
							color=Color::lerp(startColor,endColor,ip.age);
							if(mHasScale) scale=Math::lerp(startScale,endScale,ip.age);
						}

						right.x=ip1.x-ip.x; right.y=ip1.y-ip.y; right.z=ip1.z-ip.z;

						Math::cross(up,viewForward,right);
						if(Math::normalizeCarefully(up,mEpsilon)==false){
							up.set(Math::X_UNIT_VECTOR3);
						}
						Math::mul(up,scale);

						vba.set3(ivi+0,0,		ip.x+up.x, ip.y+up.y, ip.z+up.z);
						vba.setABGR(ivi+0,1,	color);
						vba.set2(ivi+0,2,		Math::QUARTER,0);

						vba.set3(ivi+1,0,		ip.x-up.x, ip.y-up.y, ip.z-up.z);
						vba.setABGR(ivi+1,1,	color);
						vba.set2(ivi+1,2,		Math::QUARTER,Math::ONE);
					}


					if(pn.visible==false){
						color=0x00000000;
						if(mHasScale) scale=0;
					}
					else if(mIndividualAttributes){
						color=Color::lerp(pn.startColor,pn.endColor,pn.age);
						if(mHasScale) scale=Math::lerp(pn.startScale,pn.endScale,pn.age);
					}
					else{
						color=Color::lerp(startColor,endColor,pn.age);
						if(mHasScale) scale=Math::lerp(startScale,endScale,pn.age);
					}

					// We re-use the up from the previous particle pair,
					//  since we always use particle(n) and particle(n+1), on the final beam cap we
					//  only need to calculate the right, and only if we drew middle particles
					if(j>0){
						Math::normalizeCarefully(right,mEpsilon);
						Math::mul(right,scale);
					}

					vi=vi+vertexesPerBeam-4;
					vba.set3(vi+0,0,		pn.x+up.x, pn.y+up.y, pn.z+up.z);
					vba.setABGR(vi+0,1,		color);
					vba.set2(vi+0,2,		Math::THREE_QUARTERS,0);

					vba.set3(vi+1,0,		pn.x-up.x, pn.y-up.y, pn.z-up.z);
					vba.setABGR(vi+1,1,		color);
					vba.set2(vi+1,2,		Math::THREE_QUARTERS,Math::ONE);

					vba.set3(vi+2,0,		pn.x+up.x+right.x, pn.y+up.y+right.y, pn.z+up.z+right.z);
					vba.setABGR(vi+2,1,		color);
					vba.set2(vi+2,2,		Math::ONE,0);

					vba.set3(vi+3,0,		pn.x-up.x+right.x, pn.y-up.y+right.y, pn.z-up.z+right.z);
					vba.setABGR(vi+3,1,		color);
					vba.set2(vi+3,2,		Math::ONE,Math::ONE);
				}
			}
			else{
				for(i=0;i<mParticles.size()/mParticlesPerBeam;i++){
					int pi=i*mParticlesPerBeam;
					int vi=i*vertexesPerBeam;
					Particle &p=mParticles[pi];
					Particle &p1=mParticles[pi+1];
					Particle &pn=mParticles[pi+mParticlesPerBeam-1];

					if(mHasScale){
						if(p.visible==false){
							scale=0;
						}
						else if(mIndividualAttributes){
							scale=Math::lerp(p.startScale,p.endScale,p.age);
						}
						else{
							scale=Math::lerp(startScale,endScale,p.age);
						}
					}

					right.x=p1.x-p.x; right.y=p1.y-p.y; right.z=p1.z-p.z;

					Math::normalizeCarefully(right,mEpsilon);
					Math::cross(up,viewForward,right);
					if(Math::normalizeCarefully(up,mEpsilon)==false){
						up.set(Math::X_UNIT_VECTOR3);
					}
					Math::mul(up,scale);
					Math::mul(right,scale);

					vba.set3(vi+0,0,		p.x+up.x-right.x, p.y+up.y-right.y, p.z+up.z-right.z);
					vba.set2(vi+0,1,		0,0);

					vba.set3(vi+1,0,		p.x-up.x-right.x, p.y-up.y-right.y, p.z-up.z-right.z);
					vba.set2(vi+1,1,		0,Math::ONE);

					vba.set3(vi+2,0,		p.x+up.x, p.y+up.y, p.z+up.z);
					vba.set2(vi+2,1,		Math::QUARTER,0);

					vba.set3(vi+3,0,		p.x-up.x, p.y-up.y, p.z-up.z);
					vba.set2(vi+3,1,		Math::QUARTER,Math::ONE);


					for(j=0;j<mParticlesPerBeam-2;++j){
						int ipi=pi+j+1;
						int ivi=vi+j*2+4;
						Particle &ip=mParticles[ipi];
						Particle &ip1=mParticles[ipi+1];

						if(mHasScale){
							if(ip.visible==false){
								scale=0;
							}
							else if(mIndividualAttributes){
								scale=Math::lerp(ip.startScale,ip.endScale,ip.age);
							}
							else{
								scale=Math::lerp(startScale,endScale,ip.age);
							}
						}

						right.x=ip1.x-ip.x; right.y=ip1.y-ip.y; right.z=ip1.z-ip.z;

						Math::cross(up,viewForward,right);
						if(Math::normalizeCarefully(up,mEpsilon)==false){
							up.set(Math::X_UNIT_VECTOR3);
						}
						Math::mul(up,scale);

						vba.set3(ivi+0,0,		ip.x+up.x, ip.y+up.y, ip.z+up.z);
						vba.set2(ivi+0,1,		Math::QUARTER,0);

						vba.set3(ivi+1,0,		ip.x-up.x, ip.y-up.y, ip.z-up.z);
						vba.set2(ivi+1,1,		Math::QUARTER,Math::ONE);
					}


					if(mHasScale){
						if(pn.visible==false){
							scale=0;
						}
						else if(mIndividualAttributes){
							scale=Math::lerp(pn.startScale,pn.endScale,pn.age);
						}
						else{
							scale=Math::lerp(startScale,endScale,pn.age);
						}
					}

					// We re-use the up from the previous particle pair,
					//  since we always use particle(n) and particle(n+1), on the final beam cap we
					//  only need to calculate the right, and only if we drew middle particles
					if(j>0){
						Math::normalizeCarefully(right,mEpsilon);
						Math::mul(right,scale);
					}

					vi=vi+vertexesPerBeam-4;
					vba.set3(vi+0,0,		pn.x+up.x, pn.y+up.y, pn.z+up.z);
					vba.set2(vi+0,1,		Math::THREE_QUARTERS,0);

					vba.set3(vi+1,0,		pn.x-up.x, pn.y-up.y, pn.z-up.z);
					vba.set2(vi+1,1,		Math::THREE_QUARTERS,Math::ONE);

					vba.set3(vi+2,0,		pn.x+up.x+right.x, pn.y+up.y+right.y, pn.z+up.z+right.z);
					vba.set2(vi+2,1,		Math::ONE,0);

					vba.set3(vi+3,0,		pn.x-up.x+right.x, pn.y-up.y+right.y, pn.z-up.z+right.z);
					vba.set2(vi+3,1,		Math::ONE,Math::ONE);
				}
			}
		}
		else{
			// Optimized path for using just line beams
			for(i=0;i<mParticles.size();i++){
				const Particle &p=mParticles[i];
				if(p.visible==false){
					color=0x00000000;
				}
				else if(mIndividualAttributes){
					color=Color::lerp(p.startColor,p.endColor,p.age);
				}
				else{
					color=Color::lerp(startColor,endColor,p.age);
				}

				vba.set3(i,0,p.x,p.y,p.z);
				if(mHasColor){
					vba.setABGR(i,1,color);
					vba.set2(i,2,Math::HALF,Math::HALF);
				}
				else{
					vba.set2(i,1,Math::HALF,Math::HALF);
				}
			}
		}

		vba.unlock();
	}

	TOADLET_PROFILE_ENDSECTION(ParticleNode::updateVertexBuffer);
}

}
}
}
