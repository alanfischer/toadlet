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

#ifndef TOADLET_EGG_IO_DATAOUTPUTSTREAMS_H
#define TOADLET_EGG_IO_DATAOUTPUTSTREAMS_H

#include <toadlet/egg/io/OutputStream.h>
#include <toadlet/egg/String.h>

namespace toadlet{
namespace egg{
namespace io{

class TOADLET_API DataOutputStream:public OutputStream{
public:
	TOADLET_SHARED_POINTERS(DataOutputStream);

	DataOutputStream(OutputStream *out);
	DataOutputStream(OutputStream::ptr out);
	virtual ~DataOutputStream();

	virtual int write(const char *buffer,int length);
	virtual void close();

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
	OutputStream *mOut;
	OutputStream::ptr mOutPtr;
};

}
}
}

#endif

