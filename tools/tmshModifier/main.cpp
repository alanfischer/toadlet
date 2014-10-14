#include <toadlet/toadlet.h>
#include <toadlet/tadpole/plugins/XANMStreamer.h>
#include <toadlet/tadpole/plugins/XMSHStreamer.h>
#include <iostream>

using namespace toadlet;

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

	if(extract){
		Skeleton::ptr skeleton=mesh->getSkeleton();
		if(skeleton==NULL){
			std::cout << "Error, Mesh does not contain a skeleton" << std::endl;
			return 0;
		}

		for(i=0;i<skeleton->getNumSequences();++i){
			Sequence::ptr sequence=skeleton->getSequence(i);

			String name;
			if(sequence->getName().length()>0){
				name=sequence->getName()+".xanm";
			}
			else{
				name=String("unnamed-") + i + ".xanm";
			}

			std::cout << "Extracting " << (const char*)name << std::endl;

			FileStream::ptr stream=new FileStream(name,FileStream::Open_WRITE_BINARY);
			if(stream->closed()){
				std::cout << "Error opening " << (const char*)name << std::endl;
				return 0;
			}
			XANMStreamer::ptr streamer=new XANMStreamer(engine);
			streamer->save(stream,sequence,NULL);
		}
	}

	if(insert){
		Skeleton::ptr skeleton=mesh->getSkeleton();
		if(skeleton==NULL){
			std::cout << "Error, Mesh does not contain a skeleton" << std::endl;
			return 0;
		}

		while(skeleton->getNumSequences()>0){
			skeleton->removeSequence(0);
		}
	
		for(i=3;i<argc;++i){
			std::cout << "Inserting " << argv[i] << std::endl;
			FileStream::ptr stream=new FileStream(argv[i],FileStream::Open_READ_BINARY);
			if(stream->closed()){
				std::cout << "Error opening " << argv[i] << std::endl;
				return 0;
			}
			XANMStreamer::ptr streamer=new XANMStreamer(engine);
			Sequence::ptr sequence=shared_static_cast<Sequence>(streamer->load(stream,NULL));
			skeleton->addSequence(sequence);
		}

		FileStream::ptr stream=new FileStream(mshFileName,FileStream::Open_WRITE_BINARY);
		if(stream->closed()){
			std::cout << "Error opening " << (const char*)mshFileName << std::endl;
			return 0;
		}

		XMSHStreamer::ptr streamer=new XMSHStreamer(engine);
		streamer->save(stream,mesh,NULL);
	}

	if(scale!=1.0f){
		int i,j,k;

		VertexBuffer::ptr vertexBuffer=mesh->getStaticVertexData()->getVertexBuffer(0);
		{
			VertexBufferAccessor vba(vertexBuffer);
			for(i=0;i<vba.getSize();++i){
				Vector3 position;
				vba.get3(i,0,position);
				position*=scale;
				vba.set3(i,0,position);
			}
		}

		Skeleton::ptr skeleton=mesh->getSkeleton();
		if(skeleton!=NULL){
			for(i=0;i<skeleton->getNumBones();++i){
				skeleton->getBone(i)->translate*=scale;
				skeleton->getBone(i)->worldToBoneTranslate*=scale;
			}
			for(i=0;i<skeleton->getNumSequences();++i){
				Sequence *sequence=skeleton->getSequence(i);
				for(j=0;j<sequence->getNumTracks();++j){
					Track *track=sequence->getTrack(j);
					VertexBufferAccessor &vba=track->getAccessor();
					for(k=0;k<track->getNumKeyFrames();++k){
						Vector3 translate;
						vba.get3(k,0,translate);
						translate*=scale;
						vba.set3(k,0,translate);
					}
				}
			}
		}

		FileStream::ptr stream=new FileStream(mshFileName,FileStream::Open_WRITE_BINARY);
		if(stream->closed()){
			std::cout << "Error opening " << (const char*)mshFileName << std::endl;
			return 0;
		}

		XMSHStreamer::ptr streamer=new XMSHStreamer(NULL);
		streamer->save(stream,mesh,NULL);
	}

	std::cout << "complete" << std::endl;

	return 1;
}

