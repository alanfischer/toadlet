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

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/handler/AABoxMeshCreator.h>

namespace toadlet{
namespace tadpole{
namespace handler{

Mesh::ptr AABoxMeshCreator::createAABoxMesh(const AABox &box,Material::ptr material){
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,24);
	{
		vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);

		int vi=0;

		// Bottom
		vba.set3(vi,0,box.maxs.x,box.mins.y,box.mins.z); vba.set3(vi,1,0,0,-Math::ONE); vba.set2(vi,2,0,Math::ONE); ++vi;
		vba.set3(vi,0,box.mins.x,box.mins.y,box.mins.z); vba.set3(vi,1,0,0,-Math::ONE); vba.set2(vi,2,Math::ONE,Math::ONE); ++vi;
		vba.set3(vi,0,box.mins.x,box.maxs.y,box.mins.z); vba.set3(vi,1,0,0,-Math::ONE); vba.set2(vi,2,Math::ONE,0); ++vi;
		vba.set3(vi,0,box.maxs.x,box.maxs.y,box.mins.z); vba.set3(vi,1,0,0,-Math::ONE); vba.set2(vi,2,0,0); ++vi;

		// Top
		vba.set3(vi,0,box.mins.x,box.mins.y,box.maxs.z); vba.set3(vi,1,0,0,Math::ONE); vba.set2(vi,2,0,Math::ONE); ++vi;
		vba.set3(vi,0,box.maxs.x,box.mins.y,box.maxs.z); vba.set3(vi,1,0,0,Math::ONE); vba.set2(vi,2,Math::ONE,Math::ONE); ++vi;
		vba.set3(vi,0,box.maxs.x,box.maxs.y,box.maxs.z); vba.set3(vi,1,0,0,Math::ONE); vba.set2(vi,2,Math::ONE,0); ++vi;
		vba.set3(vi,0,box.mins.x,box.maxs.y,box.maxs.z); vba.set3(vi,1,0,0,Math::ONE); vba.set2(vi,2,0,0); ++vi;

		// Left
		vba.set3(vi,0,box.mins.x,box.mins.y,box.mins.z); vba.set3(vi,1,-Math::ONE,0,0); vba.set2(vi,2,0,Math::ONE); ++vi;
		vba.set3(vi,0,box.mins.x,box.mins.y,box.maxs.z); vba.set3(vi,1,-Math::ONE,0,0); vba.set2(vi,2,Math::ONE,Math::ONE); ++vi;
		vba.set3(vi,0,box.mins.x,box.maxs.y,box.maxs.z); vba.set3(vi,1,-Math::ONE,0,0); vba.set2(vi,2,Math::ONE,0); ++vi;
		vba.set3(vi,0,box.mins.x,box.maxs.y,box.mins.z); vba.set3(vi,1,-Math::ONE,0,0); vba.set2(vi,2,0,0); ++vi;

		// Right
		vba.set3(vi,0,box.maxs.x,box.mins.y,box.maxs.z); vba.set3(vi,1,Math::ONE,0,0); vba.set2(vi,2,0,Math::ONE); ++vi;
		vba.set3(vi,0,box.maxs.x,box.mins.y,box.mins.z); vba.set3(vi,1,Math::ONE,0,0); vba.set2(vi,2,Math::ONE,Math::ONE); ++vi;
		vba.set3(vi,0,box.maxs.x,box.maxs.y,box.mins.z); vba.set3(vi,1,Math::ONE,0,0); vba.set2(vi,2,Math::ONE,0); ++vi;
		vba.set3(vi,0,box.maxs.x,box.maxs.y,box.maxs.z); vba.set3(vi,1,Math::ONE,0,0); vba.set2(vi,2,0,0); ++vi;

		// Back
		vba.set3(vi,0,box.mins.x,box.mins.y,box.mins.z); vba.set3(vi,1,0,-Math::ONE,0); vba.set2(vi,2,0,Math::ONE); ++vi;
		vba.set3(vi,0,box.maxs.x,box.mins.y,box.mins.z); vba.set3(vi,1,0,-Math::ONE,0); vba.set2(vi,2,Math::ONE,Math::ONE); ++vi;
		vba.set3(vi,0,box.maxs.x,box.mins.y,box.maxs.z); vba.set3(vi,1,0,-Math::ONE,0); vba.set2(vi,2,Math::ONE,0); ++vi;
		vba.set3(vi,0,box.mins.x,box.mins.y,box.maxs.z); vba.set3(vi,1,0,-Math::ONE,0); vba.set2(vi,2,0,0); ++vi;

		// Front
		vba.set3(vi,0,box.mins.x,box.maxs.y,box.maxs.z); vba.set3(vi,1,0,Math::ONE,0); vba.set2(vi,2,0,Math::ONE); ++vi;
		vba.set3(vi,0,box.maxs.x,box.maxs.y,box.maxs.z); vba.set3(vi,1,0,Math::ONE,0); vba.set2(vi,2,Math::ONE,Math::ONE); ++vi;
		vba.set3(vi,0,box.maxs.x,box.maxs.y,box.mins.z); vba.set3(vi,1,0,Math::ONE,0); vba.set2(vi,2,Math::ONE,0); ++vi;
		vba.set3(vi,0,box.mins.x,box.maxs.y,box.mins.z); vba.set3(vi,1,0,Math::ONE,0); vba.set2(vi,2,0,0); ++vi;

		vba.unlock();
	}

	IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT8,36);
	{
		iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);

		int i=0;
		iba.set(i++,0);	iba.set(i++,1); iba.set(i++,2);
		iba.set(i++,0);	iba.set(i++,2); iba.set(i++,3);

		iba.set(i++,4);	iba.set(i++,5); iba.set(i++,6);
		iba.set(i++,4);	iba.set(i++,6); iba.set(i++,7);

		iba.set(i++,8);	iba.set(i++,9); iba.set(i++,10);
		iba.set(i++,8);	iba.set(i++,10); iba.set(i++,11);

		iba.set(i++,12);iba.set(i++,13); iba.set(i++,14);
		iba.set(i++,12);iba.set(i++,14); iba.set(i++,15);

		iba.set(i++,16);iba.set(i++,17); iba.set(i++,18);
		iba.set(i++,16);iba.set(i++,18); iba.set(i++,19);

		iba.set(i++,20);iba.set(i++,21); iba.set(i++,22);
		iba.set(i++,20);iba.set(i++,22); iba.set(i++,23);

		iba.unlock();
	}

	Mesh::SubMesh::ptr subMesh(new Mesh::SubMesh());
	subMesh->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer));
	if(material==NULL){
		material=mEngine->getMaterialManager()->createDiffuseMaterial(NULL);
	}
	material->retain();
	subMesh->material=material;

	Mesh::ptr mesh(new Mesh());
	mesh->addSubMesh(subMesh);
	mesh->setBound(Bound(box));
	mesh->setStaticVertexData(VertexData::ptr(new VertexData(vertexBuffer)));

	return mesh;
}

}
}
}
