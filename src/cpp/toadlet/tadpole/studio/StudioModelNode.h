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
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/studio/StudioModel.h>

namespace toadlet{
namespace tadpole{
namespace studio{

class TOADLET_API StudioModelNode:public node::Node,public Renderable{
public:
	TOADLET_NODE(StudioModelNode,Node);

	class TOADLET_API SubModel:public Renderable{
	public:
		TOADLET_SHARED_POINTERS(SubModel);

		SubModel(StudioModelNode *modelNode,int bodypartIndex,int modelIndex,int meshIndex);

		Material *getRenderMaterial() const{return material;}
		const Matrix4x4 &getRenderTransform() const{return modelNode->getWorldTransform();}
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

	void destroy();

	void setModel(StudioModel::ptr model);
	StudioModel::ptr getModel() const{return mModel;}

	void queueRenderables(node::CameraNode *camera,RenderQueue *queue);
	Material *getRenderMaterial() const{return mSkeletonMaterial;}
	const Matrix4x4 &getRenderTransform() const{return getWorldTransform();}
	void render(peeper::Renderer *renderer) const;

	peeper::VertexBufferAccessor vba;
	peeper::IndexBufferAccessor iba;

protected:
	void updateChrome(StudioModel *model,int bodypartsIndex,int modelIndex,node::CameraNode *camera);
	void findChrome(Vector2 &chrome,const Vector3 &normal,const Vector3 &forward,const Vector3 &right);

	void createSkeletonBuffers();
	void updateSkeletonBuffers();

	StudioModel::ptr mModel;
	egg::Collection<SubModel::ptr> mSubModels;
	egg::Collection<Vector3> mBoneTranslates;
	egg::Collection<Quaternion> mBoneRotates;
	egg::Collection<Vector3> mTransformedPoints;
	egg::Collection<Vector3> mTransformedNormals;
	egg::Collection<Vector2> mTransformedChromes;

	Material::ptr mSkeletonMaterial;
	peeper::VertexBuffer::ptr mSkeletonVertexBuffer;
	peeper::VertexData::ptr mSkeletonVertexData;
	peeper::IndexData::ptr mSkeletonIndexData;
};

}
}
}

#endif
