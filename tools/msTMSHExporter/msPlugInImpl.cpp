#include "stdafx.h"
#include "msPlugInImpl.h"

#include <toadlet/egg/io/FileStream.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/MeshManager.h>
#include <toadlet/tadpole/handler/XMSHHandler.h>
#include <toadlet/tadpole/handler/XANMHandler.h>

#pragma warning(disable:4996)

using namespace toadlet::egg::io;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::handler;
using namespace toadlet::tadpole::mesh;

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
    strcpy (szTitle, "Toadlet Mesh/Animation...");
	Math::setMatrix3x3FromX(milkshapeToToadlet,Math::degToRad(90));
	Math::setMatrix3x3FromX(toadletToMilkshape,Math::degToRad(-90));
}



cPlugIn::~cPlugIn ()
{
}



int
cPlugIn::GetType ()
{
    return cMsPlugIn::eTypeExport;
}



const char*
cPlugIn::GetTitle ()
{
    return szTitle;
}

class Vertex{
public:
	Vector3 position;
	Vector3 normal;
	Vector2 texCoord;
	int bone;

	bool operator==(const Vertex &v) const{
		return v.position==position && v.normal==normal && v.texCoord==texCoord && v.bone==bone;
	}
};

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
//	Matrix3x3 matrix;
//	Math::setMatrix3x3FromEulerAngleXYZ(matrix,angle);
//	if(rotate){
//		// This multiply uses toadletToMilkshape, seems odd but it works
//		matrix=matrix*toadletToMilkshape;
//	}
//	Math::setQuaternionFromMatrix3x3(quat,matrix);
	Math::setQuaternionFromEulerAngleXYZ(quat,angle);
	quat.w*=-1.0f;
	Math::normalize(quat);
}

int
cPlugIn::Execute (msModel *pModel)
{
    if (!pModel)
        return -1;

    //
    // check, if we have something to export
    //
    if (msModel_GetMeshCount (pModel) == 0)
    {
        ::MessageBox (NULL, "The model is empty!  Nothing exported!", "Toadlet Mesh/Animation Export", MB_OK | MB_ICONWARNING);
        return 0;
    }

	msModel_SetFrame(pModel,0);

    //
    // choose filename
    //
    OPENFILENAME ofn;
    memset (&ofn, 0, sizeof (OPENFILENAME));
    
    char szFile[MS_MAX_PATH];
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
    ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    ofn.lpstrTitle = "Export Toadlet Mesh";

    if (!::GetSaveFileName (&ofn))
        return 0;

	int result=-1;
	String name=szFile;
	if(name.find(".xmsh")!=String::npos){
		result=exportMesh(pModel,name);
	}
	else if(name.find(".xanm")!=String::npos){
		result=exportAnimation(pModel,name);
	}
	else{
        ::MessageBox (NULL, "Exporting model to unknown format!\nPlease choose .xmsh, or .xanm", "Toadlet Mesh Export", MB_OK | MB_ICONWARNING);
	}

    // Don't forget to destroy the model
    msModel_Destroy(pModel);

    return result;
}

void
cPlugIn::findEmptyBones(msModel *pModel,Collection<int> &emptyBones){
	int i,j,k;
	char name[1024];
	char name2[1024];
	strcpy(name,"");
	strcpy(name2,"");

	emptyBones.clear();
	for(i=0;i<msModel_GetBoneCount(pModel);++i){
		msBone *msbone=msModel_GetBoneAt(pModel,i);
		msBone_GetParentName(msbone,name,1024);
		if(strlen(name)==0){
			msBone_GetName(msbone,name,1024);
			for(j=0;j<msModel_GetBoneCount(pModel);++j){
				msBone *msbone2=msModel_GetBoneAt(pModel,j);
				msBone_GetParentName(msbone2,name2,1024);
				if(strcmp(name2,name)==0){
					break;
				}
			}
			if(j==msModel_GetBoneCount(pModel)){
				emptyBones.add(i);
			}
		}
	}

	for(i=0;i<msModel_GetMeshCount(pModel);++i){
		msMesh *msmesh=msModel_GetMeshAt(pModel,i);
		for(j=0;j<msMesh_GetVertexCount(msmesh);++j){
			msVertex *msvertex=msMesh_GetVertexAt(msmesh,j);
			int bone=msVertex_GetBoneIndex(msvertex);
			for(k=0;k<emptyBones.size();++k){
				if(emptyBones[k]==bone){
					emptyBones.erase(emptyBones.begin()+k);
					break;
				}
			}
		}
	}
}

