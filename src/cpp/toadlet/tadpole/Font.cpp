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

namespace toadlet{
namespace tadpole{

Font::Font(float pointSize,int innerSpace,Texture::ptr texture,const wchar *charSet,Glyph **glyphs,int numChars):BaseResource(),
	mPointSize(0),
	mInnerSpace(0)
	//mTexture,
	//mGlyphs
{
	mPointSize=pointSize;
	mInnerSpace=innerSpace;
	mTexture=texture;
	mTexture->retain();

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

bool Font::updateVertexBufferForString(VertexBuffer::ptr vertexBuffer,const String &string,const Vector4 &color,int alignment,bool pixelSpace,bool flip){
	VertexFormat *format=vertexBuffer->getVertexFormat();

	int positionIndex=format->findElement(VertexFormat::Semantic_POSITION);
	int texCoordIndex=format->findElement(VertexFormat::Semantic_TEXCOORD);
	int colorIndex=format->findElement(VertexFormat::Semantic_COLOR);

	if(positionIndex<0 || texCoordIndex<0){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"invalid VertexFormat");
		return false;
	}

	uint32 rgba=color.getRGBA();

	int maxLength=vertexBuffer->getSize()/4;
	int length=string.length();
	if(length>maxLength){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"VertexBuffer too small");
		return false;
	}

	scalar scalarTexWidth=Math::fromInt(mTexture->getFormat()->width);
	scalar scalarTexHeight=Math::fromInt(mTexture->getFormat()->height);

	int height=getStringHeight(string);

	int x=0;
	int y=0;
	if(flip==false){
		if((alignment&Alignment_BIT_TOP)>0){
		}
		else if((alignment&Alignment_BIT_BOTTOM)>0){
			y-=height;
		}
		else if((alignment&Alignment_BIT_VCENTER)>0){
			y-=height/2;
		}
	}
	else{
		if((alignment&Alignment_BIT_TOP)>0){
		}
		else if((alignment&Alignment_BIT_BOTTOM)>0){
			y+=height;
		}
		else if((alignment&Alignment_BIT_VCENTER)>0){
			y+=height/2;
		}
	}

	vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);

	int lineLength=0;
	char c=(char)10;

//	if(flip){
//		y+=mPointSize;
//	}

	scalar pointSize=
		#if defined(TOADLET_FIXED_POINT)
			Math::fromFloat(mPointSize);
		#else
			mPointSize;
		#endif

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

			if(flip==false){
				y+=(int)mPointSize;
			}
			else{
				y-=(int)mPointSize;
			}
		}

		c=string.at(i);

		int i4=i*4;
		Glyph *g=getGlyph(c);
		if(g!=NULL){
			scalar goffsety=0,gheight=0,gadvancey=0;
			if(flip==false){
				goffsety=g->offsety;
				gheight=g->height;
				gadvancey=g->advancey;
			}
			else{
				goffsety=-g->offsety;
				gheight=-g->height;
				gadvancey=-g->advancey;
			}

			if(pixelSpace){
				vba.set3(i4+0,positionIndex,	Math::fromInt(x+g->offsetx),			Math::fromInt(y+goffsety+gheight), 0);
				vba.set3(i4+1,positionIndex,	Math::fromInt(x+g->offsetx+g->width),	Math::fromInt(y+goffsety+gheight), 0);
				vba.set3(i4+2,positionIndex,	Math::fromInt(x+g->offsetx+g->width),	Math::fromInt(y+goffsety), 0);
				vba.set3(i4+3,positionIndex,	Math::fromInt(x+g->offsetx),			Math::fromInt(y+goffsety), 0);
			}
			else{
				vba.set3(i4+0,positionIndex,	Math::div(Math::fromInt(x+g->offsetx),pointSize),			Math::div(Math::fromInt(y+goffsety+gheight),pointSize), 0);
				vba.set3(i4+1,positionIndex,	Math::div(Math::fromInt(x+g->offsetx+g->width),pointSize),	Math::div(Math::fromInt(y+goffsety+gheight),pointSize), 0);
				vba.set3(i4+2,positionIndex,	Math::div(Math::fromInt(x+g->offsetx+g->width),pointSize),	Math::div(Math::fromInt(y+goffsety),pointSize), 0);
				vba.set3(i4+3,positionIndex,	Math::div(Math::fromInt(x+g->offsetx),pointSize),			Math::div(Math::fromInt(y+goffsety),pointSize), 0);
			}

			vba.set2(i4+0,texCoordIndex,	Math::div(Math::fromInt(g->x),scalarTexWidth),			Math::div(Math::fromInt(g->y+g->height),scalarTexHeight));
			vba.set2(i4+1,texCoordIndex,	Math::div(Math::fromInt(g->x+g->width),scalarTexWidth),	Math::div(Math::fromInt(g->y+g->height),scalarTexHeight));
			vba.set2(i4+2,texCoordIndex,	Math::div(Math::fromInt(g->x+g->width),scalarTexWidth),	Math::div(Math::fromInt(g->y),scalarTexHeight));
			vba.set2(i4+3,texCoordIndex,	Math::div(Math::fromInt(g->x),scalarTexWidth),			Math::div(Math::fromInt(g->y),scalarTexHeight));

			x+=Math::toInt(g->advancex);
			y+=Math::toInt(g->advancey);

			if(colorIndex>=0){
				vba.setRGBA(i4+0,colorIndex,rgba);
				vba.setRGBA(i4+1,colorIndex,rgba);
				vba.setRGBA(i4+2,colorIndex,rgba);
				vba.setRGBA(i4+3,colorIndex,rgba);
			}
		}
		else{
			vba.set3(i4+0,positionIndex,0,0,0);
			vba.set3(i4+1,positionIndex,0,0,0);
			vba.set3(i4+2,positionIndex,0,0,0);
			vba.set3(i4+3,positionIndex,0,0,0);
		}
	}

	// Zero out the rest of the vertex buffer
	for(;i<maxLength;++i){
		int i4=i*4;
		vba.set3(i4+0,positionIndex,0,0,0);
		vba.set3(i4+1,positionIndex,0,0,0);
		vba.set3(i4+2,positionIndex,0,0,0);
		vba.set3(i4+3,positionIndex,0,0,0);
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
	int height=(int)mPointSize;

	int i;
	for(i=start;i<end;++i){
		wchar c=string.at(i);
		if(c==(char)10){
			height+=(int)mPointSize;
		}
	}

	return height;
}

}
}
