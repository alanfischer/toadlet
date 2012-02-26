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
#include <toadlet/tadpole/creator/TorusMeshCreator.h>

namespace toadlet{
namespace tadpole{
namespace creator{

Mesh::ptr TorusMeshCreator::createTorusMesh(VertexBuffer::ptr vertexBuffer,scalar majorRadius,scalar minorRadius,int numMajor,int numMinor){
	{
		vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);

		Vector3 normal;
		scalar majorStep=Math::TWO_PI/numMajor;
		scalar minorStep=Math::TWO_PI/numMinor;
		int i,j;
		for(i=0;i<numMajor;i++){
			scalar a0=i*majorStep;
			scalar a1=a0+majorStep;
			scalar x0=Math::cos(a0);
			scalar y0=Math::sin(a0);
			scalar x1=Math::cos(a1);
			scalar y1=Math::sin(a1);

			for(j=0;j<=numMinor;++j){
				int index=(i*(numMinor+1)+j)*2;
				scalar b=j*minorStep;
				scalar c=Math::cos(b);
				scalar r=Math::mul(minorRadius,c)+majorRadius;
				scalar z=Math::mul(minorRadius,Math::sin(b));

				vba.set3(index,0,Math::mul(x0,r),Math::mul(y0,r),z);
				normal.set(Math::mul(x0,c),Math::mul(y0,c),Math::div(z,minorRadius));
				Math::normalize(normal);
				vba.set3(index,1,normal);
				vba.set2(index,2,Math::div(Math::fromInt(i),Math::fromInt(numMajor)),Math::div(Math::fromInt(j),Math::fromInt(numMinor)));

				vba.set3(index+1,0,Math::mul(x1,r),Math::mul(y1,r),z);
				normal.set(Math::mul(x1,c),Math::mul(y1,c),Math::div(z,minorRadius));
				Math::normalize(normal);
				vba.set3(index+1,1,normal);
				vba.set2(index+1,2,Math::div(Math::fromInt(i+1),Math::fromInt(numMajor)),Math::div(Math::fromInt(j),Math::fromInt(numMinor)));
			}
		}

		vba.unlock();
	}

	Mesh::SubMesh::ptr subMesh(new Mesh::SubMesh());
	subMesh->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,vertexBuffer->getSize()));

	Mesh::ptr mesh(new Mesh());
	mesh->addSubMesh(subMesh);
	mesh->setStaticVertexData(VertexData::ptr(new VertexData(vertexBuffer)));
	mesh->setBound(Bound(Sphere(majorRadius)));

	return mesh;
}

Mesh::ptr TorusMeshCreator::createTorusMesh(scalar majorRadius,scalar minorRadius,int numMajor,int numMinor,Material::ptr material){
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,getTorusVertexCount(numMajor,numMinor));

	Mesh::ptr mesh=createTorusMesh(vertexBuffer,majorRadius,minorRadius,numMajor,numMinor);
	if(material==NULL){
		material=mEngine->getMaterialManager()->createDiffuseMaterial(NULL);
	}
	mesh->getSubMesh(0)->material=material;

	return mesh;
}

}
}
}
