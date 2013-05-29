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

#ifndef TOADLET_TADPOLE_MESHCOMPONENT_H
#define TOADLET_TADPOLE_MESHCOMPONENT_H

#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/tadpole/Mesh.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Visible.h>
#include <toadlet/tadpole/Spacial.h>
#include <toadlet/tadpole/SkeletonComponent.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API MeshComponent:public BaseComponent,public TransformListener,public Spacial,public Visible,public Attachable{
public:
	TOADLET_COMPONENT(MeshComponent);

	class TOADLET_API SubMesh:public Object,public Renderable{
	public:
		TOADLET_OBJECT(SubMesh);

		SubMesh(MeshComponent *parent,Mesh::SubMesh *meshSubMesh);
		virtual ~SubMesh(){}

		void setTransform(Transform::ptr transform){mTransform->set(transform);}
		Transform *getTransform() const{return mTransform;}

		// Renderable
		Material *getRenderMaterial() const{return mMaterial;}
		Transform *getRenderTransform() const{return mWorldTransform!=NULL?mWorldTransform.get():mParent->getWorldTransform();}
		Bound *getRenderBound() const{return mWorldBound!=NULL?mWorldBound.get():mParent->getWorldBound();}
		void render(RenderManager *manager) const;

	protected:
		Material::ptr mMaterial;
		IndexData::ptr mIndexData;
		VertexData::ptr mVertexData;
		MeshComponent *mParent;
		Mesh::SubMesh *mMeshSubMesh;

		Transform::ptr mTransform;
		Transform::ptr mWorldTransform;
		Bound::ptr mWorldBound;
		int mScope;

		friend class MeshComponent;
	};

	MeshComponent(Engine *engine);
	void destroy();

	void parentChanged(Node *node);

	void setMesh(const String &name);
	void setMesh(Mesh *mesh);
	Mesh *getMesh() const{return mMesh;}

	inline int getNumSubMeshes() const{return mSubMeshes.size();}
	SubMesh *getSubMesh(int i){return mSubMeshes[i];}
	SubMesh *getSubMesh(const String &name);
	Material *getSubMaterial(int i){return getSubMesh(i)->getRenderMaterial();}
	Material *getSubMaterial(const String &name){return getSubMesh(name)->getRenderMaterial();}

	inline SkeletonComponent *getSkeleton() const{return mSkeleton;}
	void setSkeleton(SkeletonComponent *skeleton);

	void frameUpdate(int dt,int scope);

	bool getActive() const{return mSkeleton!=NULL;}

	// Spacial
	void setTransform(Transform::ptr transform);
	Transform *getTransform() const{return mTransform;}
	Bound *getBound() const{return mBound;}
	Transform *getWorldTransform() const{return mWorldTransform;}
	Bound *getWorldBound() const{return mWorldBound;}
	void transformChanged(Transform *transform);

	// Visible
	bool getRendered() const{return mRendered;}
	void setRendered(bool rendered){mRendered=rendered;}
	RenderState::ptr getSharedRenderState();
	void gatherRenderables(Camera *camera,RenderableSet *set);

	// Attachable
	int getNumAttachments(){return mSkeleton!=NULL?mSkeleton->getNumAttachments():0;}
	String getAttachmentName(int index){return mSkeleton!=NULL?mSkeleton->getAttachmentName(index):(char*)NULL;}
	int getAttachmentIndex(const String &name){return mSkeleton!=NULL?mSkeleton->getAttachmentIndex(name):0;}
	bool getAttachmentTransform(Transform *result,int index){return mSkeleton!=NULL?mSkeleton->getAttachmentTransform(result,index):false;}

	void createVertexBuffer();
	void updateVertexBuffer();

protected:
	void setBound(Bound::ptr bound);

	Engine *mEngine;
	bool mRendered;
	Mesh::ptr mMesh;
	Collection<SubMesh::ptr> mSubMeshes;
	RenderState::ptr mSharedRenderState;
	SkeletonComponent::ptr mSkeleton;
	VertexData::ptr mDynamicVertexData;
	Transform::ptr mTransform;
	Bound::ptr mBound;
	Transform::ptr mWorldTransform;
	Bound::ptr mWorldBound;
};

}
}

#endif
