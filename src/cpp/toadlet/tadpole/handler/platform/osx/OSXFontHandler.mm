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

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/handler/platform/osx/OSXFontHandler.h>
#include <dlfcn.h>

#if defined(TOADLET_PLATFORM_IPHONE)
#	import <UIKit/UIKit.h>
#else
#	import <AppKit/AppKit.h>
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace handler{

static String defaultCharacterSet("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789~!@#$%^&*()_+|{}:\"'<>?`-=\\/[];,. \t");

size_t FontStreamGetBytes(void *info,void *buffer,size_t count){
	return ((InputStream*)info)->read((char*)buffer,count);
}

off_t FontStreamSkipForward(void *info,off_t count){
	char *buffer=new char[count];
	int amount=((InputStream*)info)->read(buffer,count);
	delete[] buffer;
	return amount;
}

void FontStreamRewind(void *info){
	((InputStream*)info)->reset();
}
	
typedef void (*CGFontGetGlyphsForUnicharsPtr)(CGFontRef, const UniChar[], const CGGlyph[], size_t);
static CGFontGetGlyphsForUnicharsPtr CGFontGetGlyphsForUnichars=NULL;

OSXFontHandler::OSXFontHandler(TextureManager *textureManager){
	// Unfortunately we need access to a private function to get the character -> glyph conversion needed to
	// allow us to use CGContextShowGlyphsWithAdvances
	// Note that on > 10.5, the function is called CGFontGetGlyphsForUnicodes, so we need to detect and deal
	// with this.
	CGFontGetGlyphsForUnichars=(CGFontGetGlyphsForUnicharsPtr)dlsym(RTLD_DEFAULT,"CGFontGetGlyphsForUnichars");

	// if the function doesn't exist, we're probably on tiger and need to grab the
	// function under its old name, CGFontGetGlyphsForUnicodes
	if(!CGFontGetGlyphsForUnichars)
		CGFontGetGlyphsForUnichars=(CGFontGetGlyphsForUnicharsPtr)dlsym(RTLD_DEFAULT,"CGFontGetGlyphsForUnicodes");

	mTextureManager=textureManager;
}

Resource::ptr OSXFontHandler::load(InputStream::ptr in,const ResourceHandlerData *handlerData){
	if(!CGFontGetGlyphsForUnichars){
		Error::nullPointer(Categories::TOADLET_TADPOLE,
			"error finding CGFontGetGlyphsForUnichars");
		return NULL;
	}

	FontData *fontData=(FontData*)handlerData;
	if(fontData==NULL){
		Error::nullPointer(Categories::TOADLET_TADPOLE,
			"invalid FontData");
		return NULL;
	}

	float pointSize=fontData->pointSize;
	
	int i,j;

	CGDataProviderSequentialCallbacks callbacks={0};
	callbacks.version=0;
	callbacks.getBytes=FontStreamGetBytes;
	callbacks.skipForward=FontStreamSkipForward;
	callbacks.rewind=FontStreamRewind;
	callbacks.releaseInfo=NULL;

	CGDataProviderRef dataProvider=CGDataProviderCreateSequential(in.get(),&callbacks);

	CGFontRef cgfont=CGFontCreateWithDataProvider(dataProvider);
	
	CGDataProviderRelease(dataProvider);

	if(cgfont==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"error loading font");
		return NULL;
	}

	const wchar_t *wcharArray=NULL;
	int numChars=0;
	if(fontData->characterSet!=(char*)NULL){
		wcharArray=fontData->characterSet.wc_str();
		numChars=fontData->characterSet.length();
	}
	else{
		wcharArray=defaultCharacterSet.wc_str();
		numChars=defaultCharacterSet.length();
	}
	UniChar charArray[numChars+1];
	for(i=0;i<numChars+1;++i){
		charArray[i]=wcharArray[i];
	}

	// Get glyphs
	egg::Collection<CGGlyph> cgglyphs(numChars);
	memset(cgglyphs.begin(),0,sizeof(CGGlyph)*cgglyphs.size());
	CGFontGetGlyphsForUnichars(cgfont,(const UniChar*)charArray,cgglyphs.begin(),numChars);

	// Get glyph sizes
	egg::Collection<CGRect> sizes(numChars);
	memset(sizes.begin(),0,sizeof(CGRect)*sizes.size());
	CGFontGetGlyphBBoxes(cgfont,cgglyphs.begin(),numChars,sizes.begin());
	egg::Collection<int> iadvances(numChars);
	CGFontGetGlyphAdvances(cgfont,cgglyphs.begin(),numChars,iadvances.begin());
	egg::Collection<float> advances(numChars);
	for(i=0;i<sizes.size();++i){
		sizes[i].origin.x=sizes[i].origin.x*pointSize/1000.0;
		sizes[i].origin.y=sizes[i].origin.y*pointSize/1000.0;
		sizes[i].size.width=sizes[i].size.width*pointSize/1000.0+1;
		sizes[i].size.height=sizes[i].size.height*pointSize/1000.0+1;
		advances[i]=(float)iadvances[i]*pointSize/1000.0+1;
	}

	// Find bitmap sizes
	int charHeight=Math::toInt(Math::sqrt(Math::fromInt(numChars)));
	int charWidth=Math::intCeil(Math::div(Math::fromInt(numChars),Math::fromInt(charHeight)));
	int charmapWidth=0;
	int charmapHeight=0;
	egg::Collection<float> heights(charHeight);
	int reportedHeight=0;
	for(i=0;i<charHeight;++i){
		int w=0;
		int h=0;

		for(j=0;j<charWidth;++j){
			w+=sizes[i].size.width-sizes[i].origin.x + 1;

			if(h<sizes[i].size.height-sizes[i].origin.y +1){
				h=sizes[i].size.height-sizes[i].origin.y +1;
			}
		}

		if(w>charmapWidth){
			charmapWidth=w;
		}

		heights[i]=h;
		charmapHeight+=h;
		if(reportedHeight<h){
			reportedHeight=h;
		}
	}

	// Find texture sizes
	int textureWidth=Math::nextPowerOf2(charmapWidth);
	int textureHeight=Math::nextPowerOf2(charmapHeight);

	// Create context
	CGColorSpaceRef colorSpace=CGColorSpaceCreateDeviceGray();
	void *data=calloc(textureHeight,textureWidth);
	CGContextRef context=CGBitmapContextCreate(data,textureWidth,textureHeight,8,textureWidth,colorSpace,kCGImageAlphaNone);
	CGColorSpaceRelease(colorSpace);
	CGContextSetGrayFillColor(context,1.0,1.0);

	CGContextSetFont(context,cgfont);
	CGContextSetFontSize(context,pointSize);
	
	// Draw characters
	egg::Collection<Font::Glyph*> glyphs(numChars);
	int x=0,y=0;
	for(i=0;i<numChars;++i){
		CGContextShowGlyphsAtPoint(context,x-sizes[i].origin.x,textureHeight-y-sizes[i].size.height-sizes[i].origin.y,&cgglyphs[i],1);

		Font::Glyph *glyph=new Font::Glyph();
		glyph->advancex=Math::fromInt(advances[i]);
		glyph->advancey=0;
		glyph->x=x;
		glyph->y=y;
		glyph->width=sizes[i].size.width;
		glyph->height=sizes[i].size.height;
		glyph->offsetx=0;
		glyph->offsety=-sizes[i].size.height;
		glyphs[i]=glyph;

		if(i%charWidth==charWidth-1){
			x=0;
			y+=heights[i/charWidth]+1; // Font antialising can make them bleed, so pad by 1
		}
		else{
			x+=sizes[i].size.width+1; // Font antialising can make them bleed, so pad by 1
		}
	}

	// Build texture
	Image::ptr image(new Image(Image::Dimension_D2,Image::Format_A_8,textureWidth,textureHeight));
	uint8 *imageData=image->getData();

	int imageStride=textureWidth*image->getPixelSize();

	memcpy(imageData,data,imageStride*charmapHeight);

	// Build font
	Font::ptr font(new Font(fontData->pointSize,0,mTextureManager->createTexture(image),wcharArray,glyphs.begin(),glyphs.size()));

	CGContextRelease(context);
	free(data);

	CGFontRelease(cgfont);

	return font;
}

}
}
}
