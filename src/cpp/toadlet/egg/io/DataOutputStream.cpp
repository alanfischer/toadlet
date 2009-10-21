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

#include <toadlet/egg/io/DataOutputStream.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace egg{
namespace io{

DataOutputStream::DataOutputStream(OutputStream *out){
	mOut=out;
}

DataOutputStream::DataOutputStream(OutputStream::ptr out){
	mOut=out;
	mOutPtr=out;
}

DataOutputStream::~DataOutputStream(){
}

int DataOutputStream::write(const char *buffer,int length){
	return mOut->write(buffer,length);
}

void DataOutputStream::close(){
	mOut->close();
}

int DataOutputStream::writeBool(bool b){
	return mOut->write((char*)&b,1);
}

int DataOutputStream::writeUInt8(uint8 i){
	return mOut->write((char*)&i,sizeof(i));
}

int DataOutputStream::writeInt8(int8 i){
	return mOut->write((char*)&i,sizeof(i));
}

int DataOutputStream::writeNullTerminatedString(const String &s){
	return mOut->write(s.c_str(),s.length()+1);
}

int DataOutputStream::writeBigUInt16(uint16 i){
	bigUInt16InPlace(i);
	return mOut->write((char*)&i,sizeof(i));
}

int DataOutputStream::writeBigInt16(int16 i){
	bigInt16InPlace(i);
	return mOut->write((char*)&i,sizeof(i));
}

int DataOutputStream::writeBigUInt32(uint32 i){
	bigUInt32InPlace(i);
	return mOut->write((char*)&i,sizeof(i));
}

int DataOutputStream::writeBigInt32(int32 i){
	bigInt32InPlace(i);
	return mOut->write((char*)&i,sizeof(i));
}

int DataOutputStream::writeBigUInt64(uint64 i){
	bigUInt64InPlace(i);
	return mOut->write((char*)&i,sizeof(i));
}

int DataOutputStream::writeBigInt64(int64 i){
	bigInt64InPlace(i);
	return mOut->write((char*)&i,sizeof(i));
}

int DataOutputStream::writeBigFloat(float f){
	bigFloatInPlace(f);
	return mOut->write((char*)&f,sizeof(f));
}

int DataOutputStream::writeBigDouble(double d){
	bigDoubleInPlace(d);
	return mOut->write((char*)&d,sizeof(d));
}

int DataOutputStream::writeBigInt16String(const String &s){
	int len=s.length();
	if(len>Extents::MAX_SHORT){
		Error::unknown(Categories::TOADLET_EGG,
			"DataOutputStream::writeBigInt16String: String too large");
		return 0;
	}
	int amt=writeBigInt16(len);
	amt+=mOut->write(s.c_str(),len);
	return amt;
}

int DataOutputStream::writeLittleUInt16(uint16 i){
	littleUInt16InPlace(i);
	return mOut->write((char*)&i,sizeof(i));
}

int DataOutputStream::writeLittleInt16(int16 i){
	littleInt16InPlace(i);
	return mOut->write((char*)&i,sizeof(i));
}

int DataOutputStream::writeLittleUInt32(uint32 i){
	littleUInt32InPlace(i);
	return mOut->write((char*)&i,sizeof(i));
}

int DataOutputStream::writeLittleInt32(int32 i){
	littleInt32InPlace(i);
	return mOut->write((char*)&i,sizeof(i));
}

int DataOutputStream::writeLittleUInt64(uint64 i){
	littleUInt64InPlace(i);
	return mOut->write((char*)&i,sizeof(i));
}

int DataOutputStream::writeLittleInt64(int64 i){
	littleInt64InPlace(i);
	return mOut->write((char*)&i,sizeof(i));
}

int DataOutputStream::writeLittleFloat(float f){
	littleFloatInPlace(f);
	return mOut->write((char*)&f,sizeof(f));
}

int DataOutputStream::writeLittleDouble(double d){
	littleDoubleInPlace(d);
	return mOut->write((char*)&d,sizeof(d));
}

int DataOutputStream::writeLittleInt16String(const String &s){
	int len=s.length();
	if(len>Extents::MAX_SHORT){
		Error::unknown(Categories::TOADLET_EGG,
			"DataOutputStream::writeLittleInt16String: String too large");
		return 0;
	}
	int amt=writeBigInt16(len);
	amt+=mOut->write(s.c_str(),len);
	return amt;
}

}
}
}

