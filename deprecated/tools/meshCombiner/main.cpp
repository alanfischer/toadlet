#include <toadlet/egg/String.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/io/FileInputStream.h>
#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/Symbols.h>

#include "../shared/LineInputStream.h"

#include <iostream>

using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;

int main(int argc,char **argv){
	String arg;
	Collection<Mesh::Ptr> meshes;
	int i,j;

	if(argc<3){
		std::cerr << "Usage: meshCombiner \"config\" \"outputFile\"" << std::endl;
		return 0;
	}
	for(i=1; i<argc; ++i){
        arg=argv[i];
        if(arg.substr(0,1)=="-"){
            for(int j=1; j<arg.length(); j++){
                if(arg.substr(j,1)=="h"){
                    std::cout << "Options for meshCombiner: "<< std::endl;
                    std::cout << "-h \thelp - print this message"<< std::endl;
                    return 1;
                }
            }
        }
	}

	String config=argv[argc-2];
	String mshFileName=argv[argc-1];

	std::cout << "--meshCombiner--" << std::endl;
	std::cout << "Combining " << mshFileName.c_str() << " using " << config.c_str() << std::endl;

	// Create a toadlet engine
	Logger::getInstance()->setCategoryReportingLevel(TOADLET_EGG_LOGGER_CATEGORY,Logger::LEVEL_DISABLED);
	Logger::getInstance()->setCategoryReportingLevel(TOADLET_EGG_CATEGORY,Logger::LEVEL_WARNING);
	Logger::getInstance()->setCategoryReportingLevel(TOADLET_TADPOLE_CATEGORY,Logger::LEVEL_WARNING);
	Engine *engine=Engine::newEngineWithAllHandlers();

	int vertexTotal=0;

	FileInputStream fin(config);
	LineInputStream lin(&fin);
	while(lin.finished()==false){
		String line=lin.readLine();
		int split1=line.find(' ');
		int split2=line.find(' ',split1+1);
		int split3=line.find(' ',split2+1);
		int split4=line.find(' ',split3+1);

		if(split1>0 && split2>0){
			String meshName=line.substr(0,split1);
			float scale=line.substr(split1+1,split2-(split1+1)).toFloat();
			float xoffset=line.substr(split2+1,split3-(split2+1)).toFloat();
			float yoffset=line.substr(split3+1,split4-(split3+1)).toFloat();
			float zoffset=line.substr(split4+1,line.length()).toFloat();

			Mesh::Ptr mesh=engine->cacheMesh(meshName);
			VertexBuffer::Ptr vertexBuffer=mesh->staticVertexData->getVertexBuffer(0);
			VertexBufferAccessor vba(vertexBuffer,Buffer::LT_READ_WRITE);
			for(i=0;i<vba.getSize();++i){
				vba.position3f(i)=vba.position3f(i)*scale+Vector3(xoffset,yoffset,zoffset);
			}
			meshes.addElement(mesh);

			vertexTotal+=vba.getSize();
		}
	}

	Mesh::Ptr newMesh(new Mesh());
	VertexFormat vertexFormat;
	vertexFormat.addVertexElement(VertexElement(VertexElement::TYPE_POSITION,(VertexElement::Format)(VertexElement::FORMAT_BIT_COUNT_3|VertexElement::FORMAT_BIT_FLOAT_32)));
	vertexFormat.addVertexElement(VertexElement(VertexElement::TYPE_NORMAL,(VertexElement::Format)(VertexElement::FORMAT_BIT_COUNT_3|VertexElement::FORMAT_BIT_FLOAT_32)));
	vertexFormat.addVertexElement(VertexElement(VertexElement::TYPE_TEX_COORD,(VertexElement::Format)(VertexElement::FORMAT_BIT_COUNT_2|VertexElement::FORMAT_BIT_FLOAT_32)));
	VertexBuffer::Ptr vertexBuffer(new VertexBuffer(Buffer::UT_STATIC,Buffer::AT_WRITE_ONLY,vertexFormat,vertexTotal));
	newMesh->staticVertexData=VertexData::Ptr(new VertexData(vertexBuffer));

	vertexTotal=0;

	for(i=0;i<meshes.size();++i){
		Mesh::Ptr mesh=meshes[i];

		for(j=0;j<mesh->subMeshes.size();++j){
			Mesh::SubMesh::Ptr sub=mesh->subMeshes[i];

			IndexBufferAccessor iba(sub->indexData->getIndexBuffer(),Buffer::LT_READ_WRITE);
			int k;
			for(k=0;k<iba.getSize();++k){
				iba.index16(k)+=vertexTotal;
			}

			// We just reuse the same submesh
			newMesh->subMeshes.addElement(sub);
		}

		VertexBufferAccessor vba(mesh->staticVertexData->getVertexBuffer(0),Buffer::LT_READ_WRITE);
		VertexBufferAccessor nvba(newMesh->staticVertexData->getVertexBuffer(0),Buffer::LT_READ_WRITE);
		for(j=0;j<vba.getSize();++j){
			nvba.position3f(vertexTotal)=vba.position3f(j);
			nvba.normal3f(vertexTotal)=vba.normal3f(j);
			nvba.texCoord2f(vertexTotal)=vba.texCoord2f(j);
			vertexTotal++;
		}
	}

	// Write out new .xmsh
	FileOutputStream::Ptr fout(new FileOutputStream(mshFileName));
	engine->getMeshManager()->getHandler("xmsh")->save(newMesh,fout);

	std::cout << "complete" << std::endl;

	return 1;
}

