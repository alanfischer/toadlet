#include <toadlet/toadlet.h>
#include <toadlet/tadpole/plugins/XMSHStreamer.h>

#include <iostream>

#include "MeshOptimizer.h"

int main(int argc,char **argv){
	String arg;
	int i,j;

	if(argc<2){
		std::cerr << "Usage: " << argv[0] << " \"mshFile\"" << std::endl;
		return 0;
	}
	for(i=1; i<argc; ++i){
        arg=argv[i];
        if(arg.substr(0,1)=="-"){
            for(j=1; j<arg.length(); j++){
                if(arg.substr(j,1)=="h"){
                    std::cout << "Options for " << argv[0] << ": " << std::endl;
                    std::cout << "-h\t\thelp - print this message" << std::endl;
                    return 1;
                }
            }
        }
		else if(i!=argc-1){
			std::cout << "Error unknown command: " << (const char*)arg << std::endl;
			return 0;
		}
	}

	String mshFileName=argv[argc-1];

	std::cout << "--meshOptimizer--" << std::endl;
	std::cout << "Optimizing " << (const char*)mshFileName << std::endl;

	// Create a toadlet engine
	Log::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG,Logger::Level_WARNING);
	Log::getInstance()->setCategoryReportingLevel(Categories::TOADLET_TADPOLE,Logger::Level_WARNING);
	Engine::ptr engine=new Engine();
	engine->setBackableRenderCaps(engine->getMaximumRenderCaps());
	engine->installHandlers();

	// Load the mesh data
	Mesh::ptr mesh=shared_static_cast<Mesh>(engine->getMeshManager()->find(mshFileName));
	if(mesh==NULL){
		std::cout << "Error loading file: " << (const char*)mshFileName << std::endl;
		return 0;
	}

	MeshOptimizer optimizer;
	bool result=optimizer.optimizeMesh(mesh,engine);

	FileStream::ptr stream=new FileStream(mshFileName,FileStream::Open_WRITE_BINARY);
	if(stream->closed()){
		std::cout << "Error opening " << (const char*)mshFileName << std::endl;
		return 0;
	}
	XMSHStreamer::ptr streamer=new XMSHStreamer(NULL);
	streamer->save(stream,mesh,NULL);

	if(result){
		std::cout << "complete" << std::endl;
		return 1;
	}
	else{
		std::cout << "incomplete" << std::endl;
		return -1;
	}
}

