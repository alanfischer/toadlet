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
#include <toadlet/tadpole/mesh/Mesh.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace mesh{

Sphere Mesh::Bound_AUTOCALCULATE(-2);

Mesh::Mesh():BaseResource(),
	bound(-Math::ONE),
	worldScale(Math::ONE)
{}

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
	/// @todo: We cant always read the VB here if it wasnt created with the proper bits.  So this needs to be rethought
/*	if(bound.equals(Bound_AUTOCALCULATE)){
		scalar ls=0;
		VertexBufferAccessor vba;
		Vector3 temp;
		int i,j;
		for(i=0;i<staticVertexData->getNumVertexBuffers();++i){
			vba.lock(staticVertexData->getVertexBuffer(0),Buffer::Access_BIT_READ);

			for(j=0;j<vba.getSize();++j){
				vba.get3(j,0,temp);
				ls=Math::lengthSquared(temp);
				if(ls>bound.radius){
					bound.radius=ls;
				}
			}

			vba.unlock();
		}

		bound.radius=Math::sqrt(bound.radius);
	}
*/}

}
}
}
