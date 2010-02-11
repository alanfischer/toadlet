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

package org.toadlet.peeper;

#include <org/toadlet/peeper/Types.h>

import org.toadlet.egg.*;
import org.toadlet.egg.Error;

public final class VertexBufferAccessor{
	// TODO: Move these to defines
	#if defined(TOADLET_FIXED_POINT)
		protected static fixed toFixed(scalar s){return s;}
		protected static float toFloat(scalar s){return org.toadlet.egg.mathfixed.Math.toFloat(s);}
		protected static scalar fromFixed(fixed x){return x;}
		protected static scalar fromFloat(float s){return org.toadlet.egg.mathfixed.Math.fromFloat(s);}
	#else
		protected static fixed toFixed(scalar s){return org.toadlet.egg.mathfixed.Math.fromFloat(s);}
		protected static float toFloat(scalar s){return s;}
		protected static scalar fromFixed(fixed x){return org.toadlet.egg.mathfixed.Math.toFloat(x);}
		protected static scalar fromFloat(float s){return s;}
	#endif

	public VertexBufferAccessor(){}

	public VertexBufferAccessor(VertexBuffer vertexBuffer,Buffer.LockType lockType){
		lock(vertexBuffer,lockType);
	}

	public void destroy(){
		unlock();
	}

	public void lock(VertexBuffer vertexBuffer,Buffer.LockType lockType){
		unlock();

		mVertexBuffer=vertexBuffer;

		VertexFormat vertexFormat=mVertexBuffer.getVertexFormat();
		mVertexSize32=vertexFormat.getVertexSize()/4;
		assert(vertexFormat.getNumVertexElements()<16/*mElementOffsets*/);
		for(int i=0;i<vertexFormat.getNumVertexElements();++i){
			mElementOffsets32[i]=vertexFormat.getVertexElement(i).offset/4;
		}

		VertexElement position=vertexFormat.getVertexElementOfType(VertexElement.Type.POSITION);
		if((position.format&VertexElement.Format.BIT_FLOAT_32)>0){
			mNativeFloat=true;
		}
		else if((position.format&VertexElement.Format.BIT_FIXED_32)>0){
			mNativeFixed=true;
		}

		mData=mVertexBuffer.lock(lockType);
	}

	public void unlock(){
		if(mVertexBuffer!=null){
			mVertexBuffer.unlock();
			mVertexBuffer=null;
			mData=null;
		}
	}

	public int getSize(){return mVertexBuffer.getSize();}

	public java.nio.ByteBuffer getData(){return mData;}

	public void set(int i,int e,scalar x){
		i=offset(i,e);
		if(mNativeFixed){
			mData.putInt((i+0)*4,toFixed(x));
		}
		else{
			mData.putFloat((i+0)*4,toFloat(x));
		}
	}

	public void set2(int i,int e,scalar x,scalar y){
		i=offset(i,e);
		if(mNativeFixed){
			mData.putInt((i+0)*4,toFixed(x));
			mData.putInt((i+1)*4,toFixed(y));
		}
		else{
			mData.putFloat((i+0)*4,toFloat(x));
			mData.putFloat((i+1)*4,toFloat(y));
		}
	}

	public void set3(int i,int e,scalar x,scalar y,scalar z){
		i=offset(i,e);
		if(mNativeFixed){
			mData.putInt((i+0)*4,toFixed(x));
			mData.putInt((i+1)*4,toFixed(y));
			mData.putInt((i+2)*4,toFixed(z));
		}
		else{
			mData.putFloat((i+0)*4,toFloat(x));
			mData.putFloat((i+1)*4,toFloat(y));
			mData.putFloat((i+2)*4,toFloat(z));
		}
	}

	public void set4(int i,int e,scalar x,scalar y,scalar z,scalar w){
		i=offset(i,e);
		if(mNativeFixed){
			mData.putInt((i+0)*4,toFixed(x));
			mData.putInt((i+1)*4,toFixed(y));
			mData.putInt((i+2)*4,toFixed(z));
			mData.putInt((i+3)*4,toFixed(w));
		}
		else{
			mData.putFloat((i+0)*4,toFloat(x));
			mData.putFloat((i+1)*4,toFloat(y));
			mData.putFloat((i+2)*4,toFloat(z));
			mData.putFloat((i+3)*4,toFloat(w));
		}
	}

