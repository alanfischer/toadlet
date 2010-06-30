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

#ifndef TOADLET_EGG_IO_DATASTREAMS_H
#define TOADLET_EGG_IO_DATASTREAMS_H

#include <toadlet/egg/io/Stream.h>
#include <toadlet/egg/String.h>

namespace toadlet{
namespace egg{
namespace io{

class TOADLET_API DataStream:public Stream{
public:
	TOADLET_SHARED_POINTERS(DataStream);

	DataStream(Stream *stream);
	DataStream(Stream::ptr stream);
	virtual ~DataStream();

	virtual void close(){mStream->close();}
	virtual bool closed(){return mStream->closed();}

	virtual bool readable(){return mStream->readable();}
	virtual int read(tbyte *buffer,int length){return mStream->read(buffer,length);}

	virtual bool writeable(){return mStream->writeable();}
	virtual int write(const tbyte *buffer,int length){return mStream->write(buffer,length);}

	virtual bool reset(){return mStream->reset();}
	virtual int length(){return mStream->length();}
	virtual int position(){return mStream->position();}
	virtual bool seek(int offs){return mStream->seek(offs);}

	bool readBool();
	int readBool(bool &b);
	uint8 readUInt8();
	int readUInt8(uint8 &i);
	int8 readInt8();
	int readInt8(int8 &i);

	int readNullTerminatedString(String &s);

	int readBigUInt16(uint16 &i);
	uint16 readBigUInt16();
	int readBigInt16(int16 &i);
	int16 readBigInt16();
	int readBigUInt32(uint32 &i);
	uint32 readBigUInt32();
	int readBigInt32(int32 &i);
	int32 readBigInt32();
	int readBigUInt64(uint64 &i);
	uint64 readBigUInt64();
	int readBigInt64(int64 &i);
	int64 readBigInt64();
	int readBigFloat(float &f);
	float readBigFloat();
	int readBigDouble(double &d);
	double readBigDouble();

	int readBigInt16String(String &s);

	int readLittleUInt16(uint16 &i);
	uint16 readLittleUInt16();
	int readLittleInt16(int16 &i);
	int16 readLittleInt16();
	int readLittleUInt32(uint32 &i);
	uint32 readLittleUInt32();
	int readLittleInt32(int32 &i);
	int32 readLittleInt32();
	int readLittleUInt64(uint64 &i);
	uint64 readLittleUInt64();
	int readLittleInt64(int64 &i);
	int64 readLittleInt64();
	int readLittleFloat(float &f);
	float readLittleFloat();
	int readLittleDouble(double &d);
	double readLittleDouble();

	int readLittleInt16String(String &s);

	int writeBool(bool b);
	int writeUInt8(uint8 i);
	int writeInt8(int8 i);
	int writeNullTerminatedString(const String &s);

	int writeBigUInt16(uint16 i);
	int writeBigInt16(int16 i);
	int writeBigUInt32(uint32 i);
	int writeBigInt32(int32 i);
	int writeBigUInt64(uint64 i);
	int writeBigInt64(int64 i);
	int writeBigFloat(float f);
	int writeBigDouble(double d);

	int writeBigInt16String(const String &s);

	int writeLittleUInt16(uint16 i);
	int writeLittleInt16(int16 i);
	int writeLittleUInt32(uint32 i);
	int writeLittleInt32(int32 i);
	int writeLittleUInt64(uint64 i);
	int writeLittleInt64(int64 i);
	int writeLittleFloat(float f);
	int writeLittleDouble(double d);

	int writeLittleInt16String(const String &s);

protected:
	Stream *mStream;
	Stream::ptr mStreamPtr;
};

}
}
}

#endif

