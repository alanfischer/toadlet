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

#ifndef TOADLET_TADPOLE_NODE_MESHNODE_H
#define TOADLET_TADPOLE_NODE_MESHNODE_H

#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/tadpole/Mesh.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Visible.h>
#include <toadlet/tadpole/SkeletonComponent.h>
#include <toadlet/tadpole/node/CameraAlignedNode.h>

namespace toadlet{
namespace tadpole{
namespace node{

class TOADLET_API MeshNode:public CameraAlignedNode,public Visible,public Attachable{
public:
	TOADLET_NODE(MeshNode,CameraAlignedNode);

	class TOADLET_API SubMesh:public Renderable{
	public:
		TOADLET_SPTR(SubMesh);

		SubMesh(MeshNode *meshNode,Mesh::SubMesh *meshSubMesh);

		Material *getRenderMaterial() const{return material;}
		const Transform &getRenderTransform() const{return hasOwnTransform?worldTransform:meshNode->getWorldTransform();}
		const Bound &getRenderBound() const{return hasOwnTransform?worldBound:meshNode->getWorldBound();}
		void render(SceneRenderer *renderer) const;

		bool rendered;
		Material::ptr material;
		IndexData::ptr indexData;
		VertexData::ptr vertexData;
		MeshNode *meshNode;
		Mesh::SubMesh *meshSubMesh;

		bool hasOwnTransform;
		Transform worldTransform;
		Bound worldBound;
		int scope;
	};

	MeshNode();
	Node *create(Scene *scene);
	void destroy();
	Node *set(Node *node);
	void *hasInterface(int type);

	void setMesh(const String &name);
	void setMesh(Mesh *mesh);
	Mesh *getMesh() const{return mMesh;}

	inline int getNumSubMeshes() const{return mSubMeshes.size();}
	SubMesh *getSubMesh(int i){return mSubMeshes[i];}
	SubMesh *getSubMesh(const String &name);

	inline SkeletonComponent *getSkeleton() const{return mSkeleton;}
	void setSkeleton(SkeletonComponent *skeleton);

	void frameUpdate(int dt,int scope);
	void updateWorldTransform();

	// Visible
	bool getRendered() const{return mRendered;}
	void setRendered(bool rendered){mRendered=rendered;}
	RenderState::ptr getSharedRenderState();
	void gatherRenderables(CameraNode *camera,RenderableSet *set);

	// Attachable
	int getNumAttachments(){return mSkeleton!=NULL?mSkeleton->getNumAttachments():0;}
	String getAttachmentName(int index){return mSkeleton!=NULL?mSkeleton->getAttachmentName(index):(char*)NULL;}
	int getAttachmentIndex(const String &name){return mSkeleton!=NULL?mSkeleton->getAttachmentIndex(name):0;}
	bool getAttachmentTransform(Transform &result,int index){return mSkeleton!=NULL?mSkeleton->getAttachmentTransform(result,index):false;}

	VertexBufferAccessor svba;
	VertexBufferAccessor dvba;

	void createVertexBuffer();
	void updateVertexBuffer();

protected:
	bool mRendered;
	Mesh::ptr mMesh;
	Collection<SubMesh::ptr> mSubMeshes;
	RenderState::ptr mSharedRenderState;
	SkeletonComponent::ptr mSkeleton;
	VertexData::ptr mDynamicVertexData;
};

}
}
}

#endif
