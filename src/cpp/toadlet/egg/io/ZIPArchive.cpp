/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/io/ZIPArchive.h>
#include <zzip/zzip.h>
#include <zzip/plugin.h>
#include <stdio.h>

#if defined(TOADLET_PLATFORM_WIN32) && defined(TOADLET_ZZIPLIB_NAME)
	#pragma comment(lib,TOADLET_ZZIPLIB_NAME)
#endif

#if defined(TOADLET_PLATFORM_WIN32) && defined(TOADLET_ZLIB_NAME)
	#pragma comment(lib,TOADLET_ZLIB_NAME)
 #endif

namespace toadlet{
namespace egg{
namespace io{

Collection<Stream::ptr> ZIPArchive::streams;
Mutex ZIPArchive::mutex;

int toadlet_zzip_openStream(Stream::ptr stream){
	int id=-1;
	ZIPArchive::mutex.lock();
		ZIPArchive::streams.add(stream);
		id=ZIPArchive::streams.size();
	ZIPArchive::mutex.unlock();
	return id;
}

void toadlet_zzip_closeStream(int id){
	ZIPArchive::mutex.lock();
		ZIPArchive::streams[id-1]->close();
		ZIPArchive::streams.removeAt(id-1);
	ZIPArchive::mutex.unlock();
}

Stream::ptr toadlet_zzip_findStream(int id){
	Stream::ptr stream;
	ZIPArchive::mutex.lock();
		stream=ZIPArchive::streams[id-1];
	ZIPArchive::mutex.unlock();
	return stream;
}

// Stream id is the name in text.  Wish I could find a better way of doing this...
int toadlet_zzip_open(zzip_char_t* name,int flags,...){
	return String(name).toInt32();
}

int toadlet_zzip_close(int fd){
	toadlet_zzip_closeStream(fd);
	return 0;
}

zzip_ssize_t toadlet_zzip_read(int fd,void *buf,zzip_size_t len){
	Stream::ptr stream=toadlet_zzip_findStream(fd);

	return stream->read((byte*)buf,len);
}

zzip_off_t toadlet_zzip_seeks(int fd,zzip_off_t offset,int whence){
	Stream::ptr stream=toadlet_zzip_findStream(fd);

	bool result=false;
	if(whence==SEEK_SET){
		result=stream->seek(offset);
	}
	else if(whence==SEEK_CUR){
		int position=stream->position();
		result=stream->seek(position+offset);
	}
	else if(whence==SEEK_END){
		int length=stream->length();
		result=stream->seek(length+offset);
	}
	return offset;
}

zzip_off_t toadlet_zzip_filesize(int fd){
	Stream::ptr stream=toadlet_zzip_findStream(fd);

	return stream->length();
}

zzip_ssize_t toadlet_zzip_write(int fd,_zzip_const void* buf,zzip_size_t len){
	Stream::ptr stream=toadlet_zzip_findStream(fd);

	return stream->write((byte*)buf,len);
}

class ZIPStream:public Stream{
public:
	TOADLET_SHARED_POINTERS(ZIPStream);

	ZIPStream(ZZIP_DIR *dir,const String &name){
		mFile=zzip_file_open(dir,name,O_WRONLY);
	}

	virtual ~ZIPStream(){
		close();
	}

	bool isOpen(){return mFile!=NULL;}
	void close(){
		if(mFile!=NULL){
			zzip_file_close(mFile);
			mFile=NULL;
		}
	}

	bool isReadable(){return true;}
	int read(byte *buffer,int length){
		return zzip_file_read(mFile,buffer,length);
	}

	bool isWriteable(){return false;}
	int write(const byte *buffer,int length){return 0;}

	bool reset(){
		return zzip_rewind(mFile)!=0;
	}

	virtual int length(){
		int current=zzip_tell(mFile);
		zzip_seek(mFile,0,SEEK_END);
		int length=zzip_tell(mFile);
		zzip_seek(mFile,current,SEEK_SET);
		return length;
	}

	virtual int position(){
		return zzip_tell(mFile);
	}

	virtual bool seek(int offs){
		return zzip_seek(mFile,offs,SEEK_SET)!=0;
	}

protected:
	ZZIP_FILE *mFile;
};

ZIPArchive::ZIPArchive(){
	mIO=new zzip_plugin_io_handlers();

	zzip_plugin_io_handlers *io=(zzip_plugin_io_handlers*)mIO;
	zzip_init_io(io,0);
	io->fd.open=toadlet_zzip_open;
	io->fd.close=toadlet_zzip_close;
	io->fd.read=toadlet_zzip_read;
	io->fd.seeks=toadlet_zzip_seeks;
	io->fd.filesize=toadlet_zzip_filesize;
	io->fd.write=toadlet_zzip_write;
}

ZIPArchive::~ZIPArchive(){
	destroy();

	delete (zzip_plugin_io_handlers*)mIO;
}

void ZIPArchive::destroy(){
	if(mDir!=NULL){
		zzip_dir_close((ZZIP_DIR*)mDir);
		mDir=NULL;
	}

	if(mStream!=NULL){
		mStream->close();
		mStream=NULL;
	}
}

bool ZIPArchive::open(Stream::ptr stream){
	destroy();

	mStream=stream;

	int id=toadlet_zzip_openStream(stream);
	String idString=String("")+id;
	mDir=(void*)zzip_dir_open_ext_io(idString.c_str(),NULL,NULL,(zzip_plugin_io_handlers*)mIO);

	return true;
}

Stream::ptr ZIPArchive::openStream(const String &name){
	if(mDir==NULL){
		Error::unknown("ZIPArchive is not open");
		return NULL;
	}

	ZIPStream::ptr stream(new ZIPStream((ZZIP_DIR*)mDir,name));
	if(stream->isOpen()){
		return stream;
	}
	else{
		return NULL;
	}
}

}
}
}
