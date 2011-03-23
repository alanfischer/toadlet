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

#ifndef TOADLET_TADPOLE_STUDIO_STUDIOMODELNODE_H
#define TOADLET_TADPOLE_STUDIO_STUDIOMODELNODE_H

#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/Attachable.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/DetailTraceable.h>
#include <toadlet/tadpole/Visible.h>
#include <toadlet/tadpole/studio/StudioModel.h>

namespace toadlet{
namespace tadpole{
namespace studio{

class StudioModelController;

class TOADLET_API StudioModelNode:public node::Node,public DetailTraceable,public Renderable,public Attachable,public Visible{
public:
	TOADLET_NODE(StudioModelNode,Node);

	class TOADLET_API SubModel:public Renderable{
	public:
		TOADLET_SHARED_POINTERS(SubModel);

		SubModel(StudioModelNode *modelNode,int bodypartIndex,int modelIndex,int meshIndex,int skinIndex);

		Material *getRenderMaterial() const{return material;}
		const Transform &getRenderTransform() const{return modelNode->getWorldTransform();}
		const Bound &getRenderBound() const{return modelNode->getRenderBound();}
		void render(peeper::Renderer *renderer) const;

		StudioModelNode *modelNode;
		StudioModel::ptr model;
		int bodypartIndex,modelIndex,meshIndex;

		studiobodyparts *sbodyparts;
		studiomodel *smodel;
		studiomesh *smesh;
		StudioModel::meshdata *mdata;
		Material::ptr material;
	};

	StudioModelNode();
	virtual ~StudioModelNode();
	Node *create(Scene *scene);
	void destroy();
	Node *set(Node *node);

	void *hasInterface(int type);

	void logicUpdate(int dt,int scope);
	void frameUpdate(int dt,int scope);

	void setModel(const egg::String &name);
	void setModel(StudioModel::ptr model);
	StudioModel::ptr getModel() const{return mModel;}

	void setSequence(int sequence);
	int getSequence() const{return mSequenceIndex;}

	void setSequenceTime(scalar time);
	scalar getSequenceTime() const{return mSequenceTime;}

	void setGaitSequence(int sequence);
	int getGaitSequence() const{return mGaitSequenceIndex;}

	void setGaitSequenceTime(scalar time);
	scalar getGaitSequenceTime() const{return mGaitSequenceTime;}

	void setRenderSkeleton(bool skeleton);
	bool getRenderSkeleton() const{return mSkeletonMaterial!=NULL;}

	void setController(int controller,scalar v);
	scalar getController(int controller) const{return mControllerValues[controller];}

	void setBlender(int blender,scalar v);
	scalar getBlender(int blender) const{return mBlenderValues[blender];}

	void setBodypart(int bodypart);
	int getBodypart() const{return mBodypartIndex;}

	void setBodypartModel(int model);
	int getBodypartModel() const{return mModelIndex;}

	void setSkin(int skin);
	int getSkin() const{return mSkinIndex;}

	// If set, it overrides any sequences or controllers
	void setLink(StudioModelNode::ptr link);
	StudioModelNode::ptr getLink() const{return mLink;}

	StudioModelController *getController();
	StudioModelController *getGaitController();

	// Traceable
	const Bound &getBound() const{return super::getBound();}
	void traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);

	// Visible
	void modifyMaterial(Material::ptr material);
	bool getRendered() const{return mRendered;}
	void setRendered(bool rendered){mRendered=rendered;}
	void gatherRenderables(node::CameraNode *camera,RenderableSet *set);

	// Renderable
	Material *getRenderMaterial() const{return mSkeletonMaterial;}
	const Transform &getRenderTransform() const{return getWorldTransform();}
	const Bound &getRenderBound() const{return getWorldBound();}
	void render(peeper::Renderer *renderer) const;

	// Attachable
	int getNumAttachments(){return mModel->header->numattachments;}
	egg::String getAttachmentName(int index){return mModel->attachment(index)->name;}
	int getAttachmentIndex(const egg::String &name);
	bool getAttachmentTransform(Transform &result,int index);

	static void setQuaternionFromEulerAngleStudio(Quaternion &r,const EulerAngle &euler);

	peeper::VertexBufferAccessor vba;
	peeper::IndexBufferAccessor iba;

protected:
	void updateSkeleton();
	void findBoneTransforms(Vector3 *translates,Quaternion *rotates,StudioModel *model,studioseqdesc *sseqdesc,studioanim *sanim,float t);
	void findBoneTranslate(Vector3 &r,int frame,float s,studiobone *sbone,studioanim *sanim);
	void findBoneRotate(Quaternion &r,int frame,float s,studiobone *sbone,studioanim *sanim);
	void updateVertexes(StudioModel *model,int bodypartsIndex,int modelIndex);
	void findChrome(Vector2 &chrome,const Vector3 &normal,const Vector3 &forward,const Vector3 &right);
	void createSubModels();
	void createSkeletonBuffers();
	void updateSkeletonBuffers();
	void destroySkeletonBuffers();

	bool mRendered;
	StudioModel::ptr mModel;
	egg::Collection<SubModel::ptr> mSubModels;

	int mBodypartIndex;
	int mModelIndex;
	int mSkinIndex;
	int mSequenceIndex;
	scalar mSequenceTime;
	int mGaitSequenceIndex;
	scalar mGaitSequenceTime;
	scalar mControllerValues[4],mAdjustedControllerValues[4];
	scalar mBlenderValues[4],mAdjustedBlenderValues[4];
	StudioModelNode::ptr mLink;
	StudioModel::ptr mLinkModel;
	animation::Controller::ptr mController,mGaitController;

	Vector3 mChromeForward,mChromeRight;
	egg::Collection<Vector3> mBoneTranslates;
	egg::Collection<Quaternion> mBoneRotates;
	egg::Collection<int16> mBoneLinks;
	egg::Collection<Vector3> mTransformedVerts;
	egg::Collection<Vector3> mTransformedNorms;
	egg::Collection<Vector2> mTransformedChromes;
	peeper::VertexBuffer::ptr mVertexBuffer;
	peeper::VertexData::ptr mVertexData;

	Material::ptr mSkeletonMaterial;
	peeper::VertexBuffer::ptr mSkeletonVertexBuffer;
	peeper::VertexData::ptr mSkeletonVertexData;
	peeper::IndexData::ptr mSkeletonIndexData;
	peeper::VertexBuffer::ptr mHitBoxVertexBuffer;
	peeper::VertexData::ptr mHitBoxVertexData;
	peeper::IndexData::ptr mHitBoxIndexData;
};

}
}
}

#endif
