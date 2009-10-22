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

/***********  c# Conversion from Java ************
 * 
 * package changed to namespace
 * Change class type from final to sealed
 * All final variables converted to readonly variables
 * Added use of out and ref function param keywords
 * Could not find equivelent of passing params as const
 * 
 * ***********************************************/

// need to use something other than "fixed" since it's a c# keyword
// unfortunately, this is only vision to this file so needs to be defined
// for each file using it
using tfixed = System.Int32;

namespace com.lightningtoads.toadlet.egg.mathfixed
{
/// <summary>
/// Math class converted primarily from the java version
/// </summary>
public sealed class Math{
	public static readonly int BITS=16;
    public static readonly int ONE = 65536;
    public static readonly int HALF = 32768;
    public static readonly int PI = 205887;
    public static readonly int TWO_PI = PI * 2;
    public static readonly int HALF_PI = PI / 2;
    public static readonly int E = 178145;
    public static readonly int SQRT_2 = 92682;

    public static readonly Vector3 ZERO_VECTOR3 = new Vector3();
    public static readonly Vector3 X_UNIT_VECTOR3 = new Vector3(ONE, 0, 0);
    public static readonly Vector3 NEG_X_UNIT_VECTOR3 = new Vector3(-ONE, 0, 0);
    public static readonly Vector3 Y_UNIT_VECTOR3 = new Vector3(0, ONE, 0);
    public static readonly Vector3 NEG_Y_UNIT_VECTOR3 = new Vector3(0, -ONE, 0);
    public static readonly Vector3 Z_UNIT_VECTOR3 = new Vector3(0, 0, ONE);
    public static readonly Vector3 NEG_Z_UNIT_VECTOR3 = new Vector3(0, 0, -ONE);
    public static readonly Matrix3x3 IDENTITY_MATRIX3X3 = new Matrix3x3();
    public static readonly Matrix4x4 IDENTITY_MATRIX4X4 = new Matrix4x4();
    public static readonly AABox ZERO_AABOX = new AABox();


	public static int milliToFixed(int m){
		return m/1*65+m/10*5+m/100*3+m/1000*6;
	}

	public static int fixedToInt(int x){
		return x>>16;
	}

	public static int intToFixed(int i){
		return i<<16;
	}

	public static float fixedToFloat(int x){
		return (float)x/(float)ONE;
	}

	public static int floatToFixed(float f){
		return (int)(f*(float)ONE);
	}

	public static int mul(int x,int y){
		return ((int) (((long) x * (long) y) >> 16));
	}

	public static int div(int x, int y){
		return (int) (((((long) x) << 32) / y) >> 16);
	}

	public static int abs(int x){
		return x<0?-x:x;
	}

	public static int square(int n){
		n = (int) (((long) n * (long) n) >> 16);
		//#ifdef TOADLET_DEBUG
			if(n<0){
				throw new System.Exception("overflow in square");  //Change RuntimeException to System.Exception  (there are several Runtime exception subclasses possible too)  -TR
			}
		//#endif
		return n;
	}

    public static tfixed sqrt(tfixed n){
		tfixed s = (n + ONE) >> 1;
		for (int i = 0; i < 8; i++) {
			// converge six times
			s = (s + div(n, s)) >> 1;
		}
		return s;
	}

	public static int clamp(int low,int high,int value){
		if(value<low){
			return low;
		}
		if(value>high){
			return high;
		}
		return value;
	}

	public static tfixed lerp(tfixed t1,tfixed t2,tfixed time){
		return mul(t1,ONE-time)+mul(t2,time);
	}

	public static int nextPowerOf2(int i){
		int result=1;
		while(result<i){
			result<<=1;
		}
		return result;
	}

	public static bool isPowerOf2(int i){
		return (i&(i-1))==0;
	}

	public static tfixed degToRad(tfixed d){
		return div(mul(d,PI),intToFixed(180));
	}

	public static tfixed radToDeg(tfixed r){
		return div(mul(r,intToFixed(180)),PI);
	}

	static readonly int SK1=498;
	static readonly int SK2=10882;

	public static int sin(int f){
		// If in range -PI/4 to PI/4: nothing needs to be done.
		// otherwise, we need to get f into that range and account for
		// sign change.

		int sign=1;
		if((f>HALF_PI)&&(f<=PI)){
			f=PI-f;
		}
		else if((f>PI)&&(f<=(PI+HALF_PI))){
			f=f-PI;
			sign=-1;
		}
		else if(f>(PI+HALF_PI)){
			f=(PI<<1)-f;
			sign=-1;
		}

		int sqr=mul(f,f);
		int result=SK1;
		result=mul(result,sqr);
		result-=SK2;
		result=mul(result,sqr);
		result+=(1<<16);
		result=mul(result,f);
		return sign*result;
	}

	static readonly int CK1=2328;
	static readonly int CK2=32551;

	public static int cos(int f){
		int sign=1;
		if((f>HALF_PI)&&(f<=PI)){
			f=PI-f;
			sign=-1;
		}
		else if((f>HALF_PI)&&(f<=(PI+HALF_PI))){
			f=f-PI;
			sign=-1;
		}
		else if(f>(PI+HALF_PI)){
			f=(PI<<1)-f;
		}

		int sqr=mul(f,f);
		int result=CK1;
		result=mul(result,sqr);
		result-=CK2;
		result=mul(result,sqr);
		result+=(1<<16);
		return result*sign;
	}

	static readonly int TK1=13323;
	static readonly int TK2=20810;

	public static int tan(int f){
		int sqr=mul(f,f);
		int result=TK1;
		result=mul(result,sqr);
		result+=TK2;
		result=mul(result,sqr);
		result+=(1<<16);
		result=mul(result,f);
		return result;
	}
	
	public static int arctan(int f){
		int sqr=mul(f,f);
		int result=1365;
		result=mul(result,sqr);
		result-=5579;
		result=mul(result,sqr);
		result+=11805;
		result=mul(result,sqr);
		result-=21646;
		result=mul(result,sqr);
		result+=65527;
		result=mul(result,f);
		return result;
	}

	static readonly int AS1=-1228;
	static readonly int AS2=4866;
	static readonly int AS3=13901;
	static readonly int AS4=102939;

