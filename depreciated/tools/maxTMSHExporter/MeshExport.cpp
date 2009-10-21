#error "This is not updated to the most recent toadlet"

#include <toadlet/egg/System.h>
#include <toadlet/egg/math/MathTextSTDStream.h>
#include <toadlet/tadpole/resource/handler/XMSHHandler.h>
#include <toadlet/tadpole/resource/handler/XANMHandler.h>

#define TOADLET_USE_GLRENDERER
#include <toadlet/Win32ChooseLibs.h>

#include "MeshExport.h"
#include "TreeTraversalClasses.h"
#include "Preview.h"
#include "../../experimental/MAXUtilities/MAXUtilities.h"

#include <fstream>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::sg;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::tadpole::skeleton;
using namespace toadlet::tadpole::resource::handler;

// Needed for progress bar
DWORD WINAPI fn(LPVOID arg){
	return(0);
}

MeshExport::MeshExport(){
}


MeshExport::~MeshExport(){
}

int MeshExport::doExport(const TCHAR *name, ExpInterface *ei, Interface *itface, BOOL suppressPrompts = FALSE, DWORD options=0){
	ExpInterface *eiface = ei;
	mInterface = itface;
	mIndexCounter=0;

	// Initialize logger to store all output
	std::ostream *log=new std::ofstream((System::getTempPath()+"toadletMeshExportLog.txt").c_str());
	Logger::getInstance()->setAllOStreams(log);

	// Create a toadlet engine
	#ifndef TOADLET_DEBUG
		try{
	#endif
			mEngine=Engine::make();
			mEngine->setSceneManager(SceneManager::make(mEngine));
	#ifndef TOADLET_DEBUG
		}
		catch(std::exception &e){
			Logger::getInstance()->getLogStream(Logger::LEVEL_ERROR) << e.what() << std::endl;
		}
	#endif

	int i;
	for(i=0;i<itface->GetMapDirCount();++i){
		mEngine->getTextureManager()->addSearchDirectory(itface->GetMapDir(i));
		Logger::getInstance()->getLogStream(Logger::LEVEL_ALERT) << "SearchDir:" << itface->GetMapDir(i) << std::endl;
	}

	mEngine->getTextureManager()->addSearchDirectory(mOptions->mPreviewTexturePath);

	// New mesh for exporting
	mMesh = mEngine->getMeshDataManager()->load(new MeshData());
	
	// Sort through the model and index the nodes
	IndexNodes in;
	in.mMeshExport = this;
	in.mNodeIndex = 0;
	ei->theScene->EnumTree(&in);

	// Get nodes/skeleton 
	if(mOptions->mExportSkeleton){
		mSkeleton = mEngine->getSkeletonDataManager()->load(new SkeletonData());
		GetSkeleton gs;
		gs.mMeshExport = this;

		mInterface->ProgressStart(_T("Exporting skeleton..."), TRUE, fn, NULL);
		gs.mDumpTime = mInterface->GetAnimRange().Start();
		ei->theScene->EnumTree(&gs);
		if(mInterface->GetCancel()){
			mInterface->ProgressEnd();
			return FALSE;
		}
		mInterface->ProgressEnd();
	}
	
	// Get animations
	int numAnimations = 0;
	if(mOptions->mExportAnimations){
		mAnimation = mEngine->getAnimationDataManager()->load(new AnimationData());

		String simpleName=name;
		int s1=simpleName.rfind('/');
		int s2=simpleName.rfind('\\');
		if(s1<s2){s1=s2;}
		int e1=simpleName.rfind('.');
		mAnimation->simpleName=simpleName.substr(s1,e1-s1);

		GetAnimation ga;
		ga.mMeshExport = this;
		TimeValue animateStart = mInterface->GetAnimRange().Start();
		TimeValue animateEnd = mInterface->GetAnimRange().End();
		mAnimation->length = TicksToSec(animateEnd-animateStart);

		mInterface->ProgressStart(_T("Exporting animations..."), TRUE, fn, NULL);
		for(TimeValue tv=animateStart; tv<=animateEnd; tv+=GetTicksPerFrame()){
			ga.mDumpTime = tv;
			ei->theScene->EnumTree(&ga);
			++numAnimations;
			mInterface->ProgressUpdate((int)((float)(TicksToSec(tv)/mAnimation->length)*100.0f));
			if(mInterface->GetCancel()){
				mInterface->ProgressEnd();
				return FALSE;
			}
		}
		mInterface->ProgressEnd();
	}

	// Get the model data into storage containers
	if(mOptions->mExportGeometry){
		mInterface->ProgressStart(_T("Exporting geometry..."), TRUE, fn, NULL);
		GetGeometry gg;
		gg.mMeshExport = this;
		gg.mDumpTime = mInterface->GetAnimRange().Start();
		ei->theScene->EnumTree(&gg);
		if(mInterface->GetCancel()){
			mInterface->ProgressEnd();
			return FALSE;
		}
		mInterface->ProgressEnd();
	}

	// Sort all the model data and fill in the mesh object
	if(!constructMeshObject()){
		return FALSE;
	}

	// Write out the resulting mesh object
	String sname=name;
	toLowerCase(sname);
	if(sname.length()>=5 && sname.substr(sname.length()-5,5)==".xanm"){
		if(mMesh->getSkeletonData()==NULL || mMesh->getSkeletonData()->getNumAnimationDatas()==0){
			return FALSE;
		}

		XANMHandler *handler=(XANMHandler*)mEngine->getAnimationDataManager()->findHandlerForName("xanm");
		FileOutputStream fout(name);
		handler->save(mMesh->getSkeletonData()->getAnimationData(0),&fout,name);
	}
	else{
		XMSHHandler *handler=(XMSHHandler*)mEngine->getMeshDataManager()->findHandlerForName("xmsh");
		FileOutputStream fout(name);
		handler->save(mMesh,&fout,name);
	}

	// Print an info box
	StringStream msg;
	msg << "Size of Normal array = " << mNormals.size() << "\n"
		<< "Size of Vertex array = " << mVertexes.size() << "\n"
		<< "Size of Tex Coord array = " << mTextureVertexes.size() << "\n"
		<< "Number of Submeshes = " << mMesh->getNumSubMeshDatas() << "\n";
	if(mSkeleton){
		msg << "Number of Skeleton Bones = " << mSkeleton->getNumBoneDatas() << "\n";
	}
	if(mOptions->mExportAnimations){
		msg << "Number of Animation Steps = " << numAnimations << "\n";
	}
	MessageBox(GetActiveWindow(), msg.str().c_str(), "Statistics", MB_OK);

	// Open a preview window
	if(mOptions->mPreview){
		// Grab the lights for the preview
		GetLights gl;
		gl.mMeshExport = this;
		gl.mDumpTime = mInterface->GetAnimRange().Start();
		ei->theScene->EnumTree(&gl);

		Preview *preview=new Preview(mEngine,mMesh);
		preview->setLights(mLights);
		preview->start();
	}
	
	return TRUE;
}

