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
#include "SkyBoxMeshCreator.h"

namespace toadlet{
namespace tadpole{

Mesh::ptr SkyBoxMeshCreator::createSkyBoxMesh(scalar size,bool unfolded,bool invert,Material::ptr bottom,Material::ptr top,Material::ptr left,Material::ptr right,Material::ptr back,Material::ptr front){
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_TEX_COORD,24);
	IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT8,36);
	{
		vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);
		iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);

		int vi=0,ii=0;
		scalar one=Math::ONE;
		scalar epsilon=Math::ONE/1024;
		scalar third=Math::ONE/3 + epsilon;
		scalar third2=Math::ONE*2/3 - epsilon;
		scalar quarter=Math::ONE/4;
		scalar half=Math::ONE/2;
		scalar quarter3=Math::ONE*3/4;
		scalar top=invert?one:0;
		scalar bot=invert?0:one;

		// Bottom
		vba.set3(vi,0,-size,-size,-size); if(unfolded) vba.set2(vi,1, quarter, one);	else vba.set2(vi,1, 0, top);	vi++;
		vba.set3(vi,0, size,-size,-size); if(unfolded) vba.set2(vi,1, half, one);		else vba.set2(vi,1, one, top);	vi++;
		vba.set3(vi,0, size, size,-size); if(unfolded) vba.set2(vi,1, half, third2);	else vba.set2(vi,1, one, bot);	vi++;
		vba.set3(vi,0,-size, size,-size); if(unfolded) vba.set2(vi,1, quarter, third2);	else vba.set2(vi,1, 0, bot);	vi++;

		iba.set(ii++,0);		iba.set(ii++,1);		iba.set(ii++,2);
		iba.set(ii++,2);		iba.set(ii++,3);		iba.set(ii++,0);

		// Top
		vba.set3(vi,0,-size,-size, size); if(unfolded) vba.set2(vi,1, quarter, 0);		else vba.set2(vi,1, one, top);	vi++;
		vba.set3(vi,0, size,-size, size); if(unfolded) vba.set2(vi,1, half, 0);			else vba.set2(vi,1, 0, top);	vi++;
		vba.set3(vi,0, size, size, size); if(unfolded) vba.set2(vi,1, half, third);		else vba.set2(vi,1, 0, bot);	vi++;
		vba.set3(vi,0,-size, size, size); if(unfolded) vba.set2(vi,1, quarter, third);	else vba.set2(vi,1, one, bot);	vi++;

		iba.set(ii++,5);		iba.set(ii++,4);		iba.set(ii++,6);
		iba.set(ii++,7);		iba.set(ii++,6);		iba.set(ii++,4);

		// Left
		vba.set3(vi,0,-size,-size,-size); if(unfolded) vba.set2(vi,1, 0, third2);		else vba.set2(vi,1, 0, top);	vi++;
		vba.set3(vi,0,-size, size,-size); if(unfolded) vba.set2(vi,1, quarter, third2);	else vba.set2(vi,1, one, top);	vi++;
		vba.set3(vi,0,-size, size, size); if(unfolded) vba.set2(vi,1, quarter, third);	else vba.set2(vi,1, one, bot);	vi++;
		vba.set3(vi,0,-size,-size, size); if(unfolded) vba.set2(vi,1, 0, third);		else vba.set2(vi,1, 0, bot);	vi++;

		iba.set(ii++,9);		iba.set(ii++,10);		iba.set(ii++,8);
		iba.set(ii++,8);		iba.set(ii++,10);		iba.set(ii++,11);

		// Right
		vba.set3(vi,0, size,-size,-size); if(unfolded) vba.set2(vi,1, quarter3, third2);else vba.set2(vi,1, one, top);	vi++;
		vba.set3(vi,0, size, size,-size); if(unfolded) vba.set2(vi,1, half, third2);	else vba.set2(vi,1, 0, top);	vi++;
		vba.set3(vi,0, size, size, size); if(unfolded) vba.set2(vi,1, half, third);		else vba.set2(vi,1, 0, bot);	vi++;
		vba.set3(vi,0, size,-size, size); if(unfolded) vba.set2(vi,1, quarter3, third);	else vba.set2(vi,1, one, bot);	vi++;

		iba.set(ii++,14);		iba.set(ii++,13);		iba.set(ii++,12);
		iba.set(ii++,14);		iba.set(ii++,12);		iba.set(ii++,15);

		// Back
		vba.set3(vi,0,-size,-size,-size); if(unfolded) vba.set2(vi,1, one, third2);		else vba.set2(vi,1, one, top);	vi++;
		vba.set3(vi,0, size,-size,-size); if(unfolded) vba.set2(vi,1, quarter3, third2);else vba.set2(vi,1, 0, top);	vi++;
		vba.set3(vi,0, size,-size, size); if(unfolded) vba.set2(vi,1, quarter3, third);	else vba.set2(vi,1, 0, bot);	vi++;
		vba.set3(vi,0,-size,-size, size); if(unfolded) vba.set2(vi,1, one, third);		else vba.set2(vi,1, one, bot);	vi++;

		iba.set(ii++,18);		iba.set(ii++,17);		iba.set(ii++,16);
		iba.set(ii++,18);		iba.set(ii++,16);		iba.set(ii++,19);

		// Front
		vba.set3(vi,0,-size, size,-size); if(unfolded) vba.set2(vi,1, quarter, third2);	else vba.set2(vi,1, 0, top);	vi++;
		vba.set3(vi,0, size, size,-size); if(unfolded) vba.set2(vi,1, half, third2);	else vba.set2(vi,1, one, top);	vi++;
		vba.set3(vi,0, size, size, size); if(unfolded) vba.set2(vi,1, half, third);		else vba.set2(vi,1, one, bot);	vi++;
		vba.set3(vi,0,-size, size, size); if(unfolded) vba.set2(vi,1, quarter, third);	else vba.set2(vi,1, 0, bot);	vi++;

		iba.set(ii++,21);		iba.set(ii++,22);		iba.set(ii++,20);
		iba.set(ii++,20);		iba.set(ii++,22);		iba.set(ii++,23);

		iba.unlock();
		vba.unlock();
	}

	Mesh::ptr mesh(new Mesh());
	mesh->setStaticVertexData(VertexData::ptr(new VertexData(vertexBuffer)));
	mesh->setBound(Bound(AABox(-size,-size,-size,size,size,size)));

	if(unfolded){
		mesh->addSubMesh(Mesh::SubMesh::ptr(new Mesh::SubMesh()));
		mesh->getSubMesh(0)->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer));
	}
	else{
		mesh->addSubMesh(Mesh::SubMesh::ptr(new Mesh::SubMesh()));
		mesh->getSubMesh(0)->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,6));
		mesh->addSubMesh(Mesh::SubMesh::ptr(new Mesh::SubMesh()));
		mesh->getSubMesh(1)->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,6,6));
		mesh->addSubMesh(Mesh::SubMesh::ptr(new Mesh::SubMesh()));
		mesh->getSubMesh(2)->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,12,6));
		mesh->addSubMesh(Mesh::SubMesh::ptr(new Mesh::SubMesh()));
		mesh->getSubMesh(3)->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,18,6));
		mesh->addSubMesh(Mesh::SubMesh::ptr(new Mesh::SubMesh()));
		mesh->getSubMesh(4)->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,24,6));
		mesh->addSubMesh(Mesh::SubMesh::ptr(new Mesh::SubMesh()));
		mesh->getSubMesh(5)->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,30,6));
	}

	mesh->getSubMesh(0)->material=bottom;
	if(unfolded==false){
		mesh->getSubMesh(1)->material=top;
		mesh->getSubMesh(2)->material=left;
		mesh->getSubMesh(3)->material=right;
		mesh->getSubMesh(4)->material=back;
		mesh->getSubMesh(5)->material=front;
	}

	return mesh;
}

}
}
