#include "stdafx.h"
// CreatePlugIn has a different signature in the msLib, so redefine it
#define CreatePlugIn MsCreatePlugIn
#include "msPlugInImpl.h"
#undef CreatePlugIn
#include "../shared/msConversion.h"
#include <toadlet/tadpole.h>

#pragma warning(disable:4996)

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

	engine=new Engine();
	engine->setHasBackableShader(true);
	engine->setHasBackableFixed(true);
	engine->setHasBackableTriangleFan(true);
	engine->setHasBackableFixed(true);
	engine->installHandlers();
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

	FileStream::ptr stream=new FileStream(name,FileStream::Open_READ_BINARY);
	if(stream->closed()){
		::MessageBox(NULL,"Toadlet Mesh/Animation Import","Error opening file",MB_OK);
		return -1;
	}

	XMSHStreamer::ptr streamer=new XMSHStreamer(engine);
	ResourceManager::ImmediateFindRequest::ptr request=new ResourceManager::ImmediateFindRequest();
	streamer->load(stream,NULL,request);
	Mesh::ptr mesh=static_pointer_cast<Mesh>(request->get());
	if(mesh==NULL){
		::MessageBox(NULL,"Toadlet Mesh/Animation Import","Error loading file",MB_OK);
		return -1;
	}

	if(msModel_GetBoneCount(pModel)>0){
		flags&=~eBones;
	}

    for(i=0;i<mesh->getNumSubMeshes();++i){
		Mesh::SubMesh::ptr subMesh=mesh->getSubMesh(i);
		msMesh *msmesh=NULL;
		bool bones=(flags & eBones) && (mesh->getVertexBoneAssignments().size()>0);

		if(flags & eMeshes){
			msmesh=msModel_GetMeshAt(pModel,msModel_AddMesh(pModel));

			String meshName=subMesh->getName();
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
					vertexesToAdd.push_back(v);
					oldToNewMap[v]=vertexesToAdd.size()-1;
				}
			}

			msVec3 vert;
			msVec3 norm;
			msVec2 tex;

			VertexBuffer::ptr vertexBuffer=mesh->getStaticVertexData()->getVertexBuffer(0);
			VertexFormat::ptr vertexFormat=vertexBuffer->getVertexFormat();
			int positionIndex=vertexFormat->findElement(VertexFormat::Semantic_POSITION);
			int normalIndex=vertexFormat->findElement(VertexFormat::Semantic_NORMAL);
			int texCoordIndex=vertexFormat->findElement(VertexFormat::Semantic_TEXCOORD);
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
					const Mesh::VertexBoneAssignmentList &vbal=mesh->getVertexBoneAssignments()[v];
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
				if(material->getPass()->getMaterialState(materialState)){
					msMaterial_SetAmbient(msmat,(float*)materialState.ambient.getData());
					msMaterial_SetDiffuse(msmat,(float*)materialState.diffuse.getData());
					msMaterial_SetSpecular(msmat,(float*)materialState.specular.getData());
					msMaterial_SetEmissive(msmat,(float*)materialState.emissive.getData());
					msMaterial_SetShininess(msmat,materialState.shininess);
					float transparency=(materialState.ambient.w+materialState.diffuse.w+materialState.specular.w)/3.0;
					msMaterial_SetTransparency(msmat,transparency);
				}

				String textureName;//=material->getPass()->getTextureName(0);
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

				msMaterial_SetDiffuse(msmat,(float*)Math::ONE_VECTOR4.getData());

				if(flags & eMeshes){
					msMesh_SetMaterialIndex(msmesh,mati);
				}
			}
		}
	}

	if((flags & eBones) && mesh->getSkeleton()!=NULL){
		Skeleton::ptr skeleton=mesh->getSkeleton();
		
		msVec3 position;
		msVec3 rotation;
		for(i=0;i<skeleton->getNumBones();++i){
			Skeleton::Bone::ptr bone=skeleton->getBone(i);
			msBone *msbone=msModel_GetBoneAt(pModel,msModel_AddBone(pModel));

			String boneName=bone->name;
			if(boneName.length()==0){
				boneName=String("Bone:")+i;
			}
			msBone_SetName(msbone,boneName);

			if(bone->parentIndex!=-1){
				String parentName=skeleton->getBone(bone->parentIndex)->name;
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
cPlugIn::buildSkeleton(msModel *pModel,const Collection<int> &emptyBones){
	Skeleton::ptr skeleton=new Skeleton();

	int i,j;
	for(i=0;i<msModel_GetBoneCount(pModel);++i){
		int newi=i;
		for(j=0;j<emptyBones.size();++j){
			if(i==emptyBones[j]){
				break;
			}
			else if(emptyBones[j]<newi){
				newi--;
			}
		}
		if(j!=emptyBones.size()){
			continue;
		}

		Skeleton::Bone::ptr bone(new Skeleton::Bone());
		skeleton->addBone(bone);
	
		msBone *msbone=msModel_GetBoneAt(pModel,i);

		bone->index=newi;

		char name[1024];
		strcpy(name,"");
		msBone_GetName(msbone,name,1024);
		bone->name=name;
		
		strcpy(name,"");
		msBone_GetParentName(msbone,name,1024);
		if(strlen(name)>0){
			bone->parentIndex=msModel_FindBoneByName(pModel,name);
			for(j=0;j<emptyBones.size();++j){
				if(emptyBones[j]<bone->parentIndex){
					bone->parentIndex--;
				}
			}
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
	}

	skeleton->compile();

	return skeleton;
}

int
cPlugIn::importAnimation(msModel *pModel,const String &name,int flags){
	Skeleton::ptr skeleton=buildSkeleton(pModel,Collection<int>());

	FileStream::ptr stream=new FileStream(name,FileStream::Open_READ_BINARY);
	if(stream->closed()){
		::MessageBox(NULL,"Toadlet Mesh/Animation Import","Error opening file",MB_OK);
		return -1;
	}

	ResourceStreamer::ptr streamer=new XANMStreamer(engine);
	ResourceManager::ImmediateFindRequest::ptr request=new ResourceManager::ImmediateFindRequest();
	streamer->load(stream,NULL,request);
	Sequence::ptr sequence=static_pointer_cast<Sequence>(request->get());
	if(sequence==NULL){
		::MessageBox(NULL,"Toadlet Mesh/Animation Import","Error loading file",MB_OK);
		return -1;
	}

	if(sequence->getNumTracks()>msModel_GetBoneCount(pModel)){
		::MessageBox(NULL,"Toadlet Mesh/Animation Import","Invalid number of tracks vs bone count",MB_OK);
		return -1;
	}

	float fps=30.0f;

	int i;
	for(i=0;i<sequence->getNumTracks();++i){
		Track::ptr track=sequence->getTrack(i);

		msBone *msbone=msModel_GetBoneAt(pModel,i);

		VertexBufferAccessor &vba=track->getAccessor();
	
		int j;
		for(j=0;j<track->getNumKeyFrames();++j){
			msVec3 msvec;
			Vector3 translate;
			Quaternion rotate,invBoneRotate;

			vba.get3(j,0,translate);
			vba.get4(j,1,rotate);

			Math::invert(invBoneRotate,skeleton->getBone(i)->rotate);
			Math::sub(translate,skeleton->getBone(i)->translate);

			Math::mul(translate,invBoneRotate);
			convertVector3ToMSVec3(translate,msvec,false);
			msBone_AddPositionKey(msbone,track->getTime(j)*fps,msvec);

			Math::postMul(rotate,invBoneRotate);
			convertQuaternionToMSVec3(rotate,msvec,false);
			msBone_AddRotationKey(msbone,track->getTime(j)*fps,msvec);
		}
	}

	return 0;
}