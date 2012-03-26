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
#include <toadlet/tadpole/platform/osx/OSXFontStreamer.h>
#include <dlfcn.h>

#if defined(TOADLET_PLATFORM_IOS)
#	import <UIKit/UIKit.h>
#else
#	import <AppKit/AppKit.h>
#endif

namespace toadlet{
namespace tadpole{

size_t FontStreamGetBytes(void *info,void *buffer,size_t count){
	return ((Stream*)info)->read((tbyte*)buffer,count);
}

off_t FontStreamSkipForward(void *info,off_t count){
	tbyte *buffer=new tbyte[count];
	int amount=((Stream*)info)->read(buffer,count);
	delete[] buffer;
	return amount;
}

void FontStreamRewind(void *info){
	((Stream*)info)->reset();
}
	
typedef void (*CGFontGetGlyphsForUnicharsPtr)(CGFontRef, const UniChar[], const CGGlyph[], size_t);
static CGFontGetGlyphsForUnicharsPtr CGFontGetGlyphsForUnichars=NULL;

OSXFontStreamer::OSXFontStreamer(TextureManager *textureManager){
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

float fromEm(float e,float pointSize,float unitsPerEm){
	e*=pointSize;
	return unitsPerEm!=0?(e/unitsPerEm):e;
}

Resource::ptr OSXFontStreamer::load(Stream::ptr in,ResourceData *resourceData,ProgressListener *listener){
	if(!CGFontGetGlyphsForUnichars){
		Error::nullPointer(Categories::TOADLET_TADPOLE,
			"error finding CGFontGetGlyphsForUnichars");
		return NULL;
	}

	FontData *fontData=(FontData*)resourceData;
	if(fontData==NULL){
		Error::nullPointer(Categories::TOADLET_TADPOLE,
			"invalid FontData");
		return NULL;
	}

	float pointSize=fontData->pointSize;

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

	int unitsPerEm=CGFontGetUnitsPerEm(cgfont);
	int pad=3;
	
	const wchar_t *wcharArray=fontData->characterSet.wc_str();
	int numChars=fontData->characterSet.length();
	UniChar charArray[numChars+1];
	int i,j;
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
		sizes[i].origin.x=fromEm(sizes[i].origin.x,pointSize,unitsPerEm)-1;
		sizes[i].origin.y=fromEm(sizes[i].origin.y,pointSize,unitsPerEm)-1;
		sizes[i].size.width=fromEm(sizes[i].size.width,pointSize,unitsPerEm)+2;
		sizes[i].size.height=fromEm(sizes[i].size.height,pointSize,unitsPerEm)+2;
		advances[i]=fromEm(iadvances[i],pointSize,unitsPerEm);
	}

	// Find bitmap sizes
	int charHeight=Math::toInt(Math::sqrt(Math::fromInt(numChars)));
	int charWidth=Math::intCeil(Math::div(Math::fromInt(numChars),Math::fromInt(charHeight)));
	int charmapWidth=0;
	int charmapHeight=0;
	egg::Collection<float> heights(charHeight);
	int reportedHeight=0;
	int x=0,y=0;
	for(i=0;i<numChars;++i){
		int width=sizes[i].size.width-(0<sizes[i].origin.x?0:sizes[i].origin.x);
		int height=sizes[i].size.height-(0<sizes[i].origin.y?0:sizes[i].origin.y);
		if(i%charWidth==charWidth-1){
			if(x>charmapWidth){
				charmapWidth=x;
			}
			x=0;
			heights[i/charWidth]=y;
			charmapHeight+=y+pad;
			if(reportedHeight<y){
				reportedHeight=y;
			}
		}
		else{
			x+=width+pad;
			if(y<height){
				y=height;
			}
		}
	}

	// Find texture sizes
	int textureWidth=Math::nextPowerOf2(charmapWidth);
	int textureHeight=Math::nextPowerOf2(charmapHeight);

	// Create context
	CGColorSpaceRef colorSpace=CGColorSpaceCreateDeviceGray();
	uint8 *data=(uint8*)calloc(textureHeight,textureWidth);
	CGContextRef context=CGBitmapContextCreate(data,textureWidth,textureHeight,8,textureWidth,colorSpace,kCGImageAlphaNone);
	CGColorSpaceRelease(colorSpace);
	CGContextSetGrayFillColor(context,1.0,1.0);

	CGContextSetFont(context,cgfont);
	CGContextSetFontSize(context,pointSize);

	CGContextTranslateCTM(context,0.f,textureHeight);
	CGContextScaleCTM(context,1.0f,-1.0f);

	// Draw characters
	egg::Collection<Font::Glyph*> glyphs(numChars);
	x=0,y=0;
	for(i=0;i<numChars;++i){
		int px=x-sizes[i].origin.x;
		int py=(y-sizes[i].origin.y);
		CGContextShowGlyphsAtPoint(context,px,py,&cgglyphs[i],1);

		Font::Glyph *glyph=new Font::Glyph();
		glyph->advancex=Math::fromInt(advances[i]);
		glyph->advancey=0;
		glyph->x=x;
		glyph->y=y;
		glyph->width=sizes[i].size.width-(0<sizes[i].origin.x?0:sizes[i].origin.x);
		glyph->height=sizes[i].size.height-(0<sizes[i].origin.y?0:sizes[i].origin.y);
		glyph->offsetx=sizes[i].origin.x;
		glyph->offsety=sizes[i].origin.y;
		glyphs[i]=glyph;
		
		#if 0
			CGRect rect;
			rect.origin.x=glyph->x;
			rect.origin.y=glyph->y;
			rect.size.width=glyph->width;
			rect.size.height=glyph->height;
			CGFloat color[]={1,1,1,1};
			CGContextSetStrokeColor(context,color);
			CGContextStrokeRect(context,rect);
		#endif

		if(i%charWidth==charWidth-1){
			x=0;
			y+=heights[i/charWidth]+pad; // Font antialising can make them bleed, so pad by 1
		}
		else{
			x+=glyph->width+pad; // Font antialising can make them bleed, so pad by 1
		}
		
		glyph->width+=1;
		glyph->y=textureHeight-(glyph->y+glyph->height);
		glyph->offsety=-glyph->offsety-glyph->height;
	}

	// Build texture
	TextureFormat::ptr textureFormat(new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_LA_8,textureWidth,textureHeight,1,0));
	tbyte *textureData=new tbyte[textureFormat->getDataSize()];
	int textureStride=textureFormat->getXPitch();
	int charmapStride=textureWidth;
	
	for(i=0;i<charmapHeight;++i){
		for(j=0;j<textureWidth;++j){
			uint8 *s=(uint8*)(data+charmapStride*i)+j;
			uint16 *d=(uint16*)(textureData+textureStride*(textureHeight-i-1))+j;
			*d=(int)((*s)<<8) | (int)(*s);
		}
	}
	
	Texture::ptr texture=mTextureManager->createTexture(textureFormat,textureData);
	
	delete[] textureData;

	// Build font
	Font::ptr font(new Font(fontData->pointSize,0,texture,wcharArray,glyphs.begin(),glyphs.size()));

	CGContextRelease(context);
	free(data);

	CGFontRelease(cgfont);

	return font;
}

}
}
