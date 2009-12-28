#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msPlugInImpl.h"
#include "msLib.h"
#include "DlgOptions.h"
#include "DlgMessage.h"

#include <toadlet/egg/io/FileInputStream.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/handler/XMSHHandler.h>
#include <toadlet/tadpole/handler/XANMHandler.h>

#include <iostream>
#include <sstream>
#include <algorithm>

#pragma warning(disable:4996)

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::handler;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CMsPlugInApp

BEGIN_MESSAGE_MAP(CMsPlugInApp, CWinApp)
	//{{AFX_MSG_MAP(CMsPlugInApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//

CMsPlugInApp::CMsPlugInApp()
{
}

/////////////////////////////////////////////////////////////////////////////
//

CMsPlugInApp theApp;


extern "C" __declspec(dllexport)
cMsPlugIn*
CreatePlugIn ()
{
    return new cPlugIn ();
}



cPlugIn::cPlugIn ()
{
    strcpy (szTitle, "Toadlet Mesh/Animation ...");
	Math::setMatrix3x3FromX(milkshapeToToadlet,Math::degToRad(90));
	Math::setMatrix3x3FromX(toadletToMilkshape,Math::degToRad(-90));
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

void
cPlugIn::convertVector3ToMSVec3(const Vector3 &tvec,msVec3 msvec,bool rotate){
	Vector3 temp(tvec);
	if(rotate){
		Math::mul(temp,toadletToMilkshape);
	}
	msvec[0]=temp.x;
	msvec[1]=temp.y;
	msvec[2]=temp.z;
}

void
cPlugIn::convertQuaternionToMSVec3(const Quaternion &quat,msVec3 msvec,bool rotate){
	if(length(quat,Quaternion(0.5f,-0.5f,-0.5f,-0.5f))<0.0001f){
		msvec[0]=-HALF_PI;
		msvec[1]=HALF_PI;
		msvec[2]=0;
		return;
	}
	else if(length(quat,Quaternion(-0.5f,-0.5f,0.5f,-0.5f))<0.0001f){
		msvec[0]=HALF_PI;
		msvec[1]=HALF_PI;
		msvec[2]=0;
		return;
	}
	else if(length(quat,Quaternion(0.0f,0.0f,-0.707107f,-0.707107f))<0.0001f){
		msvec[0]=-HALF_PI;
		msvec[1]=0;
		msvec[2]=0;
		return;
	}

	Quaternion temp(quat);
	temp.w*=-1.0f;
	Math::normalize(temp);
	Matrix3x3 matrix;
	Math::setMatrix3x3FromQuaternion(matrix,temp);
	if(rotate){
		// This multiply uses milkshapeToToadlet, seems odd but it works
		matrix=matrix*milkshapeToToadlet;
	}
	EulerAngle angle;
	Math::setEulerAngleXYZFromMatrix3x3(angle,matrix);
	msvec[0]=-angle.x;
	msvec[1]=-angle.y;
	msvec[2]=-angle.z;
}

void
cPlugIn::convertMSVec3ToVector3(const msVec3 &msvec,Vector3 &tvec,bool rotate){
	tvec.x=msvec[0];
	tvec.y=msvec[1];
	tvec.z=msvec[2];
	if(rotate){
		Math::mul(tvec,milkshapeToToadlet);
	}
}

void
cPlugIn::convertMSVec3ToQuaternion(const msVec3 &msvec,Quaternion &quat,bool rotate){
	EulerAngle angle(-msvec[0],-msvec[1],-msvec[2]);
	Matrix3x3 matrix;
	Math::setMatrix3x3FromEulerAngleXYZ(matrix,angle);
	if(rotate){
		// This multiply uses toadletToMilkshape, seems odd but it works
		matrix=matrix*toadletToMilkshape;
	}
	Math::setQuaternionFromMatrix3x3(quat,matrix);
	quat.w*=-1.0f;
	Math::normalize(quat);
}

int
cPlugIn::Execute (msModel *pModel)
{
    if (!pModel)
        return -1;

    //
    // switch the module state for MFC Dlls
    //
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    //
    // ask for deletion
    //
    if (msModel_GetBoneCount (pModel) > 0)
    {
        int RC = ::AfxMessageBox (IDS_WARNING_MODEL_DELETE, MB_YESNOCANCEL);
        if (RC != IDYES)
            return 0;
    }

    //
    // options dialog
    //
    cDlgOptions dlgOptions (NULL);
    if (dlgOptions.DoModal () != IDOK)
        return 0;

    CString sPath = dlgOptions.GetPathName ();
    if (sPath.IsEmpty ())
        return 0;

    int nOptionFlags = dlgOptions.GetOptionFlags ();

	String name=(LPCTSTR)sPath;

	int result=-1;
	if(name.find(".xmsh")!=String::npos){
		result=importMesh(pModel,name,nOptionFlags);
	}
	else if(name.find(".xanm")!=String::npos){
		result=importAnimation(pModel,name,nOptionFlags);
	}
	else{
        ::MessageBox (NULL, "Importing model from unknown format!\nPlease choose .xmsh, or .xanm", "Toadlet Mesh/Animation Import", MB_OK | MB_ICONWARNING);
	}

	return result;
}

int
cPlugIn::importMesh(msModel *pModel,const String &name,int flags){
    int i,j;

	FileInputStream::ptr fin(new FileInputStream(name));
	if(fin->isOpen()==false){
		::AfxMessageBox("Error opening file");
		return -1;
	}

	XMSHHandler::ptr handler(new XMSHHandler(NULL,NULL,NULL));
	Mesh::ptr mesh=shared_static_cast<Mesh>(handler->load(fin,NULL));
	if(mesh==NULL){
		::AfxMessageBox("Error loading file");
		return -1;
	}

    cDlgMessage dlgMessage (NULL);
    dlgMessage.Create (IDD_MESSAGE, NULL);
    dlgMessage.SetTitle ("Importing...");

	if(msModel_GetBoneCount(pModel)>0){
		flags&=~eBones;
	}

	/*
	if((flags & eBones) && !(mesh->vertexBoneAssignments.size()>0)){
		::AfxMessageBox("Bones not available");

		delete engine;

		return -1;
	}
	*/

    for(i=0;i<mesh->subMeshes.size();++i){
		Mesh::SubMesh::ptr subMesh=mesh->subMeshes[i];
		msMesh *msmesh=NULL;
		bool bones=(flags & eBones) && (mesh->vertexBoneAssignments.size()>0);

		if(flags & eMeshes){
			msmesh=msModel_GetMeshAt(pModel,msModel_AddMesh(pModel));

			String meshName=subMesh->name;
			if(meshName.length()==0){
				std::stringstream ss;
				ss << "Mesh:" << i;
				meshName=ss.str().c_str();
			}
			msMesh_SetName(msmesh,meshName);

			Collection<int> vertexesToAdd;
			Map<int,int> oldToNewMap;
			IndexBufferAccessor iba(subMesh->indexData->getIndexBuffer());
			for(j=0;j<iba.getSize();++j){
				int v=iba.get(j);
				Collection<int>::iterator x=std::find(vertexesToAdd.begin(),vertexesToAdd.end(),v);
				if(x==vertexesToAdd.end()){
					vertexesToAdd.add(v);
					oldToNewMap[v]=vertexesToAdd.size()-1;
				}
			}

			msVec3 vert;
			msVec3 norm;
			msVec2 tex;

			VertexBuffer::ptr vertexBuffer=mesh->staticVertexData->getVertexBuffer(0);
			VertexFormat::ptr vertexFormat=vertexBuffer->getVertexFormat();
			int positionIndex=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_POSITION);
			int normalIndex=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_NORMAL);
			int texCoordIndex=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_TEX_COORD);
			VertexBufferAccessor vba(vertexBuffer);
			for(j=0;j<vertexesToAdd.size();++j){
				msVertex *vertex=msMesh_GetVertexAt(msmesh,msMesh_AddVertex(msmesh));
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
					const Mesh::VertexBoneAssignmentList &assignment=mesh->vertexBoneAssignments[v];
					if(assignment.size()>0){
						msVertex_SetBoneIndex(vertex,assignment[0].bone);
					}
				}
			}

			for(j=0;j<iba.getSize();j+=3){
				int i1=oldToNewMap[iba.get(j+0)];
				int i2=oldToNewMap[iba.get(j+1)];
				int i3=oldToNewMap[iba.get(j+2)];

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

				const LightEffect &le=material->getLightEffect();
				float opacity=1.0f;//material->getOpacity();

				String textureName;
				if(material->getNumTextureStages()>0){
					textureName=material->getTextureStage(0)->getTextureName();
				}

				msMaterial_SetAmbient(msmat,(float*)le.ambient.getData());
				msMaterial_SetDiffuse(msmat,(float*)le.diffuse.getData());
				msMaterial_SetSpecular(msmat,(float*)le.specular.getData());
				msMaterial_SetEmissive(msmat,(float*)le.emissive.getData());
				msMaterial_SetShininess(msmat,le.shininess);
				msMaterial_SetTransparency(msmat,opacity);
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

				msMaterial_SetDiffuse(msmat,(scalar*)Colors::WHITE.getData());

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
				std::stringstream ss;
				ss << "Bone:" << i;
				boneName=ss.str().c_str();
			}
			msBone_SetName(msbone,boneName);

			if(bone->parentIndex!=-1){
				String parentName=skeleton->bones[bone->parentIndex]->name;
				if(parentName.length()==0){
					std::stringstream ss;
					ss << "Bone:" << bone->parentIndex;
					parentName=ss.str().c_str();
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

	FileInputStream::ptr fin(new FileInputStream(name));
	if(fin->isOpen()==false){
		::AfxMessageBox("Error opening file");
		return -1;
	}

	XANMHandler::ptr handler(new XANMHandler());
	Sequence::ptr sequence=shared_static_cast<Sequence>(handler->load(fin,NULL));
	if(sequence==NULL){
		::AfxMessageBox("Error loading file");
		return -1;
	}

	if(sequence->tracks.size()>msModel_GetBoneCount(pModel)){
		::AfxMessageBox("Invalid number of tracks vs bone count");
		return -1;
	}

    cDlgMessage dlgMessage (NULL);
    dlgMessage.Create (IDD_MESSAGE, NULL);
    dlgMessage.SetTitle ("Importing...");

	float fps=30.0f;

	int i;
	for(i=0;i<sequence->tracks.size();++i){
		Track::ptr track=sequence->tracks[i];

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