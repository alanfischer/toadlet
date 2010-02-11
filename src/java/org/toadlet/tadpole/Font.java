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

package org.toadlet.tadpole;

#include <org/toadlet/tadpole/Types.h>

import org.toadlet.egg.Categories;
import org.toadlet.egg.Error;
import org.toadlet.egg.Resource;
import org.toadlet.peeper.*;

public class Font implements Resource{
	public static class Glyph{
		public Glyph(){}

		public scalar advancex=0;
		public scalar advancey=0;
		public int x=0;
		public int y=0;
		public short width=0;
		public short height=0;
		public short offsetx=0;
		public short offsety=0;
	}

	public static class Alignment{
		public static int BIT_LEFT=		1<<0;
		public static int BIT_RIGHT=	1<<1;
		public static int BIT_HCENTER=	1<<2;
		public static int BIT_TOP=		1<<3;
		public static int BIT_BOTTOM=	1<<4;
		public static int BIT_VCENTER=	1<<5;
	}

	public Font(float pointWidth,float pointHeight,int height,int innerSpace,Texture texture,char[] charSet,Glyph[] glyphs,int numChars){
		mPointWidth=pointWidth;
		mPointHeight=pointHeight;
		mHeight=height;
		mInnerSpace=innerSpace;
		mTexture=texture;

		mGlyphs=new Glyph[Byte.MAX_VALUE-Byte.MIN_VALUE];
		int i;
		for(i=0;i<numChars;++i){
			char c=charSet[i];
			mGlyphs[c]=glyphs[i];
		}
	}

	public boolean updateVertexBufferForString(VertexBuffer vertexBuffer,CharSequence string,Color color,int alignment){
		VertexFormat format=vertexBuffer.getVertexFormat();

		int positionElement=format.getVertexElementIndexOfType(VertexElement.Type.POSITION);
		int texCoordElement=format.getVertexElementIndexOfType(VertexElement.Type.TEX_COORD);
		int colorElement=format.getVertexElementIndexOfType(VertexElement.Type.COLOR);

		if(positionElement<0 || texCoordElement<0){
			Error.unknown(Categories.TOADLET_TADPOLE,
				"invalid VertexFormat");
			return false;
		}

		int abgr=color.getABGR();

		int maxLength=vertexBuffer.getSize()/4;
		int length=string.length();
		if(length>maxLength){
			Error.unknown(Categories.TOADLET_TADPOLE,
				"VertexBuffer too small");
			return false;
		}

		scalar scalarTexWidth=Math.fromInt(mTexture.getWidth());
		scalar scalarTexHeight=Math.fromInt(mTexture.getHeight());

		int width=getStringWidth(string),height=getStringHeight(string);

		int x=0;
		int y=0;
		if((alignment&Alignment.BIT_TOP)>0){
		}
		else if((alignment&Alignment.BIT_BOTTOM)>0){
			y-=height;
		}
		else if((alignment&Alignment.BIT_VCENTER)>0){
			y-=height/2;
		}

		vba.lock(vertexBuffer,Buffer.LockType.WRITE_ONLY);

		int lineLength=0;
		char c=(char)10; // The first loop will add mHeight, but we work from the baseline of the letters, so that is correct

		int i,j;
		for(i=0;i<length;++i){
			if(c==(char)10){
				for(j=i;j<length && string.charAt(j)!=(char)10;++j);
				lineLength=getStringWidth(string,i,j);

				if((alignment&Alignment.BIT_LEFT)>0){
					x=0;
				}
				else if((alignment&Alignment.BIT_RIGHT)>0){
					x=-lineLength;
				}
				else if((alignment&Alignment.BIT_HCENTER)>0){
					x=-lineLength/2;
				}

				y+=mHeight;
			}

			c=string.charAt(i);

			int i4=i*4;
			Glyph g=getGlyph(c);
			if(g!=null){
				vba.set3(i4+0,positionElement,	Math.fromInt(x+g.offsetx), Math.fromInt(y+g.offsety), 0);
				vba.set2(i4+0,texCoordElement,	Math.div(Math.fromInt(g.x),scalarTexWidth), Math.div(Math.fromInt(g.y),scalarTexHeight));

				vba.set3(i4+1,positionElement,	Math.fromInt(x+g.offsetx+g.width), Math.fromInt(y+g.offsety), 0);
				vba.set2(i4+1,texCoordElement,	Math.div(Math.fromInt(g.x+g.width),scalarTexWidth), Math.div(Math.fromInt(g.y),scalarTexHeight));

				vba.set3(i4+2,positionElement,	Math.fromInt(x+g.offsetx+g.width), Math.fromInt(y+g.offsety+g.height), 0);
				vba.set2(i4+2,texCoordElement,	Math.div(Math.fromInt(g.x+g.width),scalarTexWidth), Math.div(Math.fromInt(g.y+g.height),scalarTexHeight));

				vba.set3(i4+3,positionElement,	Math.fromInt(x+g.offsetx), Math.fromInt(y+g.offsety+g.height), 0);
				vba.set2(i4+3,texCoordElement,	Math.div(Math.fromInt(g.x),scalarTexWidth), Math.div(Math.fromInt(g.y+g.height),scalarTexHeight));

				x+=Math.toInt(g.advancex);
				y+=Math.toInt(g.advancey);

				if(colorElement>=0){
					vba.setABGR(i4+0,colorElement,abgr);
					vba.setABGR(i4+1,colorElement,abgr);
					vba.setABGR(i4+2,colorElement,abgr);
					vba.setABGR(i4+3,colorElement,abgr);
				}
			}
			else{
				vba.set3(i4+0,positionElement,0,0,0);
				vba.set3(i4+1,positionElement,0,0,0);
				vba.set3(i4+2,positionElement,0,0,0);
				vba.set3(i4+3,positionElement,0,0,0);
			}
		}

		// Zero out the rest of the vertex buffer
		for(;i<maxLength;++i){
			int i4=i*4;
			vba.set3(i4+0,positionElement,0,0,0);
			vba.set3(i4+1,positionElement,0,0,0);
			vba.set3(i4+2,positionElement,0,0,0);
			vba.set3(i4+3,positionElement,0,0,0);
		}

		vba.unlock();

		return true;
	}

