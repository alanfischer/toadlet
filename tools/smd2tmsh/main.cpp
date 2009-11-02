#include <toadlet/egg/Collection.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/io/FileInputStream.h>
#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Mesh.h>
#include <toadlet/tadpole/handler/XMSHHandler.h>
#include <iostream>
#include "SMDConverter.h"

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::handler;

int main(int argc,char **argv){
	String arg;
	Collection<String> files;
	int i;
	float positionEpsilon=0;
	float normalEpsilon=0;
	float texCoordEpsilon=0;
	bool removeSkeleton=false;
	bool invertFaces=false;

	if(argc<2){
		std::cerr << "Usage: " << argv[0] << " \"refFile\" \"smdFile1\" \"smdfile2\" etc..." << std::endl;
		return 0;
	}
	for(i=1; i<argc; ++i){
        arg=argv[i];
        if(arg.substr(0,1)=="-"){
			int j=1;
			if(arg.substr(j,1)=="p"){
				positionEpsilon=arg.substr(j+1,arg.length()).toFloat();
			}
			if(arg.substr(j,1)=="n"){
				normalEpsilon=arg.substr(j+1,arg.length()).toFloat();
			}
			if(arg.substr(j,1)=="t"){
				texCoordEpsilon=arg.substr(j+1,arg.length()).toFloat();
			}
			if(arg.substr(j,1)=="k"){
				removeSkeleton=true;
			}
			if(arg.substr(j,1)=="i"){
				invertFaces=true;
			}
            if(arg.substr(j,1)=="h"){
                std::cout << "Options for " << argv[0] << ": "<< std::endl;
                std::cout << "-h \thelp - print this message"<< std::endl;
				std::cout << "-pamt \tepsilon - set position epsilon" << std::endl;
				std::cout << "-namt \tepsilon - set normal epsilon" << std::endl;
				std::cout << "-tamt \tepsilon - set texCoord epsilon" << std::endl;
				std::cout << "-i \tinvert - invert faces" << std::endl;
                return 1;
            }
        }
		else{
			files.add(String(argv[i]));
		}
	}

	std::cout << "--smd2tmsh--" << std::endl;

	// Create a toadlet engine
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG_LOGGER,Logger::Level_DISABLED);
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG,Logger::Level_WARNING);
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_TADPOLE,Logger::Level_WARNING);

	Engine *engine=new Engine();

	SMDConverter smd(engine);
	smd.setPositionEpsilon(positionEpsilon);
	smd.setNormalEpsilon(normalEpsilon);
	smd.setTexCoordEpsilon(texCoordEpsilon);
	smd.setRemoveSkeleton(removeSkeleton);
	smd.setInvertFaces(invertFaces);

	// Load each file
	for(i=0;i<files.size();++i){
		String fileName=files[i];

		std::cout << "Loading " << fileName << std::endl;

		FileInputStream::ptr in(new FileInputStream(fileName));
		if(in->isOpen()){
			smd.load(in);
		}
		else{
			std::cout << "Error opening " << fileName << std::endl;
		}
		in->close();
	}

	if(smd.getMesh()!=NULL){
		String outputName=files[0];
		int p=outputName.rfind('.');
		outputName=outputName.substr(0,p)+".xmsh";

		std::cout << "Saving " << outputName << std::endl;

		FileOutputStream::ptr out(new FileOutputStream(outputName));
		if(out->isOpen()){
			XMSHHandler handler(NULL,NULL,NULL);
			handler.save(smd.getMesh(),out);

			out->close();

			std::cout << "complete" << std::endl;
		}
		else{
			std::cout << "error saving file" << std::endl;
		}
	}
	else{
		std::cout << "error loading file" << std::endl;
	}
	
	delete engine;

	return 1;
}

