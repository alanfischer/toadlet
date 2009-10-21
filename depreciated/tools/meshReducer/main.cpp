#error "This is not updated to the most recent toadlet"

#include "ReducerGUI.h"
#include <toadlet/egg/String.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/io/FileInputStream.h>
#include <toadlet/egg/io/FileOutputStream.h>
#include "reducer/Reducer.h"

#define TOADLET_USE_GLRENDERER
#include <toadlet/Win32ChooseLibs.h>

#include <iostream>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::resource::handler;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::tadpole::sg;
using namespace reducer;

int main(int argc,char **argv){
	float percentReduction=-1.0f;
	StringStream ss;
	if(argc<2){
		std::cout << "For the command line version do:" << std::endl;
		std::cout << "Usage: meshReducer meshname %reduction" << std::endl;
		std::cout << "Or for the GUI version just do:" << std::endl;
		std::cout << "Usage: meshReducer meshname" << std::endl;
		return 0;
	}
	else if(argc>=3){
		ss.str(argv[2]);
		ss >> percentReduction;
		if(percentReduction<0 || percentReduction>100){
			std::cout << "%reduction must be between 0 and 100" << std::endl;
			return 0;
		}
	}
	String mshFileName=argv[1];

	// Create a toadlet engine
	Engine *engine=Engine::make();
	engine->setSceneManager(SceneManager::make(engine));

	// Load the mesh data
	MeshEntity::Ptr mesh;
	mesh=MeshEntity::make(engine);
	mesh->load(mshFileName);
	MeshData::Ptr meshData=mesh->getMeshData();

	// Add a light
	LightEntity::Ptr light;
	light=LightEntity::make(engine);
	light->setDiffuse(Vector4(0.5f,0.5f,0.5f,1));
	light->setType(Light::LT_DIRECTION);
	light->setDirection(Vector3(0,0,1));
	engine->getSceneManager()->attach(light);

	SceneStates *states=engine->getSceneManager()->getSceneStates();
	states->ambientLight=Vector4(0.2,0.2,0.2,1);

	// Percentage given so work in command line mode
	if(percentReduction!=-1.0f){
		// Create output file
		int loc=mshFileName.find_last_of(".");
		String reducedFileName=mshFileName.substr(0,loc)+String("_reduced_")+ss.str()+String(".xmsh");
		FileOutputStream fout(reducedFileName);

		// Reduce the mesh
		Reducer meshReducer(engine);
		meshReducer.computeProgressiveMesh(meshData);
		meshReducer.doProgressiveMesh(0.01*percentReduction);

		Handler<MeshData> *handler=engine->getMeshDataManager()->findHandlerForName(".xmsh");
		handler->save(meshData,&fout,reducedFileName);

		return 1;
	}

	// Otherwise begin the GUI version
	else{
		int i;
		meshData->getVertexBuffer()->setStatic(false);
		for(i=0;i<meshData->getNumSubMeshDatas();i++){
			meshData->getSubMeshData(i)->getIndexBuffer()->setStatic(false);
		}

		// Lauch the viewer
		ReducerGUI *reducerGUI=new ReducerGUI(engine,mesh,mshFileName);
		reducerGUI->start();

		while(reducerGUI->running()){
			toadlet::egg::System::msleep(100);
		}

		delete reducerGUI;

		return 1;
	}
}

