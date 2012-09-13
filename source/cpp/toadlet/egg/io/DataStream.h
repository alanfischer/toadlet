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
#include <toadlet/egg/Object.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/mathfixed/Math.h>

namespace toadlet{
namespace egg{
namespace io{

/// @todo: This should be implemented cleaner and more optimized, with more direct writing of structures and then converting if necessary
class TOADLET_API DataStream:public Object,public Stream{
public:
	TOADLET_OBJECT(DataStream);

	DataStream(Stream *stream);
	virtual ~DataStream();

	virtual void close(){mStream->close();}
	virtual bool closed(){return mStream->closed();}

	virtual bool readable(){return mStream->readable();}
	virtual int read(tbyte *buffer,int length);
	
	virtual bool writeable(){return mStream->writeable();}
	virtual int write(const tbyte *buffer,int length){return mStream->write(buffer,length);}

	virtual bool reset(){return mStream->reset();}
	virtual int length(){return mStream->length();}
	virtual int position(){return mStream->position();}
	virtual bool seek(int offs){return mStream->seek(offs);}

	// read
	bool readBool();
	uint8 readUInt8();
	int8 readInt8();
	int readAll(tbyte *&resultBuffer);

	int readNullTerminatedString(String &s);
	String readNullTerminatedString();

	int readAllString(String &s);
	String readAllString();

	// read Big
	uint16 readBUInt16();
	int16 readBInt16();
	uint32 readBUInt32();
	int32 readBInt32();
	uint64 readBUInt64();
	int64 readBInt64();
	float readBFloat();
	double readBDouble();

	int readBVector2(math::Vector2 &v);
	int readBVector3(math::Vector3 &v);
	int readBVector4(math::Vector4 &v);
	int readBQuaternion(math::Quaternion &q);
	int readBAABox(math::AABox &a);
	int readBSphere(math::Sphere &s);

	int readBVector2(mathfixed::Vector2 &v);
	int readBVector3(mathfixed::Vector3 &v);
	int readBVector4(mathfixed::Vector4 &v);
	int readBQuaternion(mathfixed::Quaternion &q);
	int readBAABox(mathfixed::AABox &a);
	int readBSphere(mathfixed::Sphere &s);

	int readBInt16String(String &s);
	String readBInt16String();

	// read Little
	uint16 readLUInt16();
	int16 readLInt16();
	uint32 readLUInt32();
	int32 readLInt32();
	uint64 readLUInt64();
	int64 readLInt64();
	float readLFloat();
	double readLDouble();

	int readLVector2(math::Vector2 &v);
	int readLVector3(math::Vector3 &v);
	int readLVector4(math::Vector4 &v);
	int readLQuaternion(math::Quaternion &q);
	int readLAABox(math::AABox &a);
	int readLSphere(math::Sphere &s);

	int readLVector2(mathfixed::Vector2 &v);
	int readLVector3(mathfixed::Vector3 &v);
	int readLVector4(mathfixed::Vector4 &v);
	int readLQuaternion(mathfixed::Quaternion &q);
	int readLAABox(mathfixed::AABox &a);
	int readLSphere(mathfixed::Sphere &s);

	int readLInt16String(String &s);
	String readLInt16String();

	// read Native
#if defined(TOADLET_LITTLE_ENDIAN)
	inline uint16 readUInt16()	{return readLUInt16();}
	inline int16 readInt16()	{return readLInt16();}
	inline uint32 readUInt32()	{return readLUInt32();}
	inline int32 readInt32()	{return readLInt32();}
	inline uint64 readUInt64()	{return readLUInt64();}
	inline int64 readInt64()	{return readLInt64();}
	inline float readFloat()	{return readLFloat();}
	inline double readDouble()	{return readLDouble();}

	inline int readVector2(math::Vector2 &v)		{return readLVector2(v);}
	inline int readVector3(math::Vector3 &v)		{return readLVector3(v);}
	inline int readVector4(math::Vector4 &v)		{return readLVector4(v);}
	inline int readQuaternion(math::Quaternion &q){return readLQuaternion(q);}
	inline int readAABox(math::AABox &a)			{return readLAABox(a);}
	inline int readSphere(math::Sphere &s)		{return readLSphere(s);}

