/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the Toadlet Engine, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

package com.lightningtoads.toadlet.egg.mathfixed{

public final class tMath{
	public static var BITS:int=16;
	public static var ONE:int=65536;
	public static var HALF:int=32768;
	public static var PI:int=205887;
	public static var TWO_PI:int=PI*2;
	public static var HALF_PI:int=PI/2;
	public static var E:int=178145;
	public static var SQRT_2:int=92682;

	public static var ZERO_VECTOR3:Vector3=new Vector3();
	public static var X_UNIT_VECTOR3:Vector3=Vector3.Vector3XYZ(ONE,0,0);
	public static var NEG_X_UNIT_VECTOR3:Vector3=Vector3.Vector3XYZ(-ONE,0,0);
	public static var Y_UNIT_VECTOR3:Vector3=Vector3.Vector3XYZ(0,ONE,0);
	public static var NEG_Y_UNIT_VECTOR3:Vector3=Vector3.Vector3XYZ(0,-ONE,0);
	public static var Z_UNIT_VECTOR3:Vector3=Vector3.Vector3XYZ(0,0,ONE);
	public static var NEG_Z_UNIT_VECTOR3:Vector3=Vector3.Vector3XYZ(0,0,-ONE);
	public static var IDENTITY_MATRIX3X3:Matrix3x3=new Matrix3x3();
	public static var ZERO_AABOX:AABox=new AABox();

	public static function milliToFixed(m:int):int{
		return m/1*65+m/10*5+m/100*3+m/1000*6;
	}

	public static function fixedToInt(x:int):int{
		return x>>16;
	}

	public static function intToFixed(i:int):int{
		return i<<16;
	}

	public static function fixedToFloat(x:int):Number{
		return Number(x/Number(ONE));
	}

	public static function floatToFixed(f:Number):int{
		return int(f*Number(ONE));
	}

	public static function mul(x:int,y:int):int{
		return ((x>>6)*(y>>6))>>4;
	}

	public static function div(x:int,y:int):int{
		return ((x<<6)/(y>>6))<<4;
	}

	public static function abs(x:int):int{
		return x<0?-x:x;
	}

	public static function square(n:int):int{
		n>>=6;
		n=(n*n)>>4;
		#ifdef TOADLET_DEBUG
			if(n<0){
				throw new Error("overflow in square");
			}
		#endif
		return n;
	}

	public static function sqrt(n:int):int{
		var num:Number=n;
		num=Math.sqrt(num/ONE)*ONE;
		return num;
	}

	public static function clamp(low:int,high:int,value:int):int{
		if(value<low){
			return low;
		}
		if(value>high){
			return high;
		}
		return value;
	}

	public static function lerp(t1:int,t2:int,time:int):int{
		return mul(t1,ONE-time) + mul(t2,time);
	}

	public static function nextPowerOf2(i:int){
		var result:int=1;
		while(result<i){
			result<<=1;
		}
		return result;
	}

	public static function isPowerOf2(i:int):Boolean{
		return (i&(i-1))==0;
	}

	public static function degToRad(d:int):int{
		return (((d>>6)*(PI>>6))<<2)/((180*65536)>>6)<<4;
	}

	public static function radToDeg(r:int):int{
		return (((r>>6)*((180*65536)>>6))<<2)/(PI>>6)<<4;
	}

	public static function sin(n:int):int{
		var num:Number=n;
		num=Math.sin(num/ONE)*ONE;
		return num;
	}

	public static function cos(n:int):int{
		var num:Number=n;
		num=Math.cos(num/ONE)*ONE;
		return num;
	}

	public static function tan(n:int):int{
		var num:Number=n;
		num=Math.tan(num/ONE)*ONE;
		return num;
	}

	// Vector3 operations
	public static function negV(v:Vector3):void{
		v.x=-v.x;
		v.y=-v.y;
		v.z=-v.z;
	}
	
	public static function addVVV(r:Vector3,v1:Vector3,v2:Vector3):void{
		r.x=v1.x+v2.x;
		r.y=v1.y+v2.y;
		r.z=v1.z+v2.z;
	}

	public static function addVV(v1:Vector3,v2:Vector3):void{
		v1.x+=v2.x;
		v1.y+=v2.y;
		v1.z+=v2.z;
	}

	public static function subVVV(r:Vector3,v1:Vector3,v2:Vector3):void{
		r.x=v1.x-v2.x;
		r.y=v1.y-v2.y;
		r.z=v1.z-v2.z;
	}
	
	public static function subVV(r:Vector3,v:Vector3):void{
		r.x-=v.x;
		r.y-=v.y;
		r.z-=v.z;
	}

	public static function mulVVS(r:Vector3,v:Vector3,f:int):void{
		f>>=6;
		r.x=((v.x>>6)*f)>>4;
		r.y=((v.y>>6)*f)>>4;
		r.z=((v.z>>6)*f)>>4;
	}

	public static function mulVS(r:Vector3,f:int):void{
		f>>=6;
		r.x=((r.x>>6)*f)>>4;
		r.y=((r.y>>6)*f)>>4;
		r.z=((r.z>>6)*f)>>4;
	}
	
	public static function divVVS(r:Vector3,v:Vector3,f:int):void{
		f>>=6;
		r.x=((v.x<<6)/f)<<4;
		r.y=((v.y<<6)/f)<<4;
		r.z=((v.z<<6)/f)<<4;
	}
	
	public static function divVS(r:Vector3,f:int):void{
		f>>=6;
		r.x=((r.x<<6)/f)<<4;
		r.y=((r.y<<6)/f)<<4;
		r.z=((r.z<<6)/f)<<4;
	}

	public static function dotVV(v1:Vector3,v2:Vector3):int{
		return (((v1.x>>6)*(v2.x>>6))>>4) + (((v1.y>>6)*(v2.y>>6))>>4) + (((v1.z>>6)*(v2.z>>6))>>4);
	}

	public static function crossVV(r:Vector3,v:Vector3):void{
		var x:int=(((r.y>>6)*(v.z>>6))>>4) - (((r.z>>6)*(v.y>>6))>>4);
		var y:int=(((r.z>>6)*(v.x>>6))>>4) - (((r.x>>6)*(v.z>>6))>>4);
		var z:int=(((r.x>>6)*(v.y>>6))>>4) - (((r.y>>6)*(v.x>>6))>>4);
		
		r.x=x;
		r.y=y;
		r.z=z;
	}

	public static function crossVVV(r:Vector3,v1:Vector3,v2:Vector3):void{
		r.x=(((v1.y>>6)*(v2.z>>6))>>4) - (((v1.z>>6)*(v2.y>>6))>>4);
		r.y=(((v1.z>>6)*(v2.x>>6))>>4) - (((v1.x>>6)*(v2.z>>6))>>4);
		r.z=(((v1.x>>6)*(v2.y>>6))>>4) - (((v1.y>>6)*(v2.x>>6))>>4);
	}

	public static function lengthSquaredV(v:Vector3):int{
		var r:int=(((v.x>>6)*(v.x>>6))>>4) + (((v.y>>6)*(v.y>>6))>>4) + (((v.z>>6)*(v.z>>6))>>4);
		#ifdef TOADLET_DEBUG
			if(r<0){
				throw new Error("overflow in lengthSquared");
			}
		#endif
		return r;
	}

	public static function lengthSquaredVV(v1:Vector3,v2:Vector3):int{
		var x:int=(v1.x-v2.x)>>6;
		var y:int=(v1.y-v2.y)>>6;
		var z:int=(v1.z-v2.z)>>6;
		var result:int=((x*x)>>4) + ((y*y)>>4) + ((z*z)>>4);
		#ifdef TOADLET_DEBUG
			if(result<0){
				throw new Error("overflow in lengthSquared");
			}
		#endif
		return result;
	}

	public static function lengthV(v:Vector3):int{
		var x:Number=Number(v.x)/ONE;
		var y:Number=Number(v.y)/ONE;
		var z:Number=Number(v.z)/ONE;
		return Math.sqrt(x*x+y*y+z*z)*ONE;
	}

	public static function lengthVV(v1:Vector3,v2:Vector3):int{
		var x:Number=Number(v1.x-v2.x)/ONE;
		var y:Number=Number(v1.y-v2.y)/ONE;
		var z:Number=Number(v1.z-v2.z)/ONE;
		return Math.sqrt(x*x+y*y+z*z)*ONE;
	}

	public static function normalizeV(v:Vector3):void{
		var l:int=lengthV(v)>>6;
		v.x=((v.x<<6)/l)<<4;
		v.y=((v.y<<6)/l)<<4;
		v.z=((v.z<<6)/l)<<4;
	}

	public static function normalizeVV(r:Vector3,v:Vector3):void{
		var l:int=lengthV(v)>>6;
		r.x=((v.x<<6)/l)<<4;
		r.y=((v.y<<6)/l)<<4;
		r.z=((v.z<<6)/l)<<4;
	}
	
	public static function normalizeCarefullyVV(r:Vector3,v:Vector3,e:int):Boolean{
		var l:int=lengthV(v)>>6;
		if(l>e){
			r.x=((v.x<<6)/l)<<4;
			r.y=((v.y<<6)/l)<<4;
			r.z=((v.z<<6)/l)<<4;
			return true;
		}
		return false;
	}
	
	public static function normalizeCarefullyV(v:Vector3,e:int):Boolean{
		var l:int=lengthV(v)>>6;
		if(l>e){
			v.x=((v.x<<6)/l)<<4;
			v.y=((v.y<<6)/l)<<4;
			v.z=((v.z<<6)/l)<<4;
			return true;
		}
		return false;
	}

	public static function allComponentsGreaterThanVV(v1:Vector3,v2:Vector3):Boolean{
		return (v1.x>v2.x && v1.y>v2.y && v1.z>v2.z);
	}

	public static function allComponentsLessThanVV(v1:Vector3,v2:Vector3):Boolean{
		return (v1.x<v2.x && v1.y<v2.y && v1.z<v2.z);
	}

	public static function setLerpVVV(r:Vector3,v1:Vector3,v2:Vector3,t:int):void{
		subVVV(r,v2,v1);
		mulVS(r,t);
		addVV(r,v1);
	}

	// Matrix3x3 operations
	public static function setMatrix3x3FromX(r:Matrix3x3,x:int):void{
		var cx:int=cos(x);
		var sx:int=sin(x);
		r.setS9(ONE,0,0,0,cx,-sx,0,sx,cx);
	}

	public static function setMatrix3x3FromY(r:Matrix3x3,y:int):void{
		var cy:int=cos(y);
		var sy:int=sin(y);
		r.setS9(cy,0,sy,0,ONE,0,-sy,0,cy);
	}

	public static function setMatrix3x3FromZ(r:Matrix3x3,z:int):void{
		var cz:int=cos(z);
		var sz:int=sin(z);
		r.setS9(cz,-sz,0,sz,cz,0,0,0,ONE);
	}

	public static function mulMMM(r:Matrix3x3,m1:Matrix3x3,m2:Matrix3x3):void{
		r.a00=(((m1.a00>>2)*(m2.a00>>2))>>12) + (((m1.a01>>2)*(m2.a10>>2))>>12) + (((m1.a02>>2)*(m2.a20>>2))>>12);
		r.a01=(((m1.a00>>2)*(m2.a01>>2))>>12) + (((m1.a01>>2)*(m2.a11>>2))>>12) + (((m1.a02>>2)*(m2.a21>>2))>>12);
		r.a02=(((m1.a00>>2)*(m2.a02>>2))>>12) + (((m1.a01>>2)*(m2.a12>>2))>>12) + (((m1.a02>>2)*(m2.a22>>2))>>12);

		r.a10=(((m1.a10>>2)*(m2.a00>>2))>>12) + (((m1.a11>>2)*(m2.a10>>2))>>12) + (((m1.a12>>2)*(m2.a20>>2))>>12);
		r.a11=(((m1.a10>>2)*(m2.a01>>2))>>12) + (((m1.a11>>2)*(m2.a11>>2))>>12) + (((m1.a12>>2)*(m2.a21>>2))>>12);
		r.a12=(((m1.a10>>2)*(m2.a02>>2))>>12) + (((m1.a11>>2)*(m2.a12>>2))>>12) + (((m1.a12>>2)*(m2.a22>>2))>>12);

		r.a20=(((m1.a20>>2)*(m2.a00>>2))>>12) + (((m1.a21>>2)*(m2.a10>>2))>>12) + (((m1.a22>>2)*(m2.a20>>2))>>12);
		r.a21=(((m1.a20>>2)*(m2.a01>>2))>>12) + (((m1.a21>>2)*(m2.a11>>2))>>12) + (((m1.a22>>2)*(m2.a21>>2))>>12);
		r.a22=(((m1.a20>>2)*(m2.a02>>2))>>12) + (((m1.a21>>2)*(m2.a12>>2))>>12) + (((m1.a22>>2)*(m2.a22>>2))>>12);
	}

	public static function mulVMV(r:Vector3,m:Matrix3x3,v:Vector3):void{
		r.x=(((m.a00>>2)*(v.x>>6))>>8) + (((m.a01>>2)*(v.y>>6))>>8) + (((m.a02>>2)*(v.z>>6))>>8);
		r.y=(((m.a10>>2)*(v.x>>6))>>8) + (((m.a11>>2)*(v.y>>6))>>8) + (((m.a12>>2)*(v.z>>6))>>8);
		r.z=(((m.a20>>2)*(v.x>>6))>>8) + (((m.a21>>2)*(v.y>>6))>>8) + (((m.a22>>2)*(v.z>>6))>>8);
	}

	public static function mulVM(v:Vector3,m:Matrix3x3):void{
		var tx:int=(((m.a00>>2)*(v.x>>6))>>8) + (((m.a01>>2)*(v.y>>6))>>8) + (((m.a02>>2)*(v.z>>6))>>8);
		var ty:int=(((m.a10>>2)*(v.x>>6))>>8) + (((m.a11>>2)*(v.y>>6))>>8) + (((m.a12>>2)*(v.z>>6))>>8);
		var tz:int=(((m.a20>>2)*(v.x>>6))>>8) + (((m.a21>>2)*(v.y>>6))>>8) + (((m.a22>>2)*(v.z>>6))>>8);
		v.x=tx;
		v.y=ty;
		v.z=tz;
	}

	public static function setMatrix3x3FromQuaternion(r:Matrix3x3,q:Quaternion):void{
		var tx:int=q.x<<1;
		var ty:int=q.y<<1;
		var tz:int=q.z<<1;
		var twx:int=mul(tx,q.w);
		var twy:int=mul(ty,q.w);
		var twz:int=mul(tz,q.w);
		var txx:int=mul(tx,q.x);
		var txy:int=mul(ty,q.x);
		var txz:int=mul(tz,q.x);
		var tyy:int=mul(ty,q.y);
		var tyz:int=mul(tz,q.y);
		var tzz:int=mul(tz,q.z);

		r.a00=ONE-(tyy+tzz);
		r.a01=txy-twz;
		r.a02=txz+twy;
		r.a10=txy+twz;
		r.a11=ONE-(txx+tzz);
		r.a12=tyz-twx;
		r.a20=txz-twy;
		r.a21=tyz+twx;
		r.a22=ONE-(txx+tyy);
	}

	// Quaternion operations
	public static function mulQQ(r:Quaternion,q:Quaternion):void{
		var x:int=mul(+r.x,q.w)+mul(r.y,q.z)-mul(r.z,q.y)+mul(r.w,q.x);
		var y:int=mul(-r.x,q.z)+mul(r.y,q.w)+mul(r.z,q.x)+mul(r.w,q.y);
		var z:int=mul(+r.x,q.y)-mul(r.y,q.x)+mul(r.z,q.w)+mul(r.w,q.z);
		var w:int=mul(-r.x,q.x)-mul(r.y,q.y)-mul(r.z,q.z)+mul(r.w,q.w);
		r.x=x;
		r.y=y;
		r.z=z;
		r.w=w;
	}

	public static function mulQQQ(r:Quaternion,q1:Quaternion,q2:Quaternion):void{
		r.x=mul(+q1.x,q2.w)+mul(q1.y,q2.z)-mul(q1.z,q2.y)+mul(q1.w,q2.x);
		r.y=mul(-q1.x,q2.z)+mul(q1.y,q2.w)+mul(q1.z,q2.x)+mul(q1.w,q2.y);
		r.z=mul(+q1.x,q2.y)-mul(q1.y,q2.x)+mul(q1.z,q2.w)+mul(q1.w,q2.z);
		r.w=mul(-q1.x,q2.x)-mul(q1.y,q2.y)-mul(q1.z,q2.z)+mul(q1.w,q2.w);
	}

	public static function lengthQ(q:Quaternion):int{
		var x:Number=Number(q.x)/ONE;
		var y:Number=Number(q.y)/ONE;
		var z:Number=Number(q.z)/ONE;
		var w:Number=Number(q.w)/ONE;
		return Math.sqrt(x*x+y*y+z*z+w*w)*ONE;
	}

	public static function lengthQQ(q1:Quaternion,q2:Quaternion):int{
		var x:Number=Number(q1.x-q2.x)/ONE;
		var y:Number=Number(q1.y-q2.y)/ONE;
		var z:Number=Number(q1.z-q2.z)/ONE;
		var w:Number=Number(q1.w-q2.w)/ONE;
		return Math.sqrt(x*x+y*y+z*z+w*w)*ONE;
	}

	public static function normalizeQ(q:Quaternion):void{
		var l:int=lengthQ(q)>>6;
		q.x=((q.x<<6)/l)<<4;
		q.y=((q.y<<6)/l)<<4;
		q.z=((q.z<<6)/l)<<4;
		q.w=((q.w<<6)/l)<<4;
	}

	public static function normalizeQQ(r:Quaternion,q:Quaternion):void{
		var l:int=lengthQ(q)>>6;
		r.x=((q.x<<6)/l)<<4;
		r.y=((q.y<<6)/l)<<4;
		r.z=((q.z<<6)/l)<<4;
		r.w=((q.w<<6)/l)<<4;
	}

	public static function normalizeCarefullyQ(q:Quaternion,epsilon:int):Boolean{
		var l:int=lengthQ(q)>>6;
		if(l>epsilon){
			q.x=((q.x<<6)/l)<<4;
			q.y=((q.y<<6)/l)<<4;
			q.z=((q.z<<6)/l)<<4;
			q.w=((q.w<<6)/l)<<4;
			return true;
		}
		return false;
	}

	public static function normalizeCarefullyQQ(r:Quaternion,q:Quaternion,epsilon:int):Boolean{
		var l:int=lengthQ(q)>>6;
		if(l>epsilon){
			r.x=((q.x<<6)/l)<<4;
			r.y=((q.y<<6)/l)<<4;
			r.z=((q.z<<6)/l)<<4;
			r.w=((q.w<<6)/l)<<4;
			return true;
		}
		return false;
	}

	public static function dotQQ(q1:Quaternion,q2:Quaternion):int{
		return mul(q1.x,q2.x) + mul(q1.y,q2.y) + mul(q1.z,q2.z) + mul(q1.w,q2.w);
	}

	public static function setLerpQQQ(r:Quaternion,q1:Quaternion,q2:Quaternion,t:int):void{
		var cosom:int=dotQQ(q1,q2);

		var scl1:int=(ONE-t);
		var scl2:int=0;
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
	public static function getClosestPointOnSegment(result:Vector3,segment:Segment,point:Vector3):void{
		var o:Vector3=segment.origin;
		var d:Vector3=segment.direction;

		if(d.x==0 && d.y==0 && d.z==0){
			result=o;
		}
		else{
			var u:int=div(mul(d.x,point.x-o.x)+mul(d.y,point.y-o.y)+mul(d.z,point.z-o.z),mul(d.x,d.x)+mul(d.y,d.y)+mul(d.z,d.z));
			if(u<0) u=0;
			else if(u>ONE) u=ONE;
			result.x=o.x+mul(u,d.x);
			result.y=o.y+mul(u,d.y);
			result.z=o.z+mul(u,d.z);
		}
	}

	public static function getClosestPointsOnSegments(point1:Vector3,point2:Vector3,seg1:Segment,seg2:Segment,epsilon:int):void{
		// For a full discussion of this method see Dan Sunday's Geometry Algorithms web site: http://www.geometryalgorithms.com/
		var a:int=dotVV(seg1.direction,seg1.direction);
		var b:int=dotVV(seg1.direction,seg2.direction);
		var c:int=dotVV(seg2.direction,seg2.direction);

		// Make sure we don't have a very small segment somewhere, if so we treat it as a point
		if(a<=epsilon){
			point1.setV(seg1.origin);
			getClosestPointOnSegment(point2,seg2,point1);
			return;
		}
		else if(c<epsilon){
			point2.setV(seg2.origin);
			getClosestPointOnSegment(point1,seg1,point2);
			return;
		}

		// Use point1 as a temporary
		subVVV(point1,seg1.origin,seg2.origin);
		var d:int=dotVV(seg1.direction,point1);
		var e:int=dotVV(seg2.direction,point1);
		var denom:int=mul(a,c)-mul(b,b);

		var u1:int=0;
		var u2:int=0;
		var u1N:int=0;
		var u2N:int=0;
		var u1D:int=denom;
		var u2D:int=denom;

		if(denom<100/* a small fixed point value */){
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

		mulVVS(point1,seg1.direction,u1);
		addVV(point1,seg1.origin);
		mulVVS(point2,seg2.direction,u2);
		addVV(point2,seg2.origin);
	}

	// AABox operations
	public static function addAAV(r:AABox,b:AABox,p:Vector3):void{
		r.mins.x=b.mins.x+p.x;
		r.mins.y=b.mins.y+p.y;
		r.mins.z=b.mins.z+p.z;

		r.maxs.x=b.maxs.x+p.x;
		r.maxs.y=b.maxs.y+p.y;
		r.maxs.z=b.maxs.z+p.z;
	}

	public static function addAV(b:AABox,p:Vector3):void{
		b.mins.x+=p.x;
		b.mins.y+=p.y;
		b.mins.z+=p.z;

		b.maxs.x+=p.x;
		b.maxs.y+=p.y;
		b.maxs.z+=p.z;
	}

	public static function subAAV(r:AABox,b:AABox,p:Vector3):void{
		r.mins.x=b.mins.x-p.x;
		r.mins.y=b.mins.y-p.y;
		r.mins.z=b.mins.z-p.z;

		r.maxs.x=b.maxs.x-p.x;
		r.maxs.y=b.maxs.y-p.y;
		r.maxs.z=b.maxs.z-p.z;
	}

	public static function subAV(b:AABox,p:Vector3):void{
		b.mins.x-=p.x;
		b.mins.y-=p.y;
		b.mins.z-=p.z;

		b.maxs.x-=p.x;
		b.maxs.y-=p.y;
		b.maxs.z-=p.z;
	}

	// Sphere operations
	public static function addSSV(r:Sphere,s:Sphere,p:Vector3):void{
		r.origin.x=s.origin.x+p.x;
		r.origin.y=s.origin.y+p.y;
		r.origin.z=s.origin.z+p.z;
	}

	public static function addSV(s:Sphere,p:Vector3):void{
		s.origin.x+=p.x;
		s.origin.y+=p.y;
		s.origin.z+=p.z;
	}

	public static function subSSV(r:Sphere,s:Sphere,p:Vector3):void{
		r.origin.x=s.origin.x-p.x;
		r.origin.y=s.origin.y-p.y;
		r.origin.z=s.origin.z-p.z;
	}

	public static function subSV(s:Sphere,p:Vector3):void{
		s.origin.x-=p.x;
		s.origin.y-=p.y;
		s.origin.z-=p.z;
	}

	// Capsule operations
	public static function addCCV(r:Capsule,c:Capsule,p:Vector3):void{
		r.origin.x=c.origin.x+p.x;
		r.origin.y=c.origin.y+p.y;
		r.origin.z=c.origin.z+p.z;
	}

	public static function addCV(c:Capsule,p:Vector3):void{
		c.origin.x+=p.x;
		c.origin.y+=p.y;
		c.origin.z+=p.z;
	}

	public static function subCCV(r:Capsule,c:Capsule,p:Vector3):void{
		r.origin.x=c.origin.x-p.x;
		r.origin.y=c.origin.y-p.y;
		r.origin.z=c.origin.z-p.z;
	}

	public static function subCV(c:Capsule,p:Vector3):void{
		c.origin.x-=p.x;
		c.origin.y-=p.y;
		c.origin.z-=p.z;
	}

	// Interesection operations
	public static function testInsideVVR(point:Vector3,sphereOrigin:Vector3,sphereRadius:int):Boolean{
		return tMath.lengthSquaredVV(point,sphereOrigin) <= tMath.square(sphereRadius);
	}

	public static function testInsideVS(point:Vector3,sphere:Sphere):Boolean{
		return tMath.lengthSquaredVV(point,sphere.origin) <= tMath.square(sphere.radius);
	}
	
	public static function testInsideVA(point:Vector3,box:AABox):Boolean{
		return tMath.allComponentsGreaterThanVV(point,box.mins) && tMath.allComponentsLessThanVV(point,box.maxs);
	}

	public static function testIntersectionAA(box1:AABox,box2:AABox):Boolean{
		// Look for a separating axis on each box for each axis
		return !(box1.mins.x>box2.maxs.x || box1.mins.y>box2.maxs.y || box1.mins.z>box2.maxs.z ||
			box2.mins.x>box1.maxs.x || box2.mins.y>box1.maxs.y || box2.mins.z>box1.maxs.z);
	}

	public static function findIntersectionSA(segment:Segment,box:AABox,point:Vector3,normal:Vector3):int{
		var segOrigin:Vector3=segment.origin;
		var segDirection:Vector3=segment.direction;

		var inside:Boolean=true;
		var quadX:int,quadY:int,quadZ:int;
		var whichPlane:int;
		var maxTX:int=0,maxTY:int=0,maxTZ:int=0;
		var candPlaneX:int=0,candPlaneY:int=0,candPlaneZ:int=0;
		var candNormX:Vector3=tMath.NEG_X_UNIT_VECTOR3;
		var candNormY:Vector3=tMath.NEG_Y_UNIT_VECTOR3;
		var candNormZ:Vector3=tMath.NEG_Z_UNIT_VECTOR3;
		var time:int=-tMath.ONE;

		// The below tests were originally < or >, but are now <= or >=
		// Without this change finds that start on the edge of a box count as inside which conflicts with how testInside works

		// X
		if(segment.origin.x<=box.mins.x){
			quadX=0; // Left
			candPlaneX=box.mins.x;
			inside=false;
		}
		else if(segment.origin.x>=box.maxs.x){
			quadX=1; // Right
			candPlaneX=box.maxs.x;
			candNormX=tMath.X_UNIT_VECTOR3;
			inside=false;
		}
		else{
			quadX=2; // Middle
		}
		// Y
		if(segment.origin.y<=box.mins.y){
			quadY=0; // Left
			candPlaneY=box.mins.y;
			inside=false;
		}
		else if(segment.origin.y>=box.maxs.y){
			quadY=1; // Right
			candPlaneY=box.maxs.y;
			candNormY=tMath.Y_UNIT_VECTOR3;
			inside=false;
		}
		else{
			quadY=2; // Middle
		}
		// Z
		if(segment.origin.z<=box.mins.z){
			quadZ=0; // Left
			candPlaneZ=box.mins.z;
			inside=false;
		}
		else if(segment.origin.z>=box.maxs.z){
			quadZ=1; // Right
			candPlaneZ=box.maxs.z;
			candNormZ=tMath.Z_UNIT_VECTOR3;
			inside=false;
		}
		else{
			quadZ=2; // Middle
		}

		// Inside
		if(inside){
			return -tMath.ONE;
		}

		// Calculate t distances to candidate planes
		// X
		if(quadX!=2 /*Middle*/ && segment.direction.x!=0){
			maxTX=tMath.div(candPlaneX-segOrigin.x,segDirection.x);
		}
		else{
			maxTX=-tMath.ONE;
		}
		// Y
		if(quadY!=2 /*Middle*/ && segment.direction.y!=0){
			maxTY=tMath.div(candPlaneY-segOrigin.y,segDirection.y);
		}
		else{
			maxTY=-tMath.ONE;
		}
		// Z
		if(quadZ!=2 /*Middle*/ && segment.direction.z!=0){
			maxTZ=tMath.div(candPlaneZ-segOrigin.z,segDirection.z);
		}
		else{
			maxTZ=-tMath.ONE;
		}
	
		// Find largets of maxT's
		// X
		if(maxTX>maxTY && maxTX>maxTZ){
			whichPlane=0;
			time=maxTX;
			normal.setV(candNormX);
		}
		// Y
		else if(maxTY>maxTZ){
			whichPlane=1;
			time=maxTY;
			normal.setV(candNormY);
		}
		// Z
		else{
			whichPlane=2;
			time=maxTZ;
			normal.setV(candNormZ);
		}
	
		// Check final candidate actually inside box and calculate final point
		if(time<0 || time>tMath.ONE){
			return -tMath.ONE<<1;
		}
		// X
		if(whichPlane!=0){
			point.x=segOrigin.x+tMath.mul(time,segDirection.x);
			if(point.x<box.mins.x || point.x>box.maxs.x){
				return -tMath.ONE;
			}
		}
		else{
			point.x=candPlaneX;
		}
		// Y
		if(whichPlane!=1){
			point.y=segOrigin.y+tMath.mul(time,segDirection.y);
			if(point.y<box.mins.y || point.y>box.maxs.y){
				return -tMath.ONE;
			}
		}
		else{
			point.y=candPlaneY;
		}
		// Z
		if(whichPlane!=2){
			point.z=segOrigin.z+tMath.mul(time,segDirection.z);
			if(point.z<box.mins.z || point.z>box.maxs.z){
				return -tMath.ONE;
			}
		}
		else{
			point.z=candPlaneZ;
		}
		
		return time;
	}

	public static function findIntersectionSS(segment:Segment,sphere:Sphere,point:Vector3,normal:Vector3):int{
		var segOrigin:Vector3=segment.origin;
		var segDirection:Vector3=segment.direction;
		var sphOrigin:Vector3=sphere.origin;
		var sphRadius:int=sphere.radius;

		// Quadratic Q(t) = a*t^2 + 2*b*t + c
		var diffX:int=segOrigin.x-sphOrigin.x;
		var diffY:int=segOrigin.y-sphOrigin.y;
		var diffZ:int=segOrigin.z-sphOrigin.z;
		var a:int=tMath.lengthSquaredV(segDirection);
		if(a<=0){
			return -tMath.ONE;
		}
		var b:int=tMath.mul(diffX,segDirection.x) + tMath.mul(diffY,segDirection.y) + tMath.mul(diffZ,segDirection.z);
		var c:int=tMath.mul(diffX,diffX) + tMath.mul(diffY,diffY) + tMath.mul(diffZ,diffZ)
			-tMath.square(sphRadius);
		var time1:int=-tMath.ONE;
		var time2:int=-tMath.ONE;

		// If no real roots, then no intersection
		var discr:int=tMath.mul(b,b) - tMath.mul(a,c);
		if(discr<0){
			time1=-tMath.ONE;
		}
		else if(discr>0){
			var root:int=tMath.sqrt(discr);
			var invA:int=tMath.mul(tMath.ONE,a);
			time1=tMath.mul(((-b)-root),invA);
			time2=tMath.mul(((-b)+root),invA);

			if(time1>tMath.ONE || time2<0){
				time1=-tMath.ONE;
			}
			else if(time1>=0){
				tMath.mulVVS(point,segDirection,time1);
				tMath.addVV(point,segOrigin);
			}
			else{
				tMath.mulVVS(point,segDirection,time2);
				tMath.addVV(point,segOrigin);
			}
		}
		else{
			time1=tMath.div(-b,a);
			if(0<=time1 && time1<=tMath.ONE){
				tMath.mulVVS(point,segDirection,time1);
				tMath.addVV(point,segOrigin);
			}
			else{
				time1=-tMath.ONE;
			}
		}
	
		if(time1!=-tMath.ONE){
			tMath.subVVV(normal,point,sphOrigin);
			tMath.normalizeCarefullyV(normal,0);
		}
		return time1;
	}
}

}
