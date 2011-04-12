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

#include "BACConverter.h"
#include <iostream>
#include <sstream>
#include <string.h>

const int EXPORT_FPS=30;

Vector3 BACTriangle::normal(){
	Vector3 v0=vertex[1]->position-vertex[0]->position;
	Vector3 v1=vertex[2]->position-vertex[0]->position;
	Vector3 n;
	Math::cross(n,v0,v1);

	if(Math::lengthSquared(n)!=0){
		Math::normalize(n);
	}

	return n;
}

float BACQuad::computeSP(){
	int i,in,ip;
	Vector3 vn,vp;
	float sp=0;
	
	for(i=0;i<4;++i){
		in=i+1;
		ip=i-1;

		if(i==0) ip=3;
		if(i==3) in=0;

		vn=vertex[in]->position-vertex[i]->position;
		Math::normalize(vn);

		vp=vertex[ip]->position-vertex[i]->position;
		Math::normalize(vp);

		sp+=Math::abs(Math::dot(vn,vp));
	}

	return sp;
}

BACConverter::BACConverter(Engine *engine){
	mEngine=engine;
	mPositionEpsilon=0.1f;
	mNormalEpsilon=0.05f;
	mTexCoordEpsilon=0.005f;
	mScaleEpsilon=0.01f;
	mRotationEpsilon=0.01f;
	mTotalFrame=0;
}

BACConverter::~BACConverter(){
	clean();
}

bool BACConverter::convertMesh(Mesh::ptr mesh,Stream *out,bool submeshes,bool quads,float adjust,int version){
	bool result=true;

	result=extractMeshData(mesh,submeshes);
	if(result==false){
		return false;
	}

	if(quads){
		constructQuads();
	}

	rewindTriangles();

	if(adjust!=0.0f){
		adjustVertexes(adjust);
	}

	if(version==6){
		writeOutModelVersion6(out);
	}
	else{
		writeOutModelVersion5(out);
	}

	clean();

	return result;
}