	inline int readVector2(mathfixed::Vector2 &v){return readLVector2(v);}
	inline int readVector3(mathfixed::Vector3 &v){return readLVector3(v);}
	inline int readVector4(mathfixed::Vector4 &v){return readLVector4(v);}
	inline int readQuaternion(mathfixed::Quaternion &q){return readLQuaternion(q);}
	inline int readAABox(mathfixed::AABox &a)	{return readLAABox(a);}
	inline int readSphere(mathfixed::Sphere &s)	{return readLSphere(s);}

	inline int readInt16String(String &s){return readLInt16String(s);}
	inline String readInt16String(){return readLInt16String();}
#else
	inline uint16 readUInt16()	{return readBUInt16();}
	inline int16 readInt16()	{return readBInt16();}
	inline uint32 readUInt32()	{return readBUInt32();}
	inline int32 readInt32()	{return readBInt32();}
	inline uint64 readUInt64()	{return readBUInt64();}
	inline int64 readInt64()	{return readBInt64();}
	inline float readFloat()	{return readBFloat();}
	inline double readDouble()	{return readBDouble();}

	inline int readVector2(math::Vector2 &v)		{return readBVector2(v);}
	inline int readVector3(math::Vector3 &v)		{return readBVector3(v);}
	inline int readVector4(math::Vector4 &v)		{return readBVector4(v);}
	inline int readQuaternion(math::Quaternion &q){return readBQuaternion(q);}
	inline int readAABox(math::AABox &a)			{return readBAABox(a);}
	inline int readSphere(math::Sphere &s)		{return readBSphere(s);}

	inline int readVector2(mathfixed::Vector2 &v){return readBVector2(v);}
	inline int readVector3(mathfixed::Vector3 &v){return readBVector3(v);}
	inline int readVector4(mathfixed::Vector4 &v){return readBVector4(v);}
	inline int readQuaternion(mathfixed::Quaternion &q){return readBQuaternion(q);}
	inline int readAABox(mathfixed::AABox &a)	{return readBAABox(a);}
	inline int readSphere(mathfixed::Sphere &s)	{return readBSphere(s);}

	inline int readInt16String(String &s){return readBInt16String(s);}
	inline String readInt16String(){return readBInt16String();}
#endif

	// write
	int writeBool(bool b);
	int writeUInt8(uint8 i);
	int writeInt8(int8 i);
	int writeNullTerminatedString(const String &s);

	// write Big
	int writeBUInt16(uint16 i);
	int writeBInt16(int16 i);
	int writeBUInt32(uint32 i);
	int writeBInt32(int32 i);
	int writeBUInt64(uint64 i);
	int writeBInt64(int64 i);
	int writeBFloat(float f);
	int writeBDouble(double d);

	int writeBVector2(const math::Vector2 &v);
	int writeBVector3(const math::Vector3 &v);
	int writeBVector4(const math::Vector4 &v);
	int writeBQuaternion(const math::Quaternion &q);
	int writeBAABox(const math::AABox &a);
	int writeBSphere(const math::Sphere &s);

	int writeBVector2(const mathfixed::Vector2 &v);
	int writeBVector3(const mathfixed::Vector3 &v);
	int writeBVector4(const mathfixed::Vector4 &v);
	int writeBQuaternion(const mathfixed::Quaternion &q);
	int writeBAABox(const mathfixed::AABox &a);
	int writeBSphere(const mathfixed::Sphere &s);

	int writeBInt16String(const String &s);

	// write Little
	int writeLUInt16(uint16 i);
	int writeLInt16(int16 i);
	int writeLUInt32(uint32 i);
	int writeLInt32(int32 i);
	int writeLUInt64(uint64 i);
	int writeLInt64(int64 i);
	int writeLFloat(float f);
	int writeLDouble(double d);

	int writeLVector2(const math::Vector2 &v);
	int writeLVector3(const math::Vector3 &v);
	int writeLVector4(const math::Vector4 &v);
	int writeLQuaternion(const math::Quaternion &q);
	int writeLAABox(const math::AABox &a);
	int writeLSphere(const math::Sphere &s);

