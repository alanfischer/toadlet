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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/tadpole/Font.h>
#include <string.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

Font::Font(float pointWidth,float pointHeight,int height,int innerSpace,Texture::ptr texture,const wchar *charSet,Glyph **glyphs,int numChars):BaseResource(),
	mPointWidth(0),
	mPointHeight(0),
	mHeight(0),
	mInnerSpace(0)
	//mTexture,
	//mGlyphs
{
	mPointWidth=pointWidth;
	mPointHeight=pointHeight;
	mHeight=height;
	mInnerSpace=innerSpace;
	mTexture=texture;

	mGlyphs.resize(Extents::MAX_UNSIGNED_CHAR,NULL);
	int i;
	for(i=0;i<numChars;++i){
		unsigned char c=(unsigned char)charSet[i];
		if(mGlyphs[c]!=NULL){
			delete mGlyphs[c];
		}
		mGlyphs[c]=glyphs[i];
	}
}

Font::~Font(){
	destroy();

	int i;
	for(i=0;i<mGlyphs.size();++i){
		if(mGlyphs[i]!=NULL){
			delete mGlyphs[i];
			mGlyphs[i]=NULL;
		}
	}
}

void Font::destroy(){
	if(mTexture!=NULL){
		mTexture->release();
		mTexture=NULL;
	}
}

bool Font::updateVertexBufferForString(VertexBuffer::ptr vertexBuffer,const String &string,const Color &color,int alignment){
	VertexFormat *format=vertexBuffer->getVertexFormat();

	int positionElement=format->getVertexElementIndexOfType(VertexElement::Type_POSITION);
	int texCoordElement=format->getVertexElementIndexOfType(VertexElement::Type_TEX_COORD);
	int colorElement=format->getVertexElementIndexOfType(VertexElement::Type_COLOR_DIFFUSE);

	if(positionElement<0 || texCoordElement<0){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"invalid VertexFormat");
		return false;
	}

	uint32 abgr=color.getABGR();

	int maxLength=vertexBuffer->getSize()/4;
	int length=string.length();
	if(length>maxLength){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"VertexBuffer too small");
		return false;
	}

	scalar scalarTexWidth=Math::fromInt(mTexture->getWidth());
	scalar scalarTexHeight=Math::fromInt(mTexture->getHeight());

	int height=getStringHeight(string);

	int x=0;
	int y=0;
	if((alignment&Alignment_BIT_TOP)>0){
	}
	else if((alignment&Alignment_BIT_BOTTOM)>0){
		y+=height;
	}
	else if((alignment&Alignment_BIT_VCENTER)>0){
		y+=height/2;
	}

	vba.lock(vertexBuffer,Buffer::AccessType_WRITE_ONLY);

	int lineLength=0;
	char c=(char)10; // The first loop will add mHeight, but we work from the baseline of the letters, so that is correct

	int i,j;
	for(i=0;i<length;++i){
		if(c==(char)10){
			for(j=i;j<length && string.at(j)!=(char)10;++j);
			lineLength=getStringWidth(string,i,j);

			if((alignment&Alignment_BIT_LEFT)>0){
				x=0;
			}
			else if((alignment&Alignment_BIT_RIGHT)>0){
				x=-lineLength;
			}
			else if((alignment&Alignment_BIT_HCENTER)>0){
				x=-lineLength/2;
			}

			y-=mHeight;
		}

		c=string.at(i);

		int i4=i*4;
		Glyph *g=getGlyph(c);
		if(g!=NULL){
			vba.set3(i4+0,positionElement,	Math::fromInt(x+g->offsetx), Math::fromInt(y+g->offsety+g->height), 0);
			vba.set2(i4+0,texCoordElement,	Math::div(Math::fromInt(g->x),scalarTexWidth), Math::div(Math::fromInt(g->y),scalarTexHeight));

			vba.set3(i4+1,positionElement,	Math::fromInt(x+g->offsetx+g->width), Math::fromInt(y+g->offsety+g->height), 0);
			vba.set2(i4+1,texCoordElement,	Math::div(Math::fromInt(g->x+g->width),scalarTexWidth), Math::div(Math::fromInt(g->y),scalarTexHeight));

			vba.set3(i4+2,positionElement,	Math::fromInt(x+g->offsetx+g->width), Math::fromInt(y+g->offsety), 0);
			vba.set2(i4+2,texCoordElement,	Math::div(Math::fromInt(g->x+g->width),scalarTexWidth), Math::div(Math::fromInt(g->y+g->height),scalarTexHeight));

			vba.set3(i4+3,positionElement,	Math::fromInt(x+g->offsetx), Math::fromInt(y+g->offsety), 0);
			vba.set2(i4+3,texCoordElement,	Math::div(Math::fromInt(g->x),scalarTexWidth), Math::div(Math::fromInt(g->y+g->height),scalarTexHeight));

			x+=Math::toInt(g->advancex);
			y+=Math::toInt(g->advancey);

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

int Font::getStringWidth(const String &string,int start,int end) const{
	int maxWidth=0;
	int width=0;

	int i;
	for(i=start;i<end;++i){
		wchar c=string.at(i);
		Glyph *g=mGlyphs[c];
		if(g!=NULL){
			width+=Math::toInt(g->advancex);
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

int Font::getStringHeight(const String &string,int start,int end) const{
	int height=mHeight;

	int i;
	for(i=start;i<end;++i){
		wchar c=string.at(i);
		if(c==(char)10){
			height+=mHeight;
		}
	}

	return height;
}

}
}
