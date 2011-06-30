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
#include <toadlet/tadpole/material/MaterialResource.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Mesh:public BaseResource{
public:
	TOADLET_SHARED_POINTERS(Mesh);

	class TOADLET_API SubMesh{
	public:
		TOADLET_SHARED_POINTERS(SubMesh);

		SubMesh():hasOwnTransform(false){}
		virtual ~SubMesh(){}

		VertexData::ptr vertexData;
		IndexData::ptr indexData;
		
		MaterialResource::ptr material;
		String materialName;

		String name;

		bool hasOwnTransform;
		Transform transform;
		Bound bound;
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
	virtual ~Mesh();

	void destroy();

	void compile();
	void compileBoneBounds();

	void setTransform(const Transform &transform){mTransform.set(transform);}
	const Transform &getTransform() const{return mTransform;}

	void setBound(const Bound &bound){mBound.set(bound);}
	const Bound &getBound() const{return mBound;}

	void addSubMesh(SubMesh::ptr subMesh){mSubMeshes.add(subMesh);}
	void removeSubMesh(SubMesh::ptr subMesh){mSubMeshes.remove(subMesh);}
	int getNumSubMeshes() const{return mSubMeshes.size();}
	SubMesh::ptr getSubMesh(int i){return mSubMeshes[i];}
	SubMesh::ptr getSubMesh(const String &name);

	void setStaticVertexData(VertexData::ptr vertexData);
	VertexData::ptr getStaticVertexData(){return mStaticVertexData;}

	void setSkeleton(Skeleton::ptr skeleton);
	Skeleton::ptr getSkeleton() const{return mSkeleton;}

	void setVertexBoneAssignments(const Collection<VertexBoneAssignmentList> &assignments){mVertexBoneAssignments=assignments;}
	const Collection<VertexBoneAssignmentList> &getVertexBoneAssignments() const{return mVertexBoneAssignments;}

protected:
	Transform mTransform;
	Bound mBound;
	Collection<SubMesh::ptr> mSubMeshes;
	VertexData::ptr mStaticVertexData;
	Skeleton::ptr mSkeleton;
	Collection<VertexBoneAssignmentList> mVertexBoneAssignments;
};

}
}

#endif