	public static int arcsin(int f){
		int fRoot=sqrt((1<<16)-f);
		int result=AS1;
		result=mul(result,f);
		result+=AS2;
		result=mul(result,f);
		result-=AS3;
		result=mul(result,f);
		result+=AS4;
		result=HALF_PI-mul(fRoot,result);
		return result;
	}

	public static int arccos(int f){
		int fRoot=sqrt((1<<16)-f);
		int result=AS1;
		result=mul(result,f);
		result+=AS2;
		result=mul(result,f);
		result-=AS3;
		result=mul(result,f);
		result+=AS4;
		result=mul(fRoot,result);
		return result;
	}

//  Based on the c++ code
    // Vector3 operations
	public static void neg(Vector3 v){
		v.x=-v.x;
		v.y=-v.y;
		v.z=-v.z;
	}

    public static void add(out Vector3 r, Vector3 v1, Vector3 v2)
    {
        r = new Vector3();
		r.x=v1.x+v2.x;
		r.y=v1.y+v2.y;
		r.z=v1.z+v2.z;
	}

    public static void add(Vector3 r, Vector3 v)
    {
		r.x+=v.x;
		r.y+=v.y;
		r.z+=v.z;
	}

    public static void sub(out Vector3 r, Vector3 v1, Vector3 v2)
    {
        r = new Vector3();
		r.x=v1.x-v2.x;
		r.y=v1.y-v2.y;
		r.z=v1.z-v2.z;
	}

    public static void sub(Vector3 r, Vector3 v)
    {
		r.x-=v.x;
		r.y-=v.y;
		r.z-=v.z;
	}

    public static void mul(out Vector3 r, Vector3 v, tfixed f)
    {
        r = new Vector3();
		r.x=mul(v.x,f);
		r.y=mul(v.y,f);
		r.z=mul(v.z,f);
	}

    public static void mul(Vector3 r, tfixed f)
    {
		r.x=mul(r.x,f);
		r.y=mul(r.y,f);
		r.z=mul(r.z,f);
	}

    public static void div(out Vector3 r, Vector3 v, tfixed f)
    {
        r = new Vector3();
		tfixed i=div(ONE,f);
		r.x=mul(v.x,i);
		r.y=mul(v.y,i);
		r.z=mul(v.z,i);
	}

    public static tfixed dot(Vector3 v1, Vector3 v2)
    {
        return mul(v1.x, v2.x) + mul(v1.y, v2.y) + mul(v1.z, v2.z);
    }

    public static tfixed length(Vector3 v){
		return sqrt(square(v.x) + square(v.y) + square(v.z));
	}
	
	public static tfixed length(Vector3 v1,Vector3 v2){
		tfixed x=v1.x-v2.x;
		tfixed y=v1.y-v2.y;
		tfixed z=v1.z-v2.z;
		return sqrt(square(x) + square(y) + square(z));
	}

    public static tfixed lengthSquared(Vector3 v){
		tfixed r=square(v.x) + square(v.y) + square(v.z);
		#if TOADLET_DEBUG
			if(r<0){
				TOADLET_ERROR(TOADLET_EGG_CATEGORY,Error::TYPE_OVERFLOW,
					"overflow in lengthSquared");
			}
		#endif
		return r;
	}

    public static tfixed lengthSquared(Vector3 v1,Vector3 v2){
		tfixed x=v1.x-v2.x;
		tfixed y=v1.y-v2.y;
		tfixed z=v1.z-v2.z;
		tfixed r=square(x) + square(y) + square(z);
		#if TOADLET_DEBUG
			if(r<0){
				TOADLET_ERROR(TOADLET_EGG_CATEGORY,Error::TYPE_OVERFLOW,
					"overflow in lengthSquared");
			}
		#endif
		return r;
	}

    public static bool allComponentsGreaterThan(Vector3 v1, Vector3 v2)
    {
        return (v1.x > v2.x && v1.y > v2.y && v1.z > v2.z);
    }

    public static bool allComponentsLessThan(Vector3 v1, Vector3 v2)
    {
        return (v1.x < v2.x && v1.y < v2.y && v1.z < v2.z);
    }

    public static void setLerp(Vector3 r, Vector3 v1, Vector3 v2, int t)
    {
        sub(out r, v2, v1);
        mul(r, t);
        add(r, v1);
    }
    
