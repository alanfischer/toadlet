#include "MeshOptimizer.h"

#include <iostream>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::mesh;

MeshOptimizer::MeshOptimizer(){
	mEpsilon=0.001f;
}

MeshOptimizer::~MeshOptimizer(){
}

bool MeshOptimizer::optimizeMesh(Mesh *mesh,Engine *engine){
	int i,j;

	// Remove duplicate sub meshes
	int combinedSubmeshes=0;
	for(i=0;i<mesh->subMeshes.size();++i){
		Mesh::SubMesh::ptr subMesh1=mesh->subMeshes[i];

		for(j=i+1;j<mesh->subMeshes.size();++j){
			Mesh::SubMesh::ptr subMesh2=mesh->subMeshes[j];

			{ // If we could have more than 1 material per submesh, we would check that they both have 1 material here
				Material::ptr material1=subMesh1->material;
				Material::ptr material2=subMesh2->material;

				bool same=false;
				if(material1!=NULL && material2!=NULL){
					same=true;
					if(material1->getLightEffect()!=material2->getLightEffect()){
						same=false;
					}
					if(material1->getLighting()!=material2->getLighting()){
						same=false;
					}
					if(material1->getFaceCulling()!=material2->getFaceCulling()){
						same=false;
					}
					if(material1->getNumTextureStages()!=material2->getNumTextureStages()){
						same=false;
					}
					else{
						int k;
						for(k=0;k<material1->getNumTextureStages();++k){
							TextureStage::ptr textureStage1=material1->getTextureStage(k);
							TextureStage::ptr textureStage2=material2->getTextureStage(k);
							if(textureStage1->getTextureName()!=textureStage2->getTextureName()){
								same=false;
								break;
							}
						}
					}
				}

				if(same){
					IndexBuffer::ptr ib1=subMesh1->indexData->getIndexBuffer();
					IndexBuffer::ptr ib2=subMesh2->indexData->getIndexBuffer();
					IndexBuffer::ptr cib=engine->getBufferManager()->cloneIndexBuffer(ib1,ib1->getUsageFlags(),ib1->getAccessType(),ib1->getIndexFormat(),ib1->getSize()+ib2->getSize());

					IndexBufferAccessor ciba(cib);
					IndexBufferAccessor iba2(ib2);
					int oldSize=ib1->getSize();
					int k;
					for(k=0;k<ib2->getSize();++k){
						ciba.set(oldSize+k,iba2.get(k));
					}

					subMesh1->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,cib,0,cib->getSize()));

					mesh->subMeshes.removeAt(j);

					combinedSubmeshes++;

					i--;
					break;
				}
			}
		}
	}

	// Zero out unused texture coodinates
	VertexBuffer::ptr vertexBuffer=mesh->staticVertexData->getVertexBuffer(0);
	VertexFormat::ptr vertexFormat=vertexBuffer->getVertexFormat();
	int positionIndex=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_POSITION);
	int normalIndex=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_NORMAL);
	int colorIndex=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_COLOR_DIFFUSE);
	int texCoordIndex=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_TEX_COORD);
	if(texCoordIndex>=0){
		Collection<uint8> vertHasTex;
		vertHasTex.resize(vertexBuffer->getSize(),0);

		for(i=0;i<mesh->subMeshes.size();++i){
			Mesh::SubMesh::ptr subMesh=mesh->subMeshes[i];

			Material::ptr material=subMesh->material;
			if(material!=NULL && material->getNumTextureStages()>0){
				IndexBufferAccessor iba(subMesh->indexData->getIndexBuffer());
				for(j=0;j<iba.getSize();++j){
					vertHasTex[iba.get(j)]=true;
				}
			}
		}

		VertexBufferAccessor vba(vertexBuffer);
		for(i=0;i<vba.getSize();++i){
			if(vertHasTex[i]==false){
				vba.set2(i,texCoordIndex,Math::ZERO_VECTOR2);
			}
		}
	}

	// Remove duplicate vertexes
	Collection<int> oldToNew;
	oldToNew.resize(vertexBuffer->getSize());

	for(i=0;i<oldToNew.size();++i){
		oldToNew[i]=i;
	}

	int combinedVertices=0;
	{
		VertexBufferAccessor vba(vertexBuffer);
		for(i=0;i<vertexBuffer->getSize();++i){
			if(oldToNew[i]!=i){ // Already collapsed
				continue;
			}

			for(j=i+1;j<vertexBuffer->getSize();++j){
				if(oldToNew[j]!=j){ // Already collapsed
					continue;
				}

				bool same=true;

				if(positionIndex>=0){
					Vector3 position1,position2;
					vba.get3(i,positionIndex,position1);
					vba.get3(j,positionIndex,position2);
					if(length(position1,position2)>mEpsilon) same=false;
				}

				if(normalIndex>=0){
					Vector3 normal1,normal2;
					vba.get3(i,normalIndex,normal1);
					vba.get3(j,normalIndex,normal2);
					if(length(normal1,normal2)>mEpsilon) same=false;
				}

				if(colorIndex>=0){
					if(vba.getRGBA(i,colorIndex)!=vba.getRGBA(j,colorIndex)) same=false;
				}

				if(texCoordIndex>=0){
					Vector2 texCoord1,texCoord2;
					vba.get2(i,texCoordIndex,texCoord1);
					vba.get2(j,texCoordIndex,texCoord2);
					if(length(texCoord1,texCoord2)>mEpsilon) same=false;
				}

				if(mesh->vertexBoneAssignments.size()>0){
					const Mesh::VertexBoneAssignmentList &a1=mesh->vertexBoneAssignments[i];
					const Mesh::VertexBoneAssignmentList &a2=mesh->vertexBoneAssignments[j];
					if(a1.size()!=a2.size()){
						same=false;
					}
					else{
						int k;
						for(k=0;k<a1.size();++k){
							if(a1[k].bone!=a2[k].bone || fabs(a1[k].weight-a2[k].weight)>mEpsilon){
								same=false;
							}
						}
					}
				}

				if(same==true){
					oldToNew[j]=i;
					combinedVertices++;
				}
			}
		}
	}

	for(i=0;i<mesh->subMeshes.size();++i){
		Mesh::SubMesh::ptr subMesh=mesh->subMeshes[i];

		IndexBufferAccessor iba(subMesh->indexData->getIndexBuffer());
		for(j=0;j<iba.getSize();++j){
			iba.set(j,oldToNew[iba.get(j)]);
		}
	}

	// Remove degenerate triangles
	int degenerateTris=0;
	Collection<int> newIndexBufferSizes(mesh->subMeshes.size());
	for(i=0;i<mesh->subMeshes.size();++i){
		Mesh::SubMesh::ptr subMesh=mesh->subMeshes[i];
		IndexBufferAccessor iba(subMesh->indexData->getIndexBuffer());
		newIndexBufferSizes[i]=iba.getSize();

		for(j=0;j<iba.getSize();j+=3){
			if(iba.get(j)==iba.get(j+1) || iba.get(j+1)==iba.get(j+2) || iba.get(j)==iba.get(j+2)){
				int k;
				for(k=j+3;k<iba.getSize();++k){
					iba.set(k-3,iba.get(k));
				}

				degenerateTris++;
				newIndexBufferSizes[i]-=3;
				j-=3;
			}
		}
	}

	// Remove empty vertexes
	Collection<uint8> vertUsed;
	vertUsed.resize(vertexBuffer->getSize(),0);

	for(i=0;i<mesh->subMeshes.size();++i){
		Mesh::SubMesh::ptr subMesh=mesh->subMeshes[i];

		IndexBufferAccessor iba(subMesh->indexData->getIndexBuffer());
		for(j=0;j<iba.getSize();++j){
			vertUsed[iba.get(j)]=true;
		}
	}

	int unusedCount=0;
	for(i=0;i<oldToNew.size();++i){
		if(vertUsed[i]==false){
			unusedCount++;
		}
	}

	VertexBuffer::ptr newVertexBuffer=engine->getBufferManager()->cloneVertexBuffer(vertexBuffer,vertexBuffer->getUsageFlags(),vertexBuffer->getAccessType(),vertexBuffer->getVertexFormat(),vertexBuffer->getSize()-unusedCount);
	Collection<Mesh::VertexBoneAssignmentList> nvertexBoneAssignments;
	if(mesh->vertexBoneAssignments.size()>0){
		nvertexBoneAssignments.resize(mesh->vertexBoneAssignments.size()-unusedCount);
	}

	unusedCount=0;
	{
		VertexBufferAccessor vba(vertexBuffer);
		VertexBufferAccessor nvba(newVertexBuffer);
		for(i=0;i<oldToNew.size();++i){
			if(vertUsed[i]==false){
				unusedCount++;
			}
			else{
				oldToNew[i]=i-unusedCount;

				if(positionIndex>=0){
					Vector3 position;
					vba.get3(i,positionIndex,position);
					nvba.set3(i-unusedCount,positionIndex,position);
				}

				if(normalIndex>=0){
					Vector3 normal;
					vba.get3(i,normalIndex,normal);
					nvba.set3(i-unusedCount,normalIndex,normal);
				}

				if(colorIndex>=0){
					nvba.setRGBA(i-unusedCount,colorIndex,vba.getRGBA(i,colorIndex));
				}

				if(texCoordIndex>=0){
					Vector2 texCoord;
					vba.get2(i,texCoordIndex,texCoord);
					nvba.set2(i-unusedCount,texCoordIndex,texCoord);
				}

				if(mesh->vertexBoneAssignments.size()>0){
					nvertexBoneAssignments[i-unusedCount]=mesh->vertexBoneAssignments[i];
				}
			}
		}
	}

	mesh->staticVertexData=VertexData::ptr(new VertexData(newVertexBuffer));
	mesh->vertexBoneAssignments=nvertexBoneAssignments;

	for(i=0;i<mesh->subMeshes.size();++i){
		Mesh::SubMesh::ptr subMesh=mesh->subMeshes[i];
		IndexBuffer::ptr indexBuffer=subMesh->indexData->getIndexBuffer();
		IndexBuffer::ptr newIndexBuffer=engine->getBufferManager()->createIndexBuffer(indexBuffer->getUsageFlags(),indexBuffer->getAccessType(),indexBuffer->getIndexFormat(),newIndexBufferSizes[i]);

		IndexBufferAccessor iba(indexBuffer);
		IndexBufferAccessor niba(newIndexBuffer);
		for(j=0;j<newIndexBufferSizes[i];++j){
			niba.set(j,oldToNew[iba.get(j)]);
		}

		subMesh->indexData=IndexData::ptr(new IndexData(subMesh->indexData->getPrimitive(),newIndexBuffer,0,newIndexBuffer->getSize()));
	}

	// Remove empty tracks
	Skeleton::ptr skeleton=mesh->skeleton;
	if(skeleton!=NULL){
		for(i=0;i<skeleton->sequences.size();++i){
			Sequence::ptr sequence=skeleton->sequences[i];
			for(j=0;j<sequence->tracks.size();++j){
				if(sequence->tracks[j]->keyFrames.size()==0){
					sequence->tracks.removeAt(j);
					j--;
				}
			}
			sequence->compile();
		}
	}

	std::cout << "Combined " << combinedSubmeshes << " sub meshes" << std::endl;
	std::cout << "Combined " << combinedVertices << " vertices" << std::endl;
	std::cout << "Removed " << degenerateTris << " degenerate triangles" << std::endl;

	return true;
}
