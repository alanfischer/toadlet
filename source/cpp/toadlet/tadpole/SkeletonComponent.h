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

#ifndef TOADLET_TADPOLE_SKELETONCOMPONENT_H
#define TOADLET_TADPOLE_SKELETONCOMPONENT_H

#include <toadlet/tadpole/Attachable.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Skeleton.h>
#include <toadlet/tadpole/Animatable.h>
#include <toadlet/tadpole/material/Material.h>
#include <toadlet/tadpole/animation/BaseAnimation.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API SkeletonComponent:public BaseComponent,public Renderable,public Attachable,public Animatable{
public:
	TOADLET_OBJECT(SkeletonComponent);

	// BoneSpaceUpdate flags
	const static int BoneSpaceUpdate_NONE=				0;
	const static int BoneSpaceUpdate_FLAG_TRANSLATE=	1<<0;
	const static int BoneSpaceUpdate_FLAG_ROTATE=		1<<1;
	const static int BoneSpaceUpdate_FLAG_MATRIX=		1<<2;
	const static int BoneSpaceUpdate_FLAG_WORLD=		1<<3;

	class Bone;

	class BoneController{
	public:
		virtual ~BoneController(){}
		virtual int updateBoneTransform(Bone *bone)=0;
	};

	class TOADLET_API TOADLET_ALIGNED Bone{
	public:
		TOADLET_SPTR(Bone);

		TOADLET_ALIGNED_NEW;

		Bone(int index):
			index(0),
			scope(-1),
			controller(NULL),
			dontUpdateFlags(0),
			useMatrixTransforms(false)
		{
			this->index=index;
		}

		void setOverrideTranslate(const Vector3 &translate){
			localTranslate.set(translate);
			dontUpdateFlags|=BoneSpaceUpdate_FLAG_TRANSLATE;
		}

		void setOverrideRotate(const Quaternion &rotate){
			localRotate.set(rotate);
			dontUpdateFlags|=BoneSpaceUpdate_FLAG_ROTATE;
		}

		void removeOverrides(){dontUpdateFlags=0;}

		int index;

		Vector3 localTranslate;
		Quaternion localRotate;

		Vector3 worldTranslate;
		Quaternion worldRotate;
		Matrix4x4 worldMatrix;
		AABox worldBound;

		Vector3 boneSpaceTranslate;
		Quaternion boneSpaceRotate;
		Matrix4x4 boneSpaceMatrix;

		int scope;
		BoneController *controller;
		int dontUpdateFlags;
		bool useMatrixTransforms;
	};

	class SequenceAnimation:public BaseAnimation{
	public:
		TOADLET_OBJECT(SequenceAnimation);

		SequenceAnimation(Sequence *sequence);

		const String &getName() const{return mSequence->getName();}		

		void setValue(scalar value){mValue=value;}
		scalar getMinValue() const{return 0;}
		scalar getMaxValue() const{return mSequence->getLength();}
		scalar getValue() const{return mValue;}

		void setWeight(scalar weight){mWeight=weight;}
		scalar getWeight() const{return mWeight;}

		void setScope(int scope){mScope=scope;}
		int getScope() const{return mScope;}

	protected:
		Sequence::ptr mSequence;
		scalar mValue;
		scalar mWeight;
		int mScope;
		Collection<int> mTrackHints;

		friend class SkeletonComponent;
	};

	SkeletonComponent(Engine *engine,Skeleton *skeleton);
	void destroy();

	void frameUpdate(int dt,int scope);

	void updateBones();
	inline int getLastUpdateFrame() const{return mLastUpdateFrame;}

	inline int getNumBones() const{return mBones.size();}
	inline Bone *getBone(int index) const{return (index>=0 && index<mBones.size())?mBones[index]:NULL;}
	inline Bone *getBone(const String &name) const{return getBone(getBoneIndex(name));}
	void setBoneScope(int i,int scope,bool recurse);

	int getBoneIndex(const String &name) const;
	int getBoneIndex(Bone *bone) const{return bone->index;}

	String getBoneName(int index) const;
	String getBoneName(Bone *bone) const{return mSkeleton->getBone(bone->index)->name;}

	void setTransform(Transform *transform);
	Transform *getTransform() const{return mTransform;}

	Bound *getBound() const{return mBound;}

	bool getActive() const{return true;}

	inline Skeleton *getSkeleton() const{return mSkeleton;}

	void setRenderSkeleton(bool skeleton);
	bool getRenderSkeleton() const{return mSkeletonMaterial!=NULL;}

	// Renderable
	Material *getRenderMaterial() const{return mSkeletonMaterial;}
	Transform *getRenderTransform() const{return mWorldTransform;}
	Bound *getRenderBound() const{return mWorldBound;}
	void render(RenderManager *manager) const;

	// Attachable
	int getNumAttachments(){return mBones.size();}
	String getAttachmentName(int index){return getBoneName(index);}
	int getAttachmentIndex(const String &name){return getBoneIndex(name);}
	bool getAttachmentTransform(Transform *result,int index);

	// Animatable
	int getNumAnimations(){return mAnimations.size();}
	Animation *getAnimation(const String &name);
	Animation *getAnimation(int index){return mAnimations[index];}

protected:
	void createSkeletonBuffers();
	void updateSkeletonBuffers();
	void destroySkeletonBuffers();

	int updateBoneTransformation(Bone *bone);
	void updateBone(Bone *bone);

	Engine *mEngine;
	Skeleton::ptr mSkeleton;
	Collection<Bone::ptr> mBones;
	Transform::ptr mTransform;
	Bound::ptr mBound;
	Transform::ptr mWorldTransform;
	Bound::ptr mWorldBound;
	int mLastUpdateFrame;
	Collection<SequenceAnimation::ptr> mAnimations;

	// For rendering the skeleton outline
	Material::ptr mSkeletonMaterial;
	VertexBuffer::ptr mSkeletonVertexBuffer;
	IndexData::ptr mSkeletonIndexData;
	VertexData::ptr mSkeletonVertexData;
	VertexBuffer::ptr mHitBoxVertexBuffer;
	VertexData::ptr mHitBoxVertexData;
	IndexData::ptr mHitBoxIndexData;
};

}
}

#endif
