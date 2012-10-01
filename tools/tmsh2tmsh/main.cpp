#include <toadlet/toadlet.h>
#include <toadlet/tadpole/plugins/TMSHStreamer.h>
#include <toadlet/tadpole/plugins/XMSHStreamer.h>
#include <iostream>

int main(int argc,char **argv){
	String arg;
	bool xmsh=false;
	int i,j;

	if(argc<2){
		std::cerr << "Usage: tsh2tmsh \"mshFile\"" << std::endl;
		return 0;
	}
	for(i=1; i<argc; ++i){
        arg=argv[i];
        if(arg.substr(0,1)=="-"){
            for(j=1; j<arg.length(); j++){
                if(arg.substr(j,1)=="h"){
                    std::cout << "Options for " << argv[0] << ": " << std::endl;
                    std::cout << "-h\t\thelp - print this message" << std::endl;
                    std::cout << "-x\t\txmsh - output xmsh, default is tmsh" << std::endl;
                    return 1;
                }
                if(arg.substr(j,1)=="x"){
					xmsh=true;
				}
			}
        }
		else if(i!=argc-1){
			std::cout << "Error unknown command: " << (const char*)arg << std::endl;
			return -1;
		}
	}

	String mshFileName=argv[argc-1];

	std::cout << "--tmsh2tmsh--" << std::endl;
	std::cout << "Converting " << (const char*)mshFileName << std::endl;

	// Create a toadlet engine
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG_LOGGER,Logger::Level_DISABLED);
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG,Logger::Level_WARNING);
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_TADPOLE,Logger::Level_WARNING);
	Engine::ptr engine=new Engine(0,0,Engine::Option_BIT_FIXEDBACKABLE|Engine::Option_BIT_SHADERBACKABLE);
	engine->installHandlers();

	// Load the mesh data
	Mesh::ptr mesh=shared_static_cast<Mesh>(engine->getMeshManager()->find(mshFileName));
	if(mesh==NULL){
		std::cout << "Error loading file: " << (const char*)mshFileName << std::endl;
		return 0;
	}

	// Output mesh information
	std::cout << "Number of SubMeshes: " << mesh->getNumSubMeshes() << std::endl;
	for(i=0;i<mesh->getNumSubMeshes();++i){
		Mesh::SubMesh *subMesh=mesh->getSubMesh(i);
		std::cout << "SubMesh: " << i << " has Material: " << (subMesh->material!=NULL) << " materialName: " << (const char*)subMesh->materialName << std::endl;
	}

	// Prepare the output file
	int loc=mshFileName.rfind('.');
	String tmshFileName=mshFileName.substr(0,loc)+(xmsh?".xmsh":".tmsh");
	FileStream::ptr stream(new FileStream(tmshFileName,FileStream::Open_WRITE_BINARY));

	// Write to desired format
	bool result=false;
	if(xmsh){
		XMSHStreamer::ptr streamer=new XMSHStreamer(engine);
		result=streamer->save(stream,mesh,NULL,NULL);
	}
	else{
		TMSHStreamer::ptr streamer=new TMSHStreamer(engine);
		result=streamer->save(stream,mesh,NULL,NULL);
	}

	if(result){
		std::cout << "complete" << std::endl;
		return 1;
	}
	else{
		std::cout << "incomplete" << std::endl;
		return -1;
	}
}

