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

#include <toadlet/egg/Logger.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/tadpole/Mesh.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

Mesh::Mesh():BaseResource(){
}

Mesh::~Mesh(){
	destroy();
}

void Mesh::destroy(){
	if(staticVertexData!=NULL){
		staticVertexData->destroy();
	}

	int i;
	for(i=0;i<subMeshes.size();++i){
		SubMesh *subMesh=subMeshes[i];
		if(subMesh->indexData!=NULL){
			subMesh->indexData->destroy();
		}
		if(subMesh->material!=NULL){
			subMesh->material->release();
		}
	}
	subMeshes.clear();

	if(skeleton!=NULL){
		skeleton->release();
		skeleton=NULL;
	}
}

void Mesh::compile(){
	compileBoneBounds();
}

void Mesh::compileBoneBounds(){
	if(skeleton!=NULL){
		VertexBufferAccessor vba(staticVertexData->getVertexBuffer(0),Buffer::Access_BIT_READ);

		Collection<bool> touched(vertexBoneAssignments.size(),false);

		Vector3 v;
		int i,j;
		for(i=0;i<vertexBoneAssignments.size();++i){
			const VertexBoneAssignmentList &vbal=vertexBoneAssignments[i];
			for(j=0;j<vbal.size();++j){
				Skeleton::Bone *bone=skeleton->bones[vbal[j].bone];
				vba.get3(i,0,v);
				Math::sub(v,bone->worldToBoneTranslate);
				if(touched[i]==false){
					touched[i]=true;
					bone->bound.set(v,v);
				}
				else{
					bone->bound.merge(v);
				}
			}
		}

		vba.unlock();
	}
}

}
}
