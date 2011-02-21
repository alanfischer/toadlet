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

#include <toadlet/tadpole/MeshManager.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

MeshManager::MeshManager(Engine *engine):ResourceManager(engine->getArchiveManager()){
	mEngine=engine;
}

Resource::ptr MeshManager::manage(const Resource::ptr &resource,const String &name){
	Mesh::ptr mesh=shared_static_cast<Mesh>(resource);
	mesh->compile();
	ResourceManager::manage(mesh,name);
	return mesh;
}

Mesh::ptr MeshManager::createBox(const AABox &box,Material::ptr material){
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
		material=mEngine->getMaterialManager()->createMaterial();
		material->setLighting(true);
	}
	material->retain();
	subMesh->material=material;

	Mesh::ptr mesh(new Mesh());
	mesh->subMeshes.resize(1);
	mesh->subMeshes[0]=subMesh;
	mesh->bound->set(box);
	mesh->staticVertexData=VertexData::ptr(new VertexData(vertexBuffer));

	return mesh;
}

Mesh::ptr MeshManager::createSkyBox(scalar size,bool unfolded,bool invert,Material::ptr bottom,Material::ptr top,Material::ptr left,Material::ptr right,Material::ptr back,Material::ptr front){
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
	mesh->staticVertexData=VertexData::ptr(new VertexData(vertexBuffer));
	mesh->bound->set(AABox(-size,-size,-size,size,size,size));

	if(unfolded){
		mesh->subMeshes.add(Mesh::SubMesh::ptr(new Mesh::SubMesh()));
		mesh->subMeshes[0]->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer));
	}
	else{
		mesh->subMeshes.add(Mesh::SubMesh::ptr(new Mesh::SubMesh()));
		mesh->subMeshes[0]->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,6));
		mesh->subMeshes.add(Mesh::SubMesh::ptr(new Mesh::SubMesh()));
		mesh->subMeshes[1]->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,6,6));
		mesh->subMeshes.add(Mesh::SubMesh::ptr(new Mesh::SubMesh()));
		mesh->subMeshes[2]->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,12,6));
		mesh->subMeshes.add(Mesh::SubMesh::ptr(new Mesh::SubMesh()));
		mesh->subMeshes[3]->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,18,6));
		mesh->subMeshes.add(Mesh::SubMesh::ptr(new Mesh::SubMesh()));
		mesh->subMeshes[4]->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,24,6));
		mesh->subMeshes.add(Mesh::SubMesh::ptr(new Mesh::SubMesh()));
		mesh->subMeshes[5]->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,30,6));
	}

	mesh->subMeshes[0]->material=bottom;
	bottom->retain();
	if(unfolded==false){
		mesh->subMeshes[1]->material=top;
		top->retain();
		mesh->subMeshes[2]->material=left;
		left->retain();
		mesh->subMeshes[3]->material=right;
		right->retain();
		mesh->subMeshes[4]->material=back;
		back->retain();
		mesh->subMeshes[5]->material=front;
		front->retain();
	}

	return mesh;
}

Mesh::ptr MeshManager::createGrid(scalar width,scalar height,int numWidth,int numHeight,Material::ptr material){
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,getGridVertexCount(numWidth,numHeight));
	IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,getGridIndexCount(numWidth,numHeight));

	Mesh::ptr mesh=createGrid(vertexBuffer,indexBuffer,width,height,numWidth,numHeight);
	if(material==NULL){
		material=mEngine->getMaterialManager()->createMaterial();
		material->setLighting(true);
	}
	material->retain();
	mesh->subMeshes[0]->material=material;

	return mesh;
}

