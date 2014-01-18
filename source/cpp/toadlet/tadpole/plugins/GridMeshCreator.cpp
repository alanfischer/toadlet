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
#include "GridMeshCreator.h"

namespace toadlet{
namespace tadpole{

Mesh::ptr GridMeshCreator::createGridMesh(VertexBuffer::ptr vertexBuffer,IndexBuffer::ptr indexBuffer,scalar width,scalar height,int numWidth,int numHeight){
	if(numWidth<=1 || numHeight<=1){
		return NULL;
	}

	{
		vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);
		iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);

		int positionIndex=vertexBuffer->getVertexFormat()->findElement(VertexFormat::Semantic_POSITION);
		int normalIndex=vertexBuffer->getVertexFormat()->findElement(VertexFormat::Semantic_NORMAL);
		int texCoordIndex=vertexBuffer->getVertexFormat()->findElement(VertexFormat::Semantic_TEXCOORD);

		int x,y,ii=0,vi=0;
		// Vertexes
		for(y=0;y<numHeight;++y){
			for(x=0;x<numWidth;++x){
				if(positionIndex>=0){
					vba.set3(vi,positionIndex,
						Math::div(Math::mul(width,Math::fromInt(x)),Math::fromInt(numWidth-1))-width/2.0,
						Math::div(Math::mul(height,Math::fromInt(y)),Math::fromInt(numHeight-1))-height/2.0,
						0
					);
				}
				if(normalIndex>=0){
					vba.set3(vi,normalIndex,0,0,Math::ONE);
				}
				if(texCoordIndex>=0){
					vba.set2(vi,texCoordIndex,
						Math::div(Math::fromInt(x),Math::fromInt(numWidth-1)),
						Math::div(Math::fromInt(y),Math::fromInt(numHeight-1))
					);
				}
				vi++;
			}
		}
		// Indexes
		for(y=0;y<numHeight-1;++y){
			if(y%2==0){ // Even row
				for(x=0;x<numWidth;++x){
					iba.set(ii++, x+(y*numWidth)+numWidth);
					iba.set(ii++, x+(y*numWidth ));
				}
				if(y!=numHeight-2){ // Add degenerate if not last row
					iba.set(ii++, --x + (y*numWidth));
				}
			}
			else{ // Odd row
				for(x=numWidth-1;x>=0;--x){
					iba.set(ii++, x+(y*numWidth)+numWidth);
					iba.set(ii++, x+(y*numWidth));
				}
				if(y!=numHeight-2){ // Add degenerate if not last row
					iba.set(ii++, ++x + (y*numWidth));
				}
			}
		}

		iba.unlock();
		vba.unlock();
	}

	Mesh::SubMesh::ptr subMesh=new Mesh::SubMesh();
	subMesh->indexData=new IndexData(IndexData::Primitive_TRISTRIP,indexBuffer);

	Mesh::ptr mesh=new Mesh();
	mesh->addSubMesh(subMesh);
	mesh->setStaticVertexData(new VertexData(vertexBuffer));
	mesh->setBound(new Bound(AABox(-width/2,-height/2,0,width/2,height/2,0)));

	return mesh;
}

Mesh::ptr GridMeshCreator::createGridMesh(scalar width,scalar height,int numWidth,int numHeight,Material::ptr material){
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,getGridVertexCount(numWidth,numHeight));
	IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,getGridIndexCount(numWidth,numHeight));

	Mesh::ptr mesh=createGridMesh(vertexBuffer,indexBuffer,width,height,numWidth,numHeight);
	if(material==NULL){
		material=mEngine->createDiffuseMaterial(NULL);
	}
	mesh->getSubMesh(0)->material=material;

	return mesh;
}

}
}
