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

#ifndef TOADLET_TADPOLE_NODE_MESHNODESKELETON_H
#define TOADLET_TADPOLE_NODE_MESHNODESKELETON_H

#include <toadlet/tadpole/Attachable.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Skeleton.h>
#include <toadlet/tadpole/material/Material.h>

namespace toadlet{
namespace tadpole{

class Engine;

namespace node{

class MeshNode;

class TOADLET_API MeshNodeSkeleton:public Renderable,public Attachable{
public:
	TOADLET_SHARED_POINTERS(MeshNodeSkeleton);

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
		TOADLET_SHARED_POINTERS(Bone);

		TOADLET_ALIGNED_NEW;

		Bone(int index):
			index(0),
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

		BoneController *controller;

		int dontUpdateFlags;

		bool useMatrixTransforms;
	};

	MeshNodeSkeleton(MeshNode *node,Skeleton::ptr skeleton);
	void destroy();

	void updateBones();
	void updateBones(int sequenceIndex,scalar sequenceTime);
	inline int getLastUpdateFrame() const{return mLastUpdateFrame;}

	inline int getNumBones() const{return mBones.size();}
	inline Bone *getBone(int index) const{return (index>=0 && index<mBones.size())?mBones[index]:NULL;}
	inline Bone *getBone(const String &name) const{return getBone(getBoneIndex(name));}

	int getBoneIndex(const String &name) const;
	int getBoneIndex(Bone *bone) const{return bone->index;}

	String getBoneName(int index) const;
	String getBoneName(Bone *bone) const{return mSkeleton->bones[bone->index]->name;}

	const AABox &getBound(){return mBound;}

	inline Skeleton::ptr getSkeleton() const{return mSkeleton;}

	void setRenderSkeleton(bool skeleton);
	bool getRenderSkeleton() const{return mSkeletonMaterial!=NULL;}

	// Renderable
	Material *getRenderMaterial() const{return mSkeletonMaterial;}
	const Transform &getRenderTransform() const;
	const Bound &getRenderBound() const;
	void render(RenderDevice *renderDevice) const;

	// Attachable
	int getNumAttachments(){return mBones.size();}
	String getAttachmentName(int index){return getBoneName(index);}
	int getAttachmentIndex(const String &name){return getBoneIndex(name);}
	bool getAttachmentTransform(Transform &result,int index);

protected:
	void createSkeletonBuffers();
	void updateSkeletonBuffers();
	void destroySkeletonBuffers();

	int updateBoneTransformation(Bone *bone);
	void updateBone(Bone *bone);

	Engine *mEngine;
	MeshNode *mNode;
	Skeleton::ptr mSkeleton;
	Collection<Bone::ptr> mBones;
	AABox mBound;
	int mLastUpdateFrame;

	TransformSequence::ptr mSequence;
	scalar mSequenceTime;
	Collection<int> mTrackHints;

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
}

#endif
