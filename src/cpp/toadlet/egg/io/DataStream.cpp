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
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace egg{
namespace io{

DataStream::DataStream(Stream *stream){
	mStream=stream;
}

DataStream::DataStream(Stream::ptr stream){
	mStream=stream;
	mStreamPtr=stream;
}

DataStream::~DataStream(){
}

int DataStream::readBool(bool &b){
	return mStream->read((tbyte*)&b,1);
}

bool DataStream::readBool(){
	bool b=false;
	mStream->read((tbyte*)&b,1);
	return b;
}

int DataStream::readUInt8(uint8 &i){
	return mStream->read((tbyte*)&i,sizeof(i));
}

uint8 DataStream::readUInt8(){
	tbyte i=0;
	mStream->read((tbyte*)&i,sizeof(i));
	return i;
}

int DataStream::readInt8(int8 &i){
	return mStream->read((tbyte*)&i,sizeof(i));
}

int8 DataStream::readInt8(){
	int8 i=0;
	mStream->read((tbyte*)&i,sizeof(i));
	return i;
}

int DataStream::readNullTerminatedString(String &s){
	int amt=0;
	char string[1024];
	int i=0;
	while(true){
		amt+=mStream->read((tbyte*)string+i,1);
		if(amt==0 || string[i]==0){
			break;
		}
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

int DataStream::readBigUInt16(uint16 &i){
	int amt=mStream->read((tbyte*)&i,sizeof(i));
	bigUInt16InPlace(i);
	return amt;
}

uint16 DataStream::readBigUInt16(){
	uint16 i=0;
	mStream->read((tbyte*)&i,sizeof(i));
	bigUInt16InPlace(i);
	return i;
}

int DataStream::readBigInt16(int16 &i){
	int amt=mStream->read((tbyte*)&i,sizeof(i));
	bigInt16InPlace(i);
	return amt;
}

int16 DataStream::readBigInt16(){
	int16 i=0;
	mStream->read((tbyte*)&i,sizeof(i));
	bigInt16InPlace(i);
	return i;
}

int DataStream::readBigUInt32(uint32 &i){
	int amt=mStream->read((tbyte*)&i,sizeof(i));
	bigUInt32InPlace(i);
	return amt;
}

uint32 DataStream::readBigUInt32(){
	uint32 i=0;
	mStream->read((tbyte*)&i,sizeof(i));
	bigUInt32InPlace(i);
	return i;
}

int DataStream::readBigInt32(int32 &i){
	int amt=mStream->read((tbyte*)&i,sizeof(i));
	bigInt32InPlace(i);
	return amt;
}

int32 DataStream::readBigInt32(){
	int32 i=0;
	mStream->read((tbyte*)&i,sizeof(i));
	bigInt32InPlace(i);
	return i;
}

int DataStream::readBigUInt64(uint64 &i){
	int amt=mStream->read((tbyte*)&i,sizeof(i));
	bigUInt64InPlace(i);
	return amt;
}

uint64 DataStream::readBigUInt64(){
	uint64 i=0;
	mStream->read((tbyte*)&i,sizeof(i));
	bigUInt64InPlace(i);
	return i;
}

int DataStream::readBigInt64(int64 &i){
	int amt=mStream->read((tbyte*)&i,sizeof(i));
	bigInt64InPlace(i);
	return amt;
}

int64 DataStream::readBigInt64(){
	int64 i=0;
	mStream->read((tbyte*)&i,sizeof(i));
	bigInt64InPlace(i);
	return i;
}

int DataStream::readBigFloat(float &f){
	int amt=mStream->read((tbyte*)&f,sizeof(f));
	bigFloatInPlace(f);
	return amt;
}

float DataStream::readBigFloat(){
	float f=0;
	mStream->read((tbyte*)&f,sizeof(f));
	bigFloatInPlace(f);
	return f;
}

int DataStream::readBigDouble(double &d){
	int amt=mStream->read((tbyte*)&d,sizeof(d));
	bigDoubleInPlace(d);
	return amt;
}

double DataStream::readBigDouble(){
	double d=0;
	mStream->read((tbyte*)&d,sizeof(d));
	bigDoubleInPlace(d);
	return d;
}

int DataStream::readBigInt16String(String &s){
	int16 len=readBigInt16();
	int amt=2;
	char *string=new char[len+1];
	amt+=mStream->read((tbyte*)string,len);
	string[len]=0;
	s=string;
	delete[] string;
	return amt;
}

int DataStream::readLittleUInt16(uint16 &i){
	int amt=mStream->read((tbyte*)&i,sizeof(i));
	littleUInt16InPlace(i);
	return amt;
}

uint16 DataStream::readLittleUInt16(){
	uint16 i=0;
	mStream->read((tbyte*)&i,sizeof(i));
	littleUInt16InPlace(i);
	return i;
}

int DataStream::readLittleInt16(int16 &i){
	int amt=mStream->read((tbyte*)&i,sizeof(i));
	littleInt16InPlace(i);
	return amt;
}

int16 DataStream::readLittleInt16(){
	int16 i=0;
	mStream->read((tbyte*)&i,sizeof(i));
	littleInt16InPlace(i);
	return i;
}

int DataStream::readLittleUInt32(uint32 &i){
	int amt=mStream->read((tbyte*)&i,sizeof(i));
	littleUInt32InPlace(i);
	return amt;
}

uint32 DataStream::readLittleUInt32(){
	uint32 i=0;
	mStream->read((tbyte*)&i,sizeof(i));
	littleUInt32InPlace(i);
	return i;
}

int DataStream::readLittleInt32(int32 &i){
	int amt=mStream->read((tbyte*)&i,sizeof(i));
	littleInt32InPlace(i);
	return amt;
}

int32 DataStream::readLittleInt32(){
	int32 i=0;
	mStream->read((tbyte*)&i,sizeof(i));
	littleInt32InPlace(i);
	return i;
}

int DataStream::readLittleUInt64(uint64 &i){
	int amt=mStream->read((tbyte*)&i,sizeof(i));
	littleUInt64InPlace(i);
	return amt;
}

uint64 DataStream::readLittleUInt64(){
	uint64 i=0;
	mStream->read((tbyte*)&i,sizeof(i));
	littleUInt64InPlace(i);
	return i;
}

int DataStream::readLittleInt64(int64 &i){
	int amt=mStream->read((tbyte*)&i,sizeof(i));
	littleInt64InPlace(i);
	return amt;
}

int64 DataStream::readLittleInt64(){
	int64 i=0;
	mStream->read((tbyte*)&i,sizeof(i));
	littleInt64InPlace(i);
	return i;
}

int DataStream::readLittleFloat(float &f){
	int amt=mStream->read((tbyte*)&f,sizeof(f));
	littleFloatInPlace(f);
	return amt;
}

float DataStream::readLittleFloat(){
	float f=0;
	mStream->read((tbyte*)&f,sizeof(f));
	littleFloatInPlace(f);
	return f;
}

int DataStream::readLittleDouble(double &d){
	int amt=mStream->read((tbyte*)&d,sizeof(d));
	littleDoubleInPlace(d);
	return amt;
}

double DataStream::readLittleDouble(){
	double d=0;
	mStream->read((tbyte*)&d,sizeof(d));
	littleDoubleInPlace(d);
	return d;
}

int DataStream::readLittleInt16String(String &s){
	int16 len=readLittleInt16();
	int amt=2;
	char *string=new char[len+1];
	amt+=mStream->read((tbyte*)string,len);
	string[len]=0;
	s=string;
	delete[] string;
	return amt;
}

int DataStream::writeBool(bool b){
	return mStream->write((tbyte*)&b,1);
}

int DataStream::writeUInt8(uint8 i){
	return mStream->write((tbyte*)&i,sizeof(i));
}

int DataStream::writeInt8(int8 i){
	return mStream->write((tbyte*)&i,sizeof(i));
}

int DataStream::writeNullTerminatedString(const String &s){
	return mStream->write((tbyte*)s.c_str(),s.length()+1);
}

int DataStream::writeBigUInt16(uint16 i){
	bigUInt16InPlace(i);
	return mStream->write((tbyte*)&i,sizeof(i));
}

int DataStream::writeBigInt16(int16 i){
	bigInt16InPlace(i);
	return mStream->write((tbyte*)&i,sizeof(i));
}

int DataStream::writeBigUInt32(uint32 i){
	bigUInt32InPlace(i);
	return mStream->write((tbyte*)&i,sizeof(i));
}

int DataStream::writeBigInt32(int32 i){
	bigInt32InPlace(i);
	return mStream->write((tbyte*)&i,sizeof(i));
}

int DataStream::writeBigUInt64(uint64 i){
	bigUInt64InPlace(i);
	return mStream->write((tbyte*)&i,sizeof(i));
}

int DataStream::writeBigInt64(int64 i){
	bigInt64InPlace(i);
	return mStream->write((tbyte*)&i,sizeof(i));
}

int DataStream::writeBigFloat(float f){
	bigFloatInPlace(f);
	return mStream->write((tbyte*)&f,sizeof(f));
}

int DataStream::writeBigDouble(double d){
	bigDoubleInPlace(d);
	return mStream->write((tbyte*)&d,sizeof(d));
}

int DataStream::writeBigInt16String(const String &s){
	int len=s.length();
	if(len>Extents::MAX_SHORT){
		Error::unknown(Categories::TOADLET_EGG,
			"DataStream::writeBigInt16String: String too large");
		return 0;
	}
	int amt=writeBigInt16(len);
	amt+=mStream->write((tbyte*)s.c_str(),len);
	return amt;
}

int DataStream::writeLittleUInt16(uint16 i){
	littleUInt16InPlace(i);
	return mStream->write((tbyte*)&i,sizeof(i));
}

int DataStream::writeLittleInt16(int16 i){
	littleInt16InPlace(i);
	return mStream->write((tbyte*)&i,sizeof(i));
}

int DataStream::writeLittleUInt32(uint32 i){
	littleUInt32InPlace(i);
	return mStream->write((tbyte*)&i,sizeof(i));
}

int DataStream::writeLittleInt32(int32 i){
	littleInt32InPlace(i);
	return mStream->write((tbyte*)&i,sizeof(i));
}

int DataStream::writeLittleUInt64(uint64 i){
	littleUInt64InPlace(i);
	return mStream->write((tbyte*)&i,sizeof(i));
}

int DataStream::writeLittleInt64(int64 i){
	littleInt64InPlace(i);
	return mStream->write((tbyte*)&i,sizeof(i));
}

int DataStream::writeLittleFloat(float f){
	littleFloatInPlace(f);
	return mStream->write((tbyte*)&f,sizeof(f));
}

int DataStream::writeLittleDouble(double d){
	littleDoubleInPlace(d);
	return mStream->write((tbyte*)&d,sizeof(d));
}

int DataStream::writeLittleInt16String(const String &s){
	int len=s.length();
	if(len>Extents::MAX_SHORT){
		Error::unknown(Categories::TOADLET_EGG,
			"DataStream::writeLittleInt16String: String too large");
		return 0;
	}
	int amt=writeBigInt16(len);
	amt+=mStream->write((tbyte*)s.c_str(),len);
	return amt;
}

}
}
}