bool BACConverter::extractMeshData(Mesh::ptr mesh,bool useSubmeshes){
	int i,j;
	Collection<BACVertex *> vertexList;
	Collection<BACVertex *>::iterator vit1,vit2,vitmp;
	Collection<BACTexCoord *> texCoordList;
	Collection<BACTexCoord *>::iterator tit1,tit2,titmp;
	Collection<int> vertexMap;
	Collection<int> texCoordMap;

	mName=mesh->getName();
	int loc=mName.rfind('.');
	if(loc!=String::npos){
		mName=mName.substr(0,loc);
	}

	VertexBuffer *vertexBuffer=mesh->staticVertexData->getVertexBuffer(0);
	VertexFormat *vertexFormat=vertexBuffer->getVertexFormat();
	mHasNormal=vertexFormat->findSemantic(VertexFormat::Semantic_NORMAL)>=0;
	mHasBone=mesh->vertexBoneAssignments.size()>0;

	// Collect vertices and texture coordinates
	int positionIndex=vertexFormat->findSemantic(VertexFormat::Semantic_POSITION);
	int normalIndex=vertexFormat->findSemantic(VertexFormat::Semantic_NORMAL);
	int texCoordIndex=vertexFormat->findSemantic(VertexFormat::Semantic_TEX_COORD);
	VertexBufferAccessor vba(vertexBuffer);
	for(i=0;i<vba.getSize();++i){
		BACVertex *v=new BACVertex();
		if(positionIndex>=0){
			vba.get3(i,positionIndex,v->position);
		}
		if(normalIndex>=0){
			vba.get3(i,normalIndex,v->normal);
		}
		// FYI: The .bac format does not support vertex colors
		if(mHasBone){
			Mesh::VertexBoneAssignmentList &boneAssignments=mesh->vertexBoneAssignments[i];
			if(boneAssignments.size()>0){
				v->bone=boneAssignments[0].bone;
			}
			else{
				v->bone=0;
			}
		}
		else{
			v->bone=0;
		}

		v->id=i;
		vertexList.add(v);

		if(texCoordIndex>=0){
			BACTexCoord *t=new BACTexCoord();
			vba.get2(i,texCoordIndex,t->texture);
			t->id=i;
			texCoordList.add(t);
		}
	}
	vba.unlock();

	Logger::alert(String("Initial numbers: ") + vertexList.size() + " vertices and " + texCoordList.size() + " texture coords");

	// Form mVertices
	vertexMap.resize(vertexList.size());
	i=0;
	for(vit1=vertexList.begin();vit1!=vertexList.end();++vit1){
		BACVertex *v1=(*vit1);

		// Store the mapping of vertexBuffer to mVertices
		vertexMap[v1->id]=i;
		vit2=vit1;
		vitmp=vit2;
		++vit2;

		// Reassign id number and add to mVertices
		v1->id=i;
		mVertices.add(v1);

		// Eliminate duplicate vertices
		while(vit2!=vertexList.end()){
			BACVertex *v2=(*vit2);
			if(	fabs(v1->position[0]-v2->position[0])<mPositionEpsilon &&
				fabs(v1->position[1]-v2->position[1])<mPositionEpsilon &&
				fabs(v1->position[2]-v2->position[2])<mPositionEpsilon &&
				Math::dot(v1->normal,v2->normal)>=1.0f-mNormalEpsilon &&
				v1->bone==v2->bone){
				// Store the mapping of vertexData id to mVertices id
				vertexMap[v2->id]=i;

				// Wipe out the duplicate vertex
				vertexList.erase(vit2);
				vit2=vitmp;
				delete v2;
			}
			vitmp=vit2;
			++vit2;
		}
		++i;
	}

	Logger::alert(String("Reduced mVertices: ") + mVertices.size() + " vertices");

	// Form mMaterials
	texCoordMap.resize(texCoordList.size());
	i=0;
	for(tit1=texCoordList.begin();tit1!=texCoordList.end();++tit1){
		BACTexCoord *t1=(*tit1);

		// Store the mapping of vertexBuffer to mMaterials
		texCoordMap[t1->id]=i;
		tit2=tit1;
		titmp=tit2;
		++tit2;

		// Reassign id number and add to mMaterials
		t1->id=i;
		mTexCoords.add(t1);
		
		// Eliminate duplicate texture coords
		while(tit2!=texCoordList.end()){
			BACTexCoord *t2=(*tit2);
			if(fabs(t1->texture[0]-t2->texture[0])<mTexCoordEpsilon && fabs(t1->texture[1]-t2->texture[1])<mTexCoordEpsilon){
				// Store the mapping of textureData id to mTexCoords id
				texCoordMap[t2->id]=i;

				// Whipe out the duplicate texture coord
				texCoordList.erase(tit2);
				tit2=titmp;
				delete t2;
			}
			titmp=tit2;
			++tit2;
		}
		++i;
	}

	// HACK: For wacky .bac and/or bitmap reasons, we have to use 1-ycoord of the texture coordinates.
	for(i=0;i<mTexCoords.size();++i){
		mTexCoords[i]->texture[1]=1.0-mTexCoords[i]->texture[1];
	}

	Logger::alert(String("Reduced mTexCoords: ") + mTexCoords.size() + " texture coords");

	// Iterate through submeshes
	int k,l,m;
	for(i=0;i<mesh->subMeshes.size();++i){
		Mesh::SubMesh *subMesh=mesh->subMeshes[i];

		// Extract triangles
		IndexBuffer *indexBuffer=subMesh->indexData->getIndexBuffer();
		IndexBufferAccessor iba(indexBuffer);
		for(j=0;j<iba.getSize();j+=3){
			k=iba.get(j);
			l=iba.get(j+1);
			m=iba.get(j+2);

			// Ignore degenerate triangles
			if(vertexMap[k]==vertexMap[l] || vertexMap[l]==vertexMap[m] || vertexMap[m]==vertexMap[k]){
				continue;
			}

			BACTriangle *tri=new BACTriangle();
			
			// Map from vertexBuffer to vertices
			tri->vertex[0]=mVertices[vertexMap[k]];
			tri->vertex[1]=mVertices[vertexMap[l]];
			tri->vertex[2]=mVertices[vertexMap[m]];

			// Map from vertexBuffer to textures
			if(texCoordMap.size()>k && texCoordMap.size()>l && texCoordMap.size()>m){
				tri->texture[0]=mTexCoords[texCoordMap[k]];
				tri->texture[1]=mTexCoords[texCoordMap[l]];
				tri->texture[2]=mTexCoords[texCoordMap[m]];
				tri->textured=true;
			}
			else{
				tri->textured=false;
			}
			
			if(useSubmeshes==true){
				tri->subMeshId=i;
			}
			else{
				tri->subMeshId=0;
			}
			tri->dead=false;
			
			mTriangles.add(tri);
		}
		iba.unlock();

		// Extract materials
		for(j=0;j<1/*sub->getNumMaterials()*/;++j){
			BACMaterial *m=new BACMaterial();

			Material *material=subMesh->material;
			if(material!=NULL){
				if(material->getName().length()==0){
					m->name="unknown";
				}
				else{
					m->name=material->getName();
				}

				RasterizerState rasterizerState;
				if(material->getRasterizerState(rasterizerState)){
					m->doubleSided=(rasterizerState.cull==RasterizerState::CullType_NONE);
				}

				MaterialState materialState;
				if(material->getMaterialState(materialState)){
					m->lighting=materialState.lighting;
				}

				Texture *texture=material->getTexture();
				String name=material->getTextureName();
				if(texture!=NULL || name.length()>0){
					int width=256,height=256;
					if(texture!=NULL){
						width=texture->getWidth();
						height=texture->getHeight();
					}
					else{
						Logger::alert(String("Texture ")+name+" not found, a texture must be available so msh2bac can determine the texture size.  Will default to 256x256");
					}
					mTextures.add(Vector2(width,height));
					m->textureIndex=mTextures.size()-1;
					m->colorIndex=-1;
				}
				else{
					mColors.add(materialState.diffuse);
					m->textureIndex=-1;
					m->colorIndex=mColors.size()-1;
				}
			}
			else{
				mColors.add(Colors::WHITE);
				m->doubleSided=true;
				m->lighting=true;
				m->colorIndex=mColors.size()-1;
				m->textureIndex=-1;
			}

			mMaterials.add(m);
		}
	}

	Logger::alert(String("Number of mTriangles: ")+mTriangles.size()+" triangles");

	if(mesh->skeleton==NULL){
		BACBone *bone=new BACBone();
		bone->name=mName;
		bone->translate=Vector3(0,0,0);
		bone->rotate=Vector3(0,0,360.0f);
		bone->handle=Vector3(0,0,0);
		bone->hasChild=false;
		bone->hasBrother=false;

		for(i=0;i<mVertices.size();++i){
			bone->vertexIndexes.add(i);
		}
		mBones.add(bone);
	}
	else{
		MeshNodeSkeleton::ptr skeleton(new MeshNodeSkeleton(NULL,mesh->skeleton));
		buildBones(mesh,skeleton,0);
	}

	return true;
}

void BACConverter::buildBones(Mesh *mesh,MeshNodeSkeleton *nodeSkeleton,int bone){
	Skeleton *skeleton=mesh->skeleton;
	Skeleton::Bone *meshBone=skeleton->bones[bone];
	int i;

	int nextChild=-1;
	int nextBrother=-1;

	BACBone *bacBone=new BACBone();

	if(meshBone->name.length()==0){
		std::stringstream ss;
		ss << "bone:" << bone;
		bacBone->name=ss.str().c_str();
	}
	else{
		bacBone->name=meshBone->name;
	}

	Vector3 worldTranslate=nodeSkeleton->getBone(bone)->worldTranslate;
	Matrix3x3 worldRotateMatrix;
	Math::setMatrix3x3FromQuaternion(worldRotateMatrix,nodeSkeleton->getBone(bone)->worldRotate);
	Vector3 worldRotate=Vector3(worldRotateMatrix.at(0,2),
								worldRotateMatrix.at(1,2),
								worldRotateMatrix.at(2,2));
	Vector3 worldHandle=Vector3(worldRotateMatrix.at(0,1),
								worldRotateMatrix.at(1,1),
								worldRotateMatrix.at(2,1));

	bacBone->translate=worldTranslate;
	bacBone->rotate=worldTranslate+worldRotate;
	bacBone->handle=worldTranslate+worldHandle;

	for(i=0;i<skeleton->bones.size();++i) if(skeleton->bones[i]->parentIndex==bone) break;
	if(i!=skeleton->bones.size()){
		bacBone->hasChild=true;
		nextChild=i;
	}
	else{
		bacBone->hasChild=false;
	}

	if(meshBone->parentIndex!=-1){
		Skeleton::Bone *parentBone=skeleton->bones[meshBone->parentIndex];
		for(i=bone+1;i<skeleton->bones.size();++i) if(skeleton->bones[i]->parentIndex==meshBone->parentIndex) break;
		if(i<skeleton->bones.size()){
			bacBone->hasBrother=true;
			nextBrother=i;
		}
		else{
			bacBone->hasBrother=false;
		}
	}
	else{
		bacBone->hasBrother=false;
	}

	// I should go through all vertexes first, and make a list for each bone, since that would
	// be more efficient, but this is just a conversion utility and it won't really matter
	for(i=0;i<mVertices.size();++i){
		if(mVertices[i]->bone==bone){
			bacBone->vertexIndexes.add(i);
		}
	}

	mBones.add(bacBone);

	if(nextChild!=-1){
		buildBones(mesh,nodeSkeleton,nextChild);
	}
	if(nextBrother!=-1){
		buildBones(mesh,nodeSkeleton,nextBrother);
	}
}

