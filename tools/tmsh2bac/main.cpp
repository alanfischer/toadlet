#include <toadlet/egg/Collection.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/io/FileInputStream.h>
#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Mesh.h>
#include <toadlet/tadpole/TextureManager.h>

#include <iostream>
#include <sstream>

#include "BACConverter.h"

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::math;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::entity;

class TextureInputStreamFactory:public InputStreamFactory{
public:
	TextureInputStreamFactory(){}

	void setDirectory(const String &directory){
		if(directory.length()>0){
			mDirectory=directory+"/";
		}
		else{
			mDirectory=directory;
		}
	}

	void setSecondaryDirectory(const String &directory){
		if(directory.length()>0){
			mSecondaryDirectory=directory+"/";
		}
		else{
			mSecondaryDirectory=directory;
		}
	}

	InputStream::ptr makeInputStream(const String &name){
		FileInputStream::ptr fis(new FileInputStream(mDirectory+name));
		if(fis->isOpen()==false){
			if(mSecondaryDirectory.length()>0){
				fis=FileInputStream::ptr(new FileInputStream(mSecondaryDirectory+name));
				if(fis->isOpen()==false){
					fis=NULL;
				}
			}
			else{
				fis=NULL;
			}
		}
		return fis;
	}

	String mDirectory;
	String mSecondaryDirectory;
};