	// Matrix3x3 operations
	public static void mul(out Matrix3x3 r,Matrix3x3 m1,Matrix3x3 m2){
        r = new Matrix3x3();

		//r.data[0+0*3] = m1.data[0+0*3] * m2.data[0+0*3] + m1.data[0+1*3] * m2.data[1+0*3] + m1.data[0+2*3] * m2.data[2+0*3];
		r.data[0+0*3] = ((((int)((((long)m1.data[0+0*3])*((long)m2.data[0+0*3]))>>16))+((int)((((long)m1.data[0+1*3])*((long)m2.data[1+0*3]))>>16)))+((int)((((long)m1.data[0+2*3])*((long)m2.data[2+0*3]))>>16)));
		//r.data[0+1*3] = m1.data[0+0*3] * m2.data[0+1*3] + m1.data[0+1*3] * m2.data[1+1*3] + m1.data[0+2*3] * m2.data[2+1*3];
		r.data[0+1*3] = ((((int)((((long)m1.data[0+0*3])*((long)m2.data[0+1*3]))>>16))+((int)((((long)m1.data[0+1*3])*((long)m2.data[1+1*3]))>>16)))+((int)((((long)m1.data[0+2*3])*((long)m2.data[2+1*3]))>>16)));
		//r.data[0+2*3] = m1.data[0+0*3] * m2.data[0+2*3] + m1.data[0+1*3] * m2.data[1+2*3] + m1.data[0+2*3] * m2.data[2+2*3];
		r.data[0+2*3] = ((((int)((((long)m1.data[0+0*3])*((long)m2.data[0+2*3]))>>16))+((int)((((long)m1.data[0+1*3])*((long)m2.data[1+2*3]))>>16)))+((int)((((long)m1.data[0+2*3])*((long)m2.data[2+2*3]))>>16)));

		//r.data[1+0*3] = m1.data[1+0*3] * m2.data[0+0*3] + m1.data[1+1*3] * m2.data[1+0*3] + m1.data[1+2*3] * m2.data[2+0*3];
		r.data[1+0*3] = ((((int)((((long)m1.data[1+0*3])*((long)m2.data[0+0*3]))>>16))+((int)((((long)m1.data[1+1*3])*((long)m2.data[1+0*3]))>>16)))+((int)((((long)m1.data[1+2*3])*((long)m2.data[2+0*3]))>>16)));
		//r.data[1+1*3] = m1.data[1+0*3] * m2.data[0+1*3] + m1.data[1+1*3] * m2.data[1+1*3] + m1.data[1+2*3] * m2.data[2+1*3];
		r.data[1+1*3] = ((((int)((((long)m1.data[1+0*3])*((long)m2.data[0+1*3]))>>16))+((int)((((long)m1.data[1+1*3])*((long)m2.data[1+1*3]))>>16)))+((int)((((long)m1.data[1+2*3])*((long)m2.data[2+1*3]))>>16)));
		//r.data[1+2*3] = m1.data[1+0*3] * m2.data[0+2*3] + m1.data[1+1*3] * m2.data[1+2*3] + m1.data[1+2*3] * m2.data[2+2*3];
		r.data[1+2*3] = ((((int)((((long)m1.data[1+0*3])*((long)m2.data[0+2*3]))>>16))+((int)((((long)m1.data[1+1*3])*((long)m2.data[1+2*3]))>>16)))+((int)((((long)m1.data[1+2*3])*((long)m2.data[2+2*3]))>>16)));

		//r.data[2+0*3] = m1.data[2+0*3] * m2.data[0+0*3] + m1.data[2+1*3] * m2.data[1+0*3] + m1.data[2+2*3] * m2.data[2+0*3];
		r.data[2+0*3] = ((((int)((((long)m1.data[2+0*3])*((long)m2.data[0+0*3]))>>16))+((int)((((long)m1.data[2+1*3])*((long)m2.data[1+0*3]))>>16)))+((int)((((long)m1.data[2+2*3])*((long)m2.data[2+0*3]))>>16)));
		//r.data[2+1*3] = m1.data[2+0*3] * m2.data[0+1*3] + m1.data[2+1*3] * m2.data[1+1*3] + m1.data[2+2*3] * m2.data[2+1*3];
		r.data[2+1*3] = ((((int)((((long)m1.data[2+0*3])*((long)m2.data[0+1*3]))>>16))+((int)((((long)m1.data[2+1*3])*((long)m2.data[1+1*3]))>>16)))+((int)((((long)m1.data[2+2*3])*((long)m2.data[2+1*3]))>>16)));
		//r.data[2+2*3] = m1.data[2+0*3] * m2.data[0+2*3] + m1.data[2+1*3] * m2.data[1+2*3] + m1.data[2+2*3] * m2.data[2+2*3];
		r.data[2+2*3] = ((((int)((((long)m1.data[2+0*3])*((long)m2.data[0+2*3]))>>16))+((int)((((long)m1.data[2+1*3])*((long)m2.data[1+2*3]))>>16)))+((int)((((long)m1.data[2+2*3])*((long)m2.data[2+2*3]))>>16)));
	}

	public static void mul(out Vector3 r,Matrix3x3 m,Vector3 v){
        r = new Vector3();

		//r.x=m.data[0+0*3]*v.x + m.data[0+1*3]*v.y + m.data[0+2*3]*v.z;
		r.x=((((int)((((long)m.data[0+0*3])*((long)v.x))>>16))+((int)((((long)m.data[0+1*3])*((long)v.y))>>16)))+((int)((((long)m.data[0+2*3])*((long)v.z))>>16)));
		//r.y=m.data[1+0*3]*v.x + m.data[1+1*3]*v.y + m.data[1+2*3]*v.z;
		r.y=((((int)((((long)m.data[1+0*3])*((long)v.x))>>16))+((int)((((long)m.data[1+1*3])*((long)v.y))>>16)))+((int)((((long)m.data[1+2*3])*((long)v.z))>>16)));
		//r.z=m.data[2+0*3]*v.x + m.data[2+1*3]*v.y + m.data[2+2*3]*v.z;
		r.z=((((int)((((long)m.data[2+0*3])*((long)v.x))>>16))+((int)((((long)m.data[2+1*3])*((long)v.y))>>16)))+((int)((((long)m.data[2+2*3])*((long)v.z))>>16)));
	}

	public static void mul(Vector3 v,Matrix3x3 m){
		int tx,ty,tz;
		//tx=m.data[0+0*3]*v.x + m.data[0+1*3]*v.y + m.data[0+2*3]*v.z;
		tx=((((int)((((long)m.data[0+0*3])*((long)v.x))>>16))+((int)((((long)m.data[0+1*3])*((long)v.y))>>16)))+((int)((((long)m.data[0+2*3])*((long)v.z))>>16)));
		//ty=m.data[1+0*3]*v.x + m.data[1+1*3]*v.y + m.data[1+2*3]*v.z;
		ty=((((int)((((long)m.data[1+0*3])*((long)v.x))>>16))+((int)((((long)m.data[1+1*3])*((long)v.y))>>16)))+((int)((((long)m.data[1+2*3])*((long)v.z))>>16)));
		//tz=m.data[2+0*3]*v.x + m.data[2+1*3]*v.y + m.data[2+2*3]*v.z;
		tz=((((int)((((long)m.data[2+0*3])*((long)v.x))>>16))+((int)((((long)m.data[2+1*3])*((long)v.y))>>16)))+((int)((((long)m.data[2+2*3])*((long)v.z))>>16)));
		v.x=tx;
		v.y=ty;
		v.z=tz;
	}

	public static void setTranspose(out Matrix3x3 r,Matrix3x3 m){
        r = new Matrix3x3();

		r.data[0+0*3]=m.data[0+0*3];
		r.data[0+1*3]=m.data[1+0*3];
		r.data[0+2*3]=m.data[2+0*3];
		r.data[1+0*3]=m.data[0+1*3];
		r.data[1+1*3]=m.data[1+1*3];
		r.data[1+2*3]=m.data[2+1*3];
		r.data[2+0*3]=m.data[0+2*3];
		r.data[2+1*3]=m.data[1+2*3];
		r.data[2+2*3]=m.data[2+2*3];
	}