	public void set2(int i,int e,scalar x[]){
		i=offset(i,e);
		if(mNativeFixed){
			mData.putInt((i+0)*4,toFixed(x[0]));
			mData.putInt((i+1)*4,toFixed(x[1]));
		}
		else{
			mData.putFloat((i+0)*4,toFloat(x[0]));
			mData.putFloat((i+1)*4,toFloat(x[1]));
		}
	}

	public void set3(int i,int e,scalar x[]){
		i=offset(i,e);
		if(mNativeFixed){
			mData.putInt((i+0)*4,toFixed(x[0]));
			mData.putInt((i+1)*4,toFixed(x[1]));
			mData.putInt((i+2)*4,toFixed(x[2]));
		}
		else{
			mData.putFloat((i+0)*4,toFloat(x[0]));
			mData.putFloat((i+1)*4,toFloat(x[1]));
			mData.putFloat((i+2)*4,toFloat(x[2]));
		}
	}

	public void set4(int i,int e,scalar x[]){
		i=offset(i,e);
		if(mNativeFixed){
			mData.putInt((i+0)*4,toFixed(x[0]));
			mData.putInt((i+1)*4,toFixed(x[1]));
			mData.putInt((i+2)*4,toFixed(x[2]));
			mData.putInt((i+3)*4,toFixed(x[3]));
		}
		else{
			mData.putFloat((i+0)*4,toFloat(x[0]));
			mData.putFloat((i+1)*4,toFloat(x[1]));
			mData.putFloat((i+2)*4,toFloat(x[2]));
			mData.putFloat((i+3)*4,toFloat(x[3]));
		}
	}

	public void set2(int i,int e,Vector2 v){
		i=offset(i,e);
		if(mNativeFixed){
			mData.putInt((i+0)*4,toFixed(v.x));
			mData.putInt((i+1)*4,toFixed(v.y));
		}
		else{
			mData.putFloat((i+0)*4,toFloat(v.x));
			mData.putFloat((i+1)*4,toFloat(v.y));
		}
	}

	public void set3(int i,int e,Vector3 v){
		i=offset(i,e);
		if(mNativeFixed){
			mData.putInt((i+0)*4,toFixed(v.x));
			mData.putInt((i+1)*4,toFixed(v.y));
			mData.putInt((i+2)*4,toFixed(v.z));
		}
		else{
			mData.putFloat((i+0)*4,toFloat(v.x));
			mData.putFloat((i+1)*4,toFloat(v.y));
			mData.putFloat((i+2)*4,toFloat(v.z));
		}
	}

	public void set4(int i,int e,Vector4 v){
		i=offset(i,e);
		if(mNativeFixed){
			mData.putInt((i+0)*4,toFixed(v.x));
			mData.putInt((i+1)*4,toFixed(v.y));
			mData.putInt((i+2)*4,toFixed(v.z));
			mData.putInt((i+3)*4,toFixed(v.w));
		}
		else{
			mData.putFloat((i+0)*4,toFloat(v.x));
			mData.putFloat((i+1)*4,toFloat(v.y));
			mData.putFloat((i+2)*4,toFloat(v.z));
			mData.putFloat((i+3)*4,toFloat(v.w));
		}
	}

	public void setRGBA(int i,int e,int c){
		mData.putInt(offset(i,e)*4,Color.swap(c));
	}

	public void setABGR(int i,int e,int c){
		mData.putInt(offset(i,e)*4,c);
	}

	public scalar get(int i,int e){
		i=offset(i,e);
		if(mNativeFixed){
			return fromFixed(mData.getInt((i+0)*4));
		}
		else{
			return fromFloat(mData.getFloat((i+0)*4));
		}
	}

