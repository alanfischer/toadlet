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

#include "MeshDataConverter.h"

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::mesh;

namespace bsp{

void MeshDataConverter::convertMeshDataToInputData(MeshData *meshData,InputData *in,int submesh){
	int i,j;

	VertexBuffer *vertexBuffer=meshData->getVertexBuffer();
	in->vertexes.resize(vertexBuffer->getNumVertexes());
	for(i=0;i<vertexBuffer->getNumVertexes();++i){
		in->vertexes[i]=vertexBuffer->position(i);
	}

	int total=0;
	if(submesh==-1){
		for(i=0;i<meshData->getNumSubMeshDatas();++i){
			total+=meshData->getSubMeshData(i)->getIndexBuffer()->getNumIndexes()/3;
		}
	}
	else{
		total+=meshData->getSubMeshData(submesh)->getIndexBuffer()->getNumIndexes()/3;
	}

	in->polygons.resize(total);

	List<Polygon>::iterator it=in->polygons.begin();

	if(submesh==-1){
		int count=0;
		for(i=0;i<meshData->getNumSubMeshDatas();++i){
			IndexBuffer *indexBuffer=meshData->getSubMeshData(i)->getIndexBuffer();
			for(j=0;j<indexBuffer->getNumIndexes();j+=3,++it){
				it->indexes.resize(3);
				it->indexes[0]=indexBuffer->index(j+0);
				it->indexes[1]=indexBuffer->index(j+1);
				it->indexes[2]=indexBuffer->index(j+2);
				it->flags=count;
				count++;
			}
		}
	}
	else{
		int count=0;
		IndexBuffer *indexBuffer=meshData->getSubMeshData(submesh)->getIndexBuffer();
		for(j=0;j<indexBuffer->getNumIndexes();j+=3,++it){
			it->indexes.resize(3);
			it->indexes[0]=indexBuffer->index(j+0);
			it->indexes[1]=indexBuffer->index(j+1);
			it->indexes[2]=indexBuffer->index(j+2);
			it->flags=count;
			count++;
		}
	}
}

}

