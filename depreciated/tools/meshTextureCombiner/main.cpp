#include <toadlet/egg/String.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/io/FileInputStream.h>
#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/Symbols.h>

#include <iostream>

#include "../shared/LineInputStream.h"

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;

int main(int argc,char **argv){
	String arg;
	int i,j;
	bool combineLikeTextures=true;
	int start=0;

	for(i=1; i<argc; ++i){
        arg=argv[i];
        if(arg.substr(0,1)=="-"){
            for(int j=1; j<arg.length(); j++){
                if(arg.substr(j,1)=="h"){
                    std::cout << "Options for meshTextureCombiner: "<< std::endl;
                    std::cout << "-h \thelp - print this message"<< std::endl;
                    std::cout << "-d \tdont - dont combine like textures"<< std::endl;
                    return 1;
                }
				else if(arg.substr(j,1)=="d"){
					combineLikeTextures=false;
				}
            }
        }
		else{
			start=i;
			break;
		}
	}
	if(argc<4){
		std::cerr << "Usage: meshTextureCombiner \"config\" \"inMesh\" \"outMesh\"" << std::endl;
		return 0;
	}

	String config=argv[start];
	String inMeshName=argv[start+1];
	String outMeshName=argv[start+2];

	std::cout << "--meshTextureCombiner--" << std::endl;
	std::cout << "Combining " << inMeshName.c_str() << " using " << config.c_str() << std::endl;

	// Create a toadlet engine
	Logger::getInstance()->setCategoryReportingLevel(TOADLET_EGG_LOGGER_CATEGORY,Logger::LEVEL_DISABLED);
	Logger::getInstance()->setCategoryReportingLevel(TOADLET_EGG_CATEGORY,Logger::LEVEL_WARNING);
	Logger::getInstance()->setCategoryReportingLevel(TOADLET_TADPOLE_CATEGORY,Logger::LEVEL_WARNING);
	Engine *engine=Engine::newEngineWithAllHandlers();

	Mesh::Ptr mesh=engine->cacheMesh(inMeshName);
	if(mesh==NULL){
		std::cout << "Error opening " << inMeshName.c_str() << std::endl;
		return -1;
	}

	FileInputStream fin(config);
	if(fin.isOpen()==false){
		std::cout << "Error opening " << config.c_str() << std::endl;
		return -1;
	}

	Collection<char> vertexesUpdated(mesh->staticVertexData->getVertexBuffer(0)->getSize());
	for(i=0;i<vertexesUpdated.size();++i){
		vertexesUpdated[i]=false;
	}
	LineInputStream lin(&fin);
	while(lin.finished()==false){
		String line=lin.readLine();
		int split1=line.find(' ');
		int split2=line.find(' ',split1+1);
		int split3=line.find(' ',split2+1);
		int split4=line.find(' ',split3+1);

		if(split1>0 && split2>0){
			String inTextureName=line.substr(0,split1);
			String outTextureName=line.substr(split1+1,split2-(split1+1));
			float uoffset=line.substr(split2+1,split3-(split2+1)).toFloat();
			float voffset=line.substr(split3+1,split4-(split3+1)).toFloat();
			float scale=line.substr(split4+1,line.length()).toFloat();

			#ifdef TOADLET_DEBUG
				std::cout << std::endl;
				std::cout << "inTexture:" << inTextureName.c_str() << std::endl;
				std::cout << "outTexture:" << outTextureName.c_str() << std::endl;
				std::cout << "uoffset:" << uoffset << std::endl;
				std::cout << "voffset:" << voffset << std::endl;
				std::cout << "scale:" << scale << std::endl;
			#endif

			VertexBuffer::Ptr vertexBuffer=mesh->staticVertexData->getVertexBuffer(0);
			VertexBufferAccessor vba(vertexBuffer,Buffer::LT_READ_WRITE);

			Collection<char> vertexesToUpdate(vba.getSize());
			for(i=0;i<vertexesToUpdate.size();++i){
				vertexesToUpdate[i]=false;
			}

			for(i=0;i<mesh->subMeshes.size();++i){
				Mesh::SubMesh::Ptr sub=mesh->subMeshes[i];

				// This utility is only made for checking the first material
				Material::Ptr material=sub->material;
				if(material!=NULL){
					String textureName=engine->getTextureManager()->cleanFilename(material->getMapName(Material::MAP_DIFFUSE));
					std::cout << "Comparing " << textureName.c_str() << " to " << inTextureName.c_str() << std::endl;
					if(textureName==inTextureName){
						std::cout << "Replacing " << inTextureName.c_str() << " with " << outTextureName.c_str() << std::endl;
						material->setMap(Material::MAP_DIFFUSE,engine->cacheTexture(outTextureName),1,outTextureName);

						IndexBufferAccessor iba(sub->indexData->getIndexBuffer(),Buffer::LT_READ_ONLY);
						for(j=0;j<iba.getSize();++j){
							int index=iba.index16(j);
							vertexesToUpdate[index]=true;
						}
					}
				}
			}

			for(i=0;i<vertexesToUpdate.size();++i){
				if(vertexesToUpdate[i] && vertexesUpdated[i]==false){
					Vector2 tex=vba.texCoord2f(i);

					// Since textures are combined to bigger textures, we need to make sure they stay within their original bounds
					if(tex.x<0){
						tex.x=0;
					}
					else if(tex.x>1){
						tex.x=1;
					}

					if(tex.y<0){
						tex.y=0;
					}
					else if(tex.y>1){
						tex.y=1;
					}

					tex*=scale;
					tex.x+=uoffset;
					tex.y+=voffset;
					vba.texCoord2f(i)=tex;
					vertexesUpdated[i]=true;
				}
			}
		}
	}

	if(combineLikeTextures){
		for(i=0;i<mesh->subMeshes.size();++i){
			Mesh::SubMesh::Ptr sub=mesh->subMeshes[i];

			String textureName;
			Material::Ptr material=sub->material;
			if(material!=NULL){
				textureName=material->getMapName(Material::MAP_DIFFUSE);
			}

			if(textureName!=""){
				for(j=i+1;j<mesh->subMeshes.size();++j){
					Mesh::SubMesh::Ptr sub2=mesh->subMeshes[j];

					String textureName2;
					Material::Ptr material=sub2->material;
					if(material!=NULL){
						textureName2=material->getMapName(Material::MAP_DIFFUSE);
					}

					if(textureName==textureName2){
						IndexBuffer::Ptr sib=sub->indexData->getIndexBuffer();
						IndexBuffer::Ptr s2ib=sub2->indexData->getIndexBuffer();

						IndexBuffer::Ptr cib(sib->cloneWithNewParameters(sib->getUsageType(),sib->getAccessType(),sib->getIndexFormat(),sib->getSize()+s2ib->getSize()));

						uint8 *s2ptr=s2ib->lock(Buffer::LT_READ_ONLY);
						uint8 *cptr=cib->lock(Buffer::LT_WRITE_ONLY);

						memcpy(cptr+sib->getBufferSize(),s2ptr,s2ib->getBufferSize());

						cib->unlock();
						s2ib->unlock();

						// TODO: This doesnt use the start & count correctly
						sub->indexData=IndexData::Ptr(new IndexData(sub->indexData->getPrimitive(),cib,0,cib->getSize()));

						mesh->subMeshes.erase(mesh->subMeshes.begin()+j);
						j--;
					}
				}
			}
		}
	}

	// Write out new mesh
	FileOutputStream::Ptr fout(new FileOutputStream(outMeshName));
	if(fout->isOpen()==false){
		std::cout << "Error opening " << outMeshName.c_str() << std::endl;
		return -1;
	}

	SharedPointer<ResourceManager<Mesh>::Handler> handler=engine->getMeshManager()->getHandler("xmsh");
	handler->save(mesh,fout);

	std::cout << "complete" << std::endl;

	return 1;
}

