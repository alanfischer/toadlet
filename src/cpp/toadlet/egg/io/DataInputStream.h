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

#ifndef TOADLET_EGG_IO_DATAINPUTSTREAMS_H
#define TOADLET_EGG_IO_DATAINPUTSTREAMS_H

#include <toadlet/egg/io/InputStream.h>
#include <toadlet/egg/String.h>

namespace toadlet{
namespace egg{
namespace io{

class TOADLET_API DataInputStream:public InputStream{
public:
	TOADLET_SHARED_POINTERS(DataInputStream);

	DataInputStream(InputStream *in);
	DataInputStream(InputStream::ptr in);
	virtual ~DataInputStream();

	virtual int read(char *buffer,int length);
	virtual bool reset();
	virtual bool seek(int offs);
	virtual int available();
	virtual void close();

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

protected:
	InputStream *mIn;
	InputStream::ptr mInPtr;
};

}
}
}

#endif