	public static void setVectorsFromMatrix3x3(Matrix3x3 m,Vector3 right,Vector3 forward,Vector3 up){
		right.x=m.data[0+0*3];right.y=m.data[0+1*3];right.z=m.data[0+2*3];
		forward.x=m.data[1+0*3];forward.y=m.data[1+1*3];forward.z=m.data[1+2*3];
		up.x=m.data[2+0*3];up.y=m.data[2+1*3];up.z=m.data[2+2*3];
	}

	public static void setMatrix3x3FromVectors(Matrix3x3 m,Vector3 right,Vector3 forward,Vector3 up){
		m.data[0+0*3]=right.x;m.data[0+1*3]=right.y;m.data[0+2*3]=right.z;
		m.data[1+0*3]=forward.x;m.data[1+1*3]=forward.y;m.data[1+2*3]=forward.z;
		m.data[2+0*3]=up.x;m.data[2+1*3]=up.y;m.data[2+2*3]=up.z;
	}

	public static void setMatrix3x3FromX(out Matrix3x3 r,int x){
        r = new Matrix3x3();

		int cx=cos(x);
		int sx=sin(x);
		r.set(ONE,0,0,0,cx,-sx,0,sx,cx);
	}

	public static void setMatrix3x3FromY(out Matrix3x3 r,int y){
        r = new Matrix3x3();

		int cy=cos(y);
		int sy=sin(y);
		r.set(cy,0,sy,0,ONE,0,-sy,0,cy);
	}

	public static void setMatrix3x3FromZ(out Matrix3x3 r,int z){
        r = new Matrix3x3();

		int cz=cos(z);
		int sz=sin(z);
		r.set(cz,-sz,0,sz,cz,0,0,0,ONE);
	}

	public static void setMatrix3x3FromAxisAngle(out Matrix3x3 r,Vector3 axis,int angle){
        r = new Matrix3x3();

		int c=cos(angle);
		int s=sin(angle);
		int t=ONE-c;
		r.setAt(0,0,c+mul(axis.x,mul(axis.x,t)));
        r.setAt(1,1,c+mul(axis.y,mul(axis.y,t)));
        r.setAt(2,2,c+mul(axis.z,mul(axis.z,t)));

        int tmp1=mul(axis.x,mul(axis.y,t));
        int tmp2=mul(axis.z,s);
        r.setAt(1,0,tmp1+tmp2);
        r.setAt(0,1,tmp1-tmp2);

        tmp1=mul(axis.x,mul(axis.z,t));
        tmp2=mul(axis.y,s);
        r.setAt(2,0,tmp1-tmp2);
        r.setAt(0,2,tmp1+tmp2);

        tmp1=mul(axis.y,mul(axis.z,t));
        tmp2=mul(axis.x,s);
        r.setAt(2,1,tmp1+tmp2);
        r.setAt(1,2,tmp1-tmp2);
	}

	public static void setMatrix3x3FromQuaternion(out Matrix3x3 r,Quaternion q){
        r = new Matrix3x3();

		int tx  = q.x<<1;
		int ty  = q.y<<1;
		int tz  = q.z<<1;
		int twx = mul(tx,q.w);
		int twy = mul(ty,q.w);
		int twz = mul(tz,q.w);
		int txx = mul(tx,q.x);
		int txy = mul(ty,q.x);
		int txz = mul(tz,q.x);
		int tyy = mul(ty,q.y);
		int tyz = mul(tz,q.y);
		int tzz = mul(tz,q.z);

		r.setAt(0,0,ONE-(tyy+tzz));
		r.setAt(0,1,txy-twz);
		r.setAt(0,2,txz+twy);
		r.setAt(1,0,txy+twz);
		r.setAt(1,1,ONE-(txx+tzz));
		r.setAt(1,2,tyz-twx);
		r.setAt(2,0,txz-twy);
		r.setAt(2,1,tyz+twx);
		r.setAt(2,2,ONE-(txx+tyy));
	}


	// Matrix4x4 operations
	public static void mul(out Matrix4x4 r,Matrix4x4 m1,Matrix4x4 m2){
        r = new Matrix4x4();

		r.setAt(0,0,mul(m1.at(0,0),m2.at(0,0)) + mul(m1.at(0,1),m2.at(1,0)) + mul(m1.at(0,2),m2.at(2,0)) + mul(m1.at(0,3),m2.at(3,0)));
		r.setAt(0,1,mul(m1.at(0,0),m2.at(0,1)) + mul(m1.at(0,1),m2.at(1,1)) + mul(m1.at(0,2),m2.at(2,1)) + mul(m1.at(0,3),m2.at(3,1)));
		r.setAt(0,2,mul(m1.at(0,0),m2.at(0,2)) + mul(m1.at(0,1),m2.at(1,2)) + mul(m1.at(0,2),m2.at(2,2)) + mul(m1.at(0,3),m2.at(3,2)));
		r.setAt(0,3,mul(m1.at(0,0),m2.at(0,3)) + mul(m1.at(0,1),m2.at(1,3)) + mul(m1.at(0,2),m2.at(2,3)) + mul(m1.at(0,3),m2.at(3,3)));

		r.setAt(1,0,mul(m1.at(1,0),m2.at(0,0)) + mul(m1.at(1,1),m2.at(1,0)) + mul(m1.at(1,2),m2.at(2,0)) + mul(m1.at(1,3),m2.at(3,0)));
		r.setAt(1,1,mul(m1.at(1,0),m2.at(0,1)) + mul(m1.at(1,1),m2.at(1,1)) + mul(m1.at(1,2),m2.at(2,1)) + mul(m1.at(1,3),m2.at(3,1)));
		r.setAt(1,2,mul(m1.at(1,0),m2.at(0,2)) + mul(m1.at(1,1),m2.at(1,2)) + mul(m1.at(1,2),m2.at(2,2)) + mul(m1.at(1,3),m2.at(3,2)));
		r.setAt(1,3,mul(m1.at(1,0),m2.at(0,3)) + mul(m1.at(1,1),m2.at(1,3)) + mul(m1.at(1,2),m2.at(2,3)) + mul(m1.at(1,3),m2.at(3,3)));

		r.setAt(2,0,mul(m1.at(2,0),m2.at(0,0)) + mul(m1.at(2,1),m2.at(1,0)) + mul(m1.at(2,2),m2.at(2,0)) + mul(m1.at(2,3),m2.at(3,0)));
		r.setAt(2,1,mul(m1.at(2,0),m2.at(0,1)) + mul(m1.at(2,1),m2.at(1,1)) + mul(m1.at(2,2),m2.at(2,1)) + mul(m1.at(2,3),m2.at(3,1)));
		r.setAt(2,2,mul(m1.at(2,0),m2.at(0,2)) + mul(m1.at(2,1),m2.at(1,2)) + mul(m1.at(2,2),m2.at(2,2)) + mul(m1.at(2,3),m2.at(3,2)));
		r.setAt(2,3,mul(m1.at(2,0),m2.at(0,3)) + mul(m1.at(2,1),m2.at(1,3)) + mul(m1.at(2,2),m2.at(2,3)) + mul(m1.at(2,3),m2.at(3,3)));

		r.setAt(3,0,mul(m1.at(3,0),m2.at(0,0)) + mul(m1.at(3,1),m2.at(1,0)) + mul(m1.at(3,2),m2.at(2,0)) + mul(m1.at(3,3),m2.at(3,0)));
		r.setAt(3,1,mul(m1.at(3,0),m2.at(0,1)) + mul(m1.at(3,1),m2.at(1,1)) + mul(m1.at(3,2),m2.at(2,1)) + mul(m1.at(3,3),m2.at(3,1)));
		r.setAt(3,2,mul(m1.at(3,0),m2.at(0,2)) + mul(m1.at(3,1),m2.at(1,2)) + mul(m1.at(3,2),m2.at(2,2)) + mul(m1.at(3,3),m2.at(3,2)));
		r.setAt(3,3,mul(m1.at(3,0),m2.at(0,3)) + mul(m1.at(3,1),m2.at(1,3)) + mul(m1.at(3,2),m2.at(2,3)) + mul(m1.at(3,3),m2.at(3,3)));
	}