Mesh::ptr MeshManager::createGrid(VertexBuffer::ptr vertexBuffer,IndexBuffer::ptr indexBuffer,scalar width,scalar height,int numWidth,int numHeight){
	{
		vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);
		iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);

		int positionIndex=vertexBuffer->getVertexFormat()->findSemantic(VertexFormat::Semantic_POSITION);
		int normalIndex=vertexBuffer->getVertexFormat()->findSemantic(VertexFormat::Semantic_NORMAL);
		int texCoordIndex=vertexBuffer->getVertexFormat()->findSemantic(VertexFormat::Semantic_TEX_COORD);

	    int x,y,ii=0,vi=0;
		// Vertexes
		for(y=0;y<numHeight;++y){
			for(x=0;x<numWidth;++x){
				if(positionIndex>=0){
					vba.set3(vi,positionIndex,
						Math::div(Math::mul(width,Math::fromInt(x)),Math::fromInt(numWidth))-width/2.0,
						Math::div(Math::mul(height,Math::fromInt(y)),Math::fromInt(numHeight))-height/2.0,
						0
					);
				}
				if(normalIndex>=0){
					vba.set3(vi,normalIndex,0,0,Math::ONE);
				}
				if(texCoordIndex>=0){
					vba.set2(vi,texCoordIndex,
						Math::div(Math::fromInt(x),Math::fromInt(numWidth)),
						Math::div(Math::fromInt(y),Math::fromInt(numHeight))
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

	Mesh::SubMesh::ptr subMesh(new Mesh::SubMesh());
	subMesh->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,indexBuffer));

	Mesh::ptr mesh(new Mesh());
	mesh->subMeshes.resize(1);
	mesh->subMeshes[0]=subMesh;
	mesh->staticVertexData=VertexData::ptr(new VertexData(vertexBuffer));
	mesh->bound->set(AABox(-width/2,-height/2,0,width/2,height/2,0));

	return mesh;
}

Mesh::ptr MeshManager::createSphere(const Sphere &sphere,int numSegments,int numRings,Material::ptr material){
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,getSphereVertexCount(numSegments,numRings));
	IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,getSphereIndexCount(numSegments,numRings));

	Mesh::ptr mesh=createSphere(vertexBuffer,indexBuffer,sphere,numSegments,numRings);
	if(material==NULL){
		material=mEngine->getMaterialManager()->createMaterial();
		material->setLighting(true);
	}
	material->retain();
	mesh->subMeshes[0]->material=material;

	return mesh;
}

// Thanks to Ogre3D for this sphere routine
Mesh::ptr MeshManager::createSphere(VertexBuffer::ptr vertexBuffer,IndexBuffer::ptr indexBuffer,const Sphere &sphere,int numSegments,int numRings){
	{
		vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);
		iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);

		int positionIndex=vertexBuffer->getVertexFormat()->findSemantic(VertexFormat::Semantic_POSITION);
		int normalIndex=vertexBuffer->getVertexFormat()->findSemantic(VertexFormat::Semantic_NORMAL);
		int texCoordIndex=vertexBuffer->getVertexFormat()->findSemantic(VertexFormat::Semantic_TEX_COORD);

		scalar deltaRingAngle=Math::div(Math::PI,Math::fromInt(numRings));
		scalar deltaSegAngle=Math::div(Math::TWO_PI,Math::fromInt(numSegments));
		unsigned short verticeIndex=0;
		unsigned short indexIndex=0;

		Vector3 normal;

		// Generate the group of rings for the sphere
		int ring;
		for(ring=0;ring<=numRings;++ring){
			scalar r0=Math::mul(sphere.radius,Math::sin(Math::mul(Math::fromInt(ring),deltaRingAngle)));
			scalar y0=Math::mul(sphere.radius,Math::cos(Math::mul(Math::fromInt(ring),deltaRingAngle)));

			// Generate the group of segments for the current ring
			int seg;
			for(seg=0;seg<=numSegments;seg++){
				scalar x0=Math::mul(r0,Math::sin(Math::mul(Math::fromInt(seg),deltaSegAngle)));
				scalar z0=Math::mul(r0,Math::cos(Math::mul(Math::fromInt(seg),deltaSegAngle)));

				// Add one vertex to the strip which makes up the sphere
				if(positionIndex>=0){
					vba.set3(verticeIndex,positionIndex,sphere.origin.x+x0,sphere.origin.y+y0,sphere.origin.z+z0);
				}

				if(normalIndex>=0){
					normal.set(x0,y0,z0);
					Math::normalize(normal);
					vba.set3(verticeIndex,normalIndex,normal.x,normal.y,normal.z);
				}

				if(texCoordIndex>=0){
					vba.set2(verticeIndex,texCoordIndex,Math::div(Math::fromInt(seg),Math::fromInt(numSegments)),Math::div(Math::fromInt(ring),Math::fromInt(numRings)));
				}

				if(ring!=numRings){
					iba.set(indexIndex++,verticeIndex+numSegments+1);
					iba.set(indexIndex++,verticeIndex);
					iba.set(indexIndex++,verticeIndex+numSegments);
					iba.set(indexIndex++,verticeIndex+numSegments+1);
					iba.set(indexIndex++,verticeIndex+1);
					iba.set(indexIndex++,verticeIndex);
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
	mesh->subMeshes.resize(1);
	mesh->subMeshes[0]=subMesh;
	mesh->staticVertexData=VertexData::ptr(new VertexData(vertexBuffer));
	mesh->bound->set(sphere);

	return mesh;
}

Mesh::ptr MeshManager::createSkyDome(const Sphere &sphere,int numSegments,int numRings,scalar fade,Material::ptr material){
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,getSphereVertexCount(numSegments,numRings));
	IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,getSphereIndexCount(numSegments,numRings));

	Mesh::ptr mesh=createSkyDome(vertexBuffer,indexBuffer,sphere,numSegments,numRings,fade);
	if(material==NULL){
		material=mEngine->getMaterialManager()->createMaterial();
		material->setDepthWrite(false);
		material->setLighting(false);
	}
	material->retain();
	mesh->subMeshes[0]->material=material;

	return mesh;
}

