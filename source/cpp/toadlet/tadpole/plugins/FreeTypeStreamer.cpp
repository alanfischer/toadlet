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

#include <toadlet/egg/Log.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/tadpole/FontData.h>
#include "FreeTypeStreamer.h"

namespace toadlet{
namespace tadpole{

inline int floatToFixed26_6(float f){
	return (int)(f*(float)(1<<6));
}

FreeTypeStreamer::FreeTypeStreamer(TextureManager *textureManager){
	mTextureManager=textureManager;

	if(FT_Init_FreeType(&mLibrary)){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"FT_Init_FreeType failed");
	}
}

FreeTypeStreamer::~FreeTypeStreamer(){
	FT_Done_FreeType(mLibrary);
}

Resource::ptr FreeTypeStreamer::load(Stream::ptr stream,ResourceData *resourceData,ProgressListener *listener){
	FontData *fontData=(FontData*)resourceData;
	if(fontData==NULL){
		Error::nullPointer(Categories::TOADLET_TADPOLE,
			"invalid FontData");
		return NULL;
	}

	const wchar_t *charArray=fontData->characterSet.wc_str();
	int numChars=numChars=fontData->characterSet.length();

	MemoryStream::ptr memoryStream=new MemoryStream(stream);
	int i=0,j=0,k=0;

	FT_Face face=NULL;
	if(FT_New_Memory_Face(mLibrary,(const FT_Byte*)memoryStream->getOriginalDataPointer(),memoryStream->length(),0,&face)){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"FT_New_Memory_Face failed");
		return NULL;
	}

	FT_Set_Char_Size(face,floatToFixed26_6(fontData->pointSize),floatToFixed26_6(fontData->pointSize),0,0);

	int maxHeight=0;
	Collection<FT_BitmapGlyph> bitmapGlyphs(numChars);
	for(i=0;i<numChars;i++){
		if(FT_Load_Char(face,charArray[i],FT_LOAD_DEFAULT)){
			// Failed to load, just continue
			continue;
		}

		FT_Glyph glyph=NULL;
		if(FT_Get_Glyph(face->glyph,&glyph)){
			// Failed to get glyph, just continue
			continue;
		}

		if(FT_Glyph_To_Bitmap(&glyph,FT_RENDER_MODE_NORMAL,0,1)){
			// Failed to convert to bitmap, just continue
			FT_Done_Glyph(glyph);
			continue;
		}

		FT_BitmapGlyph bitmapGlyph=(FT_BitmapGlyph)glyph;

		if(maxHeight<bitmapGlyph->bitmap.rows){
			maxHeight=bitmapGlyph->bitmap.rows;
		}

		bitmapGlyphs[i]=bitmapGlyph;
	}

	int charCountHeight=Math::toInt(Math::sqrt(Math::fromInt(numChars)));
	int charCountWidth=Math::intCeil(Math::div(Math::fromInt(numChars),Math::fromInt(charCountHeight)));
	int pad=1;

	int charmapWidth=0;
	int charmapHeight=charCountHeight*(maxHeight+pad);

	for(i=0;i<charCountHeight;++i){
		int w=0;
		for(j=0;j<charCountWidth;++j){
			w+=bitmapGlyphs[i]->bitmap.width+pad;
		}
		if(w>charmapWidth){
			charmapWidth=w;
		}
	}

	int textureWidth=Math::nextPowerOf2(charmapWidth);
	int textureHeight=Math::nextPowerOf2(charmapHeight);
	TextureFormat::ptr textureFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_LA_8,textureWidth,textureHeight,1,0);
	tbyte *textureData=new tbyte[textureFormat->getDataSize()];
	memset(textureData,0,textureFormat->getDataSize());
	int textureStride=textureFormat->getXPitch();

	Collection<Font::Glyph*> glyphs(bitmapGlyphs.size());	
	int x=0,y=0;
	for(i=0;i<bitmapGlyphs.size();++i){
		FT_BitmapGlyph bitmapGlyph=bitmapGlyphs[i];
		if(bitmapGlyph!=NULL){
			FT_Bitmap &bitmap=bitmapGlyph->bitmap;

			Font::Glyph *glyph=new Font::Glyph();
			#if defined(TOADLET_FIXED_POINT)
				glyph->advancex=bitmapGlyph->root.advance.x;
				glyph->advancey=bitmapGlyph->root.advance.y;
			#else
				glyph->advancex=mathfixed::Math::fixedToFloat(bitmapGlyph->root.advance.x);
				glyph->advancey=mathfixed::Math::fixedToFloat(bitmapGlyph->root.advance.y);
			#endif
			glyph->x=x;
			glyph->y=y;
			glyph->width=bitmap.width;
			glyph->height=bitmap.rows;
			glyph->offsetx=bitmapGlyph->left;
			glyph->offsety=-bitmapGlyph->top;
			glyphs[i]=glyph;

			for(j=0;j<bitmapGlyph->bitmap.rows;++j){
				for(k=0;k<bitmapGlyph->bitmap.width;++k){
					uint8 *s=(uint8*)(bitmapGlyph->bitmap.buffer+j*bitmapGlyph->bitmap.width)+k;
					uint16 *d=(uint16*)(textureData+(y+j)*textureStride)+x+k;
					*d=(int)((*s)<<8) | (int)(*s);
				}
			}

			if(i%charCountWidth==charCountWidth-1){
				x=0;
				y+=maxHeight+pad;
			}
			else{
				x+=bitmapGlyph->bitmap.width+pad;
			}
		}
	}

	Texture::ptr texture=mTextureManager->createTexture(textureFormat,textureData);
	Font::ptr font=new Font(fontData->pointSize,0,texture,charArray,&glyphs[0],glyphs.size());

	// Clean up FreeType data
	for(i=0;i<bitmapGlyphs.size();++i){
		FT_Done_Glyph((FT_Glyph)bitmapGlyphs[i]);
	}
	bitmapGlyphs.clear();

	FT_Done_Face(face);
	face=NULL;

	delete[] textureData;

	return font;
}

}
}
