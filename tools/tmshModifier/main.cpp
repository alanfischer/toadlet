#include <toadlet/egg/String.h>
#include <toadlet/egg/io/FileInputStream.h>
#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/handler/XANMHandler.h>
#include <toadlet/tadpole/handler/XMSHHandler.h>

#include <iostream>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::handler;
using namespace toadlet::tadpole::mesh;

int main(int argc,char **argv){
	String arg;
	int i,j;
	float scale=1.0f;
	bool insert=false;
	bool extract=false;

	if(argc<3){
		std::cerr << "Usage: " << argv[0] << " -[e|i|sscale] \"xmshFile\" [\"xanmFile[s]\"]" << std::endl;
		return 0;
	}
	for(i=1; i<argc; ++i){
        arg=argv[i];
        if(arg.substr(0,1)=="-"){
            for(j=1; j<arg.length(); j++){
                if(arg.substr(j,1)=="h"){
                    std::cout << "Options for " << argv[0] << ": " << std::endl;
                    std::cout << "-h\t\thelp - print this message" << std::endl;
                    std::cout << "-e\t\textract - extract sequences from the mesh" << std::endl;
                    std::cout << "-i\t\tinsert - insert the list of sequences into the mesh" << std::endl;
                    std::cout << "-sscale\t\tscale - scale the mesh" << std::endl;
                    return 1;
                }
				if(arg.substr(i,j)=="e"){
					extract=true;
				}
				if(arg.substr(i,j)=="i"){
					insert=true;
				}
				if(arg.substr(i,j)=="s"){
					scale=arg.substr(j+1,arg.length()).toFloat();
				}
            }
        }
	}

	String mshFileName=argv[2];

	std::cout << "--tmshModifier--" << std::endl;

	std::cout << "Modifiying " << (const char*)mshFileName << std::endl;

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

	if(extract){
		Skeleton::ptr skeleton=mesh->skeleton;
		if(skeleton==NULL){
			std::cout << "Error, Mesh does not contain a skeleton" << std::endl;
			return 0;
		}

		for(i=0;i<skeleton->sequences.size();++i){
			Sequence::ptr sequence=skeleton->sequences[i];

			String name;
			if(sequence->getName().length()>0){
				name=sequence->getName()+".xanm";
			}
			else{
				name=String("unnamed-") + i + ".xanm";
			}

			std::cout << "Extracting " << (const char*)name << std::endl;

			FileOutputStream::ptr fout(new FileOutputStream(name));
			if(fout->isOpen()==false){
				std::cout << "Error opening " << (const char*)name << std::endl;
				return 0;
			}
			XANMHandler::ptr handler(new XANMHandler());
			handler->save(sequence,fout);
		}
	}

	if(insert){
		Skeleton::ptr skeleton=mesh->skeleton;
		if(skeleton==NULL){
			std::cout << "Error, Mesh does not contain a skeleton" << std::endl;
			return 0;
		}

		skeleton->sequences.clear();

		for(i=3;i<argc;++i){
			std::cout << "Inserting " << argv[i] << std::endl;
			FileInputStream::ptr fin(new FileInputStream(argv[i]));
			if(fin->isOpen()==false){
				std::cout << "Error opening " << argv[i] << std::endl;
				return 0;
			}
			XANMHandler::ptr handler(new XANMHandler());
			Sequence::ptr sequence=shared_static_cast<Sequence>(handler->load(fin,NULL));
			skeleton->sequences.add(sequence);
		}

		FileOutputStream::ptr fout(new FileOutputStream(mshFileName));
		if(fout->isOpen()==false){
			std::cout << "Error opening " << (const char*)mshFileName << std::endl;
			return 0;
		}
		XMSHHandler::ptr handler(new XMSHHandler(NULL,NULL,NULL));
		handler->save(mesh,fout);
	}

	if(scale!=1.0f){
		int i,j,k;

		VertexBuffer::ptr vertexBuffer=mesh->staticVertexData->getVertexBuffer(0);
		{
			VertexBufferAccessor vba(vertexBuffer);
			for(i=0;i<vba.getSize();++i){
				Vector3 position;
				vba.get3(i,0,position);
				position*=scale;
				vba.set3(i,0,position);
			}
		}

		Skeleton::ptr skeleton=mesh->skeleton;
		if(skeleton!=NULL){
			for(i=0;i<skeleton->bones.size();++i){
				skeleton->bones[i]->translate*=scale;
				skeleton->bones[i]->worldToBoneTranslate*=scale;
			}
			for(i=0;i<skeleton->sequences.size();++i){
				for(j=0;j<skeleton->sequences[i]->tracks.size();++j){
					for(k=0;k<skeleton->sequences[i]->tracks[j]->keyFrames.size();++k){
						skeleton->sequences[i]->tracks[j]->keyFrames[k].translate*=scale;
					}
				}
			}
		}

		FileOutputStream::ptr fout(new FileOutputStream(mshFileName));
		if(fout->isOpen()==false){
			std::cout << "Error opening " << (const char*)mshFileName << std::endl;
			return 0;
		}
		XMSHHandler::ptr handler(new XMSHHandler(NULL,NULL,NULL));
		handler->save(mesh,fout);
	}

	std::cout << "complete" << std::endl;

	return 1;
}

