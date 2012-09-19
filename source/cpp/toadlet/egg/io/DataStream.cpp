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

#include <toadlet/egg/io/DataStream.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace egg{
namespace io{

DataStream::DataStream(Stream *stream){
	mStream=stream;
}

DataStream::~DataStream(){
}

int DataStream::read(tbyte *buffer,int length){
	int amount=0,total=0;

	while(length>0){
		amount=mStream->read(buffer+total,length);
		if(amount==0){
			return total;
		}
		else if(amount<0){
			return amount;
		}
		length-=amount;
		total+=amount;
	}

	return total;
}

bool DataStream::readBool(){
	bool b=false;
	read((tbyte*)&b,1);
	return b;
}

uint8 DataStream::readUInt8(){
	tbyte i=0;
	read((tbyte*)&i,sizeof(i));
	return i;
}

int8 DataStream::readInt8(){
	int8 i=0;
	read((tbyte*)&i,sizeof(i));
	return i;
}

int DataStream::readAll(tbyte *&resultBuffer){
	const static int bufferSize=4096;
	Collection<tbyte*> buffers;
	int amount=0,total=0;
	int i=0;

	resultBuffer=NULL;
	int resultAmount=0;

	if(length()<=0){
		return -1;
	}

	while(true){
		tbyte *buffer=new tbyte[bufferSize];
		if(buffer==NULL){
			int i;
			for(i=0;i<buffers.size();++i){
				delete[] buffers[i];
			}
			Error::insufficientMemory(Categories::TOADLET_EGG,"unable to allocate buffer");
			return total;
		}

		amount=read(buffer,bufferSize);
		if(amount<=0){
			delete[] buffer;
			break;
		}
		else{
			total+=amount;
			buffers.add(buffer);
		}
	}

	// Null terminate it just in case its used for a string
	resultBuffer=new tbyte[total+1];
	resultBuffer[total]=0;
	resultAmount=total;

	for(i=0;i<buffers.size();++i){
		int thing=bufferSize;
		if(total<thing){
			thing=total;
		}
		memcpy(resultBuffer+i*bufferSize,buffers[i],thing);
		total-=bufferSize;
		delete[] buffers[i];
	}

	return resultAmount;
}

int DataStream::readNullTerminatedString(String &s){
	s=String();
	int amt=0,total=0;
	char string[1024];
	int i=0;
	while(true){
		amt=read((tbyte*)string+i,1);
		if(amt==0 || string[i]==0){
			break;
		}
		total+=amt;
		i++;
		if(i==1023){
			string[i]=0;
			s+=string;
			i=0;
		}
	}
	string[i]=0;
	s+=string;
	return amt;
}

String DataStream::readNullTerminatedString(){
	String s;
	readNullTerminatedString(s);
	return s;
}

int DataStream::readAllString(String &s){
	tbyte *data=NULL;
	int length=readAll(data);
	s=data;
	delete[] data;
	return length;
}

String DataStream::readAllString(){
	String s;
	readAllString(s);
	return s;
}

uint16 DataStream::readBUInt16(){
	uint16 i=0;
	read((tbyte*)&i,sizeof(i));
	bigUInt16InPlace(i);
	return i;
}

int16 DataStream::readBInt16(){
	int16 i=0;
	read((tbyte*)&i,sizeof(i));
	bigInt16InPlace(i);
	return i;
}

uint32 DataStream::readBUInt32(){
	uint32 i=0;
	read((tbyte*)&i,sizeof(i));
	bigUInt32InPlace(i);
	return i;
}

int32 DataStream::readBInt32(){
	int32 i=0;
	read((tbyte*)&i,sizeof(i));
	bigInt32InPlace(i);
	return i;
}

uint64 DataStream::readBUInt64(){
	uint64 i=0;
	read((tbyte*)&i,sizeof(i));
	bigUInt64InPlace(i);
	return i;
}

int64 DataStream::readBInt64(){
	int64 i=0;
	read((tbyte*)&i,sizeof(i));
	bigInt64InPlace(i);
	return i;
}

float DataStream::readBFloat(){
	float f=0;
	read((tbyte*)&f,sizeof(f));
	bigFloatInPlace(f);
	return f;
}

double DataStream::readBDouble(){
	double d=0;
	read((tbyte*)&d,sizeof(d));
	bigDoubleInPlace(d);
	return d;
}


int DataStream::readBVector2(math::Vector2 &v){
	v.x=readBFloat();
	v.y=readBFloat();
	return 4*2;
}

int DataStream::readBVector3(math::Vector3 &v){
	v.x=readBFloat();
	v.y=readBFloat();
	v.z=readBFloat();
	return 4*3;
}

int DataStream::readBVector4(math::Vector4 &v){
	v.x=readBFloat();
	v.y=readBFloat();
	v.z=readBFloat();
	v.w=readBFloat();
	return 4*4;
}

int DataStream::readBQuaternion(math::Quaternion &q){
	q.x=readBFloat();
	q.y=readBFloat();
	q.z=readBFloat();
	q.w=readBFloat();
	return 4*4;
}

int DataStream::readBAABox(math::AABox &a){
	int amount=0;
	amount+=readBVector3(a.mins);
	amount+=readBVector3(a.maxs);
	return amount;
}

int DataStream::readBSphere(math::Sphere &s){
	int amount=0;
	amount+=readBVector3(s.origin);
	s.radius=readBFloat();amount+=4;
	return amount;
}

int DataStream::readBVector2(mathfixed::Vector2 &v){
	v.x=readBInt32();
	v.y=readBInt32();
	return 4*2;
}

int DataStream::readBVector3(mathfixed::Vector3 &v){
	v.x=readBInt32();
	v.y=readBInt32();
	v.z=readBInt32();
	return 4*3;
}

int DataStream::readBVector4(mathfixed::Vector4 &v){
	v.x=readBInt32();
	v.y=readBInt32();
	v.z=readBInt32();
	v.w=readBInt32();
	return 4*4;
}

int DataStream::readBQuaternion(mathfixed::Quaternion &q){
	q.x=readBInt32();
	q.y=readBInt32();
	q.z=readBInt32();
	q.w=readBInt32();
	return 4*4;
}

int DataStream::readBAABox(mathfixed::AABox &a){
	int amount=0;
	amount+=readBVector3(a.mins);
	amount+=readBVector3(a.maxs);
	return amount;
}

int DataStream::readBSphere(mathfixed::Sphere &s){
	int amount=0;
	amount+=readBVector3(s.origin);
	s.radius=readBInt32();amount+=4;
	return amount;
}

int DataStream::readBInt16String(String &s){
	int16 len=readBInt16();
	int amt=2;
	char *string=new char[len+1];
	amt+=read((tbyte*)string,len);
	string[len]=0;
	s=string;
	delete[] string;
	return amt;
}

String DataStream::readBInt16String(){
	String s;
	readBInt16String(s);
	return s;
}

uint16 DataStream::readLUInt16(){
	uint16 i=0;
	read((tbyte*)&i,sizeof(i));
	littleUInt16InPlace(i);
	return i;
}

int16 DataStream::readLInt16(){
	int16 i=0;
	read((tbyte*)&i,sizeof(i));
	littleInt16InPlace(i);
	return i;
}

uint32 DataStream::readLUInt32(){
	uint32 i=0;
	read((tbyte*)&i,sizeof(i));
	littleUInt32InPlace(i);
	return i;
}

int32 DataStream::readLInt32(){
	int32 i=0;
	read((tbyte*)&i,sizeof(i));
	littleInt32InPlace(i);
	return i;
}

uint64 DataStream::readLUInt64(){
	uint64 i=0;
	read((tbyte*)&i,sizeof(i));
	littleUInt64InPlace(i);
	return i;
}

int64 DataStream::readLInt64(){
	int64 i=0;
	read((tbyte*)&i,sizeof(i));
	littleInt64InPlace(i);
	return i;
}

float DataStream::readLFloat(){
	float f=0;
	read((tbyte*)&f,sizeof(f));
	littleFloatInPlace(f);
	return f;
}

double DataStream::readLDouble(){
	double d=0;
	read((tbyte*)&d,sizeof(d));
	littleDoubleInPlace(d);
	return d;
}

int DataStream::readLVector2(math::Vector2 &v){
	v.x=readLFloat();
	v.y=readLFloat();
	return 4*2;
}

int DataStream::readLVector3(math::Vector3 &v){
	v.x=readLFloat();
	v.y=readLFloat();
	v.z=readLFloat();
	return 4*3;
}

int DataStream::readLVector4(math::Vector4 &v){
	v.x=readLFloat();
	v.y=readLFloat();
	v.z=readLFloat();
	v.w=readLFloat();
	return 4*4;
}

int DataStream::readLQuaternion(math::Quaternion &q){
	q.x=readLFloat();
	q.y=readLFloat();
	q.z=readLFloat();
	q.w=readLFloat();
	return 4*4;
}

int DataStream::readLAABox(math::AABox &a){
	int amount=0;
	amount+=readLVector3(a.mins);
	amount+=readLVector3(a.maxs);
	return amount;
}

int DataStream::readLSphere(math::Sphere &s){
	int amount=0;
	amount+=readLVector3(s.origin);
	s.radius=readLFloat();amount+=4;
	return amount;
}

int DataStream::readLVector2(mathfixed::Vector2 &v){
	v.x=readLInt32();
	v.y=readLInt32();
	return 4*2;
}

int DataStream::readLVector3(mathfixed::Vector3 &v){
	v.x=readLInt32();
	v.y=readLInt32();
	v.z=readLInt32();
	return 4*3;
}

int DataStream::readLVector4(mathfixed::Vector4 &v){
	v.x=readLInt32();
	v.y=readLInt32();
	v.z=readLInt32();
	v.w=readLInt32();
	return 4*4;
}

int DataStream::readLQuaternion(mathfixed::Quaternion &q){
	q.x=readLInt32();
	q.y=readLInt32();
	q.z=readLInt32();
	q.w=readLInt32();
	return 4*4;
}

int DataStream::readLAABox(mathfixed::AABox &a){
	int amount=0;
	amount+=readLVector3(a.mins);
	amount+=readLVector3(a.maxs);
	return amount;
}

int DataStream::readLSphere(mathfixed::Sphere &s){
	int amount=0;
	amount+=readLVector3(s.origin);
	s.radius=readLInt32();amount+=4;
	return amount;
}

int DataStream::readLInt16String(String &s){
	int16 len=readLInt16();
	int amt=2;
	char *string=new char[len+1];
	amt+=read((tbyte*)string,len);
	string[len]=0;
	s=string;
	delete[] string;
	return amt;
}

String DataStream::readLInt16String(){
	String s;
	readLInt16String(s);
	return s;
}

int DataStream::writeBool(bool b){
	return write((tbyte*)&b,1);
}

int DataStream::writeUInt8(uint8 i){
	return write((tbyte*)&i,sizeof(i));
}

int DataStream::writeInt8(int8 i){
	return write((tbyte*)&i,sizeof(i));
}

int DataStream::writeNullTerminatedString(const String &s){
	return write((tbyte*)s.c_str(),s.length()+1);
}

int DataStream::writeBUInt16(uint16 i){
	bigUInt16InPlace(i);
	return write((tbyte*)&i,sizeof(i));
}

int DataStream::writeBInt16(int16 i){
	bigInt16InPlace(i);
	return write((tbyte*)&i,sizeof(i));
}

int DataStream::writeBUInt32(uint32 i){
	bigUInt32InPlace(i);
	return write((tbyte*)&i,sizeof(i));
}

int DataStream::writeBInt32(int32 i){
	bigInt32InPlace(i);
	return write((tbyte*)&i,sizeof(i));
}

int DataStream::writeBUInt64(uint64 i){
	bigUInt64InPlace(i);
	return write((tbyte*)&i,sizeof(i));
}

int DataStream::writeBInt64(int64 i){
	bigInt64InPlace(i);
	return write((tbyte*)&i,sizeof(i));
}

int DataStream::writeBFloat(float f){
	bigFloatInPlace(f);
	return write((tbyte*)&f,sizeof(f));
}

int DataStream::writeBDouble(double d){
	bigDoubleInPlace(d);
	return write((tbyte*)&d,sizeof(d));
}

int DataStream::writeBVector2(const math::Vector2 &v){
	int amount=0;
	amount+=writeBFloat(v.x);
	amount+=writeBFloat(v.y);
	return amount;
}

int DataStream::writeBVector3(const math::Vector3 &v){
	int amount=0;
	amount+=writeBFloat(v.x);
	amount+=writeBFloat(v.y);
	amount+=writeBFloat(v.z);
	return amount;
}

int DataStream::writeBVector4(const math::Vector4 &v){
	int amount=0;
	amount+=writeBFloat(v.x);
	amount+=writeBFloat(v.y);
	amount+=writeBFloat(v.z);
	amount+=writeBFloat(v.w);
	return amount;
}

int DataStream::writeBQuaternion(const math::Quaternion &q){
	int amount=0;
	amount+=writeBFloat(q.x);
	amount+=writeBFloat(q.y);
	amount+=writeBFloat(q.z);
	amount+=writeBFloat(q.w);
	return amount;
}

int DataStream::writeBAABox(const math::AABox &a){
	int amount=0;
	amount+=writeBVector3(a.mins);
	amount+=writeBVector3(a.maxs);
	return amount;
}

int DataStream::writeBSphere(const math::Sphere &s){
	int amount=0;
	amount+=writeBVector3(s.origin);
	amount+=writeBFloat(s.radius);
	return amount;
}

int DataStream::writeBVector2(const mathfixed::Vector2 &v){
	int amount=0;
	amount+=writeBInt32(v.x);
	amount+=writeBInt32(v.y);
	return amount;
}

int DataStream::writeBVector3(const mathfixed::Vector3 &v){
	int amount=0;
	amount+=writeBInt32(v.x);
	amount+=writeBInt32(v.y);
	amount+=writeBInt32(v.z);
	return amount;
}

int DataStream::writeBVector4(const mathfixed::Vector4 &v){
	int amount=0;
	amount+=writeBInt32(v.x);
	amount+=writeBInt32(v.y);
	amount+=writeBInt32(v.z);
	amount+=writeBInt32(v.w);
	return amount;
}

int DataStream::writeBQuaternion(const mathfixed::Quaternion &q){
	int amount=0;
	amount+=writeBInt32(q.x);
	amount+=writeBInt32(q.y);
	amount+=writeBInt32(q.z);
	amount+=writeBInt32(q.w);
	return amount;
}

int DataStream::writeBAABox(const mathfixed::AABox &a){
	int amount=0;
	amount+=writeBVector3(a.mins);
	amount+=writeBVector3(a.maxs);
	return amount;
}

int DataStream::writeBSphere(const mathfixed::Sphere &s){
	int amount=0;
	amount+=writeBVector3(s.origin);
	amount+=writeBInt32(s.radius);
	return amount;
}

int DataStream::writeBInt16String(const String &s){
	int len=s.length();
	if(len>Extents::MAX_SHORT){
		Error::unknown(Categories::TOADLET_EGG,
			"DataStream::writeBInt16String: String too large");
		return 0;
	}
	int amt=writeBInt16(len);
	amt+=write((tbyte*)s.c_str(),len);
	return amt;
}

int DataStream::writeLUInt16(uint16 i){
	littleUInt16InPlace(i);
	return write((tbyte*)&i,sizeof(i));
}

int DataStream::writeLInt16(int16 i){
	littleInt16InPlace(i);
	return write((tbyte*)&i,sizeof(i));
}

int DataStream::writeLUInt32(uint32 i){
	littleUInt32InPlace(i);
	return write((tbyte*)&i,sizeof(i));
}

int DataStream::writeLInt32(int32 i){
	littleInt32InPlace(i);
	return write((tbyte*)&i,sizeof(i));
}

int DataStream::writeLUInt64(uint64 i){
	littleUInt64InPlace(i);
	return write((tbyte*)&i,sizeof(i));
}

int DataStream::writeLInt64(int64 i){
	littleInt64InPlace(i);
	return write((tbyte*)&i,sizeof(i));
}

int DataStream::writeLFloat(float f){
	littleFloatInPlace(f);
	return write((tbyte*)&f,sizeof(f));
}

int DataStream::writeLDouble(double d){
	littleDoubleInPlace(d);
	return write((tbyte*)&d,sizeof(d));
}

int DataStream::writeLVector2(const math::Vector2 &v){
	int amount=0;
	amount+=writeLFloat(v.x);
	amount+=writeLFloat(v.y);
	return amount;
}

int DataStream::writeLVector3(const math::Vector3 &v){
	int amount=0;
	amount+=writeLFloat(v.x);
	amount+=writeLFloat(v.y);
	amount+=writeLFloat(v.z);
	return amount;
}

int DataStream::writeLVector4(const math::Vector4 &v){
	int amount=0;
	amount+=writeLFloat(v.x);
	amount+=writeLFloat(v.y);
	amount+=writeLFloat(v.z);
	amount+=writeLFloat(v.w);
	return amount;
}

int DataStream::writeLQuaternion(const math::Quaternion &q){
	int amount=0;
	amount+=writeLFloat(q.x);
	amount+=writeLFloat(q.y);
	amount+=writeLFloat(q.z);
	amount+=writeLFloat(q.w);
	return amount;
}

int DataStream::writeLAABox(const math::AABox &a){
	int amount=0;
	amount+=writeLVector3(a.mins);
	amount+=writeLVector3(a.maxs);
	return amount;
}

int DataStream::writeLSphere(const math::Sphere &s){
	int amount=0;
	amount+=writeLVector3(s.origin);
	amount+=writeLFloat(s.radius);
	return amount;
}

int DataStream::writeLVector2(const mathfixed::Vector2 &v){
	int amount=0;
	amount+=writeLInt32(v.x);
	amount+=writeLInt32(v.y);
	return amount;
}

int DataStream::writeLVector3(const mathfixed::Vector3 &v){
	int amount=0;
	amount+=writeLInt32(v.x);
	amount+=writeLInt32(v.y);
	amount+=writeLInt32(v.z);
	return amount;
}

int DataStream::writeLVector4(const mathfixed::Vector4 &v){
	int amount=0;
	amount+=writeLInt32(v.x);
	amount+=writeLInt32(v.y);
	amount+=writeLInt32(v.z);
	amount+=writeLInt32(v.w);
	return amount;
}

int DataStream::writeLQuaternion(const mathfixed::Quaternion &q){
	int amount=0;
	amount+=writeLInt32(q.x);
	amount+=writeLInt32(q.y);
	amount+=writeLInt32(q.z);
	amount+=writeLInt32(q.w);
	return amount;
}

int DataStream::writeLAABox(const mathfixed::AABox &a){
	int amount=0;
	amount+=writeLVector3(a.mins);
	amount+=writeLVector3(a.maxs);
	return amount;
}

int DataStream::writeLSphere(const mathfixed::Sphere &s){
	int amount=0;
	amount+=writeLVector3(s.origin);
	amount+=writeLInt32(s.radius);
	return amount;
}

int DataStream::writeLInt16String(const String &s){
	int len=s.length();
	if(len>Extents::MAX_SHORT){
		Error::unknown(Categories::TOADLET_EGG,
			"DataStream::writeLInt16String: String too large");
		return 0;
	}
	int amt=writeLInt16(len);
	amt+=write((tbyte*)s.c_str(),len);
	return amt;
}

}
}
}