Mesh::ptr MeshManager::createSkyDome(VertexBuffer::ptr vertexBuffer,IndexBuffer::ptr indexBuffer,const Sphere &sphere,int numSegments,int numRings,scalar fade){
	{
		vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);
		iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);

		int positionIndex=vertexBuffer->getVertexFormat()->findSemantic(VertexFormat::Semantic_POSITION);
		int normalIndex=vertexBuffer->getVertexFormat()->findSemantic(VertexFormat::Semantic_NORMAL);
		int texCoordIndex=vertexBuffer->getVertexFormat()->findSemantic(VertexFormat::Semantic_TEX_COORD);

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
	mesh->subMeshes.resize(1);
	mesh->subMeshes[0]=subMesh;
	mesh->staticVertexData=VertexData::ptr(new VertexData(vertexBuffer));
	mesh->bound->set(sphere);

	return mesh;
}

Mesh::ptr MeshManager::createGeoSphere(const Sphere &sphere,int depth,bool icosahedron,Material::ptr material){
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,getGeoSphereVertexCount(depth,icosahedron));
	IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,getGeoSphereIndexCount(depth,icosahedron));

	Mesh::ptr mesh=createGeoSphere(vertexBuffer,indexBuffer,sphere,depth,icosahedron);
	if(material==NULL){
		material=mEngine->getMaterialManager()->createMaterial();
		material->setLighting(true);
	}
	material->retain();
	mesh->subMeshes[0]->material=material;

	return mesh;
}

Mesh::ptr MeshManager::createGeoSphere(VertexBuffer::ptr vertexBuffer,IndexBuffer::ptr indexBuffer,const Sphere &sphere,int depth,bool icosahedron){
	currentSphere=sphere;
	vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);

	int vertexIndex=0;
	int i,j;

	Collection<IndexTri> old;

	if(icosahedron){
		int indices[]={
			0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 1, 1, 10, 6, 2, 6,
			7, 3, 7, 8, 4, 8, 9, 5, 9, 10, 6, 2, 1, 7, 3, 2, 8, 4, 3, 9,
			5, 4, 10, 1, 5, 11, 7, 6, 11, 8, 7, 11, 9, 8, 11, 10, 9, 11,
			6, 10
		};
		int indicesLength=sizeof(indices)/sizeof(int);
		scalar y = Math::fromMilli(447);
		scalar a = Math::fromMilli(894);
		scalar b = Math::fromMilli(276);
		scalar c = Math::fromMilli(723);
		scalar d = Math::fromMilli(850);
		scalar e = Math::fromMilli(525);
		geoSet(vertexIndex++,Vector3(0, Math::ONE, 0));
		geoSet(vertexIndex++,Vector3(a, y, 0));
		geoSet(vertexIndex++,Vector3(b, y, -d));
		geoSet(vertexIndex++,Vector3(-c, y, -e));
		geoSet(vertexIndex++,Vector3(-c, y, e));
		geoSet(vertexIndex++,Vector3(b, y, d));
		geoSet(vertexIndex++,Vector3(c, -y, -e));
		geoSet(vertexIndex++,Vector3(-b, -y, -d));
		geoSet(vertexIndex++,Vector3(-a, -y, 0));
		geoSet(vertexIndex++,Vector3(-b, -y, d));
		geoSet(vertexIndex++,Vector3(c, -y, e));
		geoSet(vertexIndex++,Vector3(0, -Math::ONE, 0));

		old.resize(indicesLength/3);
		for(i=0;i<old.size();++i){
			old[i].set(indices[i*3+0],indices[i*3+1],indices[i*3+2]);
		}
	}
	else{
		/* Six equidistant points lying on the unit sphere */
		int xplus = vertexIndex;
		geoSet(vertexIndex++,Math::X_UNIT_VECTOR3);
		int xmin = vertexIndex;
		geoSet(vertexIndex++,Math::NEG_X_UNIT_VECTOR3);
		int yplus = vertexIndex;
		geoSet(vertexIndex++,Math::Y_UNIT_VECTOR3);
		int ymin = vertexIndex;
		geoSet(vertexIndex++,Math::NEG_Y_UNIT_VECTOR3);
		int zplus = vertexIndex;
		geoSet(vertexIndex++,Math::Z_UNIT_VECTOR3);
		int zmin = vertexIndex;
		geoSet(vertexIndex++,Math::NEG_Z_UNIT_VECTOR3);

		old.resize(8);
		old[0].set(yplus,zplus,xplus);
		old[1].set(xmin, zplus,yplus);
		old[2].set(ymin, zplus,xmin);
		old[3].set(xplus,zplus,ymin);
		old[4].set(zmin, yplus,xplus);
		old[5].set(zmin, xmin, yplus);
		old[6].set(zmin, ymin, xmin);
		old[7].set(zmin, xplus,ymin);
	}

	Vector3 pt0,pt1,pt2;

	/* Subdivide each starting triangle (maxlevels - 1) times */
	int level;
	for(level=1;level<=depth;++level){
		Collection<IndexTri> next(old.size()*4);

		/*
		 * Subdivide each polygon in the old approximation and normalize the
		 * next points thus generated to lie on the surface of the unit
		 * sphere. Each input triangle with vertBuf labelled [0,1,2] as
		 * shown below will be turned into four next triangles:
		 * 
		 * Make next points
		 *   a = (0+2)/2
		 *   b = (0+1)/2
		 *   c = (1+2)/2
		 *   
		 * 1   /\   Normalize a, b, c
		 *    /  \
		 * b /____\ c
		 * 
		 * Construct next triangles
		 * 
		 *    /\    /\   [0,b,a] 
		 *   /  \  /  \  [b,1,c]
		 *  /____\/____\ [a,b,c]
		 *  0 a 2 [a,c,2]
		 */
		for(i=0;i<old.size();++i){
			int newi=i*4;
			IndexTri oldt=old[i],newt=next[newi];

			vba.get3(oldt.index[0],0,pt0);
			vba.get3(oldt.index[1],0,pt1);
			vba.get3(oldt.index[2],0,pt2);
			Vector3 av;Math::lerp(av,pt0,pt2,Math::HALF);Math::normalize(av);
			Vector3 bv;Math::lerp(bv,pt0,pt1,Math::HALF);Math::normalize(bv);
			Vector3 cv;Math::lerp(cv,pt1,pt2,Math::HALF);Math::normalize(cv);
			int a=vertexIndex;
			geoSet(vertexIndex++,av);
			int b=vertexIndex;
			geoSet(vertexIndex++,bv);
			int c=vertexIndex;
			geoSet(vertexIndex++,cv);

			next[newi++].set(oldt.index[0],b,a);
			next[newi++].set(b,oldt.index[1],c);
            next[newi++].set(a,b,c);
            next[newi++].set(a,c,oldt.index[2]);
        }

        /* Continue subdividing next triangles */
        old=next;
    }

	vba.unlock();

	iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);
	for(i=0;i<old.size();++i){
		for(j=0;j<3;++j){
			iba.set(i*3+j,old[i].index[j]);
		}
	}
	iba.unlock();

	Mesh::SubMesh::ptr subMesh(new Mesh::SubMesh());
	subMesh->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer));

	Mesh::ptr mesh(new Mesh());
	mesh->subMeshes.resize(1);
	mesh->subMeshes[0]=subMesh;
	mesh->staticVertexData=VertexData::ptr(new VertexData(vertexBuffer));
	mesh->bound->set(sphere);

	return mesh;
}