int
cPlugIn::exportMesh(msModel *pModel,const String &name){
	Engine *engine=new Engine();

	Mesh::ptr mesh(new Mesh());

	Collection<Vertex> vertexes;
	Collection<String> meshNames;
	Collection<Collection<int> > indexes;
	Collection<int> materialIndexes;
	Collection<int> emptyBones;

	findEmptyBones(pModel,emptyBones);

	bool bones=false;
	if((msModel_GetBoneCount(pModel)-emptyBones.size())>1){ // Need at least 2 bones for it to be animated
		bones=true;
	}

	int meshCount=msModel_GetMeshCount(pModel);
	indexes.resize(meshCount);
	meshNames.resize(meshCount);
	materialIndexes.resize(meshCount);

	int i,j,k,l;
    for(i=0;i<meshCount;++i){
		msMesh *msmesh=msModel_GetMeshAt(pModel,i);

		char name[256];
		msMesh_GetName(msmesh,name,256);
		meshNames[i]=name;

		materialIndexes[i]=msMesh_GetMaterialIndex(msmesh);

		for(j=0;j<msMesh_GetTriangleCount(msmesh);++j){
			msTriangle *triangle=msMesh_GetTriangleAt(msmesh,j);

			word verts[3],norms[3];
			msTriangle_GetVertexIndices(triangle,verts);
			msTriangle_GetNormalIndices(triangle,norms);

			for(k=0;k<3;++k){
				msVertex *vertex=msMesh_GetVertexAt(msmesh,verts[k]);

				msVec3 vert;
				msVec2 tex;
				msVec3 norm;
				int bone;

				msVertex_GetVertex(vertex,vert);
				msVertex_GetTexCoords(vertex,tex);
				msMesh_GetVertexNormalAt(msmesh,norms[k],norm);
				bone=msVertex_GetBoneIndex(vertex);
				if(bone==-1){
					bone=0; // If bone is unassigned, assign it to the root bone
				}

				for(l=0;l<emptyBones.size();++l){
					if(emptyBones[l]<bone){
						bone--;
					}
				}

				Vertex v;
				convertMSVec3ToVector3(vert,v.position,true);
				v.texCoord=Vector2(tex[0],1.0f-tex[1]);
				convertMSVec3ToVector3(norm,v.normal,true);
				v.bone=bone;

				int l;
				for(l=0;l<vertexes.size();++l){
					if(vertexes[l]==v)break;
				}

				if(l==vertexes.size()){
					vertexes.add(v);
					indexes[i].add(vertexes.size()-1);
				}
				else{
					indexes[i].add(l);
				}
			}
		}
	}

	VertexFormat::ptr vertexFormat=engine->getVertexFormats().POSITION_NORMAL_TEX_COORD;
	VertexBuffer::ptr vertexBuffer=engine->getBufferManager()->createVertexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_READ_WRITE,vertexFormat,vertexes.size());
	mesh->staticVertexData=VertexData::ptr(new VertexData(vertexBuffer));

	if(bones){
		mesh->vertexBoneAssignments.resize(vertexes.size());
	}

	{
		VertexBufferAccessor vba(vertexBuffer);
		for(i=0;i<vertexes.size();++i){
			vba.set3(i,0,vertexes[i].position);
			vba.set3(i,1,vertexes[i].normal);
			vba.set2(i,2,vertexes[i].texCoord);
			if(bones){
				Mesh::VertexBoneAssignment assignment;
				assignment.bone=vertexes[i].bone;
				assignment.weight=1.0f;
				mesh->vertexBoneAssignments[i].add(assignment);
			}
		}
	}

    for(i=0;i<indexes.size();++i){
		Mesh::SubMesh::ptr sub(new Mesh::SubMesh());
		mesh->subMeshes.add(sub);

		sub->name=meshNames[i];

		IndexBuffer::ptr indexBuffer=engine->getBufferManager()->createIndexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_READ_WRITE,IndexBuffer::IndexFormat_UINT_16,indexes[i].size());
		sub->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,indexes[i].size()));

		{
			IndexBufferAccessor iba(indexBuffer);
			for(j=0;j<indexes[i].size();++j){
				iba.set(j,indexes[i][j]);
			}
		}

		// TODO: Store the materials outside of the submesh, so I can reference materials multiple times
		int materialIndex=materialIndexes[i];
		if(materialIndex>=0){
			msMaterial *msmat=msModel_GetMaterialAt(pModel,materialIndex);

			Material::ptr material(new Material());
			material->setSaveLocally(true);

			char name[256];
			msMaterial_GetName(msmat,name,256);
			material->setName(name);

			char texture[1024];
			msMaterial_GetDiffuseTexture(msmat,texture,1024);
			if(strlen(texture)>0){
				TextureStage::ptr diffuseStage(new TextureStage());
				diffuseStage->setTextureName(texture);
				material->setTextureStage(0,diffuseStage);
			}

			//material->setOpacity(msMaterial_GetTransparency(msmat));

			LightEffect le;
			msMaterial_GetAmbient(msmat,le.ambient.getData());
			msMaterial_GetDiffuse(msmat,le.diffuse.getData());
			msMaterial_GetSpecular(msmat,le.specular.getData());
			msMaterial_GetEmissive(msmat,le.emissive.getData());
			le.shininess=msMaterial_GetShininess(msmat);
			material->setLightEffect(le);

			material->setLighting(true);

			sub->material=material;
		}
		else{
			MessageBox(NULL,"Mesh has no material, exporting empty material","Warning",NULL);

			sub->material=Material::ptr(new Material());
		}
	}

	if(bones){
		mesh->skeleton=buildSkeleton(pModel,emptyBones);
	}

	FileStream::ptr stream(new FileStream(name,FileStream::OpenFlags_WRITE|FileStream::OpenFlags_BINARY));
	XMSHHandler::ptr handler(new XMSHHandler(NULL,NULL,NULL));

	handler->save(mesh,stream);

	delete engine;

	return 0;
}

