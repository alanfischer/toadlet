#include "stdafx.h"
// CreatePlugIn has a different signature in the msLib, so redefine it
#define CreatePlugIn MSCreatePlugIn
#include "msPlugInImpl.h"
#undef CreatePlugIn
#include "../shared/msConversion.h"

#include <toadlet/egg/io/FileStream.h>
#include <toadlet/tadpole/MeshManager.h>
#include <toadlet/tadpole/handler/XMSHHandler.h>
#include <toadlet/tadpole/handler/XANMHandler.h>

#pragma warning(disable:4996)

using namespace toadlet;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::handler;

enum {
    eMeshes         = 1,
    eMaterials      = 2,
    eBones          = 4,
    eKeyFrames      = 8,
	eAll			= 0xFFFF
};

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


extern "C" __declspec(dllexport)
cMsPlugIn*
CreatePlugIn ()
{
    return new cPlugIn ();
}



cPlugIn::cPlugIn ()
{
    strcpy (szTitle, "Toadlet Mesh/Animation ...");
}



cPlugIn::~cPlugIn ()
{
}



int
cPlugIn::GetType ()
{
    return cMsPlugIn::eTypeImport;
}



const char*
cPlugIn::GetTitle ()
{
    return szTitle;
}

int
cPlugIn::Execute (msModel *pModel)
{
    if (!pModel)
        return -1;

    //
    // ask for deletion
    //
    if (msModel_GetBoneCount (pModel) > 0)
    {
        int result=::MessageBox (NULL, "The model is not empty!  Continuing will delete the model!", "Toadlet Mesh/Animation Import", MB_OKCANCEL | MB_ICONWARNING);
		if(result!=IDOK)
            return 0;
    }

    //
    // choose filename
    //
    OPENFILENAME ofn;
    memset (&ofn, 0, sizeof (OPENFILENAME));
    
	char szFile[MS_MAX_PATH]={0};
    char szFileTitle[MS_MAX_PATH];
    char szDefExt[32] = "xmsh\0xanm";
    char szFilter[128] = "Toadlet XML Mesh Files (*.xmsh)\0*.xmsh\0Toadlet XML Animation Files (*.xanm)\0*.xanm\0All Files (*.*)\0*.*\0\0";
    szFile[0] = '\0';
    szFileTitle[0] = '\0';

    ofn.lStructSize = sizeof (OPENFILENAME);
    ofn.lpstrDefExt = szDefExt;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MS_MAX_PATH;
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = MS_MAX_PATH;
    ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrTitle = "Import Toadlet Mesh";

    if (!::GetOpenFileName (&ofn))
        return 0;

	int result=-1;
	String name=szFile;
	if(name.find(".xmsh")!=String::npos){
		result=importMesh(pModel,name,eAll);
	}
	else if(name.find(".xanm")!=String::npos){
		result=importAnimation(pModel,name,eAll);
	}
	else{
        ::MessageBox (NULL, "Importing model from unknown format!\nPlease choose .xmsh, or .xanm", "Toadlet Mesh/Animation Import", MB_OK | MB_ICONWARNING);
	}

	return result;
}

