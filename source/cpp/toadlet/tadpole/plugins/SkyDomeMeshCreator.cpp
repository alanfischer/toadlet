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
#include "SkyDomeMeshCreator.h"

namespace toadlet{
namespace tadpole{

Mesh::ptr SkyDomeMeshCreator::createSkyDomeMesh(VertexBuffer::ptr vertexBuffer,IndexBuffer::ptr indexBuffer,const Sphere &sphere,int numSegments,int numRings,scalar fade){
	{
		vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);
		iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);

		int positionIndex=vertexBuffer->getVertexFormat()->findElement(VertexFormat::Semantic_POSITION);
		int normalIndex=vertexBuffer->getVertexFormat()->findElement(VertexFormat::Semantic_NORMAL);
		int texCoordIndex=vertexBuffer->getVertexFormat()->findElement(VertexFormat::Semantic_TEXCOORD);

		scalar deltaRingAngle=Math::div(Math::HALF_PI,Math::fromInt(numRings));
		scalar deltaSegAngle=Math::div(Math::TWO_PI,Math::fromInt(numSegments));
		unsigned short verticeIndex=0;
		unsigned short indexIndex=0;

		Vector3 normal;

		// Generate the group of rings for the sphere
		int ring;
		for(ring=0;ring<=numRings;++ring){
			scalar r0=Math::mul(sphere.radius,Math::sin(Math::mul(Math::fromInt(ring),deltaRingAngle)));
			scalar z0=Math::mul(sphere.radius,Math::cos(Math::mul(Math::fromInt(ring),deltaRingAngle)));

			// Generate the group of segments for the current ring
			int seg;
			for(seg=0;seg<=numSegments;seg++){
				scalar x0=Math::mul(r0,Math::cos(Math::mul(Math::fromInt(seg),deltaSegAngle)));
				scalar y0=Math::mul(r0,Math::sin(Math::mul(Math::fromInt(seg),deltaSegAngle)));

				// Add one vertex to the strip which makes up the sphere
				if(positionIndex>=0){
					vba.set3(verticeIndex,positionIndex,sphere.origin.x+x0,sphere.origin.y+y0,sphere.origin.z+z0);
				}

				normal.set(x0,y0,z0);
				Math::normalize(normal);
				if(normalIndex>=0){
					vba.set3(verticeIndex,normalIndex,normal.x,normal.y,normal.z);
				}

				normal.z=Math::ONE-normal.z;
				scalar l=Math::length(normal);
				normal.z=0;
				Math::normalizeCarefully(normal,0);
				Math::mul(normal,l);
				scalar tx=Math::mul(normal.x,fade)+Math::HALF,ty=Math::mul(normal.y,fade)+Math::HALF;
				tx=Math::clamp(0,Math::ONE,tx);
				ty=Math::clamp(0,Math::ONE,ty);

				if(texCoordIndex>=0){
					vba.set2(verticeIndex,texCoordIndex,tx,ty);
				}

				if(ring!=numRings){
					iba.set(indexIndex++,verticeIndex+numSegments);
					iba.set(indexIndex++,verticeIndex);
					iba.set(indexIndex++,verticeIndex+numSegments+1);
					iba.set(indexIndex++,verticeIndex);
					iba.set(indexIndex++,verticeIndex+1);
					iba.set(indexIndex++,verticeIndex+numSegments+1);
				}
				verticeIndex++;
			}
		}

		iba.unlock();
		vba.unlock();
	}

	Mesh::SubMesh::ptr subMesh(new Mesh::SubMesh());
	subMesh->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer));

	Mesh::ptr mesh(new Mesh());
	mesh->addSubMesh(subMesh);
	mesh->setStaticVertexData(VertexData::ptr(new VertexData(vertexBuffer)));
	mesh->setBound(Bound(sphere));

	return mesh;
}

Mesh::ptr SkyDomeMeshCreator::createSkyDomeMesh(const Sphere &sphere,int numSegments,int numRings,scalar fade,Material::ptr material){
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,getSkyDomeVertexCount(numSegments,numRings));
	IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,getSkyDomeIndexCount(numSegments,numRings));

	Mesh::ptr mesh=createSkyDomeMesh(vertexBuffer,indexBuffer,sphere,numSegments,numRings,fade);
	if(material==NULL){
		material=mEngine->createSkyBoxMaterial(NULL);
	}
	mesh->getSubMesh(0)->material=material;

	return mesh;
}

}
}
