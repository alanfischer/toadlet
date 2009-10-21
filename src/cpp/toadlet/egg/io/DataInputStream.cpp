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

#include <toadlet/egg/io/DataInputStream.h>
#include <toadlet/egg/EndianConversion.h>

namespace toadlet{
namespace egg{
namespace io{

DataInputStream::DataInputStream(InputStream *in){
	mIn=in;
}

DataInputStream::DataInputStream(InputStream::ptr in){
	mIn=in;
	mInPtr=in;
}

DataInputStream::~DataInputStream(){
}

int DataInputStream::read(char *buffer,int length){
	return mIn->read(buffer,length);
}

bool DataInputStream::reset(){
	return mIn->reset();
}

bool DataInputStream::seek(int offs){
	return mIn->seek(offs);
}

int DataInputStream::available(){
	return mIn->available();
}

void DataInputStream::close(){
	mIn->close();
}

int DataInputStream::readBool(bool &b){
	return mIn->read((char*)&b,1);
}

bool DataInputStream::readBool(){
	bool b=false;
	mIn->read((char*)&b,1);
	return b;
}

int DataInputStream::readUInt8(uint8 &i){
	return mIn->read((char*)&i,sizeof(i));
}

uint8 DataInputStream::readUInt8(){
	uint8 i=0;
	mIn->read((char*)&i,sizeof(i));
	return i;
}

int DataInputStream::readInt8(int8 &i){
	return mIn->read((char*)&i,sizeof(i));
}

int8 DataInputStream::readInt8(){
	int8 i=0;
	mIn->read((char*)&i,sizeof(i));
	return i;
}

int DataInputStream::readNullTerminatedString(String &s){
	int amt=0;
	char string[1024];
	int i=0;
	while(true){
		amt+=mIn->read(&string[i],1);
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

int DataInputStream::readBigUInt16(uint16 &i){
	int amt=mIn->read((char*)&i,sizeof(i));
	bigUInt16InPlace(i);
	return amt;
}

uint16 DataInputStream::readBigUInt16(){
	uint16 i=0;
	mIn->read((char*)&i,sizeof(i));
	bigUInt16InPlace(i);
	return i;
}

int DataInputStream::readBigInt16(int16 &i){
	int amt=mIn->read((char*)&i,sizeof(i));
	bigInt16InPlace(i);
	return amt;
}

int16 DataInputStream::readBigInt16(){
	int16 i=0;
	mIn->read((char*)&i,sizeof(i));
	bigInt16InPlace(i);
	return i;
}

int DataInputStream::readBigUInt32(uint32 &i){
	int amt=mIn->read((char*)&i,sizeof(i));
	bigUInt32InPlace(i);
	return amt;
}

uint32 DataInputStream::readBigUInt32(){
	uint32 i=0;
	mIn->read((char*)&i,sizeof(i));
	bigUInt32InPlace(i);
	return i;
}

int DataInputStream::readBigInt32(int32 &i){
	int amt=mIn->read((char*)&i,sizeof(i));
	bigInt32InPlace(i);
	return amt;
}

int32 DataInputStream::readBigInt32(){
	int32 i=0;
	mIn->read((char*)&i,sizeof(i));
	bigInt32InPlace(i);
	return i;
}

int DataInputStream::readBigUInt64(uint64 &i){
	int amt=mIn->read((char*)&i,sizeof(i));
	bigUInt64InPlace(i);
	return amt;
}

uint64 DataInputStream::readBigUInt64(){
	uint64 i=0;
	mIn->read((char*)&i,sizeof(i));
	bigUInt64InPlace(i);
	return i;
}

int DataInputStream::readBigInt64(int64 &i){
	int amt=mIn->read((char*)&i,sizeof(i));
	bigInt64InPlace(i);
	return amt;
}

int64 DataInputStream::readBigInt64(){
	int64 i=0;
	mIn->read((char*)&i,sizeof(i));
	bigInt64InPlace(i);
	return i;
}

int DataInputStream::readBigFloat(float &f){
	int amt=mIn->read((char*)&f,sizeof(f));
	bigFloatInPlace(f);
	return amt;
}

float DataInputStream::readBigFloat(){
	float f=0;
	mIn->read((char*)&f,sizeof(f));
	bigFloatInPlace(f);
	return f;
}

int DataInputStream::readBigDouble(double &d){
	int amt=mIn->read((char*)&d,sizeof(d));
	bigDoubleInPlace(d);
	return amt;
}

double DataInputStream::readBigDouble(){
	double d=0;
	mIn->read((char*)&d,sizeof(d));
	bigDoubleInPlace(d);
	return d;
}

int DataInputStream::readBigInt16String(String &s){
	int16 len=readBigInt16();
	int amt=2;
	char *string=new char[len+1];
	amt+=mIn->read(string,len);
	string[len]=0;
	s=string;
	delete[] string;
	return amt;
}

int DataInputStream::readLittleUInt16(uint16 &i){
	int amt=mIn->read((char*)&i,sizeof(i));
	littleUInt16InPlace(i);
	return amt;
}

uint16 DataInputStream::readLittleUInt16(){
	uint16 i=0;
	mIn->read((char*)&i,sizeof(i));
	littleUInt16InPlace(i);
	return i;
}

int DataInputStream::readLittleInt16(int16 &i){
	int amt=mIn->read((char*)&i,sizeof(i));
	littleInt16InPlace(i);
	return amt;
}

int16 DataInputStream::readLittleInt16(){
	int16 i=0;
	mIn->read((char*)&i,sizeof(i));
	littleInt16InPlace(i);
	return i;
}

int DataInputStream::readLittleUInt32(uint32 &i){
	int amt=mIn->read((char*)&i,sizeof(i));
	littleUInt32InPlace(i);
	return amt;
}

uint32 DataInputStream::readLittleUInt32(){
	uint32 i=0;
	mIn->read((char*)&i,sizeof(i));
	littleUInt32InPlace(i);
	return i;
}

int DataInputStream::readLittleInt32(int32 &i){
	int amt=mIn->read((char*)&i,sizeof(i));
	littleInt32InPlace(i);
	return amt;
}

int32 DataInputStream::readLittleInt32(){
	int32 i=0;
	mIn->read((char*)&i,sizeof(i));
	littleInt32InPlace(i);
	return i;
}

int DataInputStream::readLittleUInt64(uint64 &i){
	int amt=mIn->read((char*)&i,sizeof(i));
	littleUInt64InPlace(i);
	return amt;
}

uint64 DataInputStream::readLittleUInt64(){
	uint64 i=0;
	mIn->read((char*)&i,sizeof(i));
	littleUInt64InPlace(i);
	return i;
}

int DataInputStream::readLittleInt64(int64 &i){
	int amt=mIn->read((char*)&i,sizeof(i));
	littleInt64InPlace(i);
	return amt;
}

int64 DataInputStream::readLittleInt64(){
	int64 i=0;
	mIn->read((char*)&i,sizeof(i));
	littleInt64InPlace(i);
	return i;
}

int DataInputStream::readLittleFloat(float &f){
	int amt=mIn->read((char*)&f,sizeof(f));
	littleFloatInPlace(f);
	return amt;
}

float DataInputStream::readLittleFloat(){
	float f=0;
	mIn->read((char*)&f,sizeof(f));
	littleFloatInPlace(f);
	return f;
}

int DataInputStream::readLittleDouble(double &d){
	int amt=mIn->read((char*)&d,sizeof(d));
	littleDoubleInPlace(d);
	return amt;
}

double DataInputStream::readLittleDouble(){
	double d=0;
	mIn->read((char*)&d,sizeof(d));
	littleDoubleInPlace(d);
	return d;
}

int DataInputStream::readLittleInt16String(String &s){
	int16 len=readLittleInt16();
	int amt=2;
	char *string=new char[len+1];
	amt+=mIn->read(string,len);
	string[len]=0;
	s=string;
	delete[] string;
	return amt;
}

}
}
}

