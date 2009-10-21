/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the Toadlet Engine, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#include <toadlet/tadpole/IndexDataFileInputStreamFactory.h>
#include <toadlet/egg/io/MemoryInputStream.h>
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace tadpole{

IndexDataFileInputStreamFactory::IndexDataFileInputStreamFactory():
	mData(NULL)
{
}

IndexDataFileInputStreamFactory::~IndexDataFileInputStreamFactory(){
	if(mData!=NULL){
		fclose(mData);
		mData=NULL;
	}
}

bool IndexDataFileInputStreamFactory::init(const String &indexFile,const String &dataFile){
	FILE *f=fopen(indexFile.c_str(),"r");
	if(f==NULL){
		Error::unknown(Logger::TOADLET_TADPOLE,
			"Invalid index file");
		return false;
	}
	char name[1024];
	while(!feof(f)){
		name[0]=0;
		Index index;
		index.position=0;
		index.length=0;
		fscanf(f,"%s %d %d",name,&index.position,&index.length);
		mIndex[String("/")+name]=index;
	}
	fclose(f);

	mData=fopen(dataFile.c_str(),"rb");
	if(mData==NULL){
		Error::unknown(Logger::TOADLET_TADPOLE,
			"Invalid data file");
		return false;
	}

	return true;
}

InputStream::ptr IndexDataFileInputStreamFactory::makeInputStream(const String &name){
	Logger::log(Logger::TOADLET_TADPOLE,Logger::Level_DEBUG,
		"Creating InputStream for "+name);

	Map<String,Index>::iterator it;
	it=mIndex.find(name);
	if(it==mIndex.end()){
		Error::unknown(Logger::TOADLET_TADPOLE,
			"File not found in data file");
	}

	int result=fseek(mData,it->second.position,SEEK_SET);
	if(result!=0){
		Error::unknown(Logger::TOADLET_TADPOLE,
			"Could not seek in data file");
		return InputStream::ptr();
	}

	int length=it->second.length;
	char *data=new char[length];

	int amount=fread(data,length,1,mData);
	if(amount==0){
		Error::unknown(Logger::TOADLET_TADPOLE,
			"Could not read data file");
		return InputStream::ptr();
	}

	MemoryInputStream::ptr in(new MemoryInputStream(data,length,true));
	return in;
}

}
}