	int writeLVector2(const mathfixed::Vector2 &v);
	int writeLVector3(const mathfixed::Vector3 &v);
	int writeLVector4(const mathfixed::Vector4 &v);
	int writeLQuaternion(const mathfixed::Quaternion &q);
	int writeLAABox(const mathfixed::AABox &a);
	int writeLSphere(const mathfixed::Sphere &s);

	int writeLInt16String(const String &s);

	// write Native
#if defined(TOADLET_LITTLE_ENDIAN)
	inline int writeUInt16(uint16 i){return writeLUInt16(i);}
	inline int writeInt16(int16 i)	{return writeLInt16(i);}
	inline int writeUInt32(uint32 i){return writeLUInt32(i);}
	inline int writeInt32(int32 i)	{return writeLInt32(i);}
	inline int writeUInt64(uint64 i){return writeLUInt64(i);}
	inline int writeInt64(int64 i)	{return writeLInt64(i);}
	inline int writeFloat(float f)	{return writeLFloat(f);}
	inline int writeDouble(double d){return writeLDouble(d);}

	inline int writeVector2(const math::Vector2 &v)		{return writeLVector2(v);}
	inline int writeVector3(const math::Vector3 &v)		{return writeLVector3(v);}
	inline int writeVector4(const math::Vector4 &v)		{return writeLVector4(v);}
	inline int writeQuaternion(const math::Quaternion &q){return writeLQuaternion(q);}
	inline int writeAABox(const math::AABox &a)			{return writeLAABox(a);}
	inline int writeSphere(const math::Sphere &s)		{return writeLSphere(s);}

	inline int writeVector2(const mathfixed::Vector2 &v){return writeLVector2(v);}
	inline int writeVector3(const mathfixed::Vector3 &v){return writeLVector3(v);}
	inline int writeVector4(const mathfixed::Vector4 &v){return writeLVector4(v);}
	inline int writeQuaternion(const mathfixed::Quaternion &q){return writeLQuaternion(q);}
	inline int writeAABox(const mathfixed::AABox &a)	{return writeLAABox(a);}
	inline int writeSphere(const mathfixed::Sphere &s)	{return writeLSphere(s);}

	inline int writeInt16String(const String &s){return writeLInt16String(s);}
#else
	inline int writeUInt16(uint16 i){return writeBUInt16(i);}
	inline int writeInt16(int16 i)	{return writeBInt16(i);}
	inline int writeUInt32(uint32 i){return writeBUInt32(i);}
	inline int writeInt32(int32 i)	{return writeBInt32(i);}
	inline int writeUInt64(uint64 i){return writeBUInt64(i);}
	inline int writeInt64(int64 i)	{return writeBInt64(i);}
	inline int writeFloat(float f)	{return writeBFloat(f);}
	inline int writeDouble(double d){return writeBDouble(d);}

	inline int writeVector2(const math::Vector2 &v)		{return writeBVector2(v);}
	inline int writeVector3(const math::Vector3 &v)		{return writeBVector3(v);}
	inline int writeVector4(const math::Vector4 &v)		{return writeBVector4(v);}
	inline int writeQuaternion(const math::Quaternion &q){return writeBQuaternion(q);}
	inline int writeAABox(const math::AABox &a)			{return writeBAABox(a);}
	inline int writeSphere(const math::Sphere &s)		{return writeBSphere(s);}

	inline int writeVector2(const mathfixed::Vector2 &v){return writeBVector2(v);}
	inline int writeVector3(const mathfixed::Vector3 &v){return writeBVector3(v);}
	inline int writeVector4(const mathfixed::Vector4 &v){return writeBVector4(v);}
	inline int writeQuaternion(const mathfixed::Quaternion &q){return writeBQuaternion(q);}
	inline int writeAABox(const mathfixed::AABox &a)	{return writeBAABox(a);}
	inline int writeSphere(const mathfixed::Sphere &s)	{return writeBSphere(s);}

	inline int writeInt16String(const String &s){return writeBInt16String(s);}
#endif

protected:
	Stream::ptr mStream;
};

}
}
}

#endif

