// This loader claims to be neither fast or well written, it simply accomplishes the task

#include "hlmdlsource/mathlib.h"
#include "hlmdlsource/studio.h"
#include "hlmdlsource/mdlviewer.h"
#include "HalfLifeMDLHandler.h"

#include <toadlet/tadpole/material/StandardMaterial.h>

extern float g_bonetransform[MAXSTUDIOBONES][3][4];

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::tadpole::material;
using namespace toadlet::tadpole::skeleton;

Vector3 getBoneTranslateVector3(int i){
	return Vector3(g_bonetransform[i][0][3],g_bonetransform[i][1][3],g_bonetransform[i][2][3]);
}

Matrix3x3 getBoneRotateMatrix3x3(int i){
	return Matrix3x3(	g_bonetransform[i][0][0],g_bonetransform[i][0][1],g_bonetransform[i][0][2],
						g_bonetransform[i][1][0],g_bonetransform[i][1][1],g_bonetransform[i][1][2],
						g_bonetransform[i][2][0],g_bonetransform[i][2][1],g_bonetransform[i][2][2]
	);
}

	toadlet::tadpole::mesh::MeshData *load(toadlet::egg::io::InputStream *in,const toadlet::egg::String &name);
	void save(toadlet::tadpole::mesh::MeshData *resource,toadlet::egg::io::OutputStream *out,const toadlet::egg::String &name);
	bool handlesType(const toadlet::egg::String &ext);

	void setTextureManager(toadlet::tadpole::resource::TextureManager *textureManager);
	void setMaterialManager(toadlet::tadpole::resource::MaterialManager *materialManager);
	void setSkeletonDataManager(toadlet::tadpole::resource::SkeletonDataManager *skeletonDataManager);
	void setAnimationDataManager(toadlet::tadpole::resource::AnimationDataManager *animationDataManager);
	void setVertexBufferManager(toadlet::tadpole::resource::VertexBufferManager *vertexBufferManager);
	void setIndexBufferManager(toadlet::tadpole::resource::IndexBufferManager *indexBufferManager);
HalfLifeMDLHandler::HalfLifeMDLHandler(){
	mTextureManager=NULL;
	mMaterialManager=NULL;
	mSkeletonDataManager=NULL;
	mAnimationDataManager=NULL;
	mVertexBufferManager=NULL;
	mIndexBufferManager=NULL;
}

HalfLifeMDLHandler::~HalfLifeMDLHandler(){
}