void BACConverter::constructQuads(){
	int i,j,k,l,count;
	int id1[3],id2[3],tex1[3],tex2[3];
	int idMatch[2];
	BACTriangle *tri1,*tri2;
	Collection<BACSharedEdge> sharedEdges;
	Collection<BACQuad *> potentialQuads;

	// Compare each triangle against each other for quad formation
	for(i=0;i<mTriangles.size();++i){
		tri1=mTriangles[i];

		if(tri1->dead==true) continue;

		// Store the ID's of the 3 vertices making up this triangle
		id1[0]=tri1->vertex[0]->id;
		id1[1]=tri1->vertex[1]->id;
		id1[2]=tri1->vertex[2]->id;

		// Look for all triangles sharing a side with t1 and store the vertex indices of those vertices on the sharing side
		sharedEdges.clear();
		for(j=0;j<mTriangles.size();++j){
			tri2=mTriangles[j];

			if(i==j) continue;
			if(tri2->dead==true || tri1->subMeshId!=tri2->subMeshId) continue;

			id2[0]=tri2->vertex[0]->id;
			id2[1]=tri2->vertex[1]->id;
			id2[2]=tri2->vertex[2]->id;

			count=0;
			for(k=0;k<3;++k){
				for(l=0;l<3;++l){
					if(id1[k]==id2[l]){
						if(count<2){
							idMatch[count]=id2[l];
						}
						++count;
					}
				}
			}
			
			// If we share an edge, and if the normals of the 2 triangles are within epsilon, store it
			if(count==2 && Math::dot(tri1->normal(),tri2->normal())>=1.0f-mNormalEpsilon){
				BACSharedEdge e;
				e.tri=tri2;
				e.edge[0]=idMatch[0];
				e.edge[1]=idMatch[1];
				sharedEdges.add(e);
			}
		}

		// No candidates for quad conversion, so continue
		if(sharedEdges.size()==0) continue;

		// Now we create a quad for each candidate
		// and store them for potential comparison with one another
		// to determine the best choice
		for(j=0;j<sharedEdges.size();++j){
			int iidOff[2];
			int iidNext[2];
			int iidLast[2];

			BACQuad *q=new BACQuad();
			q->subMeshId=tri1->subMeshId;

			// Reassign tri2 and id2 for convience
			// NOTE: tri1 and id1 remain as before
			tri2=sharedEdges[j].tri;
			id2[0]=tri2->vertex[0]->id;
			id2[1]=tri2->vertex[1]->id;
			id2[2]=tri2->vertex[2]->id;
				
			// For both triangles, find the "off" vertex, the one not shared
			// We also need to store the order of the next 2 vertices around
			// the triangle so we know to progress around the resulting quad.
			// NOTE: here we are storing the LOCAL index id's of these vertices,
			// those used to index into id1 and id2. This is done so the same
			// index numbers work for texture coordinates. 
			int id;
			for(k=0;k<3;++k){
				id=tri1->vertex[k]->id;
				if(id!=sharedEdges[j].edge[0] && id!=sharedEdges[j].edge[1]){
					iidOff[0]=k;
					if(k==2){
						iidNext[0]=0;
						iidLast[0]=1;
					}
					else if(k==1){
						iidNext[0]=2;
						iidLast[0]=0;
					}
					else{
						iidNext[0]=1;
						iidLast[0]=2;
					}
					break;
				}
			}
			for(k=0;k<3;++k){
				id=tri2->vertex[k]->id;
				if(id!=sharedEdges[j].edge[0] && id!=sharedEdges[j].edge[1]){
					iidOff[1]=k;
					if(k==2){
						iidNext[1]=0;
						iidLast[1]=1;
					}
					else if(k==1){
						iidNext[1]=2;
						iidLast[1]=0;
					}
					else{
						iidNext[1]=1;
						iidLast[1]=2;
					}
					break;
				}
			}

			// The vertex ordering for the quad is based on the assumption that the vertex 
			// direction for quads needs to be the same as that for triangles
			// We go in the following order:
			// 1. Off shared edge vertex of tri1
			// 2. Next vertex of tri1 = a shared vertex with tri2
			// 3. Off hypotenuse vertex of tri2
			// 4. Next vertex of tri2 = other shared vertex with tri1
			// HACK: Guess what? This is incorrect, and I don't know why - mascot must suck
			q->vertex[0]=mVertices[id1[iidOff[0]]];
			q->vertex[3]=mVertices[id1[iidNext[0]]];
			q->vertex[2]=mVertices[id2[iidOff[1]]];
			q->vertex[1]=mVertices[id2[iidNext[1]]];

			// Order the texture coordinates based on the same scheme, if necessary
			if(tri1->textured==true && tri2->textured==true){
				tex1[0]=tri1->texture[0]->id;
				tex1[1]=tri1->texture[1]->id;
				tex1[2]=tri1->texture[2]->id;

				tex2[0]=tri2->texture[0]->id;
				tex2[1]=tri2->texture[1]->id;
				tex2[2]=tri2->texture[2]->id;

				// Double check to make sure we share texture coordinates - if not then this quad cannot be considered
				if(mTexCoords[tex1[iidNext[0]]]!=mTexCoords[tex2[iidLast[1]]] || mTexCoords[tex1[iidLast[0]]]!=mTexCoords[tex2[iidNext[1]]]){
					delete q;
					sharedEdges[j].quad=NULL;
					continue;
				}

				// HACK: the texture coordinates also appear to follow this "left hand" convention
				q->texture[0]=mTexCoords[tex1[iidOff[0]]];
				q->texture[3]=mTexCoords[tex1[iidNext[0]]];
				q->texture[2]=mTexCoords[tex2[iidOff[1]]];
				q->texture[1]=mTexCoords[tex2[iidNext[1]]];
				q->textured=true;
			}
			else{
				q->textured=false;
			}

			// Now store this new quad candidate
			sharedEdges[j].quad=q;
		}

		// Last we select the best for quad 
		// based on a measure of it's "squareness"
		// This "squareness" is computed by summing the differences in each of the
		// quad's corners from an ideal square's 90 deg. Call this the "inverse squareness potential" - msp
		// That quad with the minimum "inverse squareness potential" is the one to be combined.
		float sp;
		float msp=4; // This could never be > 4 - And even that would require a totally unphysical quad
		int selectQuad=-1;
		for(j=0;j<sharedEdges.size();++j){
			if(sharedEdges[j].quad!=NULL){
				sp=sharedEdges[j].quad->computeSP();
				if(sp<msp){
					selectQuad=j;
					msp=sp;
				}
			}
		}

		// Now add this quad, delete the others in sharedEdges and schedule tri1 and the shared edge tri for deletion
		if(selectQuad!=-1){
			mQuads.add(sharedEdges[selectQuad].quad);
			tri1->dead=true;
			sharedEdges[selectQuad].tri->dead=true;

			for(j=0;j<sharedEdges.size();++j){
				if(sharedEdges[j].quad!=NULL && selectQuad!=j){
					delete sharedEdges[j].quad;
				}
			}
		}
		sharedEdges.clear();
	}

	// Clean out dead triangles
	for(i=mTriangles.size()-1;i>=0;--i){
		tri1=mTriangles[i];
		if(tri1->dead==true){
			delete tri1;
			mTriangles.erase(mTriangles.begin()+i);
		}
	}

	Logger::alert(String("Reduced number of mTriangles after quad formation: ")+mTriangles.size()+" triangles");
	Logger::alert(String("Number of mQuads: ")+mQuads.size()+" quads");
}