	// Axis Angle operations
	public static int setAxisAngleFromQuaternion(Vector3 axis,Quaternion q,int epsilon){
		int angle=arccos(q.w)<<1;

		int sinHalfAngle=sin(angle>>1);
		if(sinHalfAngle>epsilon){
			int invSinHalfAngle=div(ONE,sinHalfAngle);
			axis.x=mul(q.x,invSinHalfAngle);
			axis.y=mul(q.y,invSinHalfAngle);
			axis.z=mul(q.z,invSinHalfAngle);

			int l=sqrt((mul(axis.x,axis.x)+mul(axis.y,axis.y)+mul(axis.z,axis.z)));
			axis.x=div(axis.x,l);
			axis.y=div(axis.y,l);
			axis.z=div(axis.z,l);
		}
		else{
			axis.x=ONE;
			axis.y=0;
			axis.z=0;
		}
		
		return angle;
	}

	// Quaternion operations
	public static void mul(out Quaternion r,Quaternion q){
        r = new Quaternion();

		int x=mul(+r.x,q.w)+mul(r.y,q.z)-mul(r.z,q.y)+mul(r.w,q.x);
		int y=mul(-r.x,q.z)+mul(r.y,q.w)+mul(r.z,q.x)+mul(r.w,q.y);
		int z=mul(+r.x,q.y)-mul(r.y,q.x)+mul(r.z,q.w)+mul(r.w,q.z);
		int w=mul(-r.x,q.x)-mul(r.y,q.y)-mul(r.z,q.z)+mul(r.w,q.w);
		r.x=x;
		r.y=y;
		r.z=z;
		r.w=w;
	}

	public static void mul(out Quaternion r,Quaternion q1,Quaternion q2){
        r = new Quaternion();

		r.x=mul(+q1.x,q2.w)+mul(q1.y,q2.z)-mul(q1.z,q2.y)+mul(q1.w,q2.x);
		r.y=mul(-q1.x,q2.z)+mul(q1.y,q2.w)+mul(q1.z,q2.x)+mul(q1.w,q2.y);
		r.z=mul(+q1.x,q2.y)-mul(q1.y,q2.x)+mul(q1.z,q2.w)+mul(q1.w,q2.z);
		r.w=mul(-q1.x,q2.x)-mul(q1.y,q2.y)-mul(q1.z,q2.z)+mul(q1.w,q2.w);
	}

	public static int length(Quaternion q){
		return sqrt(mul(q.x,q.x) + mul(q.y,q.y) + mul(q.z,q.z) + mul(q.w,q.w));
	}

	public static int length(Quaternion q1,Quaternion q2){
		int x=q1.x-q2.x;
		int y=q1.y-q2.y;
		int z=q1.z-q2.z;
		int w=q1.w-q2.w;
		return sqrt((mul(x,x) + mul(y,y) + mul(z,z) + mul(w,w)));
	}

	public static int lengthSquared(Quaternion q){
		int r=mul(q.x,q.x) + mul(q.y,q.y) + mul(q.z,q.z) + mul(q.w,q.w);
		//#ifdef TOADLET_DEBUG
			if(r<0){
                throw new System.Exception("overflow in lengthSquared");    //Change RuntimeException to System.Exception  (there are several Runtime exception subclasses possible too)  -TR
			}
		//#endif
		return r;
	}

	public static int lengthSquared(Quaternion q1,Quaternion q2){
		int x=q1.x-q2.x;
		int y=q1.y-q2.y;
		int z=q1.z-q2.z;
		int w=q1.w-q2.w;
		int r=mul(x,x) + mul(y,y) + mul(z,z) + mul(w,w);
		//#ifdef TOADLET_DEBUG
			if(r<0){
                throw new System.Exception("overflow in lengthSquared");    //Change RuntimeException to System.Exception  (there are several Runtime exception subclasses possible too)  -TR
			}
		//#endif
		return r;
	}

	public static void normalize(Quaternion q){
		int l=div(ONE,length(q));
		q.x=mul(q.x,l);
		q.y=mul(q.y,l);
		q.z=mul(q.z,l);
		q.w=mul(q.w,l);
	}

	public static void normalize(out Quaternion r,Quaternion q){
        r = new Quaternion();

		int l=div(ONE,length(q));
		r.x=mul(q.x,l);
		r.y=mul(q.y,l);
		r.z=mul(q.z,l);
		r.w=mul(q.w,l);
	}