MeshData *HalfLifeMDLHandler::load(InputStream *in,const String &name){
	StudioModel tempmodel;
	mstudiobone_t *pbones;
	mstudioseqdesc_t *pseqdesc;
	int i;

	// Clear tempmodel
	memset(&tempmodel.m_origin,0,sizeof(tempmodel.m_origin));
	memset(&tempmodel.m_angles,0,sizeof(tempmodel.m_angles));
	memset(&tempmodel.m_sequence,0,sizeof(tempmodel.m_sequence));
	memset(&tempmodel.m_frame,0,sizeof(tempmodel.m_frame));
	memset(&tempmodel.m_bodynum,0,sizeof(tempmodel.m_bodynum));
	memset(&tempmodel.m_skinnum,0,sizeof(tempmodel.m_skinnum));
	memset(&tempmodel.m_controller,0,sizeof(tempmodel.m_controller));
	memset(&tempmodel.m_blending,0,sizeof(tempmodel.m_blending));
	memset(&tempmodel.m_mouth,0,sizeof(tempmodel.m_mouth));

	tempmodel.Init((char*)(mEngine->getDirectory()+"/"+name).c_str());

	tempmodel.SetController(0,0);
	tempmodel.SetController(1,0);
	tempmodel.SetController(2,0);
	tempmodel.SetController(3,0);
	tempmodel.SetMouth(0);
	tempmodel.SetBodygroup(0,0);
	tempmodel.SetupModel(0);

	SkeletonData::Ptr skeletonData=mEngine->getSkeletonDataManager()->load(new SkeletonData());

	// First, handle bones & animations
	pbones=(mstudiobone_t *)((byte *)tempmodel.m_pstudiohdr + tempmodel.m_pstudiohdr->boneindex);

	int numBones=tempmodel.m_pstudiohdr->numbones;

	skeletonData->bones.resize(numBones);

	for(i=0;i<numBones;i++){
		BoneData *bone=new BoneData();
		skeletonData->bones[i]=bone;
		bone->index=i;
		bone->parent=pbones[i].parent;

		if(bone->parent!=-1){
			skeletonData->bones[bone->parent]->children.push_back(i);
		}
	}

	int numAnimations=tempmodel.m_pstudiohdr->numseq;

	skeletonData->animations.resize(numAnimations);
	for(i=0;i<numAnimations;++i){
		AnimationData::Ptr animation=mEngine->getAnimationDataManager()->load(new AnimationData());
		skeletonData->animations[i]=animation;
		
		tempmodel.SetSequence(i);

		pseqdesc=(mstudioseqdesc_t *)((byte *)tempmodel.m_pstudiohdr + tempmodel.m_pstudiohdr->seqindex) + (int)tempmodel.m_sequence;
		int numKeyFrames=pseqdesc->numframes;

		animation->tracks.resize(numBones);
		animation->length=(float)pseqdesc->numframes/(float)pseqdesc->fps;
		int j;
		for(j=0;j<numBones;++j){
			AnimationData::Track *track=new AnimationData::Track(animation);
			track->bone=j;
			track->keyFrames.resize(numKeyFrames);
			animation->tracks[j]=track;

			if(i==0){
				skeletonData->bones[j]->worldToBoneTranslate=-getBoneTranslateVector3(j);
//				skeletonData->bones[j]->worldToBoneRotate=makeQuaternion(makeInverse(getBoneRotateMatrix3x3(j)));
			}
		}

		for(j=0;j<numKeyFrames;++j){
			tempmodel.m_frame=j;
			tempmodel.SetUpBones();

			int k;
			for(k=0;k<numBones;++k){
				animation->tracks[k]->keyFrames[j].time=(float)j/(float)pseqdesc->fps;

				int parent=skeletonData->bones[k]->parent;
				if(parent!=-1){
					Matrix3x3 invParent=makeInverse(getBoneRotateMatrix3x3(parent));
					animation->tracks[k]->keyFrames[j].translate=invParent*(getBoneTranslateVector3(k)-getBoneTranslateVector3(parent));
					animation->tracks[k]->keyFrames[j].rotate=makeQuaternion(invParent*getBoneRotateMatrix3x3(k));
				}
				else{
					animation->tracks[k]->keyFrames[j].translate=getBoneTranslateVector3(k);
					animation->tracks[k]->keyFrames[j].rotate=makeQuaternion(getBoneRotateMatrix3x3(k));

				}

				if(i==0 && j==0){
					skeletonData->bones[k]->translate=animation->tracks[k]->keyFrames[j].translate;
					skeletonData->bones[k]->rotate=animation->tracks[k]->keyFrames[j].rotate;
				}
			}
		}

		animation->compile();
	}

	skeletonData->compile();

	// Textures
	int numTextures=tempmodel.m_pstudiohdr->numtextures;
	Collection<Texture::Ptr> textures;
	textures.resize(numTextures);

	mstudiotexture_t *ptexture=(mstudiotexture_t *)((byte*)tempmodel.m_pstudiohdr + tempmodel.m_pstudiohdr->textureindex);

	int outwidth=0,outheight=0;
	if(tempmodel.m_pstudiohdr->textureindex!=0){
		for(i=0;i<numTextures;i++){
			for(outwidth=1;outwidth<ptexture[i].width;outwidth<<=1);
			if(outwidth>256)outwidth=256;
			for(outheight=1;outheight<ptexture[i].height;outheight<<=1);
			if(outheight>256)outheight=256;

			Image *image=new Image(Image::DIMENSION_2D,Image::FORMAT_RGB,Image::TYPE_BYTE,outwidth,outheight,1);
			unsigned char *out=image->getData();

			int	row1[256], row2[256], col1[256], col2[256];
			byte *pix1, *pix2, *pix3, *pix4;
			byte *pal=((byte*)tempmodel.m_pstudiohdr)+ptexture[i].width*ptexture[i].height+ptexture[i].index;
			byte *data=((byte*)tempmodel.m_pstudiohdr)+ptexture[i].index;

			int j;
			for(j=0;j<outwidth;j++){
				col1[j] = (int)((j + 0.25) * (ptexture[i].width / (float)outwidth));
				col2[j] = (int)((j + 0.75) * (ptexture[i].width / (float)outwidth));
			}

			for(j=0;j<outheight;j++){
				row1[j] = (int)((j + 0.25) * (ptexture[i].height / (float)outheight)) * ptexture[i].width;
				row2[j] = (int)((j + 0.75) * (ptexture[i].height / (float)outheight)) * ptexture[i].width;
			}

			for(j=0;j<outheight;j++){
				int k;
				for(k=0;k<outwidth;k++,out+=3){
					pix1 = &pal[data[row1[j] + col1[k]] * 3];
					pix2 = &pal[data[row1[j] + col2[k]] * 3];
					pix3 = &pal[data[row2[j] + col1[k]] * 3];
					pix4 = &pal[data[row2[j] + col2[k]] * 3];

					out[0] = (pix1[0] + pix2[0] + pix3[0] + pix4[0])>>2;
					out[1] = (pix1[1] + pix2[1] + pix3[1] + pix4[1])>>2;
					out[2] = (pix1[2] + pix2[2] + pix3[2] + pix4[2])>>2;
				}
			}

			textures[i]=mEngine->getTextureManager()->load(new Texture(image));
		}
	}

	tempmodel.SetSequence(0);
	tempmodel.m_frame=0;
	tempmodel.SetUpBones();

	// MeshData
	int group=0;
	mstudiobodyparts_t *pbodypart=(mstudiobodyparts_t*)((byte*)tempmodel.m_pstudiohdr + tempmodel.m_pstudiohdr->bodypartindex) + group;
	int numModels=pbodypart->nummodels;
	int m;
	for(m=0;m<numModels;++m){
		String loadName=name;
		if(m>0){
			StringStream ss;
			ss << name << ":" << m;
			loadName=ss.str();
		}

		MeshData *meshDataMeshData::Ptr meshData=mEngine->getMeshDataManager()->load(new MeshData(),loadName);
		meshData->skeletonData=skeletonData;

		// Reset the model
		tempmodel.SetBodygroup(0,m);
		tempmodel.SetupModel(0);

		int numVertexes=tempmodel.m_pmodel->numverts;
		byte *pvertbone=((byte *)tempmodel.m_pstudiohdr + tempmodel.m_pmodel->vertinfoindex);
		Vector3 *pstudioverts=(Vector3*)((byte *)tempmodel.m_pstudiohdr + tempmodel.m_pmodel->vertindex);

		int numNormals=tempmodel.m_pmodel->numnorms;
		byte *pnormbone=((byte *)tempmodel.m_pstudiohdr + tempmodel.m_pmodel->norminfoindex);
		Vector3 *pstudionorms=(Vector3*)((byte *)tempmodel.m_pstudiohdr + tempmodel.m_pmodel->normindex);

		Collection<Vector3> positions;
		Collection<Vector3> normals;
		Collection<Vector2> texCoords;
		Collection<int> bones;

		Collection<Collection<int> > allindexes;

		int numSubMeshes=tempmodel.m_pmodel->nummesh;
		meshData->subMeshes.resize(numSubMeshes);

		for(i=0;i<numSubMeshes;i++){
			short *ptricmds;
			mstudiomesh_t *pmesh;
			short *pskinref;

			pmesh=(mstudiomesh_t *)((byte *)tempmodel.m_pstudiohdr + tempmodel.m_pmodel->meshindex) + i;
			ptricmds=(short *)((byte *)tempmodel.m_pstudiohdr + pmesh->triindex);
			pskinref=(short *)((byte *)tempmodel.m_ptexturehdr + tempmodel.m_ptexturehdr->skinindex);

			SubMeshData *subMesh=new SubMeshData(meshData);
			meshData->subMeshes[i]=subMesh;

			StandardMaterial *material=new StandardMaterial();
			subMesh->materials.push_back(mEngine->getMaterialManager()->load(material));
			material->setMap(StandardMaterial::MAP_DIFFUSE,textures[pmesh->skinref],1);

			unsigned int width=0,height=0;
			textures[pmesh->skinref]->getSize(width,height);

			float s=1.0/(float)width;
			float t=1.0/(float)height;

			Collection<int> indexes;

			int j=0;
			while(j=*(ptricmds++)){
				if(j<0){
					j=-j;

					Vector3 firstPosition=pstudioverts[ptricmds[0]];
					Vector3 firstNormal=pstudionorms[ptricmds[1]];
					Vector2 firstTexCoord=Vector2(ptricmds[2]*s,ptricmds[3]*t);
					int firstBone=pvertbone[ptricmds[0]];
					j--;ptricmds+=4;

					Vector3 secondPosition=pstudioverts[ptricmds[0]];
					Vector3 secondNormal=pstudionorms[ptricmds[1]];
					Vector2 secondTexCoord=Vector2(ptricmds[2]*s,ptricmds[3]*t);
					int secondBone=pvertbone[ptricmds[0]];
					j--;ptricmds+=4;

					for(;j>0;j--,ptricmds+=4){
						Vector3 thirdPosition=pstudioverts[ptricmds[0]];
						Vector3 thirdNormal=pstudionorms[ptricmds[1]];
						Vector2 thirdTexCoord=Vector2(ptricmds[2]*s,ptricmds[3]*t);
						int thirdBone=pvertbone[ptricmds[0]];

						indexes.push_back(positions.size());
						positions.push_back(thirdPosition);
						normals.push_back(thirdNormal);
						texCoords.push_back(thirdTexCoord);
						bones.push_back(thirdBone);

						indexes.push_back(positions.size());
						positions.push_back(secondPosition);
						normals.push_back(secondNormal);
						texCoords.push_back(secondTexCoord);
						bones.push_back(secondBone);

						indexes.push_back(positions.size());
						positions.push_back(firstPosition);
						normals.push_back(firstNormal);
						texCoords.push_back(firstTexCoord);
						bones.push_back(firstBone);

						secondPosition=thirdPosition;
						secondNormal=thirdNormal;
						secondTexCoord=thirdTexCoord;
						secondBone=thirdBone;
					}
				}
				else{
					bool invert=false;

					Vector3 firstPosition=pstudioverts[ptricmds[0]];
					Vector3 firstNormal=pstudionorms[ptricmds[1]];
					Vector2 firstTexCoord=Vector2(ptricmds[2]*s,ptricmds[3]*t);
					int firstBone=pvertbone[ptricmds[0]];
					j--;ptricmds+=4;

					Vector3 secondPosition=pstudioverts[ptricmds[0]];
					Vector3 secondNormal=pstudionorms[ptricmds[1]];
					Vector2 secondTexCoord=Vector2(ptricmds[2]*s,ptricmds[3]*t);
					int secondBone=pvertbone[ptricmds[0]];
					j--;ptricmds+=4;

					for(;j>0;j--,ptricmds+=4){
						Vector3 thirdPosition=pstudioverts[ptricmds[0]];
						Vector3 thirdNormal=pstudionorms[ptricmds[1]];
						Vector2 thirdTexCoord=Vector2(ptricmds[2]*s,ptricmds[3]*t);
						int thirdBone=pvertbone[ptricmds[0]];

						if(invert){
							indexes.push_back(positions.size());
							positions.push_back(firstPosition);
							normals.push_back(firstNormal);
							texCoords.push_back(firstTexCoord);
							bones.push_back(firstBone);

							indexes.push_back(positions.size());
							positions.push_back(secondPosition);
							normals.push_back(secondNormal);
							texCoords.push_back(secondTexCoord);
							bones.push_back(secondBone);

							indexes.push_back(positions.size());
							positions.push_back(thirdPosition);
							normals.push_back(thirdNormal);
							texCoords.push_back(thirdTexCoord);
							bones.push_back(thirdBone);
						}
						else{
							indexes.push_back(positions.size());
							positions.push_back(thirdPosition);
							normals.push_back(thirdNormal);
							texCoords.push_back(thirdTexCoord);
							bones.push_back(thirdBone);

							indexes.push_back(positions.size());
							positions.push_back(secondPosition);
							normals.push_back(secondNormal);
							texCoords.push_back(secondTexCoord);
							bones.push_back(secondBone);

							indexes.push_back(positions.size());
							positions.push_back(firstPosition);
							normals.push_back(firstNormal);
							texCoords.push_back(firstTexCoord);
							bones.push_back(firstBone);
						}

						firstPosition=secondPosition;
						firstNormal=secondNormal;
						firstTexCoord=secondTexCoord;
						firstBone=secondBone;

						secondPosition=thirdPosition;
						secondNormal=thirdNormal;
						secondTexCoord=thirdTexCoord;
						secondBone=thirdBone;

						invert=!invert;
					}
				}
			}

			allindexes.push_back(indexes);
		}

		// Now pack the vertex & index buffers
		numVertexes=positions.size();
		meshData->vertexBuffer=mEngine->getVertexBufferManager()->load(new VertexBuffer());
		meshData->vertexBuffer->resize((VertexBuffer::VertexType)(VertexBuffer::VT_POSITION|VertexBuffer::VT_NORMAL|VertexBuffer::VT_TEXCOORD1_DIM2|VertexBuffer::VT_BONE),numVertexes);

		for(i=0;i<numVertexes;++i){
			meshData->vertexBuffer->position(i)=positions[i];
			meshData->vertexBuffer->normal(i)=normals[i];
			meshData->vertexBuffer->texCoord2d(i)=texCoords[i];
			BoneAssignmentCollection bac;
			BoneAssignment ba;
			ba.index=bones[i];
			ba.weight=1;
			bac.push_back(ba);
			meshData->vertexBuffer->bone(i)=bac;
		}

		for(i=0;i<allindexes.size();++i){
			IndexBuffer::Ptr indexBuffer=mEngine->getIndexBufferManager()->load(new IndexBuffer());
			meshData->subMeshes[i]->indexBuffer=indexBuffer;

			int numIndexes=allindexes[i].size();
			indexBuffer->resize(numIndexes);

			int j;
			for(j=0;j<allindexes[i].size();++j){
				indexBuffer->index(j)=allindexes[i][j];
			}
		}
	}

	return meshData;
}

void HalfLifeMDLLoader::reset(){
	mCache.clear();
}
