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
#include <toadlet/egg/Resource.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/MeshSkeleton.h>

namespace toadlet{
namespace tadpole{

/// @todo:  Move Mesh to mesh::Mesh, MeshSkeleton to mesh::Skeleton and MeshSkeletonSequence to mesh::Sequence
///  And possibly MeshEntitySkeleton to something else, but not sure what.
///  This would let the 'toadlet mesh' system be more separate, so if a users wishes to replace it or integrate a new
///   mesh system, it would be easier.

/// @todo: Replace Mesh::SubMesh::material/materialName & TextureStage::texture/textureName with a "dummy resource?"
///  Bonus: If it was a reloadable resource, then it could even just load up dummy resources in place and load in the full
///   when the system gets to it.

class TOADLET_API Mesh:public egg::Resource{
public:
	TOADLET_SHARED_POINTERS(Mesh,egg::Resource);

	class TOADLET_API SubMesh{
	public:
		TOADLET_SHARED_POINTERS(SubMesh,SubMesh);

		SubMesh(){}
		virtual ~SubMesh(){}

		peeper::IndexData::ptr indexData;
		
		Material::ptr material;
		egg::String materialName;

		egg::String name;
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

	typedef egg::Collection<VertexBoneAssignment> VertexBoneAssignmentList;

	Mesh();
	virtual ~Mesh();

	void compile();

	egg::String name;

	scalar boundingRadius; // Before world scale
	scalar worldScale;

	egg::Collection<SubMesh::ptr> subMeshes;
	peeper::VertexData::ptr staticVertexData;

	MeshSkeleton::ptr skeleton;
	egg::Collection<VertexBoneAssignmentList> vertexBoneAssignments;
};

}
}

#endif