	public static bool normalizeCarefully(Quaternion q,int epsilon){
		int l=length(q);
		if(l>epsilon){
			l=div(ONE,l);
			q.x=mul(q.x,l);
			q.y=mul(q.y,l);
			q.z=mul(q.z,l);
			q.w=mul(q.w,l);
			return true;
		}
		return false;
	}

	public static bool normalizeCarefully(out Quaternion r,Quaternion q,int epsilon){
        r = new Quaternion();

		int l=length(q);
		if(l>epsilon){
			l=div(ONE,l);
			r.x=mul(q.x,l);
			r.y=mul(q.y,l);
			r.z=mul(q.z,l);
			r.w=mul(q.w,l);
			return true;
		}
		return false;
	}

	public static int dot(Quaternion q1,Quaternion q2){
		return mul(q1.x,q2.x) + mul(q1.y,q2.y) + mul(q1.z,q2.z) + mul(q1.w,q2.w);
	}

	private static int[] quaternionFromMatrix3x3_next={1,2,0};
	public static void setQuaternionFromMatrix3x3(out Quaternion r,Matrix3x3 mat){
        r = new Quaternion();

		// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
		// article "Quaternion Calculus and Fast Animation".

		int trace=mat.at(0,0)+mat.at(1,1)+mat.at(2,2);
		int root;

		if(trace>0){
			// |w| > 1/2, may as well choose w > 1/2
			root=sqrt(trace+ONE); // 2w
			r.w=root>>1;
			root=div(HALF,root); // 1/(4w)
			r.x=mul(mat.at(2,1)-mat.at(1,2),root);
			r.y=mul(mat.at(0,2)-mat.at(2,0),root);
			r.z=mul(mat.at(1,0)-mat.at(0,1),root);
		}
		else{
			// |w| <= 1/2
			int i=0;
			if(mat.at(1,1)>mat.at(0,0)){
				i=1;
			}
			if(mat.at(2,2)>mat.at(i,i)){
				i=2;
			}
			int j=quaternionFromMatrix3x3_next[i];
			int k=quaternionFromMatrix3x3_next[j];

			root=sqrt(mat.at(i,i)-mat.at(j,j)-mat.at(k,k)+ONE);
			trace=root>>1; // trace used as temp in the below code, and as a dummy assignment
			trace=(i==0?(r.x=trace):(i==1?(r.y=trace):(r.z=trace)));
			root=div(HALF,root);
			r.w=mul(mat.at(k,j)-mat.at(j,k),root);
			trace=mul(mat.at(j,i)+mat.at(i,j),root);
			trace=(j==0?(r.x=trace):(j==1?(r.y=trace):(r.z=trace)));
			trace=mul(mat.at(k,i)+mat.at(i,k),root);
			trace=(k==0?(r.x=trace):(k==1?(r.y=trace):(r.z=trace)));
		}
	}

	public static void setQuaternionFromAxisAngle(out Quaternion r,Vector3 axis,int angle){
        r = new Quaternion();

		int halfAngle=angle>>1;
		int sinHalfAngle=sin(halfAngle);
		r.x=mul(axis.x,sinHalfAngle);
		r.y=mul(axis.y,sinHalfAngle);
		r.z=mul(axis.z,sinHalfAngle);
		r.w=cos(halfAngle);
	}

	public static void setLerp(out Quaternion r,Quaternion q1,Quaternion q2,int t){
        r = new Quaternion();

		int cosom=dot(q1,q2);

		int scl1=(ONE-t);
		int scl2=0;
		if(cosom<0){
			scl2=-t;
		}
		else{
			scl2=t;
		}

		r.x=mul(scl1,q1.x) + mul(scl2,q2.x);
		r.y=mul(scl1,q1.y) + mul(scl2,q2.y);
		r.z=mul(scl1,q1.z) + mul(scl2,q2.z);
		r.w=mul(scl1,q1.w) + mul(scl2,q2.w);
	}


	// Segment operations
    // This function was converted from the c++ version -TR
    public static void getClosestPointOnSegment(out Vector3 result, Segment segment, Vector3 point)
    {
        result = new Vector3(); 
        
        Vector3 o = segment.origin;  // cannot make const/readonly -TR
        Vector3 d = segment.direction;  // cannot make const/readonly -TR
        
        if (d.x == 0 && d.y == 0 && d.z == 0)
        {
            result = o;
        }
        else
        {
            tfixed u = div(mul(d.x, point.x - o.x) + mul(d.y, point.y - o.y) + mul(d.z, point.z - o.z), mul(d.x, d.x) + mul(d.y, d.y) + mul(d.z, d.z));
            if (u < 0) u = 0;
            else if (u > ONE) u = ONE;
            result.x = o.x + mul(u, d.x);
            result.y = o.y + mul(u, d.y);
            result.z = o.z + mul(u, d.z);
        }
    }

    // This function was converted from the c++ version -TR
    public static void getClosestPointsOnSegments(Vector3 point1,Vector3 point2,Segment seg1,Segment seg2,tfixed epsilon){
	    // For a full discussion of this method see Dan Sunday's Geometry Algorithms web site: http://www.geometryalgorithms.com/
	    tfixed a=dot(seg1.direction,seg1.direction);
	    tfixed b=dot(seg1.direction,seg2.direction);
	    tfixed c=dot(seg2.direction,seg2.direction);

	    // Make sure we don't have a very small segment somewhere, if so we treat it as a point
	    if(a<=epsilon){
		    point1=seg1.origin;
		    getClosestPointOnSegment(out point2,seg2,point1);
		    return;
	    }
	    else if(c<epsilon){
		    point2=seg2.origin;
		    getClosestPointOnSegment(out point1,seg1,point2);
		    return;
	    }

	    // Use point1 as a temporary
	    sub(out point1,seg1.origin,seg2.origin);
	    tfixed d=dot(seg1.direction,point1);
	    tfixed e=dot(seg2.direction,point1);
	    tfixed denom=mul(a,c)-mul(b,b);
    	
	    tfixed u1=0;
	    tfixed u2=0;
	    tfixed u1N=0;
	    tfixed u2N=0;
	    tfixed u1D=denom;
	    tfixed u2D=denom;
    	
	    if(denom<100/* a small tfixed point value */){
		    // Segments are practically parallel
		    u1N=0;
		    u1D=ONE;
		    u2N=e;
		    u2D=c;
	    }
	    else{
		    // Find the closest point on 2 infinite lines
		    u1N=mul(b,e)-mul(c,d);
		    u2N=mul(a,e)-mul(b,d);
    		
		    // Check for closest point outside seg1 bounds
		    if(u1N<0){
			    u1N=0;
			    u2N=e;
			    u2D=c;
		    }
		    else if(u1N>u1D){
			    u1N=u1D;
			    u2N=e+b;
			    u2D=c;
		    }
	    }
    	
	    // Check for closest point outside seg2 bounds
	    if(u2N<0){
		    u2N=0;
		    // Recalculate u1N
		    if(-d<0){
			    u1N=0;
		    }
		    else if(-d>a){
			    u1N=u1D;
		    }
		    else{
			    u1N=-d;
			    u1D=a;
		    }
	    }
	    else if(u2N>u2D){
		    u2N=u2D;
		    // Recalculate u1N
		    if((-d+b)<0){
			    u1N=0;
		    }
		    else if((-d+b)>a){
			    u1N=u1D;
		    }
		    else{
			    u1N=(-d+b);
			    u1D=a;
		    }
	    }
    	
	    u1=div(u1N,u1D);
	    u2=div(u2N,u2D);

	    mul(out point1,seg1.direction,u1);
	    add(point1,seg1.origin);
	    mul(out point2,seg2.direction,u2);
	    add(point2,seg2.origin);
    }


