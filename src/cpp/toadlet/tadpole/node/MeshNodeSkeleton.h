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

#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/mesh/Skeleton.h>

namespace toadlet{
namespace tadpole{
namespace node{

class MeshNode;

class TOADLET_API MeshNodeSkeleton:public Renderable{
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

	class TOADLET_API Bone{
	public:
		TOADLET_SHARED_POINTERS(Bone);

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
		Matrix3x3 worldRotateMatrix;

		Vector3 boneSpaceTranslate;
		Matrix3x3 boneSpaceRotate;

		BoneController *controller;

		int dontUpdateFlags;

		bool useMatrixTransforms;
	};

	MeshNodeSkeleton(MeshNode *node,mesh::Skeleton::ptr skeleton);

	void updateBones();
	void updateBones(int sequenceIndex,scalar sequenceTime);
	inline int getLastUpdateFrame() const{return mLastUpdateFrame;}

	inline int getNumBones() const{return mBones.size();}
	inline Bone *getBone(int index) const{return mBones[index];}
	Bone *getBone(const egg::String &name) const;

	egg::String getBoneName(int index) const;
	egg::String getBoneName(Bone *bone) const;

	inline mesh::Skeleton::ptr getSkeleton() const{return mSkeleton;}

	void setRenderable(bool renderable);
	void updateVertexData();

	Material *getRenderMaterial() const{return mMaterial;}
	Transform *getRenderTransform() const;
	void render(peeper::Renderer *renderer) const;

protected:
	int updateBoneTransformation(Bone *bone);
	void updateBone(Bone *bone);

	MeshNode *mNode;
	mesh::Skeleton::ptr mSkeleton;
	egg::Collection<Bone::ptr> mBones;
	int mLastUpdateFrame;

	mesh::Sequence::ptr mSequence;
	scalar mSequenceTime;
	egg::Collection<int> mTrackHints;

	// For rendering the skeleton outline
	Material::ptr mMaterial;
	peeper::IndexData::ptr mIndexData;
	peeper::VertexData::ptr mVertexData;

	Vector3 cache_updateBone_vector;
	Quaternion cache_updateBone_quaternion;
};

}
}
}

#endif
