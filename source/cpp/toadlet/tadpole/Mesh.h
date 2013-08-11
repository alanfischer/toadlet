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

#ifndef TOADLET_TADPOLE_MESH_H
#define TOADLET_TADPOLE_MESH_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/Bound.h>
#include <toadlet/tadpole/Transform.h>
#include <toadlet/tadpole/Skeleton.h>
#include <toadlet/tadpole/material/Material.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Mesh:public BaseResource{
public:
	TOADLET_RESOURCE(Mesh,Mesh);

	class TOADLET_API SubMesh:public BaseResource{
	public:
		TOADLET_RESOURCE(SubMesh,SubMesh);

		SubMesh(){}
		virtual ~SubMesh(){}

		VertexData::ptr vertexData;
		IndexData::ptr indexData;
		
		Material::ptr material;
		String materialName;

		Transform::ptr transform;
		Bound::ptr bound;
	};

	class TOADLET_API VertexBoneAssignment{
	public:
		VertexBoneAssignment(unsigned short b,scalar w):
			bone(b),
			weight(w){}

		VertexBoneAssignment():
			bone(0),
			weight(Math::ONE){}

		unsigned short bone;
		scalar weight;
	};

	typedef Collection<VertexBoneAssignment> VertexBoneAssignmentList;

	Mesh();

	void destroy();

	void compile();
	void compileBoneBounds();

	virtual void setTransform(Transform::ptr transform){mTransform=transform;}
	Transform *getTransform() const{return mTransform;}

	virtual void setBound(Bound::ptr bound){mBound=bound;}
	Bound *getBound() const{return mBound;}

	virtual void addSubMesh(SubMesh::ptr subMesh){mSubMeshes.add(subMesh);}
	virtual void removeSubMesh(SubMesh::ptr subMesh){mSubMeshes.remove(subMesh);}
	int getNumSubMeshes() const{return mSubMeshes.size();}
	SubMesh::ptr getSubMesh(int i){return mSubMeshes[i];}
	SubMesh::ptr getSubMesh(const String &name);

	virtual void setStaticVertexData(VertexData::ptr vertexData);
	VertexData::ptr getStaticVertexData(){return mStaticVertexData;}

	virtual void setSkeleton(Skeleton::ptr skeleton);
	Skeleton::ptr getSkeleton() const{return mSkeleton;}

	virtual void setVertexBoneAssignments(const Collection<VertexBoneAssignmentList> &assignments){mVertexBoneAssignments=assignments;}
	const Collection<VertexBoneAssignmentList> &getVertexBoneAssignments() const{return mVertexBoneAssignments;}

protected:
	Transform::ptr mTransform;
	Bound::ptr mBound;
	Collection<SubMesh::ptr> mSubMeshes;
	VertexData::ptr mStaticVertexData;
	Skeleton::ptr mSkeleton;
	Collection<VertexBoneAssignmentList> mVertexBoneAssignments;
};

}
}

#endif
