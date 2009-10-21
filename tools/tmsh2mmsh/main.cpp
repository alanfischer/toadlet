#include <toadlet/egg/String.h>
#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Mesh.h>

#include <iostream>

#include "MicroMeshConverter.h"
#include "../shared/MeshPreprocess.h"

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::math;
using namespace toadlet::tadpole;

int main(int argc,char **argv){
	String arg;
	float scale=1;
	int forceBytes=0;
	float texCoordAdjust=0;
	float positionAdjust=0;
	int i,j;

	if(argc<2){
		std::cerr << "Usage: " << argv[0] << " \"mshFile\"" << std::endl;
		return 0;
	}
	for(i=1; i<argc; ++i){
        arg=argv[i];
        if(arg.substr(0,1)=="-"){
			j=1;
            if(arg.substr(j,1)=="h"){
                std::cout << "Options for " << argv[0] << ": " << std::endl;
                std::cout << "-h\t\thelp - print this message" << std::endl;
                std::cout << "-sscale\tscale - set scale amount" << std::endl;
				std::cout << "-b\t\tforce byte - use bytes for all packing" << std::endl;
				std::cout << "-w\t\tforce word - use words for all packing" << std::endl;
				std::cout << "-d\t\tforce dword - use dwords for all packing" << std::endl;
				std::cout << "-aadj\t\tadjust position - adjust vertex positions out along poly plane by this amount" << std::endl;
				std::cout << "-fadj\t\tadjust texcoord - adjust texcoords inward by this amount" << std::endl;
                return 1;
            }
			else if(arg.substr(j,1)=="s"){
				scale=arg.substr(j+1,arg.length()).toFloat();
			}
			else if(arg.substr(j,1)=="b"){
				forceBytes=1;
			}
			else if(arg.substr(j,1)=="w"){
				forceBytes=2;
			}
			else if(arg.substr(j,1)=="d"){
				forceBytes=4;
			}
			else if(arg.substr(j,1)=="a"){
				positionAdjust=arg.substr(j+1,arg.length()).toFloat();
			}
			else if(arg.substr(j,1)=="f"){
				texCoordAdjust=arg.substr(j+1,arg.length()).toFloat();
			}
        }
		else if(i!=argc-1){
			std::cout << "Error unknown command: " << arg << std::endl;
			return -1;
		}
	}

	String mshFileName=argv[argc-1];

	std::cout << "--tmsh2mmsh--" << std::endl;
	std::cout << "Converting " << mshFileName << std::endl;

	// Create a toadlet engine
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG_LOGGER,Logger::Level_DISABLED);
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG,Logger::Level_WARNING);
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_TADPOLE,Logger::Level_WARNING);
	Engine *engine=new Engine();

	// Load the mesh data
	Mesh::ptr mesh=engine->loadMesh(mshFileName);
	if(mesh==NULL){
		std::cout << "Error loading file: " << mshFileName << std::endl;
		return 0;
	}

	// Preprocess mesh
	if(positionAdjust!=0.0f){
		MeshPreprocess::adjustPositions(mesh,positionAdjust);
	}
	if(texCoordAdjust!=0.0f){
		MeshPreprocess::adjustTexCoords(mesh,texCoordAdjust);
	}

	// Prepare the output file
	int loc=mshFileName.rfind('.');
	String mmshFileName=mshFileName.substr(0,loc)+String(".mmsh");
	FileOutputStream fout(mmshFileName);

	// Write to .mmsh format
	MicroMeshConverter converter;
	bool result=converter.convertMesh(mesh,&fout,scale,forceBytes,true);

	if(result){
		std::cout << "complete" << std::endl;
		return 1;
	}
	else{
		std::cout << "incomplete" << std::endl;
		return -1;
	}
}

