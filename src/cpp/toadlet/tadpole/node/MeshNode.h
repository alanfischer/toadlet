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
#include <toadlet/tadpole/animation/AnimationController.h>
#include <toadlet/tadpole/animation/SkeletonAnimation.h>
#include <toadlet/tadpole/mesh/Mesh.h>
#include <toadlet/tadpole/node/MeshNodeSkeleton.h>
#include <toadlet/tadpole/node/Node.h>
#include <toadlet/tadpole/node/Renderable.h>

namespace toadlet{
namespace tadpole{
namespace node{

class Scene;

// TODO: Make MeshNode a ParentNode, and integrate the SkeletonParentNode into this
// TODO: Maybe rework how the animation framework ties into non-parent nodes.
//  It would be handy to have the MeshNode & SpriteNode be able to have their dedicated controllers without the support work of
//   setting the Node to receive callbacks etc.
//  Another nice feature would be the ability for Animations to be run by a Thread pool, so a central AnimationManager would take
//   care of the updating.  But perhaps this would be better suited by having Nodes update their Animations as we do now, and then the
//   whole scenegraph be updated by a ThreadPool, taking into account dependencies
class TOADLET_API MeshNode:public Node,public Renderable{
public:
	TOADLET_NODE(MeshNode,Node);

	class TOADLET_API SubMesh:public Renderable{
	public:
		TOADLET_SHARED_POINTERS(SubMesh);

		SubMesh(MeshNode *meshNode,mesh::Mesh::SubMesh *meshSubMesh);

		void queueRenderable(SceneNode *queue,CameraNode *camera){} // Queuing done by parent
		Material *getRenderMaterial() const{return material;}
		const Matrix4x4 &getRenderTransform() const{return meshNode->getWorldRenderTransform();}
		void render(peeper::Renderer *renderer) const;

		Material::ptr material;
		peeper::IndexData::ptr indexData;
		peeper::VertexData::ptr vertexData;
		MeshNode *meshNode;
		mesh::Mesh::SubMesh *meshSubMesh;
	};

	/// Specialization of the AnimationController that allows for easy access to playing single sequences.
	class TOADLET_API MeshAnimationController:public animation::AnimationController{
	public:
		TOADLET_SHARED_POINTERS(MeshAnimationController);

		MeshAnimationController(MeshNode *node);

		void setSequenceIndex(int index);
		int getSequenceIndex() const;

		virtual void start();
		virtual void stop();

		virtual void logicUpdate(int dt);
		virtual void renderUpdate(int dt);

		void skeletonChanged();

	protected:
		MeshNode *mMeshNode;
		animation::SkeletonAnimation::ptr mAnimation;
		int mStartingFrame;
	};

	MeshNode();
	virtual Node *create(Engine *engine);
	virtual void destroy();

	Renderable *isRenderable(){return this;}

	void setMesh(const egg::String &name);
	void setMesh(mesh::Mesh::ptr mesh);

	inline const mesh::Mesh::ptr &getMesh() const{return mMesh;}

	inline int getNumSubMeshes() const{return mSubMeshes.size();}
	SubMesh *getSubMesh(int i){return mSubMeshes[i];}
	SubMesh *getSubMesh(const egg::String &name);

	inline MeshNodeSkeleton::ptr getSkeleton() const{return mSkeleton;}
	void setSkeleton(MeshNodeSkeleton::ptr skeleton);

	MeshAnimationController::ptr getAnimationController();

	void logicUpdate(int dt);
	void renderUpdate(int dt);
	void queueRenderables(Scene *scene);

	peeper::VertexBufferAccessor svba;
	peeper::VertexBufferAccessor dvba;

	void createVertexBuffer();
	void updateVertexBuffer();

	void queueRenderable(SceneNode *queue,CameraNode *camera);
	Material *getRenderMaterial() const{return NULL;}
	const Matrix4x4 &getRenderTransform() const{return mWorldRenderTransform;}
	void render(peeper::Renderer *renderer) const{} // Rendering done by children

protected:
	mesh::Mesh::ptr mMesh;
	egg::Collection<SubMesh::ptr> mSubMeshes;
	MeshNodeSkeleton::ptr mSkeleton;
	peeper::VertexData::ptr mDynamicVertexData;

	MeshAnimationController::ptr mAnimationController;

	Vector3 cache_updateVertexBuffer_positionVector;
	Vector3 cache_updateVertexBuffer_normalVector;
};

}
}
}

#endif
