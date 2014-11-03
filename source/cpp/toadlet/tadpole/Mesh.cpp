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

#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/tadpole/Mesh.h>

namespace toadlet{
namespace tadpole{

Mesh::Mesh():BaseResource(){
}

void Mesh::destroy(){
	if(mStaticVertexData!=NULL){
		mStaticVertexData->destroy();
		mStaticVertexData=NULL;
	}

	int i;
	for(i=0;i<mSubMeshes.size();++i){
		SubMesh *subMesh=mSubMeshes[i];
		if(subMesh->indexData!=NULL){
			subMesh->indexData->destroy();
		}
	}
	mSubMeshes.clear();

	mSkeleton=NULL;
	mVertexBoneAssignments.clear();

	BaseResource::destroy();
}

void Mesh::compile(){
	compileBoneBounds();
}

void Mesh::compileBoneBounds(){
	if(mSkeleton!=NULL){
		VertexBufferAccessor vba(mStaticVertexData->getVertexBuffer(0),Buffer::Access_BIT_READ);

		Collection<bool> touched(mSkeleton->getNumBones(),false);

		Vector3 v;
		int i,j;
		for(i=0;i<mVertexBoneAssignments.size();++i){
			const VertexBoneAssignmentList &vbal=mVertexBoneAssignments[i];
			for(j=0;j<vbal.size();++j){
				Skeleton::Bone *bone=mSkeleton->getBone(vbal[j].bone);
				vba.get3(i,0,v);
				Math::mul(v,bone->worldToBoneRotate);
				Math::add(v,bone->worldToBoneTranslate);
				if(touched[vbal[j].bone]==false){
					touched[vbal[j].bone]=true;
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

void Mesh::addSubMesh(SubMesh::ptr subMesh){
	mSubMeshes.push_back(subMesh);
}

void Mesh::removeSubMesh(SubMesh::ptr subMesh){
	mSubMeshes.erase(remove(mSubMeshes.begin(),mSubMeshes.end(),subMesh),mSubMeshes.end());
}

Mesh::SubMesh::ptr Mesh::getSubMesh(const String &name){
	int i;
	for(i=0;i<mSubMeshes.size();++i){
		if(mSubMeshes[i]->getName().equals(name)){
			return mSubMeshes[i];
		}
	}

	return NULL;
}

void Mesh::setStaticVertexData(VertexData::ptr vertexData){
	if(mStaticVertexData!=NULL){
		mStaticVertexData->destroy();
	}

	mStaticVertexData=vertexData;
}

void Mesh::setSkeleton(Skeleton::ptr skeleton){
	mSkeleton=skeleton;
}

}
}
