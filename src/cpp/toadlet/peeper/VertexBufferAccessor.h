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
#include <toadlet/peeper/Color.h>
#if defined(TOADLET_DEBUG)
	#include <toadlet/egg/Error.h>
#endif

namespace toadlet{
namespace peeper{

class TOADLET_API VertexBufferAccessor{
public:
	VertexBufferAccessor();
	VertexBufferAccessor(VertexBuffer *vertexBuffer,Buffer::AccessType accessType=Buffer::AccessType_READ_WRITE);

	virtual ~VertexBufferAccessor();

	void lock(VertexBuffer *vertexBuffer,Buffer::AccessType accessType=Buffer::AccessType_READ_WRITE);
	void unlock();

	inline int getSize() const{return mVertexBuffer->getSize();}

	inline uint8 *getData(){return mData;}

	inline void set(int i,int e,scalar x){
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=toFixed(x);
		}
		else{
			mFloatData[i]=toFloat(x);
		}
	}

	inline void set2(int i,int e,scalar x,scalar y){
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=toFixed(x);
			mFixedData[i+1]=toFixed(y);
		}
		else{
			mFloatData[i]=toFloat(x);
			mFloatData[i+1]=toFloat(y);
		}
	}

	inline void set3(int i,int e,scalar x,scalar y,scalar z){
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=toFixed(x);
			mFixedData[i+1]=toFixed(y);
			mFixedData[i+2]=toFixed(z);
		}
		else{
			mFloatData[i]=toFloat(x);
			mFloatData[i+1]=toFloat(y);
			mFloatData[i+2]=toFloat(z);
		}
	}

	inline void set4(int i,int e,scalar x,scalar y,scalar z,scalar w){
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=toFixed(x);
			mFixedData[i+1]=toFixed(y);
			mFixedData[i+2]=toFixed(z);
			mFixedData[i+3]=toFixed(w);
		}
		else{
			mFloatData[i]=toFloat(x);
			mFloatData[i+1]=toFloat(y);
			mFloatData[i+2]=toFloat(z);
			mFloatData[i+3]=toFloat(w);
		}
	}

	inline void set2(int i,int e,scalar x[]){
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=toFixed(x[0]);
			mFixedData[i+1]=toFixed(x[1]);
		}
		else{
			mFloatData[i]=toFloat(x[0]);
			mFloatData[i+1]=toFloat(x[1]);
		}
	}

	inline void set3(int i,int e,scalar x[]){
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=toFixed(x[0]);
			mFixedData[i+1]=toFixed(x[1]);
			mFixedData[i+2]=toFixed(x[2]);
		}
		else{
			mFloatData[i]=toFloat(x[0]);
			mFloatData[i+1]=toFloat(x[1]);
			mFloatData[i+2]=toFloat(x[2]);
		}
	}

	inline void set4(int i,int e,scalar x[]){
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=toFixed(x[0]);
			mFixedData[i+1]=toFixed(x[1]);
			mFixedData[i+2]=toFixed(x[2]);
			mFixedData[i+3]=toFixed(x[3]);
		}
		else{
			mFloatData[i]=toFloat(x[0]);
			mFloatData[i+1]=toFloat(x[1]);
			mFloatData[i+2]=toFloat(x[2]);
			mFloatData[i+3]=toFloat(x[3]);
		}
	}

	inline void set2(int i,int e,const Vector2 &v){
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=toFixed(v.x);
			mFixedData[i+1]=toFixed(v.y);
		}
		else{
			mFloatData[i]=toFloat(v.x);
			mFloatData[i+1]=toFloat(v.y);
		}
	}

	inline void set3(int i,int e,const Vector3 &v){
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=toFixed(v.x);
			mFixedData[i+1]=toFixed(v.y);
			mFixedData[i+2]=toFixed(v.z);
		}
		else{
			mFloatData[i]=toFloat(v.x);
			mFloatData[i+1]=toFloat(v.y);
			mFloatData[i+2]=toFloat(v.z);
		}
	}

	inline void set4(int i,int e,const Vector4 &v){
		i=offset(i,e);
		if(mNativeFixed){
			mFixedData[i]=toFixed(v.x);
			mFixedData[i+1]=toFixed(v.y);
			mFixedData[i+2]=toFixed(v.z);
			mFixedData[i+3]=toFixed(v.w);
		}
		else{
			mFloatData[i]=toFloat(v.x);
			mFloatData[i+1]=toFloat(v.y);
			mFloatData[i+2]=toFloat(v.z);
			mFloatData[i+3]=toFloat(v.w);
		}
	}

	inline void set(int i,int e,const Color &c){
		mColorData[offset(i,e)]=c.getABGR();
	}

	inline void setRGBA(int i,int e,uint32 c){
		mColorData[offset(i,e)]=Color::swap(c);
	}

	inline void setABGR(int i,int e,uint32 c){
		mColorData[offset(i,e)]=c;
	}

	inline scalar get(int i,int e){
		i=offset(i,e);
		if(mNativeFixed){
			return fromFixed(mFixedData[i]);
		}
		else{
			return fromFloat(mFloatData[i]);
		}
	}

	inline void get2(int i,int e,scalar x[]){
		i=offset(i,e);
		if(mNativeFixed){
			x[0]=fromFixed(mFixedData[i]);
			x[1]=fromFixed(mFixedData[i+1]);
		}
		else{
			x[0]=fromFloat(mFloatData[i]);
			x[1]=fromFloat(mFloatData[i+1]);
		}
	}

	inline void get3(int i,int e,scalar x[]){
		i=offset(i,e);
		if(mNativeFixed){
			x[0]=fromFixed(mFixedData[i]);
			x[1]=fromFixed(mFixedData[i+1]);
			x[2]=fromFixed(mFixedData[i+2]);
		}
		else{
			x[0]=fromFloat(mFloatData[i]);
			x[1]=fromFloat(mFloatData[i+1]);
			x[2]=fromFloat(mFloatData[i+2]);
		}
	}

	inline void get4(int i,int e,scalar x[]){
		i=offset(i,e);
		if(mNativeFixed){
			x[0]=fromFixed(mFixedData[i]);
			x[1]=fromFixed(mFixedData[i+1]);
			x[2]=fromFixed(mFixedData[i+2]);
			x[3]=fromFixed(mFixedData[i+3]);
		}
		else{
			x[0]=fromFloat(mFloatData[i]);
			x[1]=fromFloat(mFloatData[i+1]);
			x[2]=fromFloat(mFloatData[i+2]);
			x[3]=fromFloat(mFloatData[i+3]);
		}
	}

	inline void get2(int i,int e,Vector2 &v){
		i=offset(i,e);
		if(mNativeFixed){
			v.x=fromFixed(mFixedData[i]);
			v.y=fromFixed(mFixedData[i+1]);
		}
		else{
			v.x=fromFloat(mFloatData[i]);
			v.y=fromFloat(mFloatData[i+1]);
		}
	}

	inline void get3(int i,int e,Vector3 &v){
		i=offset(i,e);
		if(mNativeFixed){
			v.x=fromFixed(mFixedData[i]);
			v.y=fromFixed(mFixedData[i+1]);
			v.z=fromFixed(mFixedData[i+2]);
		}
		else{
			v.x=fromFloat(mFloatData[i]);
			v.y=fromFloat(mFloatData[i+1]);
			v.z=fromFloat(mFloatData[i+2]);
		}
	}

	inline void get4(int i,int e,Vector4 &v){
		i=offset(i,e);
		if(mNativeFixed){
			v.x=fromFixed(mFixedData[i]);
			v.y=fromFixed(mFixedData[i+1]);
			v.z=fromFixed(mFixedData[i+2]);
			v.w=fromFixed(mFixedData[i+3]);
		}
		else{
			v.x=fromFloat(mFloatData[i]);
			v.y=fromFloat(mFloatData[i+1]);
			v.z=fromFloat(mFloatData[i+2]);
			v.w=fromFloat(mFloatData[i+3]);
		}
	}

	inline void get(int i,int e,Color &c){
		c.setABGR(mColorData[offset(i,e)]);
	}

	inline uint32 getRGBA(int i,int e){
		return Color::swap(mColorData[offset(i,e)]);
	}

	inline uint32 getABGR(int i,int e){
		return mColorData[offset(i,e)];
	}

protected:
	#if defined(TOADLET_FIXED_POINT)
		static inline egg::mathfixed::fixed toFixed(scalar s){return s;}
		static inline float toFloat(scalar s){return egg::mathfixed::Math::toFloat(s);}
		static inline scalar fromFixed(egg::mathfixed::fixed x){return x;}
		static inline scalar fromFloat(float s){return egg::mathfixed::Math::fromFloat(s);}
	#else
		static inline egg::mathfixed::fixed toFixed(scalar s){return egg::mathfixed::Math::fromFloat(s);}
		static inline float toFloat(scalar s){return s;}
		static inline scalar fromFixed(egg::mathfixed::fixed x){return egg::mathfixed::Math::toFloat(x);}
		static inline scalar fromFloat(float s){return s;}
	#endif

	inline int offset(int vertex,int element){
		#if defined(TOADLET_DEBUG)
			if(vertex<0 || element<0 || (vertex*mVertexSize32 + mElementOffsets32[element])*sizeof(int32)>=(uint32)mVertexBuffer->getDataSize()){
				egg::Error::unknown(egg::Categories::TOADLET_PEEPER,"vertex out of bounds");
				return 0;
			}
		#endif
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

}
}

#endif
