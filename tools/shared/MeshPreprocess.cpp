/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine 
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 * 
 * All source code for the Toadlet Engine, including 
 * this file, is the sole property of Lightning Toads 
 * Productions, LLC. It has been developed on our own 
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed 
 * without our explicit permission. 
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#include <toadlet/egg/Collection.h>
#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/peeper/VertexBufferAccessor.h>

#include "MeshPreprocess.h"

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::mesh;

#define EPSILON 0.0001f

template <typename T> void addNoDuplicates(Collection<T> &collection,const T &t){
	int i;
	for(i=0;i<collection.size();++i){
		if(collection[i]==t){
			break;
		}
	}
	if(i==collection.size()){
		collection.add(t);
	}
}

void MeshPreprocess::adjustTexCoords(Mesh *mesh,float texCoordAdjust){
	VertexData::ptr vertexData=mesh->staticVertexData;
	VertexBuffer::ptr vertexBuffer=vertexData->getVertexBuffer(0);
	VertexFormat::ptr vertexFormat=vertexBuffer->getVertexFormat();
	int texCoordIndex=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_TEX_COORD);

	VertexBufferAccessor vba(vertexBuffer);
	if(texCoordAdjust>0 && texCoordIndex>=0){
		TOADLET_ASSERT(vertexFormat->getVertexElement(texCoordIndex).format==(VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_2));

		Collection<Collection<int> > texCoordFriends;
		texCoordFriends.resize(vertexBuffer->getSize());

		int i;
		for(i=0;i<mesh->subMeshes.size();++i){
			Mesh::SubMesh::ptr sub=mesh->subMeshes[i];
			IndexData::ptr indexData=sub->indexData;
			IndexBuffer::ptr indexBuffer=indexData->getIndexBuffer();
			IndexBufferAccessor iba(indexBuffer);

			TOADLET_ASSERT(indexBuffer->getIndexFormat()==IndexBuffer::IndexFormat_UINT_16);

			int j;
			for(j=0;j<indexBuffer->getSize()/3;++j){
				int i0=iba.get(j*3+0);
				int i1=iba.get(j*3+1);
				int i2=iba.get(j*3+2);

				{
					Collection<int> &friends=texCoordFriends[i0];
					addNoDuplicates(friends,i1);
					addNoDuplicates(friends,i2);
				}

				{
					Collection<int> &friends=texCoordFriends[i1];
					addNoDuplicates(friends,i0);
					addNoDuplicates(friends,i2);
				}

				{
					Collection<int> &friends=texCoordFriends[i2];
					addNoDuplicates(friends,i0);
					addNoDuplicates(friends,i1);
				}
			}
		}

		Vector2 texCoord;
		for(i=0;i<texCoordFriends.size();++i){
			Collection<int> &friends=texCoordFriends[i];

			Vector2 avg;
			int j;
			for(j=0;j<friends.size();++j){
				vba.get2(friends[j],texCoordIndex,texCoord);
				avg+=texCoord;
			}
			avg/=friends.size();

			vba.get2(i,texCoordIndex,texCoord);
			Vector2 adjust=(avg-texCoord);
			if(Math::length(adjust)>EPSILON){
				Math::normalize(adjust);
				adjust*=texCoordAdjust;
				texCoord+=adjust;
				vba.set2(i,texCoordIndex,texCoord);
			}
		}
	}
}

void MeshPreprocess::adjustPositions(Mesh *mesh,float positionAdjust){
	VertexData::ptr vertexData=mesh->staticVertexData;
	VertexBuffer::ptr vertexBuffer=vertexData->getVertexBuffer(0);
	VertexFormat::ptr vertexFormat=vertexBuffer->getVertexFormat();
	int positionIndex=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_POSITION);

	VertexBufferAccessor vba(vertexBuffer);
	if(positionAdjust!=0){
		int i;
		for(i=0;i<mesh->subMeshes.size();++i){
			Mesh::SubMesh::ptr sub=mesh->subMeshes[i];
			IndexData::ptr indexData=sub->indexData;
			IndexBuffer::ptr indexBuffer=indexData->getIndexBuffer();
			IndexBufferAccessor iba(indexBuffer);

			TOADLET_ASSERT(indexBuffer->getIndexFormat()==IndexBuffer::IndexFormat_UINT_16);

			Vector3 pi0,pi1,pi2;
			int j;
			for(j=0;j<indexBuffer->getSize()/3;++j){
				int i0=iba.get(j*3+0);
				int i1=iba.get(j*3+1);
				int i2=iba.get(j*3+2);

				vba.get3(i0,positionIndex,pi0);
				vba.get3(i1,positionIndex,pi1);
				vba.get3(i2,positionIndex,pi2);

				Vector3 center;
				center+=pi0;
				center+=pi1;
				center+=pi2;
				center/=3.0f;

				Vector3 dir;

				dir=pi0-center;
				if(Math::length(dir)>EPSILON){
					Math::normalize(dir);
					dir*=positionAdjust;
					pi0+=dir;
					vba.set3(i0,positionIndex,pi0);
				}

				dir=pi1-center;
				if(Math::length(dir)>EPSILON){
					Math::normalize(dir);
					dir*=positionAdjust;
					pi1+=dir;
					vba.set3(i1,positionIndex,pi1);
				}

				dir=pi2-center;
				if(Math::length(dir)>EPSILON){
					Math::normalize(dir);
					dir*=positionAdjust;
					pi2+=dir;
					vba.set3(i2,positionIndex,pi2);
				}
			}
		}
	}
}
