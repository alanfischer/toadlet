#include "MAPExport.h"
#include "TreeTraversalClasses.h"

#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/material/StandardMaterial.h>
#include <toadlet/Win32ChooseLibs.h>

#include "../csg/MAPHandler.h"
#include "../MAXUtilities/MAXUtilities.h"

#include <fstream>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::material;
using namespace csg;

// Needed for progress bar
DWORD WINAPI fn(LPVOID arg){
	return(0);
}

MAPExport::MAPExport(){
	mInvalidObjectCount=0;
	mEngine=toadlet::tadpole::Engine::make();
}


MAPExport::~MAPExport(){
	delete mEngine;
}

int MAPExport::doExport(const TCHAR *name, ExpInterface *ei, Interface *itface, BOOL suppressPrompts = FALSE, DWORD options=0){
	ExpInterface *eiface = ei;
	mInterface = itface;
	mIndexCounter=0;

	// Initialize logger to store all output
	std::ostream *log=new std::ofstream((System::getTempPath()+"MAPExportLog.txt").c_str());
	Logger::getInstance()->setAllOStreams(log);

	mEngine->getTextureManager()->addSearchDirectory(mOptions->mTexturePath);

	// Get the model data into storage containers
	{
		mInterface->ProgressStart(_T("Exporting brushes..."), TRUE, fn, NULL);
		GetGeometry gg;
		gg.mExport = this;
		gg.mDumpTime = mInterface->GetAnimRange().Start();
		ei->theScene->EnumTree(&gg);
		if(mInterface->GetCancel()){
			mInterface->ProgressEnd();
			return FALSE;
		}
		mInterface->ProgressEnd();
	}

	csg::Map map;

	MapEntity world;
	world.brushCollection=mBrushCollection;
	map.mapEntities.push_back(world);

	int i;
	for(i=0;i<mLights.size();++i){
		MapEntity entity;

		entity.keyValues.push_back(std::pair<String,String>("classname","light"));

		{
			StringStream ss;
			ss << mLights[i].getPosition().x << " " << mLights[i].getPosition().y << " " << mLights[i].getPosition().z;
			entity.keyValues.push_back(std::pair<String,String>("origin",ss.str()));
		}

		{
			StringStream ss;
			ss << mLights[i].getDiffuse().x*255 << " " << mLights[i].getDiffuse().y*255 << " " << mLights[i].getDiffuse().z*255 << " 255";
			entity.keyValues.push_back(std::pair<String,String>("_light",ss.str()));
		}

		{
			StringStream ss;
			ss << mLights[i].getLinearAttenuation();
			entity.keyValues.push_back(std::pair<String,String>("_fade",ss.str()));
		}

		map.mapEntities.push_back(entity);
	}

	String sname=(char*)name;
	MAPHandler handler;
	FileOutputStream fout(sname);
	handler.save(&map,&fout,sname,mOptions->mInvertYTexCoord);

	// Print an info box
	StringStream msg;
	msg << "Number of brushes = " << mBrushCollection.brushes.size() << std::endl;
	msg << "Number of invalid objects = " << mInvalidObjectCount << std::endl;
	msg << "Number of unknown objects = " << mNonBrushObjects.size() << std::endl;
	msg << "Number of missing maps = " << mMissingMaps.size() << std::endl;
	MessageBox(GetActiveWindow(), msg.str().c_str(), "Statistics", MB_OK);

	InvalidObjectMap::iterator it;
	for(it=mInvalidObjectMap.begin();it!=mInvalidObjectMap.end();++it){
		if(it->second.size()>0){
			StringStream msg;
			for(i=0;i<it->second.size();++i){
				msg << "Object #" << i << ":" << it->second[i] << std::endl;
			}
			String title;
			switch(it->first){
				case(Brush::CR_ERROR_NO_THICKNESS):
					title="No thickness errors";
				break;
				case(Brush::CR_ERROR_INVALID_NORMAL):
					title="Invalid normal errors";
				break;
				case(Brush::CR_ERROR_INVALID_POLYGON):
					title="Invalid polygon errors";
				break;
				case(Brush::CR_ERROR_INFINITE_POLYGON):
					title="Infinite polygon errors";
				break;
				default:
					title="Unknown errors";
				break;
			}

			MessageBox(GetActiveWindow(),msg.str().c_str(),title.c_str(),MB_OK);
		}
	}

	if(mMissingMaps.size()>0){
		StringStream msg;
		for(i=0;i<mMissingMaps.size();++i){
			msg << "Map #" << i << ":" << mMissingMaps[i] << std::endl;
		}

		MessageBox(GetActiveWindow(),msg.str().c_str(),"Missing maps",MB_OK);
	}

	return TRUE;
}

void MAPExport::setNodeIndex(INode *node, int index){
	mNodeIndexMap[node]=index;
}

int MAPExport::getNodeIndex(INode *node){
	NodeIndexMap::iterator it = mNodeIndexMap.find(node);
	if(it!=mNodeIndexMap.end()){
		return it->second;
	}
	else{
		return -777;
	}
}

void MAPExport::addBrush(Brush *brush){
	mBrushCollection.brushes.push_back(brush);
}

void MAPExport::addLight(const toadlet::peeper::Light &light){
	mLights.push_back(light);
}

int MAPExport::getMaterialIndexFromMtl(Mtl *mtl){
	MtlToMaterialIndexMap::iterator it=mMtlToMaterialIndexMap.find(mtl);
	if(it!=mMtlToMaterialIndexMap.end()){
		return it->second;
	}
	else{
		toadlet::tadpole::material::Material::Ptr mat;

		maxMaterialToToadletMaterial(mEngine,mtl,mInterface->GetTime(),mat);

		toadlet::tadpole::material::StandardMaterial::Ptr smat=shared_dynamic_cast<toadlet::tadpole::material::StandardMaterial>(mat);
		if(smat!=NULL && smat->getMap(StandardMaterial::MAP_DIFFUSE)==NULL){
			mMissingMaps.push_back(smat->getMapName(StandardMaterial::MAP_DIFFUSE));
		}

		mBrushCollection.materials.push_back(mat);
		mMtlToMaterialIndexMap[mtl]=mBrushCollection.materials.size()-1;

		return mBrushCollection.materials.size()-1;
	}
}

void MAPExport::reportInvalidObject(const String &name,Brush::CompileResult result){
	mInvalidObjectMap[result].push_back(name);
	mInvalidObjectCount++;
}

void MAPExport::reportNonBrushObject(const String &name){
	mNonBrushObjects.push_back(name);
}

Vector3 MAPExport::snapToGrid(const Vector3 &point){
	return point;
	float spacing=0.25f;
	Vector3 result;
	result.x=round(point.x/spacing)*spacing;
	result.y=round(point.y/spacing)*spacing;
	result.z=round(point.z/spacing)*spacing;
	return result;
}
