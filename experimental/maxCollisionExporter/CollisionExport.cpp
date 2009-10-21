#include "CollisionExport.h"
#include "TreeTraversalClasses.h"

#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/Win32ChooseLibs.h>

#include <fstream>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;

// Needed for progress bar
DWORD WINAPI fn(LPVOID arg){
	return(0);
}

CollisionExport::CollisionExport(){
}


CollisionExport::~CollisionExport(){
}


int CollisionExport::doExport(const TCHAR *name, ExpInterface *ei, Interface *itface, BOOL suppressPrompts = FALSE, DWORD options=0){
	ExpInterface *eiface = ei;
	mInterface = itface;
	mIndexCounter=0;

	// Initialize logger to store all output
	std::ostream *log=new std::ofstream((System::getTempPath()+"collisionExportLog.txt").c_str());
	Logger::getInstance()->setAllOStreams(log);

	// Sort through the model and index the nodes
	IndexNodes in;
	in.mCollisionExport = this;
	in.mNodeIndex = 0;
	ei->theScene->EnumTree(&in);

	// Get the model data into storage containers
	{
		mInterface->ProgressStart(_T("Exporting solids..."), TRUE, fn, NULL);
		GetGeometry gg;
		gg.mCollisionExport = this;
		gg.mDumpTime = mInterface->GetAnimRange().Start();
		ei->theScene->EnumTree(&gg);
		if(mInterface->GetCancel()){
			mInterface->ProgressEnd();
			return FALSE;
		}
		mInterface->ProgressEnd();
	}

	// Write out the resulting data
	String sname=(char*)name;
	XCOLHandler handler;
	FileOutputStream fout(sname);
	handler.save(&mShapeCollection,&fout,sname);

	// Print an info box
	StringStream msg;
	msg << "Number of shapes = " << mShapeCollection.getNumShapeDatas() << "\n";
	MessageBox(GetActiveWindow(), msg.str().c_str(), "Statistics", MB_OK);

	return TRUE;
}


bool CollisionExport::skipNode(INode* node){
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


void CollisionExport::setNodeIndex(INode *node, int index){
	mNodeIndexMap[node]=index;
}


int CollisionExport::getNodeIndex(INode *node){
	NodeIndexMap::iterator it = mNodeIndexMap.find(node);
	if(it!=mNodeIndexMap.end()){
		return it->second;
	}
	else{
		return -777;
	}
}

