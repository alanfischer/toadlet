#include "stdafx.h"
// CreatePlugIn has a different signature in the msLib, so redefine it
#define CreatePlugIn MsCreatePlugIn
#include "msPlugInImpl.h"
#undef CreatePlugIn
#include "../shared/msConversion.h"
#include <toadlet/tadpole.h>
#include <toadlet/peeper.h>

#pragma warning(disable:4996)

using namespace toadlet::egg::io;
using namespace toadlet::peeper;

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
	Mesh::VertexBoneAssignmentList bones;

	bool operator==(const Vertex &v) const{
		if(bones.size()!=v.bones.size()) return false;

		for(int i=0;i<bones.size();++i){
			if(bones[i].bone==v.bones[i].bone || bones[i].weight!=v.bones[i].weight) return false;
		}

		return v.position==position && v.normal==normal && v.texCoord==texCoord;
	}
};

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

	HINSTANCE hInstance=GetModuleHandle(0);

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

	DestroyWindow(hwndProgress);

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
				emptyBones.push_back(i);
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
	Mesh::ptr mesh=new Mesh();

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

		int triangleCount=msMesh_GetTriangleCount(msmesh);
		for(j=0;j<triangleCount;++j){
			progressUpdated((float)j/(float)triangleCount);

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
				v.bones.push_back(Mesh::VertexBoneAssignment(bone,1));

				int l;
				for(l=0;l<vertexes.size();++l){
					if(vertexes[l]==v)break;
				}

				if(l==vertexes.size()){
					vertexes.push_back(v);
					indexes[i].push_back(vertexes.size()-1);
				}
				else{
					indexes[i].push_back(l);
				}
			}
		}
	}

	VertexFormat::ptr vertexFormat=new BackableVertexFormat();
	vertexFormat->create();
	vertexFormat->addElement(VertexFormat::Semantic_POSITION,0,VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_3);
	vertexFormat->addElement(VertexFormat::Semantic_NORMAL,0,VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_3);
	vertexFormat->addElement(VertexFormat::Semantic_TEXCOORD,0,VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_2);

	VertexBuffer::ptr vertexBuffer=new BackableBuffer();
	vertexBuffer->create(Buffer::Usage_BIT_STATIC,Buffer::Access_READ_WRITE,vertexFormat,vertexes.size());
	mesh->setStaticVertexData(new VertexData(vertexBuffer));

	Collection<Mesh::VertexBoneAssignmentList> vbas;

	if(bones){
		vbas.resize(vertexes.size());
	}

	{
		VertexBufferAccessor vba(vertexBuffer);
		for(i=0;i<vertexes.size();++i){
			vba.set3(i,0,vertexes[i].position);
			vba.set3(i,1,vertexes[i].normal);
			vba.set2(i,2,vertexes[i].texCoord);
			if(bones){
				vbas[i]=vertexes[i].bones;
			}
		}
	}

	mesh->setVertexBoneAssignments(vbas);

	int indexCount=indexes.size();
    for(i=0;i<indexCount;++i){
		progressUpdated((float)i/(float)indexCount);

		Mesh::SubMesh::ptr sub=new Mesh::SubMesh();
		mesh->addSubMesh(sub);

		sub->setName(meshNames[i]);

		IndexBuffer::ptr indexBuffer=new BackableBuffer();
		indexBuffer->create(Buffer::Usage_BIT_STATIC,Buffer::Access_READ_WRITE,IndexBuffer::IndexFormat_UINT16,indexes[i].size());
		sub->indexData=new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,indexes[i].size());

		{
			IndexBufferAccessor iba(indexBuffer);
			for(j=0;j<indexes[i].size();++j){
				iba.set(j,indexes[i][j]);
			}
		}

		/// @todo: Store the materials outside of the submesh, so I can reference materials multiple times
		int materialIndex=materialIndexes[i];
		if(materialIndex>=0){
			msMaterial *msmat=msModel_GetMaterialAt(pModel,materialIndex);

			Material::ptr material=engine->createDiffuseMaterial(NULL);

			char name[256];
			msMaterial_GetName(msmat,name,256);
			material->setName(name);

			char texture[1024];
			msMaterial_GetDiffuseTexture(msmat,texture,1024);
			if(strlen(texture)>0){
//				material->getPass()->setTextureName(texture);
			}

			MaterialState materialState;
			{
				materialState.light=true;
				msMaterial_GetAmbient(msmat,materialState.ambient.getData());
				msMaterial_GetDiffuse(msmat,materialState.diffuse.getData());
				msMaterial_GetSpecular(msmat,materialState.specular.getData());
				msMaterial_GetEmissive(msmat,materialState.emissive.getData());
				materialState.shininess=msMaterial_GetShininess(msmat);
				float transparency=msMaterial_GetTransparency(msmat);
				materialState.ambient.w=materialState.diffuse.w=materialState.specular.w=transparency;
			}
			material->getPass()->setMaterialState(materialState);

			sub->material=material;
		}
		else{
			MessageBox(NULL,"Mesh has no material, exporting empty material","Warning",NULL);

			sub->material=engine->getMaterialManager()->createMaterial();
		}
	}

	if(bones){
		mesh->setSkeleton(buildSkeleton(pModel,emptyBones));
	}

	FileStream::ptr stream=new FileStream(name,FileStream::Open_WRITE_BINARY);
	XMSHStreamer::ptr streamer=new XMSHStreamer(engine);
	 
	streamer->save(stream,mesh,NULL);

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
cPlugIn::exportAnimation(msModel *pModel,const String &name){
	Collection<int> emptyBones;
	findEmptyBones(pModel,emptyBones);

	Sequence::ptr sequence=new Sequence();

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
	int boneCount=msModel_GetBoneCount(pModel);
	for(i=0;i<boneCount;++i){
		progressUpdated((float)i/(float)boneCount);

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

		Track::ptr track=new Track(engine->getVertexFormats().POSITION_ROTATE_SCALE);
		sequence->addTrack(track);

		track->setIndex(newi);

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
					Math::setMatrix3x3FromQuaternion(rotate,skeleton->getBone(newi)->rotate);
					Math::mul(frames[k].translate,rotate);
					Math::add(frames[k].translate,skeleton->getBone(newi)->translate);
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
				Math::setMatrix3x3FromQuaternion(rotate,skeleton->getBone(newi)->rotate);
				Math::mul(frame.translate,rotate);
				Math::add(frame.translate,skeleton->getBone(newi)->translate);
				frame.translateSet=true;
				if(insertAfter==-1){
					frames.push_back(frame);
				}
				else{
					frames.insert(frames.begin()+insertAfter+1,frame);
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
					Math::preMul(frames[k].rotate,skeleton->getBone(newi)->rotate);
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
				Math::preMul(frame.rotate,skeleton->getBone(newi)->rotate);
				frame.rotateSet=true;
				if(insertAfter==-1){
					frames.push_back(frame);
				}
				else{
					frames.insert(frames.begin()+insertAfter+1,frame);
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
				frames[j].rotate.x*=-1;
				frames[j].rotate.y*=-1;
				frames[j].rotate.z*=-1;
				frames[j].rotate.w*=-1;
			}
		}

		VertexBufferAccessor &vba=track->getAccessor();
		for(j=0;j<frames.size();++j){
			track->addKeyFrame(frames[j].time/fps);

			vba.set3(j,0,frames[j].translate);
			vba.set4(j,1,frames[j].rotate);
		}

		// Cut track if it has no keyframes
		if(track->getNumKeyFrames()==0){
			sequence->removeTrack(sequence->getNumTracks()-1);
		}
	}

	sequence->setLength(maxTime/fps);

	FileStream::ptr stream=new FileStream(name,FileStream::Open_WRITE_BINARY);
	XANMStreamer::ptr streamer=new XANMStreamer(engine);

	streamer->save(stream,sequence,NULL);

	return 0;
}

void 
cPlugIn::progressUpdated(float amount){
}