void BACConverter::rewindTriangles(){
	int i;
	BACTriangle *tri1;
	int id1[3],tex1[3];

	// Rewind remaining triangle vertices and textures
	for(i=0;i<mTriangles.size();++i){
		tri1=mTriangles[i];

		id1[0]=tri1->vertex[0]->id;
		id1[1]=tri1->vertex[1]->id;
		id1[2]=tri1->vertex[2]->id;

		if(tri1->textured==true){
			tex1[0]=tri1->texture[0]->id;
			tex1[1]=tri1->texture[1]->id;
			tex1[2]=tri1->texture[2]->id;
		}

		tri1->vertex[1]=mVertices[id1[2]];
		tri1->vertex[2]=mVertices[id1[1]];

		if(tri1->textured==true){
			tri1->texture[1]=mTexCoords[tex1[2]];
			tri1->texture[2]=mTexCoords[tex1[1]];
		}
	}
}

void BACConverter::adjustVertexes(float amount){
	int i;
	for(i=0;i<mTriangles.size();++i){
		BACTriangle *tri1=mTriangles[i];

		Vector3 center;
		center+=tri1->vertex[0]->position;
		center+=tri1->vertex[1]->position;
		center+=tri1->vertex[2]->position;
		center/=3.0f;

		Vector3 dir;

		dir=tri1->vertex[0]->position-center;
		Math::normalize(dir);
		dir*=amount;
		tri1->vertex[0]->position+=dir;

		dir=tri1->vertex[1]->position-center;
		Math::normalize(dir);
		dir*=amount;
		tri1->vertex[1]->position+=dir;

		dir=tri1->vertex[2]->position-center;
		Math::normalize(dir);
		dir*=amount;
		tri1->vertex[2]->position+=dir;
	}
	for(i=0;i<mQuads.size();++i){
		BACQuad *quad1=mQuads[i];

		Vector3 center;
		center+=quad1->vertex[0]->position;
		center+=quad1->vertex[1]->position;
		center+=quad1->vertex[2]->position;
		center+=quad1->vertex[3]->position;
		center/=4.0f;

		Vector3 dir;

		dir=quad1->vertex[0]->position-center;
		Math::normalize(dir);
		dir*=amount;
		quad1->vertex[0]->position+=dir;

		dir=quad1->vertex[1]->position-center;
		Math::normalize(dir);
		dir*=amount;
		quad1->vertex[1]->position+=dir;

		dir=quad1->vertex[2]->position-center;
		Math::normalize(dir);
		dir*=amount;
		quad1->vertex[2]->position+=dir;

		dir=quad1->vertex[3]->position-center;
		Math::normalize(dir);
		dir*=amount;
		quad1->vertex[3]->position+=dir;
	}
}

