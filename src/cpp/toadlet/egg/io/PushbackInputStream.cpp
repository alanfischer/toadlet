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

#include <toadlet/egg/io/PushbackInputStream.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/Error.h>
#include <string.h> // memcpy

using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;

namespace toadlet{
namespace egg{
namespace io{

PushbackInputStream::PushbackInputStream(InputStream::ptr in){
	mIn=in;
	mSize=1;
	mPosition=mSize;
	mBuffer=new char[mSize];
}

PushbackInputStream::PushbackInputStream(InputStream::ptr in,int size){
	mIn=in;
	mSize=size;
	mPosition=mSize;
	mBuffer=new char[mSize];
}

PushbackInputStream::~PushbackInputStream(){
	delete mBuffer;
}

int PushbackInputStream::read(char *buffer,int length){
	int num=minVal(mSize-mPosition,length);
	if(num>0){
		memcpy(buffer,mBuffer+mPosition,num);
		mPosition+=num;
		length-=num;
		buffer+=num;
	}

	if(length>0){
		length=mIn->read(buffer,length);
		if(length==-1){
			return(num>0?num:-1);
		}
		num+=length;
	}

	return num;
}

void PushbackInputStream::unread(char *buffer,int length){
	if(mPosition<length){
		Error::unknown(Categories::TOADLET_EGG,
			"PushbackInputStream::unread: Insufficient space");
		return;
	}

	memcpy(mBuffer+(mPosition-length),buffer,length);

	mPosition-=length;
}

bool PushbackInputStream::reset(){
	bool result=mIn->reset();

	if(result){
		mPosition=mSize;
	}

	return result;
}

bool PushbackInputStream::seek(int offs){
	bool result=mIn->reset();

	if(result){
		mPosition=mSize;
	}

	return result;
}

int PushbackInputStream::available(){
	return (mSize-mPosition)+mIn->available();
}

void PushbackInputStream::close(){
	mIn->close();
}

}
}
}

