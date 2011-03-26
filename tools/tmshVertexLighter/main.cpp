#include "VertexLighter.h"
#include <toadlet/tadpole/handler/XMSHHandler.h>
#include <iostream>

int main(int argc,char **argv){
	String arg;
	Collection<String> files;
	Collection<int> skip;
	int i=0;
	Vector3 direction=Vector3(0,0,1);
	Vector4 diffuse=Colors::WHITE;
	Vector4 ambient=Colors::BLACK;
	float falloff=1.0f;
	bool keepNormals=false;
	bool lightEdges=true;
	float edgeEpsilon=0.05f;

	if(argc<2){
		std::cerr << "Usage: " << argv[0] << " \"mshFile1\" \"mshFile2\" etc..." << std::endl;
		return 0;
	}
	for(i=1; i<argc; ++i){
        arg=argv[i];
        if(arg.substr(0,1)=="-"){
            for(int j=1; j<arg.length(); j++){
                if(arg.substr(j,1)=="h"){
                    std::cout << "Options for msh2bac: "<< std::endl;
                    std::cout << "-h\thelp - print this message"<< std::endl;
                    std::cout << "-dx,y,z\tdirection - set light direction" << std::endl;
					std::cout << "-cr,g,b\tdiffuse color - set light diffuse color" << std::endl;
					std::cout << "-ar,g,b\tambient color - set light ambient color" << std::endl;
					std::cout << "-ffall\tfalloff - set light falloff factor" << std::endl;
					std::cout << "-ssub\tsubmesh - leave specified submesh at full brightness" << std::endl;
					std::cout << "-k\tkeep normals - keep normals in the model" << std::endl;
					std::cout << "-eeps\tedge skip - Don't light edges of the model, using epsilon to calculate the edge" << std::endl;
                    return 1;
                }
                if(arg.substr(j,1)=="d"){
					int comma1=arg.find(',',j+1);
					int comma2=arg.find(',',comma1+1);
					int end=arg.length();
					if(comma1==-1 || comma2==-1){
						std::cout << "Bad parameters" << std::endl;
						return -1;
					}
					direction.x=arg.substr(j+1,comma1-(j+1)).toFloat();
					direction.y=arg.substr(comma1+1,comma2-(comma1+1)).toFloat();
					direction.z=arg.substr(comma2+1,end-(comma2+1)).toFloat();
				}
                if(arg.substr(j,1)=="c"){
					int comma1=arg.find(',',j+1);
					int comma2=arg.find(',',comma1+1);
					int end=arg.length();
					if(comma1==-1 || comma2==-1){
						std::cout << "Bad parameters" << std::endl;
						return -1;
					}
					diffuse.x=arg.substr(j+1,comma1-(j+1)).toFloat();
					diffuse.y=arg.substr(comma1+1,comma2-(comma1+1)).toFloat();
					diffuse.z=arg.substr(comma2+1,end-(comma2+1)).toFloat();
				}
                if(arg.substr(j,1)=="a"){
					int comma1=arg.find(',',j+1);
					int comma2=arg.find(',',comma1+1);
					int end=arg.length();
					if(comma1==-1 || comma2==-1){
						std::cout << "Bad parameters" << std::endl;
						return -1;
					}
					ambient.x=arg.substr(j+1,comma1-(j+1)).toFloat();
					ambient.y=arg.substr(comma1+1,comma2-(comma1+1)).toFloat();
					ambient.z=arg.substr(comma2+1,end-(comma2+1)).toFloat();
				}
                if(arg.substr(j,1)=="f"){
					falloff=arg.substr(j+1,arg.length()-(j+1)).toFloat();
				}
				if(arg.substr(j,1)=="k"){
					keepNormals=true;
				}
				if(arg.substr(j,1)=="s"){
					skip.add(arg.substr(j+1,arg.length()-(j+1)).toInt32());
				}
				if(arg.substr(j,1)=="e"){
					lightEdges=false;
					edgeEpsilon=arg.substr(j+1,arg.length()-(j+1)).toFloat();
				}
            }
        }
		else{
			files.push_back(toadlet::egg::String(argv[i]));
		}
	}

	std::cout << "--tmshVertexLighter--" << std::endl;

	// Create a toadlet engine
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG,Logger::Level_DISABLED);
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG,Logger::Level_WARNING);
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_TADPOLE,Logger::Level_WARNING);
	Engine *engine=new Engine();

	VertexLighter lighter(engine);
	lighter.setLightDirection(direction);
	lighter.setLightDiffuseColor(diffuse);
	lighter.setLightAmbientColor(ambient);
	lighter.setLightFalloffFactor(falloff);
	lighter.setKeepNormals(keepNormals);
	lighter.setLightEdges(lightEdges,edgeEpsilon);

	bool result=true;

	for(i=0;i<files.size();++i){
		String mshFileName=files[i];

		std::cout << "Lighting " << (const char*)mshFileName << std::endl;
		
		// Load the mesh data
		Mesh::ptr mesh=engine->getMeshManager()->findMesh(mshFileName);
		if(mesh==NULL){
			std::cerr << "Error loading \"" << (const char*)mshFileName << "\" failed!  File invalid or not found." << std::endl;
			result=false;
			continue;
		}

		std::cout << "Lighting mesh " << (const char*)mshFileName << std::endl;

		// Light meshes
		lighter.lightMesh(mesh);

		// Relight specified submeshes to full brightness
		VertexBuffer::ptr vertexBuffer=mesh->staticVertexData->getVertexBuffer(0);
		VertexBufferAccessor vba(vertexBuffer);

		int ci=vertexBuffer->getVertexFormat()->findSemantic(VertexFormat::Semantic_COLOR);
		TOADLET_ASSERT(ci>=0);

		int j;
		for(j=0;j<skip.size();++j){
			if(skip[j]>=0 && skip[j]<mesh->subMeshes.size()){
				Mesh::SubMesh::ptr sub=mesh->subMeshes[skip[j]];
				IndexBuffer::ptr indexBuffer=sub->indexData->getIndexBuffer();
				IndexBufferAccessor iba(indexBuffer);

				int k;
				for(k=0;k<indexBuffer->getSize();++k){
					vba.setRGBA(iba.get(k),ci,Colors::WHITE.getRGBA());
				}
			}
			else{
				std::cout << "Submesh " << skip[j] << " unavailable" << std::endl;
			}
		}

		FileStream::ptr fout(new FileStream(mshFileName,FileStream::Open_WRITE_BINARY));

		String extension;
		int index=mshFileName.rfind('.');
		if(index!=String::npos) extension=mshFileName.substr(index+1,mshFileName.length()).toLower();
		if(extension=="xmsh"){
			XMSHHandler handler(NULL);
			handler.save(mesh,fout);
		}
		else{
			std::cerr << "Error saving \"" << (const char*)mshFileName << "\"" << std::endl;
		}
	}

	delete engine;

	if(result){
		std::cout << "complete" << std::endl;
		return 1;
	}
	else{
		std::cout << "incomplete" << std::endl;
		return -1;
	}
}