void BACConverter::writeOutModelVersion6(Stream *tout){
	int i,j;

	std::stringstream out;
	
	out << ";BAC utf8" << std::endl;
 	out << std::endl;
	out << "( Head" << std::endl;
	out << "  ( bacVersion 6.0 )" << std::endl;
	out << ")" << std::endl;
 	out << std::endl;
	out << ";(C) Lightning Toads Productions, LLC" << std::endl;
 	out << std::endl;

	out << "( Figure" << std::endl;
	out << "  ( name \"" << mName.c_str() << "\" )" << std::endl; 

 	out << std::endl;

	if(mTextures.size()>0){
		out << "  ( Textures" << std::endl;
		for(i=0;i<mTextures.size();++i){
			const Vector2 &texture=mTextures[i];
			out << "    ( i2 " << (int)texture.x << " " << (int)texture.y << " ) ; id: " << i << std::endl;
		}
		out << "  )" << std::endl;

		out << std::endl;
	}

	if(mColors.size()>0){
		out << "  ( Colors" << std::endl;
		for(i=0;i<mColors.size();++i){
			const Vector4 &color=mColors[i];
			out << "    ( f3 " << color.x << " " << color.y << " " << color.z << " ) ; id: " << i << std::endl;
		}
		out << "  )" << std::endl;

	 	out << std::endl;
	}

	out << "  ( Materials" << std::endl;
	for(i=0;i<mMaterials.size();++i){
		BACMaterial *material=mMaterials[i];

		out << "    ( material" << std::endl;
		out << "      ( name \"" << material->name.c_str() << "\" ) ; id: " << i << std::endl;
		out << "      ( blendMode normal )" << std::endl;
		if(material->doubleSided){
			out << "      ( doubleFace true )" << std::endl;
		}
		else{
			out << "      ( doubleFace false )" << std::endl;
		}
		out << "      ( transparent false )" << std::endl;
		if(material->lighting){
			out << "      ( lighting true )" << std::endl;
		}
		else{
			out << "      ( lighting false )" << std::endl;
		}
		out << "      ( textureIndex " << material->textureIndex << " )" << std::endl;
		out << "      ( colorIndex " << material->colorIndex << " )" << std::endl;
		out << "      ( shininess 0.0000 )" << std::endl;
		out << "      ( specular 0.0000 )" << std::endl;
		out << "    ) ; material" << std::endl;
	}
	out << "  ) ; Materials" << std::endl;

	out << std::endl;

	out << "  ( Vertices" << std::endl;
	out << "    ( coords" << std::endl;
	for(i=0;i<mVertices.size();++i){
		const Vector3 &position=mVertices[i]->position;
		out << "      ( pnt " << position.x << " " << position.y << " " << position.z << " )" << std::endl;
	}
	out << "    ) ; coords" << std::endl;
	out << "    ( normals" << std::endl;
	for(i=0;i<mVertices.size();++i){
		const Vector3 &normal=mVertices[i]->normal;
		out << "      ( vct " << normal.x << " " << normal.y << " " << normal.z << " )" << std::endl;
	}
	out << "    ) ; normals" << std::endl;
	out << "  ) ; Vertices" << std::endl;

	out << std::endl;

	out << "  ( Bones" << std::endl;
	for(i=0;i<mBones.size();++i){
		BACBone *b=mBones[i];
		out << "    ( bone" << std::endl;
		out << "      ( name \"" << b->name.c_str() << "\" ) " << std::endl;
		if(b->hasChild){
			out << "      ( hasChild true )" << std::endl;
		}
		else{
			out << "      ( hasChild false )" << std::endl;
		}
		if(b->hasBrother){
			out << "      ( hasBrother true )" << std::endl;
		}
		else{
			out << "      ( hasBrother false )" << std::endl;
		}
		out << "      ( translate " << b->translate.x << " " << b->translate.y << " " << b->translate.z << " )" << std::endl;
		out << "      ( rotate " << b->rotate.x << " " << b->rotate.y << " " << b->rotate.z << " )" << std::endl;
		out << "      ( handle " << b->handle.x << " " << b->handle.y << " " << b->handle.z << " )" << std::endl;
		if(b->vertexIndexes.size()>0){
			out << "      ( vertexIndices" << std::endl;
			out << "        ";
			for(j=0;j<b->vertexIndexes.size();++j){
				out << b->vertexIndexes[j] << " ";
			}
			out << std::endl;
			out << "      ) ; vertexIndices" << std::endl;
		}
		else{
			out << "      ; no vertexIndices" << std::endl;
		}
		out << "    ) ; bone" << std::endl;
	}
	out << "  ) ; Bones" << std::endl;

	out << std::endl;

	out << "  ( TextureCoords" << std::endl;
	if(mTexCoords.size()==0){
		// Dummy tex coord
		out << "    ( f2 0 0 )" << std::endl;
	}
	else{
		for(i=0;i<mTexCoords.size();++i){
			const Vector2 &texCoord=mTexCoords[i]->texture;
			out << "    ( f2 " << texCoord.x << " " << texCoord.y << " )" << std::endl;
		}
	}
	out << "  ) ; TextureCoords" << std::endl;

	out << std::endl;

	out << "  ( Polygons; " << mTriangles.size() << " triangles and " << mQuads.size() << " quads " << std::endl;
	for(i=0;i<mTriangles.size();++i){
		BACTriangle *t=mTriangles[i];
		out << "    ( face " << t->subMeshId;
		out << " ( i3 " << t->vertex[0]->id << " " << t->vertex[1]->id << " " << t->vertex[2]->id << " )";
		if(t->textured==true){
			out << " ( i3 " << t->texture[0]->id << " " << t->texture[1]->id << " " << t->texture[2]->id << " )";
		}
		else{
			out << " ( i3 0 0 0 )";
		}
		out << " )" << std::endl;
	}
	for(i=0;i<mQuads.size();++i){
		BACQuad *q=mQuads[i];
		out << "    ( face " << q->subMeshId;
		out << " ( i4 " << q->vertex[0]->id << " " << q->vertex[1]->id << " " << q->vertex[2]->id << " " << q->vertex[3]->id << " )";
		if(q->textured==true){
			out << " ( i4 " << q->texture[0]->id << " " << q->texture[1]->id << " " << q->texture[2]->id << " " << q->texture[3]->id << " )";
		}
		else{
			out << " ( i4 0 0 0 0 )";
		}
		out << " )" << std::endl;
	}
	out << "  ) ; Polygons" << std::endl;
	out << ") ; Figure" << std::endl;

	std::string string=out.str();
	tout->write((tbyte*)string.c_str(),string.length());
}

