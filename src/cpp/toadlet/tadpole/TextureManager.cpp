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

#include <toadlet/peeper/BackableTexture.h>
#include <toadlet/peeper/BackablePixelBufferRenderTarget.h>
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

TextureManager::TextureManager(Engine *engine,bool backable):ResourceManager(engine->getArchiveManager()),
	mEngine(NULL),
	mBackable(false)
{
	mEngine=engine;
	mBackable=backable;
}

TextureManager::~TextureManager(){
	int i;
	for(i=0;i<mRenderTargets.size();++i){
		mRenderTargets[i]->setRenderTargetDestroyedListener(NULL);
	}
}

void TextureManager::destroy(){
	ResourceManager::destroy();

	int i;
	for(i=0;i<mRenderTargets.size();++i){
		PixelBufferRenderTarget::ptr renderTarget=mRenderTargets[i];
		renderTarget->setRenderTargetDestroyedListener(NULL);
		renderTarget->destroy();
	}
	mRenderTargets.clear();
}

Texture::ptr TextureManager::createTexture(Image::ptr image){
	tbyte *mipData=image->getData();

	return createTexture(
		Texture::Usage_BIT_STATIC|Texture::Usage_BIT_AUTOGEN_MIPMAPS,
		image->getDimension(),image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth(),
		0,&mipData
	);
}

Texture::ptr TextureManager::createTexture(int mipLevels,Image::ptr mipImages[]){
	Image *image=mipImages[0];

	tbyte **mipDatas=new tbyte*[mipLevels];
	int i;
	for(i=0;i<mipLevels;++i){
		mipDatas[i]=mipImages[i]->getData();
	}

	Texture::ptr texture=createTexture(
		Texture::Usage_BIT_STATIC,
		image->getDimension(),image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth(),
		mipLevels,mipDatas
	);

	delete[] mipDatas;

	return texture;
}

Texture::ptr TextureManager::createTexture(int usage,Texture::Dimension dimension,int format,int width,int height,int depth,int mipLevels,tbyte *mipDatas[]){
	Logger::debug(Categories::TOADLET_TADPOLE,"TextureManager::createTexture");

	Renderer *renderer=mEngine->getRenderer();
	Texture::ptr texture;
	if(mBackable || renderer==NULL){
		Logger::debug(Categories::TOADLET_TADPOLE,"creating BackableTexture");

		BackableTexture::ptr backableTexture(new BackableTexture());
		backableTexture->create(usage,dimension,format,width,height,depth,mipLevels,mipDatas);
		if(renderer!=NULL){
			backableTexture->setBack(Texture::ptr(renderer->createTexture()),renderer);
		}
		texture=backableTexture;
	}
	else{
		Logger::debug(Categories::TOADLET_TADPOLE,"creating Texture");

		texture=Texture::ptr(renderer->createTexture());
		if(BackableTexture::convertCreate(texture,renderer,usage,dimension,format,width,height,depth,mipLevels,mipDatas)==false){
			return NULL;
		}
	}

	manage(shared_static_cast<Texture>(texture));

	return texture;
}

Image::ptr TextureManager::createImage(Texture *texture){
	Image::ptr image(Image::createAndReallocate(texture->getDimension(),texture->getFormat(),texture->getWidth(),texture->getHeight(),texture->getDepth()));
	if(image==NULL){
		return NULL;
	}

	texture->read(image->getWidth(),image->getHeight(),image->getDepth(),0,image->getData());
	return image;
}

PixelBufferRenderTarget::ptr TextureManager::createPixelBufferRenderTarget(){
	Logger::debug(Categories::TOADLET_TADPOLE,"TextureManager::createPixelBufferRenderTarget");

	PixelBufferRenderTarget::ptr renderTarget;
	if(mBackable || mEngine->getRenderer()==NULL){
		BackablePixelBufferRenderTarget::ptr backableRenderTarget(new BackablePixelBufferRenderTarget());
		backableRenderTarget->create();
		if(mEngine->getRenderer()!=NULL){
			PixelBufferRenderTarget::ptr back(mEngine->getRenderer()->createPixelBufferRenderTarget());
			backableRenderTarget->setBack(back);
		}
		renderTarget=backableRenderTarget;
	}
	else{
		renderTarget=PixelBufferRenderTarget::ptr(mEngine->getRenderer()->createPixelBufferRenderTarget());
		if(renderTarget->create()==false){
			return NULL;
		}
	}

	mRenderTargets.add(renderTarget);

	renderTarget->setRenderTargetDestroyedListener(this);

	return renderTarget;
}

void TextureManager::contextActivate(Renderer *renderer){
	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		if(texture!=NULL && texture->getRootTexture(0)!=texture){
			Texture::ptr back(renderer->createTexture());
			shared_static_cast<BackableTexture>(texture)->setBack(back,renderer);
		}
	}

	for(i=0;i<mRenderTargets.size();++i){
		PixelBufferRenderTarget::ptr renderTarget=mRenderTargets[i];
		if(renderTarget!=NULL && renderTarget->getRootRenderTarget()!=renderTarget){
			PixelBufferRenderTarget::ptr back(renderer->createPixelBufferRenderTarget());
			shared_static_cast<BackablePixelBufferRenderTarget>(renderTarget)->setBack(back);
		}
	}
}

void TextureManager::contextDeactivate(Renderer *renderer){
	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		if(texture!=NULL && texture->getRootTexture(0)!=texture){
			shared_static_cast<BackableTexture>(texture)->setBack(NULL,NULL);
		}
	}

	for(i=0;i<mRenderTargets.size();++i){
		PixelBufferRenderTarget::ptr renderTarget=mRenderTargets[i];
		if(renderTarget!=NULL && renderTarget->getRootPixelBufferRenderTarget()!=renderTarget){
			shared_static_cast<BackablePixelBufferRenderTarget>(renderTarget)->setBack(NULL);
		}
	}
}

void TextureManager::preContextReset(peeper::Renderer *renderer){
	Logger::debug("TextureManager::preContextReset");

	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		if(texture!=NULL){
			texture->resetDestroy();
		}
	}
}

void TextureManager::postContextReset(peeper::Renderer *renderer){
	Logger::debug("TextureManager::postContextReset");

	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		if(texture!=NULL){
			texture->resetCreate();
		}
	}
}

void TextureManager::renderTargetDestroyed(RenderTarget *renderTarget){
	mRenderTargets.remove(renderTarget);
}

Texture::ptr TextureManager::createNormalization(int size){
	Image::ptr image(Image::createAndReallocate(Image::Dimension_CUBE,Image::Format_RGB_8,size,size,Image::CubeSide_MAX));

	Vector3 HALF_VECTOR3(Math::HALF,Math::HALF,Math::HALF);

	uint8 *data=image->getData();

	float offset = 0.5f;
	float halfSize = size * 0.5f;
	Vector3 temp;
	int pos=0;

	int i,j;
	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set(halfSize,(j+offset-halfSize),-(i+offset-halfSize));
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);
			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set(-halfSize,(j+offset-halfSize),(i+offset-halfSize));
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set((i+offset-halfSize),-halfSize,(j+offset-halfSize));
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set((i+offset-halfSize),halfSize,-(j+offset-halfSize));
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set((i+offset-halfSize),(j+offset-halfSize),halfSize);
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set(-(i+offset-halfSize),(j+offset-halfSize),-halfSize);
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	return createTexture(Texture::Usage_BIT_STATIC,Texture::Dimension_D2,image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth(),1,&data);
}

}
}