int
cPlugIn::importMesh(msModel *pModel,const String &name,int flags){
    int i,j,k;

	FileStream::ptr stream(new FileStream(name,FileStream::Open_READ_BINARY));
	if(stream->closed()){
		::MessageBox(NULL,"Toadlet Mesh/Animation Import","Error opening file",MB_OK);
		return -1;
	}

	XMSHHandler::ptr handler(new XMSHHandler(NULL));
	Mesh::ptr mesh=shared_static_cast<Mesh>(handler->load(stream,NULL));
	if(mesh==NULL){
		::MessageBox(NULL,"Toadlet Mesh/Animation Import","Error loading file",MB_OK);
		return -1;
	}

	if(msModel_GetBoneCount(pModel)>0){
		flags&=~eBones;
	}

    for(i=0;i<mesh->subMeshes.size();++i){
		Mesh::SubMesh::ptr subMesh=mesh->subMeshes[i];
		msMesh *msmesh=NULL;
		bool bones=(flags & eBones) && (mesh->vertexBoneAssignments.size()>0);

		if(flags & eMeshes){
			msmesh=msModel_GetMeshAt(pModel,msModel_AddMesh(pModel));

			String meshName=subMesh->name;
			if(meshName.length()==0){
				meshName=String("Mesh:")+i;
			}
			msMesh_SetName(msmesh,meshName);

			Collection<int> vertexesToAdd;
			Map<int,int> oldToNewMap;
			IndexBufferAccessor iba(subMesh->indexData->getIndexBuffer());
			for(j=0;j<iba.getSize();++j){
				int v=iba.get(j);
				int x=0;
				for(x=0;x<vertexesToAdd.size();++x){
					if(v==vertexesToAdd[x]) break;
				}
				if(x==vertexesToAdd.size()){
					vertexesToAdd.add(v);
					oldToNewMap[v]=vertexesToAdd.size()-1;
				}
			}

			msVec3 vert;
			msVec3 norm;
			msVec2 tex;

			VertexBuffer::ptr vertexBuffer=mesh->staticVertexData->getVertexBuffer(0);
			VertexFormat::ptr vertexFormat=vertexBuffer->getVertexFormat();
			int positionIndex=vertexFormat->findSemantic(VertexFormat::Semantic_POSITION);
			int normalIndex=vertexFormat->findSemantic(VertexFormat::Semantic_NORMAL);
			int texCoordIndex=vertexFormat->findSemantic(VertexFormat::Semantic_TEX_COORD);
			VertexBufferAccessor vba(vertexBuffer);
			for(j=0;j<vertexesToAdd.size();++j){
				int vid=msMesh_AddVertex(msmesh);
				msVertex *vertex=msMesh_GetVertexAt(msmesh,vid);
				msVertexEx *vertexEx=msMesh_GetVertexExAt(msmesh,vid);
				int v=vertexesToAdd[j];

				if(positionIndex>=0){
					Vector3 position;
					vba.get3(v,positionIndex,position);
					convertVector3ToMSVec3(position,vert,true);
					msVertex_SetVertex(vertex,vert);
				}

				if(normalIndex>=0){
					Vector3 normal;
					vba.get3(v,normalIndex,normal);
					convertVector3ToMSVec3(normal,norm,true);
					msMesh_SetVertexNormalAt(msmesh,msMesh_AddVertexNormal(msmesh),norm);
				}

				if(texCoordIndex>=0){
					Vector2 texCoord;
					vba.get2(v,texCoordIndex,texCoord);
					tex[0]=texCoord.x;
					tex[1]=1.0f-texCoord.y;
					msVertex_SetTexCoords(vertex,tex);
				}

				if(bones){
					const Mesh::VertexBoneAssignmentList &vbal=mesh->vertexBoneAssignments[v];
					int vbalsize=vbal.size();
					if(vbalsize==1){
						msVertex_SetBoneIndex(vertex,vbal[0].bone);
					}
					else if(vbalsize>1){
						msVertex_SetBoneIndex(vertex,vbal[0].bone);
						for(k=1;k<vbalsize;++k){
							msVertexEx_SetBoneIndices(vertexEx,k-1,vbal[k].bone);
						}
						for(k=0;k<vbalsize;++k){
							msVertexEx_SetBoneWeights(vertexEx,k,vbal[k].weight*100);
						}
					}
				}
			}

			for(j=0;j<iba.getSize();j+=3){
				int i1=oldToNewMap[(int)iba.get(j+0)];
				int i2=oldToNewMap[(int)iba.get(j+1)];
				int i3=oldToNewMap[(int)iba.get(j+2)];

				msTriangle *triangle=msMesh_GetTriangleAt(msmesh,msMesh_AddTriangle(msmesh));
				word inds[3]={i1,i2,i3};
				msTriangle_SetVertexIndices(triangle,inds);
				msTriangle_SetNormalIndices(triangle,inds);
			}
		}

		if(flags & eMaterials){
			Material::ptr material=subMesh->material;
			if(material!=NULL){
				int mati=msModel_AddMaterial(pModel);
				msMaterial *msmat=msModel_GetMaterialAt(pModel,mati);

				String materialName=material->getName();
				if(materialName.length()==0){
					materialName="Unnamed";
				}
				msMaterial_SetName(msmat,materialName);

				MaterialState materialState;
				if(material->getMaterialState(materialState)){
					msMaterial_SetAmbient(msmat,(float*)materialState.ambient.getData());
					msMaterial_SetDiffuse(msmat,(float*)materialState.diffuse.getData());
					msMaterial_SetSpecular(msmat,(float*)materialState.specular.getData());
					msMaterial_SetEmissive(msmat,(float*)materialState.emissive.getData());
					msMaterial_SetShininess(msmat,materialState.shininess);
					msMaterial_SetTransparency(msmat,1.0);
				}

				String textureName=material->getTextureName();
				msMaterial_SetDiffuseTexture(msmat,textureName);

				//msMaterial_SetAlphaTexture(msmat,szAlphaTexture);

				if(flags & eMeshes){
					msMesh_SetMaterialIndex(msmesh,mati);
				}
			}
			else{
				int mati=msModel_AddMaterial(pModel);
				msMaterial *msmat=msModel_GetMaterialAt(pModel,mati);
				msMaterial_SetName(msmat,subMesh->materialName);

				msMaterial_SetDiffuse(msmat,(scalar*)Math::ONE_VECTOR4.getData());

				if(flags & eMeshes){
					msMesh_SetMaterialIndex(msmesh,mati);
				}
			}
		}
	}

	if((flags & eBones) && mesh->skeleton!=NULL){
		Skeleton::ptr skeleton=mesh->skeleton;
		
		msVec3 position;
		msVec3 rotation;
		for(i=0;i<skeleton->bones.size();++i){
			Skeleton::Bone::ptr bone=skeleton->bones[i];
			msBone *msbone=msModel_GetBoneAt(pModel,msModel_AddBone(pModel));

			String boneName=bone->name;
			if(boneName.length()==0){
				boneName=String("Bone:")+i;
			}
			msBone_SetName(msbone,boneName);

			if(bone->parentIndex!=-1){
				String parentName=skeleton->bones[bone->parentIndex]->name;
				if(parentName.length()==0){
					parentName=String("Bone:")+bone->parentIndex;
				}
				msBone_SetParentName(msbone,parentName);
			}

			convertVector3ToMSVec3(bone->translate,position,bone->parentIndex==-1);
			msBone_SetPosition(msbone,position);

			convertQuaternionToMSVec3(bone->rotate,rotation,bone->parentIndex==-1);
			msBone_SetRotation(msbone,rotation);
		}
	}

	return 0;
}

