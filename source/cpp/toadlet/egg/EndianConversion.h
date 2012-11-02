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

#ifndef TOADLET_EGG_ENDIANCONVERSION_H
#define TOADLET_EGG_ENDIANCONVERSION_H

#include <toadlet/egg/Types.h>

namespace toadlet{
namespace egg{

inline void swap2(uint16 &swap){
	swap=((((swap) >> 8) & 0xFF) | (((swap) << 8) & 0xFF00));
}

inline void swap4(uint32 &swap){
	swap=((((swap) >> 24) & 0xFF) | ((((swap) & 0xFF0000) >> 8) & 0xFF00) \
		| (((swap) & 0xFF00) << 8) | ((swap) << 24));
}

typedef union{
	uint64 u64;
	uint32 u32[2];
} endianConversion64;

inline void swap8(uint64 &swap){
	endianConversion64 conv;
	uint32 t;
	conv.u64=swap;
	swap4(conv.u32[0]);
	swap4(conv.u32[1]);
	t=conv.u32[0];
	conv.u32[0]=conv.u32[1];
	conv.u32[1]=t;
	swap=conv.u64;
}

#if defined(TOADLET_LITTLE_ENDIAN)
	inline void littleInt16InPlace(int16 &swap)		{}
	inline void littleUInt16InPlace(uint16 &swap)	{}
	inline void littleInt32InPlace(int32 &swap)		{}
	inline void littleUInt32InPlace(uint32 &swap)	{}
	inline void littleInt64InPlace(int64 &swap)		{}
	inline void littleUInt64InPlace(uint64 &swap)	{}
	inline void littleFloatInPlace(float &swap)		{}
	inline void littleDoubleInPlace(double &swap)	{}

	inline void bigInt16InPlace(int16 &swap)		{swap2((uint16&)swap);}
	inline void bigUInt16InPlace(uint16 &swap)		{swap2((uint16&)swap);}
	inline void bigInt32InPlace(int32 &swap)		{swap4((uint32&)swap);}
	inline void bigUInt32InPlace(uint32 &swap)		{swap4((uint32&)swap);}
	inline void bigInt64InPlace(int64 &swap)		{swap8((uint64&)swap);}
	inline void bigUInt64InPlace(uint64 &swap)		{swap8((uint64&)swap);}
	inline void bigFloatInPlace(float &swap)		{swap4((uint32&)swap);}
	inline void bigDoubleInPlace(double &swap)		{swap8((uint64&)swap);}

	inline int16  littleInt16(int16 swap)			{return swap;}
	inline uint16 littleUInt16(uint16 swap)			{return swap;}
	inline int32  littleInt32(int32 swap)			{return swap;}
	inline uint32 littleUInt32(uint32 swap)			{return swap;}
	inline int64  littleInt64(int64 swap)			{return swap;}
	inline uint64 littleUInt64(uint64 swap)			{return swap;}
	inline float  littleFloat(float swap)			{return swap;}
	inline double littleDouble(double swap)			{return swap;}

	inline int16  bigInt16(int16 swap)				{swap2((uint16&)swap);return swap;}
	inline uint16 bigUInt16(uint16 swap)			{swap2((uint16&)swap);return swap;}
	inline int32  bigInt32(int32 swap)				{swap4((uint32&)swap);return swap;}
	inline uint32 bigUInt32(uint32 swap)			{swap4((uint32&)swap);return swap;}
	inline int64  bigInt64(int64 swap)				{swap8((uint64&)swap);return swap;}
	inline uint64 bigUInt64(uint64 swap)			{swap8((uint64&)swap);return swap;}
	inline float  bigFloat(float swap)				{swap4((uint32&)swap);return swap;}
	inline double bigDouble(double swap)			{swap8((uint64&)swap);return swap;}
#elif defined(TOADLET_BIG_ENDIAN)
	inline void bigInt16InPlace(int16 &swap)		{}
	inline void bigUInt16InPlace(uint16 &swap)		{}
	inline void bigInt32InPlace(int32 &swap)		{}
	inline void bigUInt32InPlace(uint32 &swap)		{}
	inline void bigInt64InPlace(int64 &swap)		{}
	inline void bigUInt64InPlace(uint64 &swap)		{}
	inline void bigFloatInPlace(float &swap)		{}
	inline void bigDoubleInPlace(double &swap)		{}

	inline void littleInt16InPlace(int16 &swap)		{swap2((uint16&)swap);}
	inline void littleUInt16InPlace(uint16 &swap)	{swap2((uint16&)swap);}
	inline void littleInt32InPlace(int32 &swap)		{swap4((uint32&)swap);}
	inline void littleUInt32InPlace(uint32 &swap)	{swap4((uint32&)swap);}
	inline void littleInt64InPlace(int64 &swap)		{swap8((uint64&)swap);}
	inline void littleUInt64InPlace(uint64 &swap)	{swap8((uint64&)swap);}
	inline void littleFloatInPlace(float &swap)		{swap4((uint32&)swap);}
	inline void littleDoubleInPlace(double &swap)	{swap8((uint64&)swap);}

	inline int16  bigInt16(int16 swap)				{return swap;}
	inline uint16 bigUInt16(uint16 swap)			{return swap;}
	inline int32  bigInt32(int32 swap)				{return swap;}
	inline uint32 bigUInt32(uint32 swap)			{return swap;}
	inline int64  bigInt64(int64 swap)				{return swap;}
	inline uint64 bigUInt64(uint64 swap)			{return swap;}
	inline float  bigFloat(float swap)				{return swap;}
	inline double bigDouble(double swap)			{return swap;}

	inline int16  littleInt16(int16 swap)			{swap2((uint16&)swap);return swap;}
	inline uint16 littleUInt16(uint16 swap)			{swap2((uint16&)swap);return swap;}
	inline int32  littleInt32(int32 swap)			{swap4((uint32&)swap);return swap;}
	inline uint32 littleUInt32(uint32 swap)			{swap4((uint32&)swap);return swap;}
	inline int64  littleInt64(int64 swap)			{swap8((uint64&)swap);return swap;}
	inline uint64 littleUInt64(uint64 swap)			{swap8((uint64&)swap);return swap;}
	inline float  littleFloat(float swap)			{swap4((uint32&)swap);return swap;}
	inline double littleDouble(double swap)			{swap8((uint64&)swap);return swap;}
#else
	#error "Unknown Endian"
#endif

}
}

#endif

