#include "Viewer.h"
#include <iostream>
#include <stdlib.h>

using namespace toadlet::egg;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::animation;
using namespace toadlet::tadpole::node;

int main(int argc,char **argv){
	String arg;
	int i=0;

	if(argc<2){
		std::cout << "Usage: " << argv[0] << " meshName [animationNumber]" << std::endl;
		return 0;
	}
	for(i=1; i<argc; ++i){
		arg=argv[i];
		if(arg.substr(0,1)=="-"){
			for(int j=1; j<arg.length(); j++){
				if(arg.substr(j,1)=="h"){
					std::cout << "Options for " << argv[0] << ": "<< std::endl;
					std::cout << "-h \thelp - print this message"<< std::endl;
					return 1;
				}
			}
		}
	}

	Viewer *viewer=new Viewer();
	viewer->setSize(640,480);
	viewer->setFullscreen(false);
	viewer->create();

	Engine *engine=viewer->getEngine();

	String meshName=argv[1];
	int loc=meshName.rfind('\\');
	if(loc!=std::string::npos){
		engine->setDirectory(meshName.substr(0,loc));
		meshName=meshName.substr(loc+1,meshName.length());
	}

	Mesh::ptr mesh=engine->getMeshManager()->findMesh(meshName);
	if(mesh==NULL){
		std::cout << "Error loading " << (const char*)meshName << std::endl;

		viewer->destroy();
		delete viewer;

		return -1;
	}

	MeshNode::ptr meshNode=engine->createNodeType(MeshNode::type(),viewer->getScene());
	meshNode->setMesh(mesh);

	if(mesh->skeleton!=NULL){
		std::cout << "Has skeleton" << std::endl;

		meshNode->getSkeleton()->setRenderable(true);

		int numSequences=mesh->skeleton->sequences.size();
		std::cout << "Number of sequences:" << numSequences << std::endl;

		if(argc>=3 && numSequences>0){
			int sequence=atoi(argv[2]);
			if(sequence>=0 && sequence<numSequences){
				meshNode->getController()->setSequenceIndex(sequence);
				meshNode->getController()->setCycling(Controller::Cycling_LOOP);
				meshNode->getController()->setTimeScale(Math::ONE);
				meshNode->getController()->start();
			}
			else{
				std::cout << "Invalid sequence number:" << sequence << std::endl;
			}
		}
	}

	viewer->start(meshNode);

	delete viewer;

	return 1;
}