int main(int argc,char **argv){
	String arg;
	Collection<String> files;
	int i=0;
	int version=3;
	bool useSubmeshes=true;
	bool forceAnimationNumbers=false;
	bool useQuads=true;
	String texDir;
	float adjust=0.0f;
	float positionEpsilon=0.1f;
	float normalEpsilon=0.05f;
	float texCoordEpsilon=0.005f;
	float rotationEpsilon=0.01f;

	if(argc<2){
		std::cerr << "Usage: " << argv[0] << " -v[3|2] \"mshFile1\" \"mshFile2\" etc..." << std::endl;
		return 0;
	}
	for(i=1; i<argc; ++i){
        arg=argv[i];
        if(arg.substr(0,1)=="-"){
			int j=1;
            if(arg.substr(j,1)=="v"){
				version=arg.substr(j+1,arg.length()).toInt32();
			}
            if(arg.substr(j,1)=="s"){
				useSubmeshes=false;
			}
            if(arg.substr(j,1)=="d"){
				texDir=arg.substr(j+1,arg.length());
			}
            if(arg.substr(j,1)=="a"){
				adjust=arg.substr(j+1,arg.length()).toFloat();
			}
			if(arg.substr(j,1)=="p"){
				positionEpsilon=arg.substr(j+1,arg.length()).toFloat();
			}
			if(arg.substr(j,1)=="n"){
				normalEpsilon=arg.substr(j+1,arg.length()).toFloat();
			}
			if(arg.substr(j,1)=="t"){
				texCoordEpsilon=arg.substr(j+1,arg.length()).toFloat();
			}
			if(arg.substr(j,1)=="r"){
				rotationEpsilon=arg.substr(j+1,arg.length()).toFloat();
			}
			if(arg.substr(j,1)=="f"){
				forceAnimationNumbers=true;
			}
			if(arg.substr(j,1)=="q"){
				useQuads=false;
			}
            if(arg.substr(j,1)=="h"){
                std::cout << "Options for " << argv[0] << ": "<< std::endl;
				std::cout << "-v[2|3]\tversion - set mascot version 3 (bac version 6, tra version 4), or mascot version 2 (bac version 5, tra version 3)" << std::endl;
                std::cout << "-h \thelp - print this message"<< std::endl;
                std::cout << "-s \tsubmeshes - disable submeshes in the .bac file" << std::endl;
				std::cout << "-f \tforce - force animation numbers" << std::endl;
				std::cout << "-q \tno quads - disable quad construction in the .bac file" << std::endl;
				std::cout << "-ddir \tdirectory - set texture directory" << std::endl;
				std::cout << "-aamt \tadjust - adjust each vertex out by amt along the polygon plane" << std::endl;
				std::cout << "-pamt \tepsilon - set position epsilon" << std::endl;
				std::cout << "-namt \tepsilon - set normal epsilon" << std::endl;
				std::cout << "-tamt \tepsilon - set texCoord epsilon" << std::endl;
				std::cout << "-ramt \tepsilon - set rotation epsilon" << std::endl;
                return 1;
            }
        }
		else{
			files.push_back(String(argv[i]));
		}
	}

	if(version!=2 && version!=3){
		std::cout << "Invalid version, use version 2 or version 3" << std::endl;
		return -1;
	}

	std::cout << "--tmsh2bac--" << std::endl;

	// Create a toadlet engine
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG_LOGGER,Logger::Level_DISABLED);
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG,Logger::Level_WARNING);
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_TADPOLE,Logger::Level_WARNING);
	Engine *engine=new Engine();
	TextureInputStreamFactory *textureInputStreamFactory=new TextureInputStreamFactory();

	textureInputStreamFactory->setDirectory(texDir);
	engine->getTextureManager()->setInputStreamFactory(textureInputStreamFactory);

	std::cout << "Compiling a version " << version << " file" << std::endl;

	if(useSubmeshes==false){
		std::cout << "Disabling submeshes" << std::endl;
	}

	// Convert each msh file to a bac file
	for(i=0;i<files.size();++i){
		String mshFileName=files[i];

		std::cout << "Converting " << mshFileName << std::endl;

		int last=Math::maxVal(mshFileName.rfind('/'),mshFileName.rfind('\\'));
		if(last>0){
			textureInputStreamFactory->setSecondaryDirectory(mshFileName.substr(0,last));
		}
		else{
			textureInputStreamFactory->setSecondaryDirectory("");
		}

		// Load the mesh data
		Mesh::ptr mesh=engine->cacheMesh(mshFileName);
		if(mesh==NULL){
			std::cerr << "Error loading \"" << mshFileName << "\" failed!  File invalid or not found." << std::endl;
			continue;
		}

		// Prepare the output file
		int loc=mshFileName.rfind('.');
		String bacFileName=mshFileName.substr(0,loc)+String(".bac");
		FileOutputStream fout(bacFileName);

		std::cout << "Writing mesh " << bacFileName << std::endl;

		// Convert to .bac format
		BACConverter bac;
		bac.setPositionEpsilon(positionEpsilon);
		bac.setNormalEpsilon(normalEpsilon);
		bac.setTexCoordEpsilon(texCoordEpsilon);
		bac.setRotationEpsilon(rotationEpsilon);
		if(bac.convertMesh(mesh,&fout,useSubmeshes,useQuads,adjust,(version==3)?6:5)==false){
			std::cout << "Error converting mesh" << std::endl;
			return -1;
		}

		// Output animations, if available
		if(mesh->skeleton!=NULL && mesh->skeleton->sequences.size()>0){
			int j;
			for(j=0;j<mesh->skeleton->sequences.size();++j){
				String traFileName;
				if(forceAnimationNumbers==false && mesh->skeleton->sequences[j]->name.length()>0){
					traFileName=mesh->skeleton->sequences[j]->name+String(".tra");
				}
				else{
					std::stringstream ss;
					ss << mshFileName.substr(0,loc) << "_" << j << ".tra";
					traFileName=ss.str().c_str();
				}
				std::cout << "Writing animation " << traFileName << std::endl;
				FileOutputStream aout(traFileName);

				if(bac.convertAnimation(mesh,mesh->skeleton->sequences[j],&aout,(version==3)?4:3)==false){
					std::cout << "Error converting animation" << std::endl;
					return -1;
				}
			}
		}
	}

	delete engine;
	delete textureInputStreamFactory;

	std::cout << "complete" << std::endl;

	return 1;
}

