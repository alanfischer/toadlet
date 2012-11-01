#include "M3GConverter.h"
#include "../shared/MeshPreprocess.h"
#include <iostream>

int main(int argc,char **argv){
	String arg;
	float scale=1;
	float texCoordAdjust=0;
	float positionAdjust=0;
	int forceBytes=0;
	bool viewable=false;
	int animation=-1;
	int i,j;

	if(argc<2){
		std::cerr << "Usage: msh2m3g \"mshFile\"" << std::endl;
		return 0;
	}
	for(i=1; i<argc; ++i){
        arg=argv[i];
        if(arg.substr(0,1)=="-"){
			j=1;
            if(arg.substr(j,1)=="h"){
                std::cout << "Options for msh2m3g: " << std::endl;
                std::cout << "-h\t\thelp - print this message" << std::endl;
                std::cout << "-sscale\tscale - set scale amount" << std::endl;
				std::cout << "-aadj\t\tadjust position - adjust vertex positions out along poly plane by this amount" << std::endl;
				std::cout << "-fadj\t\tadjust texcoord - adjust texcoords inward by this amount" << std::endl;
				std::cout << "-b\t\tforce byte - use bytes for all packing" << std::endl;
				std::cout << "-w\t\tforce word - use words for all packing" << std::endl;
				std::cout << "-d\t\tforce dword - use dwords for all packing" << std::endl;
				std::cout << "-v\t\tviewable - for testing, builds a complete m3g scenegraph" << std::endl;
				std::cout << "-ganm\t\tweight - for testing, set the weight of this animation to 1" << std::endl;
                return 1;
            }
			else if(arg.substr(j,1)=="s"){
				scale=arg.substr(j+1,arg.length()).toFloat();
			}
			else if(arg.substr(j,1)=="a"){
				positionAdjust=arg.substr(j+1,arg.length()).toFloat();
			}
			else if(arg.substr(j,1)=="f"){
				texCoordAdjust=arg.substr(j+1,arg.length()).toFloat();
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
			else if(arg.substr(j,1)=="v"){
				viewable=true;
			}
			else if(arg.substr(j,1)=="g"){
				animation=arg.substr(j+1,arg.length()).toInt32();
			}
        }
		else if(i!=argc-1){
			std::cout << "Error unknown command: " << arg.c_str() << std::endl;
			return -1;
		}
	}

	String mshFileName=argv[argc-1];

	std::cout << "--tmsh2m3g--" << std::endl;
	std::cout << "Converting " << (const char*)mshFileName << std::endl;

	// Create a toadlet engine
	Log::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG_LOGGER,Logger::Level_DISABLED);
	Log::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG,Logger::Level_WARNING);
	Log::getInstance()->setCategoryReportingLevel(Categories::TOADLET_TADPOLE,Logger::Level_WARNING);
	Engine::ptr engine=new Engine(0,0,Engine::Option_BIT_FIXEDBACKABLE|Engine::Option_BIT_SHADERBACKABLE);
	engine->installHandlers();

	// Load the mesh data
	Mesh::ptr mesh=shared_static_cast<Mesh>(engine->getMeshManager()->find(mshFileName));
	if(mesh==NULL){
		std::cout << "Error loading file: " << (const char*)mshFileName << std::endl;
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
	String m3gFileName=mshFileName.substr(0,loc)+String(".m3g");
	FileStream::ptr stream(new FileStream(m3gFileName,FileStream::Open_WRITE_BINARY));

	// Write to .m3g format
	M3GConverter converter;
	bool result=converter.convertMesh(mesh,stream,scale,forceBytes,true,viewable,animation);

	if(result){
		std::cout << "complete" << std::endl;
		return 1;
	}
	else{
		std::cout << "incomplete" << std::endl;
		return -1;
	}
}