Skeleton::ptr
cPlugIn::buildSkeleton(msModel *pModel,const Collection<int> &emptyBones){
	Skeleton::ptr skeleton(new Skeleton());

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
		skeleton->bones.add(bone);

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
cPlugIn::exportAnimation(msModel *pModel,const String &name){
	Engine *engine=new Engine();

	Collection<int> emptyBones;
	findEmptyBones(pModel,emptyBones);

	Sequence::ptr sequence(new Sequence());

	int s1=name.rfind('/');
	if(s1==String::npos){s1=0;}
	int s2=name.rfind('\\');
	if(s2==String::npos){s2=0;}
	int start=Math::maxVal(s1,s2)+1;
	int end=name.rfind('.');
	if(end<0){end=name.length();}
	
	sequence->setName(name.substr(start,end-start));

	float maxTime=0;
	float fps=30.0f;

	Skeleton::ptr skeleton=buildSkeleton(pModel,emptyBones);

	int i,j,k;
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

		msBone *msbone=msModel_GetBoneAt(pModel,i);

		Track::ptr track(new Track());
		sequence->tracks.add(track);

		track->index=newi;

		Collection<Frame> frames;

		for(j=0;j<msBone_GetPositionKeyCount(msbone);++j){
			msPositionKey *positionKey=msBone_GetPositionKeyAt(msbone,j);

			if(positionKey->fTime>maxTime){
				maxTime=positionKey->fTime;
			}

			int insertAfter=-1;
			for(k=0;k<frames.size();++k){
				if(frames[k].time==positionKey->fTime){
					convertMSVec3ToVector3(positionKey->Position,frames[k].translate,false);
					Matrix3x3 rotate;
					Math::setMatrix3x3FromQuaternion(rotate,skeleton->bones[newi]->rotate);
					Math::mul(frames[k].translate,rotate);
					Math::add(frames[k].translate,skeleton->bones[newi]->translate);
					frames[k].translateSet=true;
					break;
				}
				else if(frames[k].time>positionKey->fTime && insertAfter==-1){
					insertAfter=k;
				}
			}
			if(k==frames.size()){
				Frame frame;
				frame.time=positionKey->fTime;
				convertMSVec3ToVector3(positionKey->Position,frame.translate,false);
				Matrix3x3 rotate;
				Math::setMatrix3x3FromQuaternion(rotate,skeleton->bones[newi]->rotate);
				Math::mul(frame.translate,rotate);
				Math::add(frame.translate,skeleton->bones[newi]->translate);
				frame.translateSet=true;
				if(insertAfter==-1){
					frames.add(frame);
				}
				else{
					frames.insert(&frames[insertAfter+1],frame);
				}
			}
		}

		for(j=0;j<msBone_GetRotationKeyCount(msbone);++j){
			msRotationKey *rotationKey=msBone_GetRotationKeyAt(msbone,j);

			if(rotationKey->fTime>maxTime){
				maxTime=rotationKey->fTime;
			}

			int insertAfter=-1;
			for(k=0;k<frames.size();++k){
				if(frames[k].time==rotationKey->fTime){
					convertMSVec3ToQuaternion(rotationKey->Rotation,frames[k].rotate,false);
					Math::preMul(frames[k].rotate,skeleton->bones[newi]->rotate);
					frames[k].rotateSet=true;
					break;
				}
				else if(frames[k].time>rotationKey->fTime && insertAfter==-1){
					insertAfter=k;
				}
			}
			if(k==frames.size()){
				Frame frame;
				frame.time=rotationKey->fTime;
				convertMSVec3ToQuaternion(rotationKey->Rotation,frame.rotate,false);
				Math::preMul(frame.rotate,skeleton->bones[newi]->rotate);
				frame.rotateSet=true;
				if(insertAfter==-1){
					frames.add(frame);
				}
				else{
					frames.insert(&frames[insertAfter+1],frame);
				}
			}
		}

		for(j=0;j<frames.size();++j){
			if(frames[j].translateSet==false){
				// First find any left frames
				int left=-1;
				for(k=j;k>=0;--k){
					if(frames[k].translateSet==true){
						left=k;
					}
				}

				// Then find any right frames
				int right=-1;
				for(k=j;k<frames.size();++k){
					if(frames[k].translateSet==true){
						right=k;
					}
				}

				if(left==-1 && right==-1){
				}
				else if(left==-1 && right!=-1){
					frames[j].translate=frames[right].translate;
				}
				else if(left!=-1 && right==-1){
					frames[j].translate=frames[left].translate;
				}
				else{
					float t=(frames[j].time-frames[left].time)/(frames[right].time-frames[left].time);
					Math::lerp(frames[j].translate,frames[left].translate,frames[right].translate,t);
				}
				frames[j].translateSet=true;
			}

			if(frames[j].rotateSet==false){
				// First find any left frames
				int left=-1;
				for(k=j;k>=0;--k){
					if(frames[k].rotateSet==true){
						left=k;
					}
				}

				// Then find any right frames
				int right=-1;
				for(k=j;k<frames.size();++k){
					if(frames[k].rotateSet==true){
						right=k;
					}
				}

				if(left==-1 && right==-1){
				}
				else if(left==-1 && right!=-1){
					frames[j].rotate=frames[right].rotate;
				}
				else if(left!=-1 && right==-1){
					frames[j].rotate=frames[left].rotate;
				}
				else{
					float t=(frames[j].time-frames[left].time)/(frames[right].time-frames[left].time);
					Math::slerp(frames[j].rotate,frames[left].rotate,frames[right].rotate,t);
					Math::normalize(frames[j].rotate);
				}
				frames[j].rotateSet=true;
			}
		}

		// Check for slerped quaternions that are > 180 degrees and fix them
		for(j=1;j<frames.size();++j){
			if(Math::dot(frames[j-1].rotate,frames[j].rotate)<0){
				frames[j].rotate*=-1;
			}
		}

		track->keyFrames.resize(frames.size());
		for(j=0;j<frames.size();++j){
			KeyFrame keyFrame;
			keyFrame.translate=frames[j].translate;
			keyFrame.rotate=frames[j].rotate;
			keyFrame.time=frames[j].time/fps;
			track->keyFrames[j]=keyFrame;
		}

		// Cut track if it has no keyframes
		if(track->keyFrames.size()==0){
			sequence->tracks.erase(sequence->tracks.begin()+sequence->tracks.size()-1);
		}
	}

	sequence->length=maxTime/fps;

	FileStream::ptr stream(new FileStream(name,FileStream::OpenFlags_WRITE|FileStream::OpenFlags_BINARY));
	XANMHandler::ptr handler(new XANMHandler());

	handler->save(sequence,stream);

	delete engine;

	return 0;
}