	// AABox operations
	public static void add(out AABox r,AABox b,Vector3 p){
        r = new AABox();
        
        r.mins.x = b.mins.x + p.x;
		r.mins.y=b.mins.y+p.y;
		r.mins.z=b.mins.z+p.z;

		r.maxs.x=b.maxs.x+p.x;
		r.maxs.y=b.maxs.y+p.y;
		r.maxs.z=b.maxs.z+p.z;
	}

	public static void add(AABox b,Vector3 p){
		b.mins.x+=p.x;
		b.mins.y+=p.y;
		b.mins.z+=p.z;

		b.maxs.x+=p.x;
		b.maxs.y+=p.y;
		b.maxs.z+=p.z;
	}

	public static void sub(out AABox r,AABox b,Vector3 p){
        r = new AABox();
        
        r.mins.x = b.mins.x - p.x;
		r.mins.y=b.mins.y-p.y;
		r.mins.z=b.mins.z-p.z;

		r.maxs.x=b.maxs.x-p.x;
		r.maxs.y=b.maxs.y-p.y;
		r.maxs.z=b.maxs.z-p.z;
	}

	public static void sub(AABox b,Vector3 p){
		b.mins.x-=p.x;
		b.mins.y-=p.y;
		b.mins.z-=p.z;

		b.maxs.x-=p.x;
		b.maxs.y-=p.y;
		b.maxs.z-=p.z;
	}


	// Sphere operations
	public static void add(out Sphere r,Sphere s,Vector3 p){
        r = new Sphere();
        
        r.origin.x = s.origin.x + p.x;
		r.origin.y=s.origin.y+p.y;
		r.origin.z=s.origin.z+p.z;
	}

	public static void add(Sphere s,Vector3 p){
		s.origin.x+=p.x;
		s.origin.y+=p.y;
		s.origin.z+=p.z;
	}

	public static void sub(out Sphere r,Sphere s,Vector3 p){
        r = new Sphere(); 
        
        r.origin.x = s.origin.x - p.x;
		r.origin.y=s.origin.y-p.y;
		r.origin.z=s.origin.z-p.z;
	}

	public static void sub(Sphere s,Vector3 p){
		s.origin.x-=p.x;
		s.origin.y-=p.y;
		s.origin.z-=p.z;
	}


	// Capsule operation
	public static void add(out Capsule r,Capsule c,Vector3 p){
        r = new Capsule();

		r.origin.x=c.origin.x+p.x;
		r.origin.y=c.origin.y+p.y;
		r.origin.z=c.origin.z+p.z;
	}

	public static void add(Capsule c,Vector3 p){
		c.origin.x+=p.x;
		c.origin.y+=p.y;
		c.origin.z+=p.z;
	}

	public static void sub(out Capsule r,Capsule c,Vector3 p){
        r = new Capsule();

		r.origin.x=c.origin.x-p.x;
		r.origin.y=c.origin.y-p.y;
		r.origin.z=c.origin.z-p.z;
	}

	public static void sub(Capsule c,Vector3 p){
		c.origin.x-=p.x;
		c.origin.y-=p.y;
		c.origin.z-=p.z;
	}


	// Interesection operations
	public static bool testInside(Vector3 point,Vector3 sphereOrigin,int sphereRadius){
		return Math.lengthSquared(point,sphereOrigin) <= Math.square(sphereRadius);
	}

	public static bool testInside(Vector3 point,Sphere sphere){
		return Math.lengthSquared(point,sphere.origin) <= Math.square(sphere.radius);
	}
	
	public static bool testInside(Vector3 point,AABox box){
		return Math.allComponentsGreaterThan(point,box.mins) && Math.allComponentsLessThan(point,box.maxs);
	}

