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

#ifndef TOADLET_PEEPER_VERTEXBUFFERACCESSOR_H
#define TOADLET_PEEPER_VERTEXBUFFERACCESSOR_H

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/mathfixed/Math.h>
#include <toadlet/peeper/VertexBuffer.h>

#if defined(TOADLET_FIXED_POINT)
	#define TOADLET_TOFIXED(x) x
	#define TOADLET_TOFLOAT(x) toadlet::egg::mathfixed::Math::toFloat(x)
	#define TOADLET_FROMFIXED(x) x
	#define TOADLET_FROMFLOAT(x) toadlet::egg::mathfixed::Math::fromFloat(x)
#else
	#define TOADLET_TOFIXED(x) toadlet::egg::mathfixed::Math::fromFloat(x)
	#define TOADLET_TOFLOAT(x) x
	#define TOADLET_FROMFIXED(x) toadlet::egg::mathfixed::Math::toFloat(x)
	#define TOADLET_FROMFLOAT(x) x
#endif

#define TOADLET_CHECK_READ() TOADLET_ASSERT(mVertexBuffer->getAccess()&toadlet::peeper::Buffer::Access_BIT_READ);
#define TOADLET_CHECK_WRITE() TOADLET_ASSERT(mVertexBuffer->getAccess()&toadlet::peeper::Buffer::Access_BIT_WRITE);

namespace toadlet{
namespace peeper{

class TOADLET_API VertexBufferAccessor{
public:
	VertexBufferAccessor();
	VertexBufferAccessor(VertexBuffer *vertexBuffer,int access=Buffer::Access_READ_WRITE);

	virtual ~VertexBufferAccessor();

	void lock(VertexBuffer *vertexBuffer,int access=Buffer::Access_READ_WRITE);
	void unlock();

	inline int getSize() const{return mVertexBuffer->getSize();}

	inline uint8 *getData(){return mData;}

	inline void set(int i,int e,scalar x){
		TOADLET_CHECK_WRITE();
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=TOADLET_TOFIXED(x);
		}
		else{
			mFloatData[i]=TOADLET_TOFLOAT(x);
		}
	}

	inline void set2(int i,int e,scalar x,scalar y){
		TOADLET_CHECK_WRITE();
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=TOADLET_TOFIXED(x);
			mFixedData[i+1]=TOADLET_TOFIXED(y);
		}
		else{
			mFloatData[i]=TOADLET_TOFLOAT(x);
			mFloatData[i+1]=TOADLET_TOFLOAT(y);
		}
	}

