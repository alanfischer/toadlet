#error "This is not updated to the most recent toadlet"

#include "BSPViewer.h"

#include <toadlet/egg/io/FileInputStream.h>
#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/Win32ChooseLibs.h>
#include "../../experimental/bsp/Compiler.h"
#include "../../experimental/bsp/MeshDataConverter.h"
#include "../../experimental/bsp/CollisionTreeHandler.h"

using namespace toadlet::egg::io;
using namespace toadlet::tadpole::mesh;
using namespace bsp;

int main(int argc,char **argv){
	String arg;
	float scale=1;
	float epsilon=0.01;
	int version=1;
	int i,j;

	if(argc<2){
		std::cerr << "Usage: bspCompiler \"mshFile\"" << std::endl;
		return 0;
	}
	for(i=1; i<argc; ++i){
        arg=argv[i];
        if(arg.substr(0,1)=="-"){
			j=1;
            if(arg.substr(j,1)=="h"){
                std::cout << "Options for bspCompiler: " << std::endl;
                std::cout << "-h\thelp - print this message" << std::endl;
				std::cout << "-sscale\tscale of the mesh" << std::endl;
				std::cout << "-eepsilon\tcompiler epsilon" << std::endl;
				std::cout << "-2\twrite a version 2 collision tree" << std::endl;
				std::cout << std::endl;
				std::cout << "Controls for the viewer: " << std::endl;
				std::cout << "\tw - forward" << std::endl;
				std::cout << "\ts - backward" << std::endl;
				std::cout << "\ta - left" << std::endl;
				std::cout << "\td - right" << std::endl;
				std::cout << "\tp - show next plane" << std::endl;
                return 1;
            }
			else if(arg.substr(j,1)=="s"){
				scale=arg.substr(j+1,arg.find(' ',j+1)-(j+1)).toFloat();
			}
			else if(arg.substr(j,1)=="e"){
				epsilon=arg.substr(j+1,arg.find(' ',j+1)-(j+1)).toFloat();
			}
			else if(arg.substr(j,1)=="2"){
				version=2;
			}
        }
	}

	String mshFileName=argv[argc-1];

	Engine *engine=Engine::make();
	MeshData::Ptr meshData=engine->getMeshDataManager()->load(mshFileName);
	
	if(meshData==NULL){
		std::cout << "Mesh " << mshFileName << " not found" << std::endl;
		return -1;
	}

	// Downscale data
	for(i=0;i<meshData->getVertexBuffer()->getNumVertexes();++i){
		meshData->getVertexBuffer()->position(i)*=scale;
	}

	InputData input;
	OutputData output;
	MeshDataConverter::convertMeshDataToInputData(meshData,&input);

	{
		// Add in additional boundaries
		AABox bound=calculateTightBox(meshData->getVertexBuffer()->position(0).getData(),meshData->getVertexBuffer()->getNumVertexes(),meshData->getVertexBuffer()->getElementSize()/sizeof(float));
		int start=input.vertexes.size();
		input.vertexes.push_back(Vector3(bound.mins.x,bound.mins.y,bound.mins.z));
		input.vertexes.push_back(Vector3(bound.mins.x,bound.maxs.y,bound.mins.z));
		input.vertexes.push_back(Vector3(bound.maxs.x,bound.maxs.y,bound.mins.z));
		input.vertexes.push_back(Vector3(bound.maxs.x,bound.mins.y,bound.mins.z));
		input.vertexes.push_back(Vector3(bound.mins.x,bound.mins.y,bound.maxs.z+10));
		input.vertexes.push_back(Vector3(bound.mins.x,bound.maxs.y,bound.maxs.z+10));
		input.vertexes.push_back(Vector3(bound.maxs.x,bound.maxs.y,bound.maxs.z+10));
		input.vertexes.push_back(Vector3(bound.maxs.x,bound.mins.y,bound.maxs.z+10));

		bsp::Polygon west;
		west.indexes.push_back(start+0);
		west.indexes.push_back(start+1);
		west.indexes.push_back(start+5);
		west.indexes.push_back(start+4);
		input.polygons.push_back(west);

		bsp::Polygon north;
		north.indexes.push_back(start+1);
		north.indexes.push_back(start+2);
		north.indexes.push_back(start+6);
		north.indexes.push_back(start+5);
		input.polygons.push_back(north);

		bsp::Polygon east;
		east.indexes.push_back(start+2);
		east.indexes.push_back(start+3);
		east.indexes.push_back(start+7);
		east.indexes.push_back(start+6);
		input.polygons.push_back(east);

		bsp::Polygon south;
		south.indexes.push_back(start+3);
		south.indexes.push_back(start+0);
		south.indexes.push_back(start+4);
		south.indexes.push_back(start+7);
		input.polygons.push_back(south);
	}

	Compiler compiler;
	compiler.setEpsilon(epsilon);
	compiler.compile(&input,&output);

	delete engine;
	engine=NULL;

	int loc=mshFileName.find_last_of(".");
	String cltFileName=mshFileName.substr(0,loc)+toadlet::egg::String(".clt");

	Node *nodes=NULL;
	// Convert to collision tree format
	if(version==1){
		FileOutputStream out(cltFileName);
		CollisionTreeHandler::saveCollisionTree(&output,&out);
		out.close();

		FileInputStream in(cltFileName);
		nodes=CollisionTreeHandler::loadCollisionTree(&in);
		in.close();
	}
	else{
		std::cout << "Version error" << std::endl;
	}

	BSPViewer *viewer=new BSPViewer();
	viewer->create();
	viewer->setCompilerData(input,nodes,output);
	viewer->setEpsilon(0.001f); // We use a fixed fine epsilon for simulator sanity
	viewer->run();

	delete viewer;

	delete[] nodes;

	return 1;
}
