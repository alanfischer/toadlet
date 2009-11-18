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

#ifndef TOADLET_TADPOLE_FONT_H
#define TOADLET_TADPOLE_FONT_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/VertexBufferAccessor.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Font:public egg::BaseResource{
public:
	TOADLET_SHARED_POINTERS(Font);

	class Glyph{
	public:
		Glyph():
			advancex(0),
			advancey(0),
			x(0),
			y(0),
			width(0),
			height(0),
			offsetx(0),
			offsety(0)
		{}

		scalar advancex;
		scalar advancey;
		int x;
		int y;
		int16 width;
		int16 height;
		int16 offsetx;
		int16 offsety;
	};

	enum Alignment{
		Alignment_BIT_LEFT=		1<<0,
		Alignment_BIT_RIGHT=	1<<1,
		Alignment_BIT_HCENTER=	1<<2,
		Alignment_BIT_TOP=		1<<3,
		Alignment_BIT_BOTTOM=	1<<4,
		Alignment_BIT_VCENTER=	1<<5,
	};

	Font(float pointSize,int innerSpace,peeper::Texture::ptr texture,const wchar *charSet,Glyph **glyphs,int numChars);
	virtual ~Font();

	void destroy();

	bool updateVertexBufferForString(peeper::VertexBuffer::ptr vertexBuffer,const egg::String &string,const peeper::Color &color,int alignment);

	int getStringWidth(const egg::String &string) const{return getStringWidth(string,0,string.length());}
	int getStringWidth(const egg::String &string,int start,int end) const;

	int getStringHeight(const egg::String &string) const{return getStringHeight(string,0,string.length());}
	int getStringHeight(const egg::String &string,int start,int end) const;

	inline float getPointSize() const{return mPointSize;}
	inline int getInnerSpace() const{return mInnerSpace;}
	inline peeper::Texture::ptr getTexture() const{return mTexture;}
	inline Glyph *getGlyph(int i) const{if(i<0 || i>=mGlyphs.size()){return NULL;}else{return mGlyphs[i];}}

	peeper::VertexBufferAccessor vba;

protected:
	float mPointSize;
	int mInnerSpace;
	peeper::Texture::ptr mTexture;
	egg::Collection<Glyph*> mGlyphs;
};

}
}

#endif