	public int getStringWidth(CharSequence string){return getStringWidth(string,0,string.length());}
	public int getStringWidth(CharSequence string,int start,int end){
		int maxWidth=0;
		int width=0;

		int i;
		for(i=start;i<end;++i){
			char c=string.charAt(i);
			Glyph g=mGlyphs[c];
			if(g!=null){
				width+=Math.toInt(g.advancex);
				width+=mInnerSpace;
			}
			else{
				if(c==(char)10){
					if(maxWidth<width){
						maxWidth=width;
					}
					width=0;
				}
			}
		}
		if(maxWidth<width){
			maxWidth=width;
		}

		return maxWidth;
	}

	public int getStringHeight(CharSequence string){return getStringHeight(string,0,string.length());}
	public int getStringHeight(CharSequence string,int start,int end){
		int height=mHeight;

		int i;
		for(i=start;i<end;++i){
			char c=string.charAt(i);
			if(c==(char)10){
				height+=mHeight;
			}
		}

		return height;
	}

	public float getPointWidth(){return mPointWidth;}
	public float getPointHeight(){return mPointHeight;}
	public int getHeight(){return mHeight;}
	public int getInnerSpace(){return mInnerSpace;}
	public Texture getTexture(){return mTexture;}
	public Glyph getGlyph(int i){if(i<0 || i>=mGlyphs.length){return null;}else{return mGlyphs[i];}}

	public VertexBufferAccessor vba=new VertexBufferAccessor();
	
	protected float mPointWidth=0;
	protected float mPointHeight=0;
	protected int mHeight=0; // TODO: Perhaps somehow mHeight should be depreciated, and we force all the font loaders to actually have a consistant mPointHeight
	protected int mInnerSpace=0;
	protected Texture mTexture=null;
	protected Glyph[] mGlyphs=null;
}
