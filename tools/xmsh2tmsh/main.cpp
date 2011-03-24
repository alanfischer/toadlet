#include <toadlet/toadlet.h>
#include <toadlet/tadpole/handler/TMSHHandler.h>
#include <iostream>

int main(int argc,char **argv){
	String arg;
	int i,j;

	if(argc<2){
		std::cerr << "Usage: xsh2tmsh \"mshFile\"" << std::endl;
		return 0;
	}
	for(i=1; i<argc; ++i){
        arg=argv[i];
        if(arg.substr(0,1)=="-"){
			j=1;
            if(arg.substr(j,1)=="h"){
                std::cout << "Options for msh2m3g: " << std::endl;
                return 1;
            }
        }
		else if(i!=argc-1){
			std::cout << "Error unknown command: " << arg.c_str() << std::endl;
			return -1;
		}
	}

	String mshFileName=argv[argc-1];

	std::cout << "--xmsh2tmsh--" << std::endl;
	std::cout << "Converting " << (const char*)mshFileName << std::endl;

	// Create a toadlet engine
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG_LOGGER,Logger::Level_DISABLED);
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG,Logger::Level_WARNING);
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_TADPOLE,Logger::Level_WARNING);
	Engine *engine=new Engine();

	// Load the mesh data
	Mesh::ptr mesh=engine->getMeshManager()->findMesh(mshFileName);
	if(mesh==NULL){
		std::cout << "Error loading file: " << (const char*)mshFileName << std::endl;
		return 0;
	}

	// Prepare the output file
	int loc=mshFileName.rfind('.');
	String tmshFileName=mshFileName.substr(0,loc)+String(".tmsh");
	FileStream::ptr stream(new FileStream(tmshFileName,FileStream::Open_WRITE_BINARY));

	// Write to .tmsh format
	TMSHHandler handler(engine);
	bool result=handler.save(mesh,stream);

	if(result){
		std::cout << "complete" << std::endl;
		return 1;
	}
	else{
		std::cout << "incomplete" << std::endl;
		return -1;
	}
}

