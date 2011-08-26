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

public final class Color{
	public scalar r,g,b,a;

	public Color(){}

	public Color(scalar r1,scalar g1,scalar b1,scalar a1){
		r=r1;
		g=g1;
		b=b1;
		a=a1;
	}

	public Color(Color color){
		r=color.r;
		g=color.g;
		b=color.b;
		a=color.a;
	}

	public static Color rgba(int rgba){
		Color color=new Color();
		color.setRGBA(rgba);
		return color;
	}

	public static Color bgra(int abgr){
		Color color=new Color();
		color.setABGR(abgr);
		return color;
	}

	public Color set(Color c){
		r=c.r;
		g=c.g;
		b=c.b;
		a=c.a;

		return this;
	}

	public Color set(scalar r1,scalar g1,scalar b1,scalar a1){
		r=r1;
		g=g1;
		b=b1;
		a=a1;

		return this;
	}

	public Color setRGBA(int rgba){
		r=Math.fromInt((rgba&0xFF000000)>>>24)/255;
		g=Math.fromInt((rgba&0x00FF0000)>>>16)/255;
		b=Math.fromInt((rgba&0x0000FF00)>>>8 )/255;
		a=Math.fromInt((rgba&0x000000FF)>>>0 )/255;

		return this;
	}

	public Color setABGR(int abgr){
		a=Math.fromInt((abgr&0xFF000000)>>>24)/255;
		b=Math.fromInt((abgr&0x00FF0000)>>>16)/255;
		g=Math.fromInt((abgr&0x0000FF00)>>>8 )/255;
		r=Math.fromInt((abgr&0x000000FF)>>>0 )/255;

		return this;
	}

	public Color reset(){
		r=0;
		g=0;
		b=0;
		a=0;

		return this;
	}

	public int getRGBA(){
		return 
			((int)Math.toInt(r*255))<<24 |
			((int)Math.toInt(g*255))<<16 |
			((int)Math.toInt(b*255))<<8  |
			((int)Math.toInt(a*255))<<0;
	}

	public int getABGR(){
		return 
			((int)Math.toInt(r*255))<<0  |
			((int)Math.toInt(g*255))<<8  |
			((int)Math.toInt(b*255))<<16 |
			((int)Math.toInt(a*255))<<24;
	}

	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		Color color=(Color)object;
		return r==color.r && g==color.g && b==color.b && a==color.a;
	}

	public int hashCode(){
		return (int)(r + g + b + a);
	}

	public static void lerp(Color result,Color start,Color end,scalar time){
		result.r=Math.lerp(start.r,end.r,time);
		result.g=Math.lerp(start.g,end.g,time);
		result.b=Math.lerp(start.b,end.b,time);
		result.a=Math.lerp(start.a,end.a,time);
	}

	// Useable for both RGBAByteColor and ABGRByteColor
	public static int lerp(int start,int end,scalar time){
		return
			(((int)Math.lerp((start&0xFF000000)>>>24,(end&0xFF000000)>>>24,time))<<24) |
			(((int)Math.lerp((start&0x00FF0000)>>>16,(end&0x00FF0000)>>>16,time))<<16) |
			(((int)Math.lerp((start&0x0000FF00)>>>8 ,(end&0x0000FF00)>>>8 ,time))<<8 ) |
			(((int)Math.lerp((start&0x000000FF)    ,(end&0x000000FF)    ,time))    ) ;
	}

	// Changes from rgba to abgr and vice versa
	public static int swap(int bytes){
		return
			((bytes&0xFF000000)>>>24) |
			((bytes&0x00FF0000)>>>8 ) |
			((bytes&0x0000FF00)<<8 ) |
			((bytes&0x000000FF)<<24) ;
	}
}