	public void get2(int i,int e,scalar x[]){
		i=offset(i,e);
		if(mNativeFixed){
			x[0]=fromFixed(mData.getInt((i+0)*4));
			x[1]=fromFixed(mData.getInt((i+1)*4));
		}
		else{
			x[0]=fromFloat(mData.getFloat((i+0)*4));
			x[1]=fromFloat(mData.getFloat((i+1)*4));
		}
	}

	public void get3(int i,int e,scalar x[]){
		i=offset(i,e);
		if(mNativeFixed){
			x[0]=fromFixed(mData.getInt((i+0)*4));
			x[1]=fromFixed(mData.getInt((i+1)*4));
			x[2]=fromFixed(mData.getInt((i+2)*4));
		}
		else{
			x[0]=fromFloat(mData.getFloat((i+0)*4));
			x[1]=fromFloat(mData.getFloat((i+1)*4));
			x[2]=fromFloat(mData.getFloat((i+2)*4));
		}
	}

	public void get4(int i,int e,scalar x[]){
		i=offset(i,e);
		if(mNativeFixed){
			x[0]=fromFixed(mData.getInt((i+0)*4));
			x[1]=fromFixed(mData.getInt((i+1)*4));
			x[2]=fromFixed(mData.getInt((i+2)*4));
			x[3]=fromFixed(mData.getInt((i+3)*4));
		}
		else{
			x[0]=fromFloat(mData.getFloat((i+0)*4));
			x[1]=fromFloat(mData.getFloat((i+1)*4));
			x[2]=fromFloat(mData.getFloat((i+2)*4));
			x[3]=fromFloat(mData.getFloat((i+3)*4));
		}
	}

	public void get2(int i,int e,Vector2 v){
		i=offset(i,e);
		if(mNativeFixed){
			v.x=fromFixed(mData.getInt((i+0)*4));
			v.y=fromFixed(mData.getInt((i+1)*4));
		}
		else{
			v.x=fromFloat(mData.getFloat((i+0)*4));
			v.y=fromFloat(mData.getFloat((i+1)*4));
		}
	}

	public void get3(int i,int e,Vector3 v){
		i=offset(i,e);
		if(mNativeFixed){
			v.x=fromFixed(mData.getInt((i+0)*4));
			v.y=fromFixed(mData.getInt((i+1)*4));
			v.z=fromFixed(mData.getInt((i+2)*4));
		}
		else{
			v.x=fromFloat(mData.getFloat((i+0)*4));
			v.y=fromFloat(mData.getFloat((i+1)*4));
			v.z=fromFloat(mData.getFloat((i+2)*4));
		}
	}

	public void get4(int i,int e,Vector4 v){
		i=offset(i,e);
		if(mNativeFixed){
			v.x=fromFixed(mData.getInt((i+0)*4));
			v.y=fromFixed(mData.getInt((i+1)*4));
			v.z=fromFixed(mData.getInt((i+2)*4));
			v.w=fromFixed(mData.getInt((i+3)*4));
		}
		else{
			v.x=fromFloat(mData.getFloat((i+0)*4));
			v.y=fromFloat(mData.getFloat((i+1)*4));
			v.z=fromFloat(mData.getFloat((i+2)*4));
			v.w=fromFloat(mData.getFloat((i+3)*4));
		}
	}

	public int getRGBA(int i,int e){
		return Color.swap(mData.getInt(offset(i,e)*4));
	}

	public int getABGR(int i,int e){
		return mData.getInt(offset(i,e)*4);
	}

	protected int offset(int vertex,int element){
		#if defined(TOADLET_DEBUG)
			if(vertex<0 || element<0 || (vertex*mVertexSize32 + mElementOffsets32[element])*4>=mVertexBuffer.getBufferSize()){
				Error.unknown(Categories.TOADLET_PEEPER,"vertex out of bounds");
				return 0;
			}
		#endif
		return vertex*mVertexSize32 + mElementOffsets32[element];
	}
	
	protected VertexBuffer mVertexBuffer=null;
	protected int mVertexSize32=0;
	protected int[] mElementOffsets32=new int[16];

	protected boolean mNativeFloat=false;
	protected boolean mNativeFixed=false;
	protected java.nio.ByteBuffer mData;
}