Skeleton::ptr
cPlugIn::buildSkeleton(msModel *pModel){
	Skeleton::ptr skeleton(new Skeleton());

	int i;
	for(i=0;i<msModel_GetBoneCount(pModel);++i){
		Skeleton::Bone::ptr bone(new Skeleton::Bone());
		skeleton->bones.add(bone);

		msBone *msbone=msModel_GetBoneAt(pModel,i);

		bone->index=i;

		char name[1024];
		strcpy(name,"");
		msBone_GetName(msbone,name,1024);
		bone->name=name;
		
		strcpy(name,"");
		msBone_GetParentName(msbone,name,1024);
		if(strlen(name)>0){
			bone->parentIndex=msModel_FindBoneByName(pModel,name);
		}
		else{
			bone->parentIndex=-1;
		}

		msVec3 position;
		msBone_GetPosition(msbone,position);
		convertMSVec3ToVector3(position,bone->translate,bone->parentIndex==-1);

		msVec3 rotation;
		msBone_GetRotation(msbone,rotation);
		convertMSVec3ToQuaternion(rotation,bone->rotate,bone->parentIndex==-1);

		Vector3 wtbtranslation(bone->translate);
		Quaternion wtbrotation(bone->rotate);
		Skeleton::Bone::ptr parentBone=bone;
		while(parentBone!=NULL){
			if(parentBone->parentIndex==-1){
				parentBone=NULL;
			}
			else{
				parentBone=skeleton->bones[parentBone->parentIndex];
				Matrix3x3 rotate;
				Math::setMatrix3x3FromQuaternion(rotate,parentBone->rotate);
				Math::mul(wtbtranslation,rotate);
				Math::add(wtbtranslation,parentBone->translate);
				Math::preMul(wtbrotation,parentBone->rotate);
			}
		}

		Math::neg(wtbtranslation);
		Math::invert(wtbrotation);

		Matrix3x3 rotate;
		Math::setMatrix3x3FromQuaternion(rotate,wtbrotation);
		Math::mul(wtbtranslation,rotate);

		bone->worldToBoneTranslate.set(wtbtranslation);
		bone->worldToBoneRotate.set(wtbrotation);
	}

	return skeleton;
}

int
cPlugIn::importAnimation(msModel *pModel,const String &name,int flags){
	Skeleton::ptr skeleton=buildSkeleton(pModel);

	FileStream::ptr stream(new FileStream(name,FileStream::Open_READ_BINARY));
	if(stream->closed()){
		::MessageBox(NULL,"Toadlet Mesh/Animation Import","Error opening file",MB_OK);
		return -1;
	}

	XANMHandler::ptr handler(new XANMHandler());
	TransformSequence::ptr sequence=shared_static_cast<TransformSequence>(handler->load(stream,NULL));
	if(sequence==NULL){
		::MessageBox(NULL,"Toadlet Mesh/Animation Import","Error loading file",MB_OK);
		return -1;
	}

	if(sequence->tracks.size()>msModel_GetBoneCount(pModel)){
		::MessageBox(NULL,"Toadlet Mesh/Animation Import","Invalid number of tracks vs bone count",MB_OK);
		return -1;
	}

	float fps=30.0f;

	int i;
	for(i=0;i<sequence->tracks.size();++i){
		TransformTrack::ptr track=sequence->tracks[i];

		msBone *msbone=msModel_GetBoneAt(pModel,i);

		int j;
		for(j=0;j<track->keyFrames.size();++j){
			msVec3 msvec;
			Vector3 vec;
			Quaternion quat;

			Math::sub(vec,track->keyFrames[j].translate,skeleton->bones[i]->translate);
			Matrix3x3 rotate;
			Math::setMatrix3x3FromQuaternion(rotate,skeleton->bones[i]->rotate);
			Matrix3x3 invrot;
			Math::invert(invrot,rotate);
			Math::mul(vec,invrot);
			convertVector3ToMSVec3(vec,msvec,false);
			msBone_AddPositionKey(msbone,track->keyFrames[j].time*fps,msvec);

			Quaternion invqrot;
			Math::invert(invqrot,skeleton->bones[i]->rotate);
			Math::mul(quat,invqrot,track->keyFrames[j].rotate);
			convertQuaternionToMSVec3(quat,msvec,false);
			msBone_AddRotationKey(msbone,track->keyFrames[j].time*fps,msvec);
		}
	}

	return 0;
}