Mesh::ptr MeshManager::createTorus(scalar majorRadius,scalar minorRadius,int numMajor,int numMinor,Material::ptr material){
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,getTorusVertexCount(numMajor,numMinor));

	Mesh::ptr mesh=createTorus(vertexBuffer,majorRadius,minorRadius,numMajor,numMinor);
	if(material==NULL){
		material=mEngine->getMaterialManager()->createMaterial();
		material->setLighting(true);
	}
	material->retain();
	mesh->subMeshes[0]->material=material;

	return mesh;
}

Mesh::ptr MeshManager::createTorus(VertexBuffer::ptr vertexBuffer,scalar majorRadius,scalar minorRadius,int numMajor,int numMinor){
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
	mesh->subMeshes.resize(1);
	mesh->subMeshes[0]=subMesh;
	mesh->staticVertexData=VertexData::ptr(new VertexData(vertexBuffer));
	mesh->bound->set(majorRadius);

	return mesh;
}

void MeshManager::geoSet(int vertexIndex,const Vector3 &vec){
	vba.set3(vertexIndex,0,
		Math::mul(vec.x,currentSphere.radius)+currentSphere.origin.x,
		Math::mul(vec.y,currentSphere.radius)+currentSphere.origin.y,
		Math::mul(vec.z,currentSphere.radius)+currentSphere.origin.z);

	scalar l=Math::length(vec);
	scalar nx=Math::div(vec.x,l),ny=Math::div(vec.y,l),nz=Math::div(vec.z,l);
	vba.set3(vertexIndex,1,nx,ny,nz);

	// This algorithm generates a distortion around the .5 texture coordinate, since it quickly goes from .5 to 1.5
	//  so you see it flip the whole texture around, this should be fixed
	scalar tx=Math::div(Math::atan2(ny,nx),Math::TWO_PI)+Math::ONE;
	scalar ty=nz/2+Math::HALF;
	vba.set2(vertexIndex,2,tx,ty);
}

}
}

