#include <toadlet/egg/Collection.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/io/FileStream.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/mesh/Mesh.h>
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

	SMDConverter smd;

	if(argc<2){
		std::cerr << "Usage: " << argv[0] << " \"refFile\" \"smdFile1\" \"smdfile2\" etc..." << std::endl;
		return 0;
	}
	for(i=1; i<argc; ++i){
        arg=argv[i];
        if(arg.substr(0,1)=="-"){
			int j=1;
			if(arg.substr(j,1)=="p"){
				smd.setPositionEpsilon(arg.substr(j+1,arg.length()).toFloat());
			}
			if(arg.substr(j,1)=="n"){
				smd.setNormalEpsilon(arg.substr(j+1,arg.length()).toFloat());
			}
			if(arg.substr(j,1)=="t"){
				smd.setTexCoordEpsilon(arg.substr(j+1,arg.length()).toFloat());
			}
			if(arg.substr(j,1)=="f"){
				smd.setFPS(arg.substr(j+1,arg.length()).toFloat());
			}
			if(arg.substr(j,1)=="k"){
				smd.setRemoveSkeleton(true);
			}
			if(arg.substr(j,1)=="i"){
				smd.setInvertFaces(true);
			}
            if(arg.substr(j,1)=="h"){
                std::cout << "Options for " << argv[0] << ": "<< std::endl;
                std::cout << "-h \thelp - print this message"<< std::endl;
				std::cout << "-pamt \tepsilon - set position epsilon" << std::endl;
				std::cout << "-namt \tepsilon - set normal epsilon" << std::endl;
				std::cout << "-tamt \tepsilon - set texCoord epsilon" << std::endl;
				std::cout << "-famt \tfps - set animation framerate" << std::endl;
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

	// Load each file
	for(i=0;i<files.size();++i){
		String fileName=files[i];

		std::cout << "Loading " << (const char*)fileName << std::endl;

		FileStream::ptr stream(new FileStream(fileName,FileStream::Open_READ_BINARY));
		if(stream->isOpen()){
			smd.load(engine,stream,fileName);
		}
		else{
			std::cout << "Error opening " << (const char*)fileName << std::endl;
		}
		stream->close();
	}

	if(smd.getMesh()!=NULL){
		String outputName=files[0];
		int p=outputName.rfind('.');
		outputName=outputName.substr(0,p)+".xmsh";

		std::cout << "Saving " << (const char*)outputName << std::endl;

		FileStream::ptr stream(new FileStream(outputName,FileStream::Open_WRITE_BINARY));
		if(stream->isOpen()){
			XMSHHandler handler(NULL,NULL,NULL);
			handler.save(smd.getMesh(),stream);

			stream->close();

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

