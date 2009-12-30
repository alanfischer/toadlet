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

#ifndef TOADLET_PEEPER_COLOR_H
#define TOADLET_PEEPER_COLOR_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class Color{
public:
	scalar r,g,b,a;

	inline Color():r(0),g(0),b(0),a(0){}

	inline Color(scalar r1,scalar g1,scalar b1,scalar a1):r(r1),g(g1),b(b1),a(a1){}

	static inline Color rgba(uint32 rgba){
		Color color;
		color.setRGBA(rgba);
		return color;
	}

	static inline Color abgr(uint32 abgr){
		Color color;
		color.setABGR(abgr);
		return color;
	}

	inline Color &set(const Color &c){
		r=c.r;
		g=c.g;
		b=c.b;
		a=c.a;

		return *this;
	}

	inline Color &set(scalar r1,scalar g1,scalar b1,scalar a1){
		r=r1;
		g=g1;
		b=b1;
		a=a1;

		return *this;
	}

	inline Color &setRGBA(uint32 rgba){
		r=Math::fromInt((rgba&0xFF000000)>>24)/255;
		g=Math::fromInt((rgba&0x00FF0000)>>16)/255;
		b=Math::fromInt((rgba&0x0000FF00)>>8 )/255;
		a=Math::fromInt((rgba&0x000000FF)>>0 )/255;

		return *this;
	}

	inline Color &setABGR(uint32 abgr){
		a=Math::fromInt((abgr&0xFF000000)>>24)/255;
		b=Math::fromInt((abgr&0x00FF0000)>>16)/255;
		g=Math::fromInt((abgr&0x0000FF00)>>8 )/255;
		r=Math::fromInt((abgr&0x000000FF)>>0 )/255;

		return *this;
	}

	inline Color &reset(){
		r=0;
		g=0;
		b=0;
		a=0;

		return *this;
	}

	inline uint32 getRGBA() const{
		return 
			((uint32)Math::toInt(r*255))<<24 |
			((uint32)Math::toInt(g*255))<<16 |
			((uint32)Math::toInt(b*255))<<8  |
			((uint32)Math::toInt(a*255))<<0;
	}

	inline uint32 getABGR() const{
		return 
			((uint32)Math::toInt(r*255))<<0  |
			((uint32)Math::toInt(g*255))<<8  |
			((uint32)Math::toInt(b*255))<<16 |
			((uint32)Math::toInt(a*255))<<24;
	}

	inline scalar *getData(){return &r;}
	inline const scalar *getData() const{return &r;}

	inline bool equals(const Color &c) const{
		return (c.r==r && c.g==g && c.b==b && c.a==a);
	}

	inline bool operator==(const Color &c) const{
		return (c.r==r && c.g==g && c.b==b && c.a==a);
	}

	inline bool operator!=(const Color &c) const{
		return (c.r!=r || c.g!=g || c.b!=b || c.a!=a);
	}

	inline Color operator+(const Color &c) const{
		return Color(r+c.r,g+c.g,b+c.b,a+c.a);
	}

	inline void operator+=(const Color &c){
		r+=c.r;
		g+=c.g;
		b+=c.b;
		a+=c.a;
	}

	inline Color operator-(const Color &c) const{
		return Color(r-c.r,g-c.g,b-c.b,a-c.a);
	}

	inline void operator-=(const Color &c){
		r-=c.r;
		g-=c.g;
		b-=c.b;
		a-=c.a;
	}

	inline Color operator*(scalar f) const{
		return Color(Math::mul(r,f),Math::mul(g,f),Math::mul(b,f),Math::mul(a,f));
	}

	inline void operator*=(scalar f){
		r=Math::mul(r,f);
		g=Math::mul(g,f);
		b=Math::mul(b,f);
		a=Math::mul(a,f);
	}

	inline Color operator*(const Color &c) const{
		return Color(Math::mul(r,c.r),Math::mul(g,c.g),Math::mul(b,c.b),Math::mul(a,c.a));
	}

	inline void operator*=(const Color &c){
		r=Math::mul(r,c.r);
		g=Math::mul(g,c.g);
		b=Math::mul(b,c.b);
		a=Math::mul(a,c.a);
	}

	inline Color operator/(scalar f) const{
		f=Math::div(Math::ONE,f);
		return Color(Math::mul(r,f),Math::mul(g,f),Math::mul(b,f),Math::mul(a,f));
	}

	inline void operator/=(scalar f){
		f=Math::mul(Math::ONE,f);
		r=Math::mul(r,f);
		g=Math::mul(g,f);
		b=Math::mul(b,f);
		a=Math::mul(a,f);
	}

	inline Color operator/(const Color &c) const{
		return Color(Math::mul(r,c.r),Math::mul(g,c.g),Math::mul(b,c.b),Math::mul(a,c.a));
	}

	inline void operator/=(const Color &c){
		r=Math::mul(r,c.r);
		g=Math::mul(g,c.g);
		b=Math::mul(b,c.b);
		a=Math::mul(a,c.a);
	}

	inline Color operator-() const{
		return Color(-r,-g,-b,-a);
	}

	inline scalar &operator[](int i){
		return *(&r+i);
	}

	inline scalar operator[](int i) const{
		return *(&r+i);
	}

	static inline void neg(Color &r,Color &c){
		r.r=-c.r;
		r.g=-c.g;
		r.b=-c.b;
		r.a=-c.a;
	}

	static inline void neg(Color &c){
		c.r=-c.r;
		c.g=-c.g;
		c.b=-c.b;
		c.a=-c.a;
	}

	static inline void add(Color &r,const Color &c1,const Color &c2){
		r.r=c1.r+c2.r;
		r.g=c1.g+c2.g;
		r.b=c1.b+c2.b;
		r.a=c1.a+c2.a;
	}

	static inline void add(Color &r,const Color &c){
		r.r+=c.r;
		r.g+=c.g;
		r.b+=c.b;
		r.a+=c.a;
	}
	
	static inline void sub(Color &r,const Color &c1,const Color &c2){
		r.r=c1.r-c2.r;
		r.g=c1.g-c2.g;
		r.b=c1.b-c2.b;
		r.a=c1.a-c2.a;
	}
	
	static inline void sub(Color &r,const Color &c){
		r.r-=c.r;
		r.g-=c.g;
		r.b-=c.b;
		r.a-=c.a;
	}

	static inline void mul(Color &r,const Color &c,scalar f){
		r.r=Math::mul(c.r,f);
		r.g=Math::mul(c.g,f);
		r.b=Math::mul(c.b,f);
		r.a=Math::mul(c.a,f);
	}
	
	static inline void mul(Color &r,scalar f){
		r.r=Math::mul(r.r,f);
		r.g=Math::mul(r.g,f);
		r.b=Math::mul(r.b,f);
		r.a=Math::mul(r.a,f);
	}

	static inline void mul(Color &r,const Color &c1,const Color &c2){
		r.r=Math::mul(c1.r,c2.r);
		r.g=Math::mul(c1.g,c2.g);
		r.b=Math::mul(c1.b,c2.b);
		r.a=Math::mul(c1.a,c2.a);
	}

	static inline void mul(Color &r,const Color &c){
		r.r=Math::mul(r.r,c.r);
		r.g=Math::mul(r.g,c.g);
		r.b=Math::mul(r.b,c.b);
		r.a=Math::mul(r.a,c.a);
	}

	static inline void div(Color &r,const Color &c,scalar f){
		scalar i=Math::div(Math::ONE,f);
		r.r=Math::mul(c.r,i);
		r.g=Math::mul(c.g,i);
		r.b=Math::mul(c.b,i);
		r.a=Math::mul(c.a,i);
	}
	
	static inline void div(Color &r,scalar f){
		scalar i=Math::div(Math::ONE,f);
		r.r=Math::mul(r.r,i);
		r.g=Math::mul(r.g,i);
		r.b=Math::mul(r.b,i);
		r.a=Math::mul(r.a,i);
	}

	static inline void div(Color &r,const Color &c1,const Color &c2){
		r.r=Math::div(c1.r,c2.r);
		r.g=Math::div(c1.g,c2.g);
		r.b=Math::div(c1.b,c2.b);
		r.a=Math::div(c1.a,c2.a);
	}

	static inline void div(Color &r,const Color &c){
		r.r=Math::div(r.r,c.r);
		r.g=Math::div(r.g,c.g);
		r.b=Math::div(r.b,c.b);
		r.a=Math::div(r.a,c.a);
	}

	static inline void madd(Color &r,const Color &b,scalar c,const Color &a){
		r.r=Math::madd(b.r,c,a.r);
		r.g=Math::madd(b.g,c,a.g);
		r.b=Math::madd(b.b,c,a.b);
		r.a=Math::madd(b.a,c,a.a);
	}

	static inline void madd(Color &r,scalar c,const Color &a){
		r.r=Math::madd(r.r,c,a.r);
		r.g=Math::madd(r.g,c,a.g);
		r.b=Math::madd(r.b,c,a.b);
		r.a=Math::madd(r.a,c,a.a);
	}

	static inline void msub(Color &r,const Color &b,scalar c,const Color &a){
		r.r=Math::msub(b.r,c,a.r);
		r.g=Math::msub(b.g,c,a.g);
		r.b=Math::msub(b.b,c,a.b);
		r.a=Math::msub(b.a,c,a.a);
	}

	static inline void msub(Color &r,scalar c,const Color &a){
		r.r=Math::msub(r.r,c,a.r);
		r.g=Math::msub(r.g,c,a.g);
		r.b=Math::msub(r.b,c,a.b);
		r.a=Math::msub(r.a,c,a.a);
	}

	static inline void lerp(Color &r,const Color &c1,const Color &c2,scalar t){
		r.r=Math::lerp(c1.r,c2.r,t);
		r.g=Math::lerp(c1.g,c2.g,t);
		r.b=Math::lerp(c1.b,c2.b,t);
		r.a=Math::lerp(c1.a,c2.a,t);
	}

	// Useable for both rgba and abgr
	static inline uint32 lerp(uint32 c1,uint32 c2,scalar t){
		return
			(((int)Math::lerp((c1&0xFF000000)>>24,(c2&0xFF000000)>>24,t))<<24) |
			(((int)Math::lerp((c1&0x00FF0000)>>16,(c2&0x00FF0000)>>16,t))<<16) |
			(((int)Math::lerp((c1&0x0000FF00)>>8 ,(c2&0x0000FF00)>>8 ,t))<<8 ) |
			(((int)Math::lerp((c1&0x000000FF)    ,(c2&0x000000FF)    ,t))    ) ;
	}

	// Changes from rgba to abgr and vice versa
	static inline uint32 swap(uint32 bytes){
		return
			((bytes&0xFF000000)>>24) |
			((bytes&0x00FF0000)>>8 ) |
			((bytes&0x0000FF00)<<8 ) |
			((bytes&0x000000FF)<<24) ;
	}
};

}
}

#endif
