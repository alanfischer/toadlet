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

#ifndef TOADLET_EGG_IO_IOSTREAM_STREAM_H
#define TOADLET_EGG_IO_IOSTREAM_STREAM_H

#include <toadlet/egg/io/Stream.h>
#include <iostream>

namespace toadlet{
namespace egg{
namespace io{

class iostream_Stream: public Object, public Stream{
	iostream_Stream(std::iostream &stream) :
		mIStream(&stream),
		mOStream(&stream){}

	iostream_Stream(std::istream &stream) :
		mIStream(&stream),
		mOStream(NULL){}

	iostream_Stream(std::ostream &stream) :
		mIStream(NULL),
		mOStream(&stream){}

	void close(){
		// NULL out both streams in either case so we dont double delete an iostream
		if(mIStream!=NULL){
			delete mIStream;
			mIStream=NULL;
			mOStream=NULL;
		}
		
		if(mOStream!=NULL){
			delete mOStream;
			mOStream=NULL;
			mIStream=NULL;
		}
	}
	
	bool closed(){return mIStream==NULL && mOStream==NULL;}
	
	bool readable(){return mIStream!=NULL;}
	int read(tbyte *buffer,int length){
		if(mIStream!=NULL){
			mIStream->read((char*)buffer,length);
			return mIStream->gcount();
		}
		return -1;
	}

	bool writeable(){return mOStream!=NULL;}
	int write(const tbyte *buffer,int length){
		if(mOStream!=NULL){
			mOStream->write((char*)buffer,length);
			return mOStream->good()?length:-1;
		}
		return -1;
	}

	bool reset(){
		if(mIStream!=NULL){
			mIStream->seekg(0,std::ios_base::beg);
		}
		if(mOStream!=NULL){
			mOStream->seekp(0,std::ios_base::beg);
		}

		if(mIStream!=NULL){
			return mIStream->good();
		}
		if(mOStream!=NULL){
			return mOStream->good();
		}
		return false;
	}
	
	int length(){
		if(mIStream!=NULL){
			int current=mIStream->tellg();
			mIStream->seekg(0,std::ios_base::end);
			int result=mIStream->tellg();
			mIStream->seekg(current,std::ios_base::beg);
			return result;
		}
		if(mOStream!=NULL){
			int current=mOStream->tellp();
			mOStream->seekp(0,std::ios_base::end);
			int result=mOStream->tellp();
			mOStream->seekp(current,std::ios_base::beg);
			return result;
		}
		return 0;
	}
	
	int position(){
		if(mIStream!=NULL){
			return mIStream->tellg();
		}
		if(mOStream!=NULL){
			return mOStream->tellp();
		}
		return 0;
	}
	
	bool seek(int offs){
		if(mIStream!=NULL){
			mIStream->seekg(offs,std::ios_base::beg);
		}
		if(mOStream!=NULL){
			mOStream->seekp(offs,std::ios_base::beg);
		}

		if(mIStream!=NULL){
			return mIStream->good();
		}
		if(mOStream!=NULL){
			return mOStream->good();
		}
		return false;
	}
	
	bool flush(){
		if(mOStream!=NULL){
			mOStream->flush();
			return true;
		}
		return false;
	}
	
protected:
	std::istream *mIStream;
	std::ostream *mOStream;
};

}
}
}

#endif