void BACConverter::writeOutModelVersion5(Stream *tout){
	DataStream *out=new DataStream(tout);

	uint32 version=5;
	out->writeLUInt32(version);

	uint32 bacAttr=0;
	if(mHasNormal){
		bacAttr|=BAC_ATTR_NORMAL;
	}
	if(mHasBone){
		bacAttr|=BAC_ATTR_VID;
	}
	out->writeLInt32(bacAttr);

	tbyte dummy[8];
	memset(dummy,0,sizeof(dummy));
	out->write(dummy,sizeof(dummy));

	out->writeLUInt32(mVertices.size());

	int i;
	for(i=0;i<mVertices.size();++i){
		BACVertex *v=mVertices[i];

		if(mHasBone){
			out->writeLUInt32(v->id);
		}

		out->writeLFloat(v->position.x);
		out->writeLFloat(v->position.y);
		out->writeLFloat(v->position.z);

		if(mHasNormal){
			out->writeLFloat(v->normal.x);
			out->writeLFloat(v->normal.y);
			out->writeLFloat(v->normal.z);
		}
	}

	out->writeLUInt16(mBones.size());

	for(i=0;i<mBones.size();++i){
		BACBone *b=mBones[i];
		if(b->hasChild){
			out->writeUInt8(1);
		}
		else{
			out->writeUInt8(0);
		}

		if(b->hasBrother){
			out->writeUInt8(1);
		}
		else{
			out->writeUInt8(0);
		}

		out->write((tbyte*)b->name.c_str(),b->name.length()+1);

		out->writeLFloat(b->translate.x);
		out->writeLFloat(b->translate.y);
		out->writeLFloat(b->translate.z);

		out->writeLFloat(b->rotate.x);
		out->writeLFloat(b->rotate.y);
		out->writeLFloat(b->rotate.z);

		out->writeLFloat(b->handle.x);
		out->writeLFloat(b->handle.y);
		out->writeLFloat(b->handle.z);

		out->writeLUInt16(b->vertexIndexes.size());

		int j;
		for(j=0;j<b->vertexIndexes.size();++j){
			out->writeLUInt16(b->vertexIndexes[j]);
		}
	}

	out->writeLUInt16(mTriangles.size() + mQuads.size());

	for(i=0;i<mTriangles.size();++i){
		BACTriangle *t=mTriangles[i];

		Vector2 texture;
		if(t->textured){
			int textureIndex=mMaterials[t->subMeshId]->textureIndex;
			if(textureIndex>=0){
				texture=mTextures[textureIndex];
			}
		}

		uint32 polyAttr=0;
		if(mMaterials[t->subMeshId]->doubleSided){
			polyAttr=POLY_ATTR_2S;
		}
		if(mMaterials[t->subMeshId]->lighting){
			polyAttr|=POLY_ATTR_LIGHT;
		}
		polyAttr=~polyAttr;
		out->writeLUInt32(polyAttr);

		out->writeLUInt16(t->vertex[0]->id);
		if(t->textured){
			out->writeLUInt16((uint16)t->texture[0]->texture.x * texture.x);
			out->writeLUInt16((uint16)t->texture[0]->texture.y * texture.y);
		}
		else{
			out->writeLUInt16(0);
			out->writeLUInt16(0);
		}

		out->writeLUInt16(t->vertex[1]->id);
		if(t->textured){
			out->writeLUInt16((uint16)t->texture[1]->texture.x * texture.x);
			out->writeLUInt16((uint16)t->texture[1]->texture.y * texture.y);
		}
		else{
			out->writeLUInt16(0);
			out->writeLUInt16(0);
		}

		out->writeLUInt16(t->vertex[2]->id);
		if(t->textured){
			out->writeLUInt16((uint16)t->texture[2]->texture.x * texture.x);
			out->writeLUInt16((uint16)t->texture[2]->texture.y * texture.y);
		}
		else{
			out->writeLUInt16(0);
			out->writeLUInt16(0);
		}

		// For triangles, we just repeat the previous coordinate
		out->writeLUInt16(t->vertex[2]->id);
		if(t->textured){
			out->writeLUInt16((uint16)t->texture[2]->texture.x * texture.x);
			out->writeLUInt16((uint16)t->texture[2]->texture.y * texture.y);
		}
		else{
			out->writeLUInt16(0);
			out->writeLUInt16(0);
		}
	}

	for(i=0;i<mQuads.size();++i){
		BACQuad *q=mQuads[i];

		Vector2 texture;
		if(q->textured){
			int textureIndex=mMaterials[q->subMeshId]->textureIndex;
			if(textureIndex>=0){
				texture=mTextures[textureIndex];
			}
		}

		uint32 polyAttr=0;
		if(mMaterials[q->subMeshId]->doubleSided){
			polyAttr|=POLY_ATTR_2S;
		}
		if(mMaterials[q->subMeshId]->lighting){
			polyAttr|=POLY_ATTR_LIGHT;
		}
		polyAttr=~polyAttr;
		out->writeLUInt32(polyAttr);

		out->writeLUInt16(q->vertex[0]->id);
		if(q->textured){
			out->writeLUInt16((uint16)q->texture[0]->texture.x * texture.x);
			out->writeLUInt16((uint16)q->texture[0]->texture.y * texture.y);
		}
		else{
			out->writeLUInt16(0);
			out->writeLUInt16(0);
		}

		out->writeLUInt16(q->vertex[1]->id);
		if(q->textured){
			out->writeLUInt16((uint16)q->texture[1]->texture.x * texture.x);
			out->writeLUInt16((uint16)q->texture[1]->texture.y * texture.y);
		}
		else{
			out->writeLUInt16(0);
			out->writeLUInt16(0);
		}

		out->writeLUInt16(q->vertex[2]->id);
		if(q->textured){
			out->writeLUInt16((uint16)q->texture[2]->texture.x * texture.x);
			out->writeLUInt16((uint16)q->texture[2]->texture.y * texture.y);
		}
		else{
			out->writeLUInt16(0);
			out->writeLUInt16(0);
		}

		out->writeLUInt16(q->vertex[3]->id);
		if(q->textured){
			out->writeLUInt16((uint16)q->texture[3]->texture.x * texture.x);
			out->writeLUInt16((uint16)q->texture[3]->texture.y * texture.y);
		}
		else{
			out->writeLUInt16(0);
			out->writeLUInt16(0);
		}
	}

	delete out;
}

bool BACConverter::convertAnimation(Mesh::ptr mesh,TransformSequence *animation,Stream *out,int version){
	extractAnimationData(mesh,animation);

	if(version==4){
		writeOutAnimationVersion4(out);
	}
	else{
		writeOutAnimationVersion3(out);
	}

	clean();

	return true;
}

