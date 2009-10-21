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

	inline scalar &operator[](int i){
		return *(&r+i);
	}

	inline scalar operator[](int i) const{
		return *(&r+i);
	}

	static inline void lerp(Color &result,const Color &start,const Color &end,scalar time){
		result.r=Math::lerp(start.r,end.r,time);
		result.g=Math::lerp(start.g,end.g,time);
		result.b=Math::lerp(start.b,end.b,time);
		result.a=Math::lerp(start.a,end.a,time);
	}

	// Useable for both rgba and abgr
	static inline uint32 lerp(uint32 start,uint32 end,scalar time){
		return
			(((int)Math::lerp((start&0xFF000000)>>24,(end&0xFF000000)>>24,time))<<24) |
			(((int)Math::lerp((start&0x00FF0000)>>16,(end&0x00FF0000)>>16,time))<<16) |
			(((int)Math::lerp((start&0x0000FF00)>>8 ,(end&0x0000FF00)>>8 ,time))<<8 ) |
			(((int)Math::lerp((start&0x000000FF)    ,(end&0x000000FF)    ,time))    ) ;
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