	public static bool testIntersection(AABox box1,AABox box2){
		// Look for a separating axis on each box for each axis
		return !(box1.mins.x>box2.maxs.x || box1.mins.y>box2.maxs.y || box1.mins.z>box2.maxs.z ||
			box2.mins.x>box1.maxs.x || box2.mins.y>box1.maxs.y || box2.mins.z>box1.maxs.z);
	}

// TODO:  Cannot convert this from java and can't find c++ version -TR
    /*   public static int findIntersection(Segment segment, AABox box, Vector3 point, Vector3 normal)
       {
           bool inside = true;
           byte quadX, quadY, quadZ;
           int whichPlane;
           int maxTX = 0, maxTY = 0, maxTZ = 0;
           int candPlaneX = 0, candPlaneY = 0, candPlaneZ = 0;
           Vector3 candNormX = Math.NEG_X_UNIT_VECTOR3;
           Vector3 candNormY = Math.NEG_Y_UNIT_VECTOR3;
           Vector3 candNormZ = Math.NEG_Z_UNIT_VECTOR3;
           int time = -Math.ONE;

           // The below tests were originally < or >, but are now <= or >=
           // Without this change finds that start on the edge of a box count as inside which conflicts with how testInside works

           // X
           if (segment.origin.x <= box.mins.x)
           {
               quadX = 0; // Left
               candPlaneX = box.mins.x;
               inside = false;
           }
           else if (segment.origin.x >= box.maxs.x)
           {
               quadX = 1; // Right
               candPlaneX = box.maxs.x;
               candNormX = Math.X_UNIT_VECTOR3;
               inside = false;
           }
           else
           {
               quadX = 2; // Middle
           }
           // Y
           if (segment.origin.y <= box.mins.y)
           {
               quadY = 0; // Left
               candPlaneY = box.mins.y;
               inside = false;
           }
           else if (segment.origin.y >= box.maxs.y)
           {
               quadY = 1; // Right
               candPlaneY = box.maxs.y;
               candNormY = Math.Y_UNIT_VECTOR3;
               inside = false;
           }
           else
           {
               quadY = 2; // Middle
           }
           // Z
           if (segment.origin.z <= box.mins.z)
           {
               quadZ = 0; // Left
               candPlaneZ = box.mins.z;
               inside = false;
           }
           else if (segment.origin.z >= box.maxs.z)
           {
               quadZ = 1; // Right
               candPlaneZ = box.maxs.z;
               candNormZ = Math.Z_UNIT_VECTOR3;
               inside = false;
           }
           else
           {
               quadZ = 2; // Middle
           }

           // Inside
           if (inside)
           {
               return -Math.ONE;
           }

           // Calculate t distances to candidate planes
           // X
           if(quadX!=2 /*Middle*/
    // && segment.direction.x!=0){
/*			maxTX=
				//#exec fpcomp.bat "(candPlaneX-segment.origin.x)/segment.direction.x"
				;
		}
		else{
			maxTX= -Math.ONE;
		}
		// Y
		if(quadY!=2 /*Middle*/// && segment.direction.y!=0){
/*			maxTY=
				//#exec fpcomp.bat "(candPlaneY-segment.origin.y)/segment.direction.y"
				;
		}
		else{
			maxTY= -Math.ONE;
		}
		// Z
		if(quadZ!=2 /*Middle*/// && segment.direction.z!=0){
    /*			maxTZ=
                    //#exec fpcomp.bat "(candPlaneZ-segment.origin.z)/segment.direction.z"
                    ;
            }
            else{
                maxTZ= -Math.ONE;
            }
	
            // Find largets of maxT's
            // X
            if(maxTX>maxTY && maxTX>maxTZ){
                whichPlane=0;
                time=maxTX;
                normal.set(candNormX);
            }
            // Y
            else if(maxTY>maxTZ){
                whichPlane=1;
                time=maxTY;
                normal.set(candNormY);
            }
            // Z
            else{
                whichPlane=2;
                time=maxTZ;
                normal.set(candNormZ);
            }
	
            // Check final candidate actually inside box and calculate final point
            if(time<0 || time>Math.ONE){
                return -Math.ONE<<1;
            }
            // X
            if(whichPlane!=0){
                point.x=
                    //#exec fpcomp.bat "segment.origin.x+time*segment.direction.x"
                    ;
                if(point.x<box.mins.x || point.x>box.maxs.x){
                    return -Math.ONE;
                }
            }
            else{
                point.x=candPlaneX;
            }
            // Y
            if(whichPlane!=1){
                point.y=
                    //#exec fpcomp.bat "segment.origin.y+time*segment.direction.y"
                    ;
                if(point.y<box.mins.y || point.y>box.maxs.y){
                    return -Math.ONE;
                }
            }
            else{
                point.y=candPlaneY;
            }
            // Z
            if(whichPlane!=2){
                point.z=
                    //#exec fpcomp.bat "segment.origin.z+time*segment.direction.z"
                    ;
                if(point.z<box.mins.z || point.z>box.maxs.z){
                    return -Math.ONE;
                }
            }
            else{
                point.z=candPlaneZ;
            }
		
            return time;
        }
    */

    // TODO:  Cannot convert this from java and can't find c++ version -TR 
/*	public static int findIntersection(Segment segment,Sphere sphere,Vector3 point,Vector3 normal){
		Vector3 segOrigin=segment.origin;
		Vector3 segDirection=segment.direction;
		Vector3 sphOrigin=sphere.origin;
		int sphRadius=sphere.radius;

		// Quadratic Q(t) = a*t^2 + 2*b*t + c
		int diffX=segOrigin.x-sphOrigin.x;
		int diffY=segOrigin.y-sphOrigin.y;
		int diffZ=segOrigin.z-sphOrigin.z;
		int a=Math.lengthSquared(segDirection);
		if(a<=0){
			return -Math.ONE;
		}
		int b=
			//#exec fpcomp.bat "diffX*segDirection.x + diffY*segDirection.y + diffZ*segDirection.z"
			;
		int c=
			//#exec fpcomp.bat "diffX*diffX + diffY*diffY + diffZ*diffZ"
			-Math.square(sphRadius);
		int time1=-Math.ONE;
		int time2=-Math.ONE;

		// If no real roots, then no intersection
		int discr=
			//#exec fpcomp.bat "(b*b - a*c)"
			;
		if(discr<0){
			time1=-Math.ONE;
		}
		else if(discr>0){
			int root=Math.sqrt(discr);
			int invA=
				//#exec fpcomp.bat "(1/a)"
				;
			time1=
				//#exec fpcomp.bat "((-b)-root)*invA"
				;
			time2=
				//#exec fpcomp.bat "((-b)+root)*invA"
				;
	
			if(time1>Math.ONE || time2<0){
				time1=-Math.ONE;
			}
			else if(time1>=0){
				Math.mul(point,segDirection,time1);
				Math.add(point,segOrigin);
			}
			else{
				Math.mul(point,segDirection,time2);
				Math.add(point,segOrigin);
			}
		}
		else{
			time1=
				//#exec fpcomp.bat (-b/a)
				;
			if(0<=time1 && time1<=Math.ONE){
				Math.mul(point,segDirection,time1);
				Math.add(point,segOrigin);
			}
			else{
				time1=-Math.ONE;
			}
		}
	
		if(time1!=-Math.ONE){
			Math.sub(normal,point,sphOrigin);
			Math.normalizeCarefully(normal,0);
		}
		return time1;
	}*/
}
}