void BACConverter::extractAnimationData(Mesh *mesh,TransformSequence *animation){
	int i,j;

	mTotalFrame=(animation->length*EXPORT_FPS)+1; // Must add 1 so we are above the max keyFrame.time*EXPORT_FPS

	for(i=0;i<animation->tracks.size();++i){
		TransformTrack *track=animation->tracks[i];

		int boneIndex=track->index;
		Skeleton::Bone *meshbone=mesh->skeleton->bones[boneIndex];

		if(track->keyFrames.size()>0){
			BACAnimationBone *bacbone=new BACAnimationBone();
			if(meshbone->name.length()==0){
				std::stringstream ss;
				ss << "bone:" << boneIndex;
				bacbone->name=ss.str().c_str();
			}
			else{
				bacbone->name=meshbone->name;
			}

			// This only works with uniform keyframes, eg. from Milkshape
			// To fix this I would need to find the minimum keyframe time difference
			// And use that as '1' for the time values

			for(j=0;j<track->keyFrames.size();++j){
				const TransformKeyFrame &keyFrame=track->keyFrames[j];

				int frame=keyFrame.time*EXPORT_FPS;

				Matrix3x3 boneRotate; Math::setMatrix3x3FromQuaternion(boneRotate,meshbone->rotate);
				Matrix3x3 invBoneRotate; Math::invert(invBoneRotate,boneRotate);

				Vector3 translate=invBoneRotate*(keyFrame.translate-meshbone->translate);
				bacbone->translatex.add(Pair<int,float>(frame,translate.x));
				bacbone->translatey.add(Pair<int,float>(frame,translate.y));
				bacbone->translatez.add(Pair<int,float>(frame,translate.z));

				bacbone->scalex.add(Pair<int,float>(frame,(keyFrame.scale.x/meshbone->scale.x)*100.0f));
				bacbone->scaley.add(Pair<int,float>(frame,(keyFrame.scale.y/meshbone->scale.y)*100.0f));
				bacbone->scalez.add(Pair<int,float>(frame,(keyFrame.scale.z/meshbone->scale.z)*100.0f));

				Vector3 zAxis=Vector3(0,0,1);
				Matrix3x3 keyframeRotate; Math::setMatrix3x3FromQuaternion(keyframeRotate,keyFrame.rotate);
				Matrix3x3 matrix=invBoneRotate*keyframeRotate;
				zAxis=matrix*zAxis;
				Math::normalize(zAxis);

				// If we're using toadlet's standard EulerAngles, then roll is matrix->euler.y, used to be matrix->euler.z
				//  But to avoid breakage if we rework the EulerAngle format, we just manually calculate the roll from the matrix
				float roll=Math::atan2(-matrix.at(0,1),matrix.at(0,0));

				bacbone->rotatex.add(Pair<int,float>(frame,zAxis.x*100.0f));
				bacbone->rotatey.add(Pair<int,float>(frame,zAxis.y*100.0f));
				bacbone->rotatez.add(Pair<int,float>(frame,zAxis.z*100.0f));
				bacbone->roll.add(Pair<int,float>(frame,Math::radToDeg(roll)));
			}

			// Clean out redundant information
			for(j=1;j<((int)bacbone->translatex.size())-1;++j){
				if(	fabs(bacbone->translatex[j-1].second-bacbone->translatex[j].second)<mPositionEpsilon &&
					fabs(bacbone->translatex[j].second-bacbone->translatex[j+1].second)<mPositionEpsilon){
					bacbone->translatex.erase(bacbone->translatex.begin()+j);
					j--;
				}
			}

			for(j=1;j<((int)bacbone->translatey.size())-1;++j){
				if(	fabs(bacbone->translatey[j-1].second-bacbone->translatey[j].second)<mPositionEpsilon &&
					fabs(bacbone->translatey[j].second-bacbone->translatey[j+1].second)<mPositionEpsilon){
					bacbone->translatey.erase(bacbone->translatey.begin()+j);
					j--;
				}
			}

			for(j=1;j<((int)bacbone->translatez.size())-1;++j){
				if(	fabs(bacbone->translatez[j-1].second-bacbone->translatez[j].second)<mPositionEpsilon &&
					fabs(bacbone->translatez[j].second-bacbone->translatez[j+1].second)<mPositionEpsilon){
					bacbone->translatez.erase(bacbone->translatez.begin()+j);
					j--;
				}
			}

			for(j=1;j<((int)bacbone->scalex.size())-1;++j){
				if(	fabs(bacbone->scalex[j-1].second-bacbone->scalex[j].second)<mScaleEpsilon &&
					fabs(bacbone->scalex[j].second-bacbone->scalex[j+1].second)<mScaleEpsilon){
					bacbone->scalex.erase(bacbone->scalex.begin()+j);
					j--;
				}
			}

			for(j=1;j<((int)bacbone->scaley.size())-1;++j){
				if(	fabs(bacbone->scaley[j-1].second-bacbone->scaley[j].second)<mScaleEpsilon &&
					fabs(bacbone->scaley[j].second-bacbone->scaley[j+1].second)<mScaleEpsilon){
					bacbone->scaley.erase(bacbone->scaley.begin()+j);
					j--;
				}
			}

			for(j=1;j<((int)bacbone->scalez.size())-1;++j){
				if(	fabs(bacbone->scalez[j-1].second-bacbone->scalez[j].second)<mScaleEpsilon &&
					fabs(bacbone->scalez[j].second-bacbone->scalez[j+1].second)<mScaleEpsilon){
					bacbone->scalez.erase(bacbone->scalez.begin()+j);
					j--;
				}
			}

			for(j=1;j<((int)bacbone->rotatex.size())-1;++j){
				if(	fabs(bacbone->rotatex[j-1].second-bacbone->rotatex[j].second)<mRotationEpsilon &&
					fabs(bacbone->rotatex[j].second-bacbone->rotatex[j+1].second)<mRotationEpsilon){
					bacbone->rotatex.erase(bacbone->rotatex.begin()+j);
					j--;
				}
			}

			for(j=1;j<((int)bacbone->rotatey.size())-1;++j){
				if(	fabs(bacbone->rotatey[j-1].second-bacbone->rotatey[j].second)<mRotationEpsilon &&
					fabs(bacbone->rotatey[j].second-bacbone->rotatey[j+1].second)<mRotationEpsilon){
					bacbone->rotatey.erase(bacbone->rotatey.begin()+j);
					j--;
				}
			}

			for(j=1;j<((int)bacbone->rotatez.size())-1;++j){
				if(	fabs(bacbone->rotatez[j-1].second-bacbone->rotatez[j].second)<mRotationEpsilon &&
					fabs(bacbone->rotatez[j].second-bacbone->rotatez[j+1].second)<mRotationEpsilon){
					bacbone->rotatez.erase(bacbone->rotatez.begin()+j);
					j--;
				}
			}

			for(j=1;j<((int)bacbone->roll.size())-1;++j){
				if(	fabs(bacbone->roll[j-1].second-bacbone->roll[j].second)<mRotationEpsilon &&
					fabs(bacbone->roll[j].second-bacbone->roll[j+1].second)<mRotationEpsilon){
					bacbone->roll.erase(bacbone->roll.begin()+j);
					j--;
				}
			}

			mAnimationBones.add(bacbone);
		}
	}
}

