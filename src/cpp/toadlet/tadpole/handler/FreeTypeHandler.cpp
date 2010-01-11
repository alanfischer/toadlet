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
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/tadpole/FontData.h>
#include <toadlet/tadpole/handler/FreeTypeHandler.h>

#if defined(TOADLET_PLATFORM_WIN32) && defined(TOADLET_LIBFREETYPE_NAME)
	#pragma comment(lib,TOADLET_LIBFREETYPE_NAME)
#endif

using namespace toadlet::peeper;
using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::image;

namespace toadlet{
namespace tadpole{
namespace handler{

inline int floatToFixed26_6(float f){
	return (int)(f*(float)(1<<6));
}

static String defaultCharacterSet("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789~!@#$%^&*()_+|{}:\"'<>?`-=\\/[];,. \t");

FreeTypeHandler::FreeTypeHandler(TextureManager *textureManager){
	mTextureManager=textureManager;

	if(FT_Init_FreeType(&mLibrary)){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"FT_Init_FreeType failed");
	}
}

FreeTypeHandler::~FreeTypeHandler(){
	if(FT_Done_FreeType(mLibrary)){
		Logger::error(Categories::TOADLET_TADPOLE,
			"FT_Done_FreeType failed");
	}
}

Resource::ptr FreeTypeHandler::load(Stream::ptr stream,const ResourceHandlerData *handlerData){
	FontData *fontData=(FontData*)handlerData;
	if(fontData==NULL){
		Error::nullPointer(Categories::TOADLET_TADPOLE,
			"invalid FontData");
		return NULL;
	}

	const wchar_t *charArray=NULL;
	int numChars=0;
	if(fontData->characterSet!=(char*)NULL){
		charArray=fontData->characterSet.wc_str();
		numChars=fontData->characterSet.length();
	}
	else{
		charArray=defaultCharacterSet.wc_str();
		numChars=defaultCharacterSet.length();
	}

	MemoryStream::ptr memoryStream(new MemoryStream(stream));
	int i=0,j=0;

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

	int charmapWidth=0;
	int charmapHeight=charCountHeight*maxHeight;

	for(i=0;i<charCountHeight;++i){
		int w=0;
		for(j=0;j<charCountWidth;++j){
			w+=bitmapGlyphs[i]->bitmap.width;
		}
		if(w>charmapWidth){
			charmapWidth=w;
		}
	}

	int textureWidth=Math::nextPowerOf2(charmapWidth);
	int textureHeight=Math::nextPowerOf2(charmapHeight);

	Collection<Font::Glyph*> glyphs(bitmapGlyphs.size());
	Image::ptr image(new Image(Image::Dimension_D2,Image::Format_A_8,textureWidth,textureHeight));
	uint8 *data=image->getData();
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
				memcpy(data+(y+j)*textureWidth+x,bitmapGlyph->bitmap.buffer+j*bitmapGlyph->bitmap.width,bitmapGlyph->bitmap.width);
			}

			if(i%charCountWidth==charCountWidth-1){
				x=0;
				y+=maxHeight;
			}
			else{
				x+=bitmapGlyph->bitmap.width;
			}
		}
	}

	Font::ptr font(new Font(fontData->pointSize,0,mTextureManager->createTexture(image),charArray,&glyphs[0],glyphs.size()));

	// Clean up FreeType data
	for(i=0;i<bitmapGlyphs.size();++i){
		FT_Done_Glyph((FT_Glyph)bitmapGlyphs[i]);
	}
	bitmapGlyphs.clear();

	FT_Done_Face(face);
	face=NULL;

	return font;
}

}
}
}