bool MeshExport::skipNode(INode* node){
	Object *obj = node->GetObjectRef();
	String nodeName(node->GetName());

	// Don't care about cameras, lights, dummies, and helpers
	if(obj->SuperClassID()==CAMERA_CLASS_ID){
		return true;
	}
	if(obj->SuperClassID()==LIGHT_CLASS_ID){
		return true;
	}
	if(obj->SuperClassID()==IK_SOLVER_CLASS_ID){
		return true;
	}
	if(obj->SuperClassID()==HELPER_CLASS_ID){
		return true;
	}
	// These ignore some max objects
	if(nodeName.find("IK Chain")!=String::npos){
		return true;
	}
	if(nodeName==String("Bip01 Footsteps")){
		return true;
	}

	return false;
}

void MeshExport::setNodeIndex(INode *node, int index){
	mNodeIndexMap[node]=index;
}

int MeshExport::getNodeIndex(INode *node){
	NodeIndexMap::iterator it = mNodeIndexMap.find(node);
	if(it!=mNodeIndexMap.end()){
		return it->second;
	}
	else{
		return -777;
	}
}

bool MeshExport::constructMeshObject(){
	int i,j,k,l;
	MtlIndexBufferMap::iterator it;

	// Determine types of vertices we are dealing with
	int vt=VertexBuffer::VT_POSITION;
	if(mNormals.size()==mVertexes.size()){
		vt|=VertexBuffer::VT_NORMAL;
	}
	if(mTextureVertexes.size()==mVertexes.size()){
		vt|=VertexBuffer::VT_TEXCOORD1_DIM2;
	}
	if(mBoneIndexes.size()==mVertexes.size() && mBoneWeights.size()==mVertexes.size()){
		vt|=VertexBuffer::VT_BONE;
	}

	// Now repack all of the storage containers to remove duplicate elements
	mInterface->ProgressStart(_T("Compacting vertices..."), TRUE, fn, NULL);
	for(i=0;i<mVertexes.size();++i){
		for(j=i+1;j<mVertexes.size();++j){
			if(i!=j){
				bool same=true;
				if(same && vt&VertexBuffer::VT_POSITION && mVertexes[i]!=mVertexes[j]){
					same=false;
				}
				else if(same && vt&VertexBuffer::VT_NORMAL && mNormals[i]!=mNormals[j]){
					same=false;
				}
				else if(same && vt&VertexBuffer::VT_TEXCOORD1_DIM2 && mTextureVertexes[i]!=mTextureVertexes[j]){
					same=false;
				}
				else if(same && vt&VertexBuffer::VT_BONE){
					if(mBoneIndexes[i].size()!=mBoneIndexes[j].size()){
						same=false;
					}
					else{
						for(k=0;k<mBoneIndexes[i].size();k++){
							if(mBoneIndexes[i][k]!=mBoneIndexes[j][k] ||
								mBoneWeights[i][k]!=mBoneWeights[j][k]){
								same=false;
								break;
							}
						}
					}
				}

				if(same){
					for(it=mMtlIndexBufferMap.begin();it!=mMtlIndexBufferMap.end();++it){
						Collection<int> &ib=it->second;
						for(l=0;l<ib.size();++l){
							if(ib[l]==j){
								ib[l]=i;
							}
							else if(ib[l]>j){
								ib[l]--;
							}
						}
					}

					if(vt&VertexBuffer::VT_POSITION){
						mVertexes.erase(mVertexes.begin()+j);
					}

					if(vt&VertexBuffer::VT_NORMAL){
						mNormals.erase(mNormals.begin()+j);
					}

					if(vt&VertexBuffer::VT_TEXCOORD1_DIM2){
						mTextureVertexes.erase(mTextureVertexes.begin()+j);
					}

					if(vt&VertexBuffer::VT_BONE){
						mBoneIndexes.erase(mBoneIndexes.begin()+j);
						mBoneWeights.erase(mBoneWeights.begin()+j);
					}

					j--;
				}
			}
		}
		if(mInterface->GetCancel()){
			mInterface->ProgressEnd();
			return false;
		}
		mInterface->ProgressUpdate((int)(((float)i/(float)mVertexes.size())*100.0f));
	}
	mInterface->ProgressEnd();

	// Create a new submesh and toadlet material
	for(it=mMtlIndexBufferMap.begin();it!=mMtlIndexBufferMap.end();++it){
		SubMeshData *subMesh=new SubMeshData(mMesh);
		mMesh->addSubMeshData(subMesh);
		
		subMesh->setIndexBuffer(mEngine->getIndexBufferManager()->load(new IndexBuffer()));

		subMesh->getIndexBuffer()->resize(it->second.size());
		for(i=0;i<it->second.size();++i){
			subMesh->getIndexBuffer()->index(i)=it->second[i];
		}
		// Convert to toadlet material
		material::Material::Ptr mat;
		maxMaterialToToadletMaterial(mEngine,it->first,mInterface->GetTime(),mat);
		subMesh->addMaterial(mat);
	}

	// Now pack the vertices and normals into Buffers for the toadlet mesh
	mMesh->setVertexBuffer(mEngine->getVertexBufferManager()->load(new VertexBuffer()));
	mMesh->getVertexBuffer()->resize(VertexBuffer::VertexType(vt),mVertexes.size());

	if(vt&VertexBuffer::VT_POSITION){
		for(i=0; i<mVertexes.size(); ++i){
			mMesh->getVertexBuffer()->position(i)=mVertexes[i];
		}
	}

	if(vt&VertexBuffer::VT_NORMAL){
		for(i=0; i<mVertexes.size(); ++i){
			Math::normalize(mNormals[i]);
			mMesh->getVertexBuffer()->normal(i)=mNormals[i];
		}
	}

	if(vt&VertexBuffer::VT_TEXCOORD1_DIM2){
		for(i=0; i<mVertexes.size(); ++i){
			mMesh->getVertexBuffer()->texCoord2d(i,0)=mTextureVertexes[i];
		}
	}

	if(vt&VertexBuffer::VT_BONE){
		for(i=0; i<mVertexes.size(); ++i){
			BoneAssignmentCollection c;
			for(j=0;j<mBoneIndexes[i].size();j++){
				BoneAssignment b;
				b.index=mBoneIndexes[i][j];
				b.weight=mBoneWeights[i][j];
				c.push_back(b);
			}
			mMesh->getVertexBuffer()->bone(i)=c;
		}
	}

	if(mSkeleton){
		if(mAnimation){
			mAnimation->compile(); // TODO: Have the engine do this automagically
			mSkeleton->addAnimationData(mAnimation);
		}
		mSkeleton->compile(); // TODO: Have the engine do this automagically
		mMesh->setSkeletonData(mSkeleton);
	}

	return true;
}