	inline void set3(int i,int e,scalar x,scalar y,scalar z){
		TOADLET_CHECK_WRITE();
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=TOADLET_TOFIXED(x);
			mFixedData[i+1]=TOADLET_TOFIXED(y);
			mFixedData[i+2]=TOADLET_TOFIXED(z);
		}
		else{
			mFloatData[i]=TOADLET_TOFLOAT(x);
			mFloatData[i+1]=TOADLET_TOFLOAT(y);
			mFloatData[i+2]=TOADLET_TOFLOAT(z);
		}
	}

	inline void set4(int i,int e,scalar x,scalar y,scalar z,scalar w){
		TOADLET_CHECK_WRITE();
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=TOADLET_TOFIXED(x);
			mFixedData[i+1]=TOADLET_TOFIXED(y);
			mFixedData[i+2]=TOADLET_TOFIXED(z);
			mFixedData[i+3]=TOADLET_TOFIXED(w);
		}
		else{
			mFloatData[i]=TOADLET_TOFLOAT(x);
			mFloatData[i+1]=TOADLET_TOFLOAT(y);
			mFloatData[i+2]=TOADLET_TOFLOAT(z);
			mFloatData[i+3]=TOADLET_TOFLOAT(w);
		}
	}

	inline void set2(int i,int e,scalar x[]){
		TOADLET_CHECK_WRITE();
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=TOADLET_TOFIXED(x[0]);
			mFixedData[i+1]=TOADLET_TOFIXED(x[1]);
		}
		else{
			mFloatData[i]=TOADLET_TOFLOAT(x[0]);
			mFloatData[i+1]=TOADLET_TOFLOAT(x[1]);
		}
	}

	inline void set3(int i,int e,scalar x[]){
		TOADLET_CHECK_WRITE();
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=TOADLET_TOFIXED(x[0]);
			mFixedData[i+1]=TOADLET_TOFIXED(x[1]);
			mFixedData[i+2]=TOADLET_TOFIXED(x[2]);
		}
		else{
			mFloatData[i]=TOADLET_TOFLOAT(x[0]);
			mFloatData[i+1]=TOADLET_TOFLOAT(x[1]);
			mFloatData[i+2]=TOADLET_TOFLOAT(x[2]);
		}
	}

	inline void set4(int i,int e,scalar x[]){
		TOADLET_CHECK_WRITE();
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=TOADLET_TOFIXED(x[0]);
			mFixedData[i+1]=TOADLET_TOFIXED(x[1]);
			mFixedData[i+2]=TOADLET_TOFIXED(x[2]);
			mFixedData[i+3]=TOADLET_TOFIXED(x[3]);
		}
		else{
			mFloatData[i]=TOADLET_TOFLOAT(x[0]);
			mFloatData[i+1]=TOADLET_TOFLOAT(x[1]);
			mFloatData[i+2]=TOADLET_TOFLOAT(x[2]);
			mFloatData[i+3]=TOADLET_TOFLOAT(x[3]);
		}
	}

	inline void set2(int i,int e,const Vector2 &v){
		TOADLET_CHECK_WRITE();
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=TOADLET_TOFIXED(v.x);
			mFixedData[i+1]=TOADLET_TOFIXED(v.y);
		}
		else{
			mFloatData[i]=TOADLET_TOFLOAT(v.x);
			mFloatData[i+1]=TOADLET_TOFLOAT(v.y);
		}
	}

	inline void set3(int i,int e,const Vector3 &v){
		TOADLET_CHECK_WRITE();
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=TOADLET_TOFIXED(v.x);
			mFixedData[i+1]=TOADLET_TOFIXED(v.y);
			mFixedData[i+2]=TOADLET_TOFIXED(v.z);
		}
		else{
			mFloatData[i]=TOADLET_TOFLOAT(v.x);
			mFloatData[i+1]=TOADLET_TOFLOAT(v.y);
			mFloatData[i+2]=TOADLET_TOFLOAT(v.z);
		}
	}

	inline void set4(int i,int e,const Vector4 &v){
		TOADLET_CHECK_WRITE();
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=TOADLET_TOFIXED(v.x);
			mFixedData[i+1]=TOADLET_TOFIXED(v.y);
			mFixedData[i+2]=TOADLET_TOFIXED(v.z);
			mFixedData[i+3]=TOADLET_TOFIXED(v.w);
		}
		else{
			mFloatData[i]=TOADLET_TOFLOAT(v.x);
			mFloatData[i+1]=TOADLET_TOFLOAT(v.y);
			mFloatData[i+2]=TOADLET_TOFLOAT(v.z);
			mFloatData[i+3]=TOADLET_TOFLOAT(v.w);
		}
	}

	inline void setRGBA(int i,int e,uint32 c){
		TOADLET_CHECK_WRITE();
		mColorData[offset(i,e)]=c;
	}

	inline void setARGB(int i,int e,uint32 c){
		TOADLET_CHECK_WRITE();
		mColorData[offset(i,e)]=Math::argbToRGBA(c);
	}

	inline scalar get(int i,int e){
		TOADLET_CHECK_READ();
		i=offset(i,e);
		if(mNativeFixed){
			return TOADLET_FROMFIXED(mFixedData[i]);
		}
		else{
			return TOADLET_FROMFLOAT(mFloatData[i]);
		}
	}

	inline void get2(int i,int e,scalar x[]){
		TOADLET_CHECK_READ();
		i=offset(i,e);
		if(mNativeFixed){
			x[0]=TOADLET_FROMFIXED(mFixedData[i]);
			x[1]=TOADLET_FROMFIXED(mFixedData[i+1]);
		}
		else{
			x[0]=TOADLET_FROMFLOAT(mFloatData[i]);
			x[1]=TOADLET_FROMFLOAT(mFloatData[i+1]);
		}
	}

	inline void get3(int i,int e,scalar x[]){
		TOADLET_CHECK_READ();
		i=offset(i,e);
		if(mNativeFixed){
			x[0]=TOADLET_FROMFIXED(mFixedData[i]);
			x[1]=TOADLET_FROMFIXED(mFixedData[i+1]);
			x[2]=TOADLET_FROMFIXED(mFixedData[i+2]);
		}
		else{
			x[0]=TOADLET_FROMFLOAT(mFloatData[i]);
			x[1]=TOADLET_FROMFLOAT(mFloatData[i+1]);
			x[2]=TOADLET_FROMFLOAT(mFloatData[i+2]);
		}
	}

	inline void get4(int i,int e,scalar x[]){
		TOADLET_CHECK_READ();
		i=offset(i,e);
		if(mNativeFixed){
			x[0]=TOADLET_FROMFIXED(mFixedData[i]);
			x[1]=TOADLET_FROMFIXED(mFixedData[i+1]);
			x[2]=TOADLET_FROMFIXED(mFixedData[i+2]);
			x[3]=TOADLET_FROMFIXED(mFixedData[i+3]);
		}
		else{
			x[0]=TOADLET_FROMFLOAT(mFloatData[i]);
			x[1]=TOADLET_FROMFLOAT(mFloatData[i+1]);
			x[2]=TOADLET_FROMFLOAT(mFloatData[i+2]);
			x[3]=TOADLET_FROMFLOAT(mFloatData[i+3]);
		}
	}

	inline void get2(int i,int e,Vector2 &v){
		TOADLET_CHECK_READ();
		i=offset(i,e);
		if(mNativeFixed){
			v.x=TOADLET_FROMFIXED(mFixedData[i]);
			v.y=TOADLET_FROMFIXED(mFixedData[i+1]);
		}
		else{
			v.x=TOADLET_FROMFLOAT(mFloatData[i]);
			v.y=TOADLET_FROMFLOAT(mFloatData[i+1]);
		}
	}

	inline void get3(int i,int e,Vector3 &v){
		TOADLET_CHECK_READ();
		i=offset(i,e);
		if(mNativeFixed){
			v.x=TOADLET_FROMFIXED(mFixedData[i]);
			v.y=TOADLET_FROMFIXED(mFixedData[i+1]);
			v.z=TOADLET_FROMFIXED(mFixedData[i+2]);
		}
		else{
			v.x=TOADLET_FROMFLOAT(mFloatData[i]);
			v.y=TOADLET_FROMFLOAT(mFloatData[i+1]);
			v.z=TOADLET_FROMFLOAT(mFloatData[i+2]);
		}
	}

	inline void get4(int i,int e,Vector4 &v){
		TOADLET_CHECK_READ();
		i=offset(i,e);
		if(mNativeFixed){
			v.x=TOADLET_FROMFIXED(mFixedData[i]);
			v.y=TOADLET_FROMFIXED(mFixedData[i+1]);
			v.z=TOADLET_FROMFIXED(mFixedData[i+2]);
			v.w=TOADLET_FROMFIXED(mFixedData[i+3]);
		}
		else{
			v.x=TOADLET_FROMFLOAT(mFloatData[i]);
			v.y=TOADLET_FROMFLOAT(mFloatData[i+1]);
			v.z=TOADLET_FROMFLOAT(mFloatData[i+2]);
			v.w=TOADLET_FROMFLOAT(mFloatData[i+3]);
		}
	}

	inline uint32 getRGBA(int i,int e){
		TOADLET_CHECK_READ();
		return mColorData[offset(i,e)];
	}

	inline uint32 getARGB(int i,int e){
		TOADLET_CHECK_READ();
		return Math::rgbaToARGB(mColorData[offset(i,e)]);
	}

protected:
	inline int offset(int vertex,int element){
		TOADLET_ASSERT(vertex>=0 && element>=0 && (vertex*mVertexSize32 + mElementOffsets32[element])*sizeof(int32)<(uint32)mVertexBuffer->getDataSize()); // "vertex out of bounds"
		return vertex*mVertexSize32 + mElementOffsets32[element];
	}

	VertexBuffer *mVertexBuffer;
	int mVertexSize32;
	int mElementOffsets32[16];

	bool mNativeFixed;
	uint8 *mData;
	int32 *mFixedData;
	float *mFloatData;
	uint32 *mColorData;
};

#undef TOADLET_CHECK_READ
#undef TOADLET_CHECK_WRITE

}
}

#endif
