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

	PlatformApplication::ptr app=new PlatformApplication();
	app->setSize(640,480);
	app->setFullscreen(false);
	app->create("d3d10","al");

	Viewer::ptr viewer=new Viewer(app);
	app->setApplet(viewer);
	Engine *engine=app->getEngine();
	Scene *scene=viewer->getScene();

	String meshName=argv[1];
	int loc=meshName.rfind('\\');
	if(loc!=std::string::npos){
		engine->getArchiveManager()->addDirectory(meshName.substr(0,loc));
		meshName=meshName.substr(loc+1,meshName.length());
	}

	Mesh::ptr mesh=engine->getMeshManager()->findMesh(meshName);
	if(mesh==NULL){
		std::cout << "Error loading " << (const char*)meshName << std::endl;

		return -1;
	}

	Node::ptr node=new Node(scene);
	MeshComponent::ptr meshComponent=new MeshComponent(engine);
	meshComponent->setMesh(mesh);
	node->attach(meshComponent);
	if(meshComponent->getSkeleton()!=NULL){
		std::cout << "Has skeleton" << std::endl;

		meshComponent->getSkeleton()->setRenderSkeleton(true);

		int numSequences=mesh->getSkeleton()->getNumSequences();
		std::cout << "Number of sequences:" << numSequences << std::endl;

		if(argc>=3 && numSequences>0){
			int sequence=atoi(argv[2]);
			if(sequence>=0 && sequence<numSequences){
				AnimationAction::ptr animation=new AnimationAction();
				animation->attach(new MeshAnimation(meshComponent,sequence));
				animation->setCycling(AnimationAction::Cycling_LOOP);
				animation->start();
				node->attach(new ActionComponent("animation",animation));
			}
			else{
				std::cout << "Invalid sequence number:" << sequence << std::endl;
			}
		}
	}

	viewer->setNode(node);

	app->start();
	app->destroy();

	return 1;
}

