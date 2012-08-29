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

#ifndef TOADLET_TADPOLE_STUDIO_STUDIOMODELCOMPONENT_H
#define TOADLET_TADPOLE_STUDIO_STUDIOMODELCOMPONENT_H

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

class TOADLET_API StudioModelComponent:public BaseComponent,public DetailTraceable,public Renderable,public Attachable,public Visible{
public:
	TOADLET_OBJECT(StudioModelComponent);

	class TOADLET_API SubModel:public Object,public Renderable{
	public:
		TOADLET_OBJECT(SubModel);

		SubModel(StudioModelComponent *parent,int bodypartIndex,int modelIndex,int meshIndex,int skinIndex);
		void destroy(){mMaterial=NULL;}
	
		Material *getRenderMaterial() const{return mMaterial;}
		const Transform &getRenderTransform() const{return mParent->getRenderTransform();}
		Bound *getRenderBound() const{return mParent->getRenderBound();}
		void render(RenderManager *manager) const;

	protected:
		StudioModelComponent *mParent;
		StudioModel::ptr mModel;
		int mBodypartIndex,mModelIndex,mMeshIndex;

		studiobodyparts *mSBodyparts;
		studiomodel *mSModel;
		studiomesh *mSMesh;
		StudioModel::meshdata *mData;
		Material::ptr mMaterial;

		friend class StudioModelComponent;
	};

	StudioModelComponent(Engine *engine);
	void destroy();

	void logicUpdate(int dt,int scope);
	void frameUpdate(int dt,int scope);

	void setModel(const String &name);
	void setModel(StudioModel *model);
	StudioModel *getModel() const{return mModel;}

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
	void setLink(StudioModelComponent *link);
	StudioModelComponent *getLink() const{return mLink;}

	StudioModelController *getController();
	StudioModelController *getGaitController();

	bool getActive() const{return true;}
	Bound *getBound() const{return mBound;}
	
	// Traceable
	Bound *getTraceableBound() const{return mBound;}
	void traceSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);

	// Visible
	bool getRendered() const{return mRendered;}
	void setRendered(bool rendered){mRendered=rendered;}
	RenderState::ptr getSharedRenderState();
	void gatherRenderables(Camera *camera,RenderableSet *set);

	// Renderable
	Material *getRenderMaterial() const{return mSkeletonMaterial;}
	const Transform &getRenderTransform() const{return mParent->getWorldTransform();}
	Bound *getRenderBound() const{return mParent->getWorldBound();}
	void render(RenderManager *manager) const;

	// Attachable
	int getNumAttachments(){return mModel->header->numattachments;}
	String getAttachmentName(int index){return mModel->attachment(index)->name;}
	int getAttachmentIndex(const String &name);
	bool getAttachmentTransform(Transform &result,int index);

	static void setQuaternionFromEulerAngleStudio(Quaternion &r,const EulerAngle &euler);

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

	Engine *mEngine;
	Bound *mBound;
	bool mRendered;
	StudioModel::ptr mModel;
	Collection<SubModel::ptr> mSubModels;
	RenderState::ptr mSharedRenderState;

	int mBodypartIndex;
	int mModelIndex;
	int mSkinIndex;
	int mSequenceIndex;
	scalar mSequenceTime;
	int mGaitSequenceIndex;
	scalar mGaitSequenceTime;
	scalar mControllerValues[4],mAdjustedControllerValues[4];
	scalar mBlenderValues[4],mAdjustedBlenderValues[4];
	StudioModelComponent::ptr mLink;
	StudioModel::ptr mLinkModel;
//	Controller::ptr mController,mGaitController;

	Vector3 mChromeForward,mChromeRight;
	Collection<Vector3> mBoneTranslates;
	Collection<Quaternion> mBoneRotates;
	Collection<int16> mBoneLinks;
	Collection<Vector3> mTransformedVerts;
	Collection<Vector3> mTransformedNorms;
	Collection<Vector2> mTransformedChromes;
	VertexBuffer::ptr mVertexBuffer;
	VertexData::ptr mVertexData;

	Material::ptr mSkeletonMaterial;
	VertexBuffer::ptr mSkeletonVertexBuffer;
	VertexData::ptr mSkeletonVertexData;
	IndexData::ptr mSkeletonIndexData;
	VertexBuffer::ptr mHitBoxVertexBuffer;
	VertexData::ptr mHitBoxVertexData;
	IndexData::ptr mHitBoxIndexData;
};

}
}
}

#endif