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
#include <toadlet/tadpole/animation/Controller.h>
#include <toadlet/tadpole/animation/SkeletonAnimation.h>
#include <toadlet/tadpole/node/MeshNodeSkeleton.h>
#include <toadlet/tadpole/node/CameraAlignedNode.h>

namespace toadlet{
namespace tadpole{
namespace node{

class TOADLET_API MeshNode:public CameraAlignedNode,public Visible,public Attachable{
public:
	TOADLET_NODE(MeshNode,CameraAlignedNode);

	class TOADLET_API SubMesh:public Renderable{
	public:
		TOADLET_SHARED_POINTERS(SubMesh);

		SubMesh(MeshNode *meshNode,Mesh::SubMesh *meshSubMesh);

		Material *getRenderMaterial() const{return material;}
		Transform *getRenderTransform() const{return worldTransform!=NULL?worldTransform:meshNode->getWorldTransform();}
		Bound *getRenderBound() const{return worldBound!=NULL?worldBound:meshNode->getWorldBound();}
		void render(peeper::Renderer *renderer) const;

		Material::ptr material;
		peeper::IndexData::ptr indexData;
		peeper::VertexData::ptr vertexData;
		MeshNode *meshNode;
		Mesh::SubMesh *meshSubMesh;

		Transform::ptr worldTransform;
		Bound::ptr worldBound;
	};

	/// Specialization of the Controller that allows for easy access to playing single sequences.
	class TOADLET_API MeshController:public animation::Controller{
	public:
		TOADLET_SHARED_POINTERS(MeshController);

		MeshController(MeshNode *node);

		void setSequenceIndex(int index){mAnimation->setSequenceIndex(index);}
		int getSequenceIndex() const{return mAnimation->getSequenceIndex();}

		void skeletonChanged();

	protected:
		MeshNode *mMeshNode;
		animation::SkeletonAnimation::ptr mAnimation;
		int mStartingFrame;
	};

	MeshNode();
	Node *create(Scene *scene);
	void destroy();
	Node *set(Node *node);

	void *hasInterface(int type);

	void setMesh(const egg::String &name);
	void setMesh(Mesh::ptr mesh);
	const Mesh::ptr &getMesh() const{return mMesh;}

	void modifyMaterial(Material::ptr material);
	bool getRendered() const{return mRendered;}
	void setRendered(bool rendered){mRendered=rendered;}

	inline int getNumSubMeshes() const{return mSubMeshes.size();}
	SubMesh *getSubMesh(int i){return mSubMeshes[i];}
	SubMesh *getSubMesh(const egg::String &name);

	inline MeshNodeSkeleton::ptr getSkeleton() const{return mSkeleton;}
	void setSkeleton(MeshNodeSkeleton::ptr skeleton);

	MeshController::ptr getController();

	void frameUpdate(int dt,int scope);
	void updateWorldTransform();
	void queueRenderables(CameraNode *camera,RenderQueue *queue);

	peeper::VertexBufferAccessor svba;
	peeper::VertexBufferAccessor dvba;

	void createVertexBuffer();
	void updateVertexBuffer();

	// Attachable
	int getNumAttachments(){return mSkeleton!=NULL?mSkeleton->getNumAttachments():0;}
	egg::String getAttachmentName(int index){return mSkeleton!=NULL?mSkeleton->getAttachmentName(index):(char*)NULL;}
	int getAttachmentIndex(const egg::String &name){return mSkeleton!=NULL?mSkeleton->getAttachmentIndex(name):0;}
	bool getAttachmentTransform(Transform *result,int index){return mSkeleton!=NULL?mSkeleton->getAttachmentTransform(result,index):false;}

protected:
	bool mRendered;
	Mesh::ptr mMesh;
	egg::Collection<SubMesh::ptr> mSubMeshes;
	MeshNodeSkeleton::ptr mSkeleton;
	peeper::VertexData::ptr mDynamicVertexData;

	MeshController::ptr mController;

	Vector3 cache_updateVertexBuffer_positionVector;
	Vector3 cache_updateVertexBuffer_normalVector;
};

}
}
}

#endif
