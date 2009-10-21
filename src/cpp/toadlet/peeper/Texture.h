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

#ifndef TOADLET_PEEPER_TEXTURE_H
#define TOADLET_PEEPER_TEXTURE_H

#include <toadlet/egg/Resource.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/image/Image.h>
#include <toadlet/peeper/Renderer.h>

namespace toadlet{
namespace peeper{

class RenderTargetPeer;

class TexturePeer{
public:
	virtual ~TexturePeer(){}

	virtual RenderTargetPeer *castToRenderTargetPeer(){return NULL;}
};

class RenderTarget;

///@todo Look at removing this direct dependence on Image?  Could replace with some sort of loader.
///  Just a though to make the Java cleaner, unless we wanted to add our own Java Image class.

///@todo Explore the Resource FacadeResource or something model.  Would allow me to drop the Peer system, and also
///  could be more optimized if you choose you dont want to let the renderer change.  Then your Texture would just
///  be an instance of a GLTexture.  Otherwise your Texture would be a FacadeTexture which stores a GLTexture.
///  would mean I could no longer do new Texture().  I would have to do some sort of textureManager->makeTexture(),
///  but I could easily handle that.  I actually have to do that ANYWAY to load them into the Engine.
///  How would that work with the Loaders that return a Texture stored with an Image?
///  Well, the FacadeTexture would have to be able to store an Image, so the ResourceHandlers would have to return a
///  FacadeTexture, unless I had some method in the Renderer that let me load & return a Texture from an Image.
///  (which is what I will NEED when we go from Peers -> Resources)  So the Engine would just do:
///  makeTextureFromImage(image){
///    if(using facades){
///			Texture texture=new FacadeTexture(image);
///			loadTexture(texture);
///		}
///		else{
///			return mRenderer->makeTexture(image);
///		}
///  }
///  Though I need to figure out how RenderTextures would work.  GLRenderTexture would be the obvious extension
///  But then would I have a FacadeRenderTexture?  Or a FacadeTexture?  It would have to be a FacadeRenderTexture,
///  because it would need to have the proper RenderTexture methods.  Texture could always be an interface.
///  BUT A HUGE PROBLEM IS:  We then have GLTexture and GLRenderTexture?  GLRenderTexture would need to extend GLTexture.
///  But if we have Texture & RenderTexture as interfaces, then GLRenderTexture could just extend GLTexture (and thus
///   implement Texture), and then implement RenderTexture, and finally overload whatever methods necessary.  So the whole
///   inheritance heiarchy would be just interfaces on the surface, and all the implementations would be classes
///   interfaces:	Texture, RenderTexture, RenderContext
///   classes:		FacadeTexture, GLTexture, GLRenderTexture, WGLRenderContext
///
/// Now what about AnimatedTextures?  These have gotten weak as of late.  They are only empty shells used to store texture
/// sequence information for sprites.
class TOADLET_API Texture:public egg::Resource,public egg::image::ImageDefinitions{
public:
	TOADLET_SHARED_POINTERS(Texture,egg::Resource);

	enum Type{
		Type_UNKNOWN=0,
		Type_NORMAL,
		Type_RENDER,
		Type_ANIMATED,
	};

	enum AddressMode{
		AddressMode_REPEAT,
		AddressMode_CLAMP_TO_EDGE,
		AddressMode_CLAMP_TO_BORDER,
		AddressMode_MIRRORED_REPEAT,
	};

	enum Filter{
		Filter_NONE,
		Filter_NEAREST,
		Filter_LINEAR,
	};

	Texture();
	Texture(egg::image::Image::ptr image);
	Texture(Renderer *renderer,egg::image::Image::ptr image);
	Texture(Dimension dimension,int format,int width,int height=1,int depth=1);
	Texture(Renderer *renderer,Dimension dimension,int format,int width,int height=1,int depth=1);

	virtual ~Texture();

	virtual RenderTarget *castToRenderTarget(){return NULL;}

	virtual Dimension getDimension() const{return mDimension;}
	virtual int getFormat() const{return mFormat;}
	virtual Type getType() const{return mType;}

	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}
	virtual int getDepth() const{return mDepth;}

	virtual void setAutoGenerateMipMaps(bool mipmaps);
	virtual bool getAutoGenerateMipMaps() const{return mAutoGenerateMipMaps;}

	void setSAddressMode(AddressMode addressMode);
	inline AddressMode getSAddressMode() const{return mSAddressMode;}

	void setTAddressMode(AddressMode addressMode);
	inline AddressMode getTAddressMode() const{return mTAddressMode;}

	void setRAddressMode(AddressMode addressMode);
	inline AddressMode getRAddressMode() const{return mRAddressMode;}

	void setMinFilter(Filter minFilter);
	inline Filter getMinFilter() const{return mMinFilter;}

	void setMipFilter(Filter mipFilter);
	inline Filter getMipFilter() const{return mMipFilter;}

	void setMagFilter(Filter magFilter);
	inline Filter getMagFilter() const{return mMagFilter;}

	void setName(const egg::String &name);
	inline const egg::String &getName() const{return mName;}

	virtual void setImage(egg::image::Image::ptr image);
	virtual egg::image::Image::ptr getImage() const{return mImage;}

	virtual void internal_setTexturePeer(TexturePeer *texturePeer,bool own);
	inline TexturePeer *internal_getTexturePeer() const{return mTexturePeer;}
	inline bool internal_ownsTexturePeer() const{return mOwnsTexturePeer;}

protected:
	Type mType;

	Dimension mDimension;
	int mFormat;
	int mWidth;
	int mHeight;
	int mDepth;

	bool mAutoGenerateMipMaps;

	AddressMode mSAddressMode;
	AddressMode mTAddressMode;
	AddressMode mRAddressMode;

	Filter mMinFilter;
	Filter mMipFilter;
	Filter mMagFilter;

	egg::String mName;

	egg::image::Image::ptr mImage;

	TexturePeer *mTexturePeer;
	bool mOwnsTexturePeer;
};

}
}

#endif