void BACConverter::writeOutAnimationVersion4(Stream *tout){
	int i,j;

	std::stringstream out;
	
	out << ";TRA utf8" << std::endl;
 	out << std::endl;
	out << "( Head" << std::endl;
	out << "  ( traVersion 4.0 )" << std::endl;
	out << ")" << std::endl;
 	out << std::endl;
	out << ";(C) Lightning Toads Productions, LLC" << std::endl;
 	out << std::endl;

	out << "( Figure" << std::endl;
	out << "  ( totalFrame " << mTotalFrame << " )" << std::endl;
	out << std::endl;

	for(i=0;i<mAnimationBones.size();++i){
		BACAnimationBone *bone=mAnimationBones[i];

		out << "  ( bone" << std::endl;
		out << "    ( name \"" << bone->name.c_str() << "\" )" << std::endl;
		out << "    ( translate.x" << std::endl;
		for(j=0;j<bone->translatex.size();++j){
			out << "      ( kf " << bone->translatex[j].first << " " << bone->translatex[j].second << " )" << std::endl;
		}
		out << "    )" << std::endl;
		out << "    ( translate.y" << std::endl;
		for(j=0;j<bone->translatey.size();++j){
			out << "      ( kf " << bone->translatey[j].first << " " << bone->translatey[j].second << " )" << std::endl;
		}
		out << "    )" << std::endl;
		out << "    ( translate.z" << std::endl;
		for(j=0;j<bone->translatez.size();++j){
			out << "      ( kf " << bone->translatez[j].first << " " << bone->translatez[j].second << " )" << std::endl;
		}
		out << "    )" << std::endl;
		out << "    ( scale.x" << std::endl;
		for(j=0;j<bone->scalex.size();++j){
			out << "      ( kf " << bone->scalex[j].first << " " << bone->scalex[j].second << " )" << std::endl;
		}
		out << "    )" << std::endl;
		out << "    ( scale.y" << std::endl;
		for(j=0;j<bone->scaley.size();++j){
			out << "      ( kf " << bone->scaley[j].first << " " << bone->scaley[j].second << " )" << std::endl;
		}
		out << "    )" << std::endl;
		out << "    ( scale.z" << std::endl;
		for(j=0;j<bone->scalez.size();++j){
			out << "      ( kf " << bone->scalez[j].first << " " << bone->scalez[j].second << " )" << std::endl;
		}
		out << "    )" << std::endl;
		out << "    ( rotate.x" << std::endl;
		for(j=0;j<bone->rotatex.size();++j){
			out << "      ( kf " << bone->rotatex[j].first << " " << bone->rotatex[j].second << " )" << std::endl;
		}
		out << "    )" << std::endl;
		out << "    ( rotate.y" << std::endl;
		for(j=0;j<bone->rotatey.size();++j){
			out << "      ( kf " << bone->rotatey[j].first << " " << bone->rotatey[j].second << " )" << std::endl;
		}
		out << "    )" << std::endl;
		out << "    ( rotate.z" << std::endl;
		for(j=0;j<bone->rotatez.size();++j){
			out << "      ( kf " << bone->rotatez[j].first << " " << bone->rotatez[j].second << " )" << std::endl;
		}
		out << "    )" << std::endl;
		out << "    ( roll" << std::endl;
		for(j=0;j<bone->roll.size();++j){
			out << "      ( kf " << bone->roll[j].first << " " << bone->roll[j].second << " )" << std::endl;
		}
		out << "    )" << std::endl;
		out << "  )" << std::endl;
	}

	out << ") ; Figure" << std::endl;

	std::string string=out.str();
	tout->write((tbyte*)string.c_str(),string.length());
}

void BACConverter::writeOutAnimationVersion3(Stream *tout){
	DataStream *out=new DataStream(tout);

	out->writeLUInt32(TRA_MAGIC+3);

	out->writeLUInt16(mTotalFrame);

	out->writeLUInt16(mAnimationBones.size());

	int i;
	for(i=0;i<mAnimationBones.size();++i){
		BACAnimationBone *bone=mAnimationBones[i];

		out->write((tbyte*)bone->name.c_str(),bone->name.length()+1);

		tbyte dummy[48];
		memset(dummy,0,sizeof(dummy));
		out->write(dummy,sizeof(dummy));

		out->writeLUInt16(bone->translatex.size());

		int j;
		for(j=0;j<bone->translatex.size();++j){
			out->writeLUInt16(bone->translatex[j].first);
			out->writeLFloat(bone->translatex[j].second);
		}

		out->writeLUInt16(bone->translatey.size());

		for(j=0;j<bone->translatey.size();++j){
			out->writeLUInt16(bone->translatey[j].first);
			out->writeLFloat(bone->translatey[j].second);
		}

		out->writeLUInt16(bone->translatez.size());

		for(j=0;j<bone->translatez.size();++j){
			out->writeLUInt16(bone->translatez[j].first);
			out->writeLFloat(bone->translatez[j].second);
		}

		out->writeLUInt16(bone->scalex.size());

		for(j=0;j<bone->scalex.size();++j){
			out->writeLUInt16(bone->scalex[j].first);
			out->writeLFloat(bone->scalex[j].second);
		}

		out->writeLUInt16(bone->scaley.size());

		for(j=0;j<bone->scaley.size();++j){
			out->writeLUInt16(bone->scaley[j].first);
			out->writeLFloat(bone->scaley[j].second);
		}

		out->writeLUInt16(bone->scalez.size());

		for(j=0;j<bone->scalez.size();++j){
			out->writeLUInt16(bone->scalez[j].first);
			out->writeLFloat(bone->scalez[j].second);
		}

		out->writeLUInt16(bone->rotatex.size());

		for(j=0;j<bone->rotatex.size();++j){
			out->writeLUInt16(bone->rotatex[j].first);
			out->writeLFloat(bone->rotatex[j].second);
		}

		out->writeLUInt16(bone->rotatey.size());

		for(j=0;j<bone->rotatey.size();++j){
			out->writeLUInt16(bone->rotatey[j].first);
			out->writeLFloat(bone->rotatey[j].second);
		}

		out->writeLUInt16(bone->rotatez.size());

		for(j=0;j<bone->rotatez.size();++j){
			out->writeLUInt16(bone->rotatez[j].first);
			out->writeLFloat(bone->rotatez[j].second);
		}

		out->writeLUInt16(bone->roll.size());

		for(j=0;j<bone->roll.size();++j){
			out->writeLUInt16(bone->roll[j].first);
			out->writeLFloat(bone->roll[j].second);
		}
	}

	delete out;
}

void BACConverter::clean(){
	int i;
	for(i=0;i<mVertices.size();++i){
		delete mVertices[i];
	}
	mVertices.clear();

	for(i=0;i<mTexCoords.size();++i){
		delete mTexCoords[i];
	}
	mTexCoords.clear();

	for(i=0;i<mTriangles.size();++i){
		delete mTriangles[i];
	}
	mTriangles.clear();

	for(i=0;i<mQuads.size();++i){
		delete mQuads[i];
	}
	mQuads.clear();
	
	for(i=0;i<mBones.size();++i){
		delete mBones[i];
	}
	mBones.clear();

	for(i=0;i<mAnimationBones.size();++i){
		delete mAnimationBones[i];
	}
	mAnimationBones.clear();

	for(i=0;i<mMaterials.size();++i){
		delete mMaterials[i];
	}
	mMaterials.clear();
}
