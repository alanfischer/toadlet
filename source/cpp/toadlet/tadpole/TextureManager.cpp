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

#include <toadlet/peeper/TextureFormatConversion.h>
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/plugins/NormalizationTextureCreator.h>
#include <toadlet/peeper/BackableTexture.h>
#if defined(TOADLET_BACKABLE)
#include <toadlet/peeper/BackablePixelBufferRenderTarget.h>
#endif

namespace toadlet{
namespace tadpole{

TextureManager::TextureManager(Engine *engine):ResourceManager(engine){
}

TextureManager::~TextureManager(){
	int i;
	for(i=0;i<mRenderTargets.size();++i){
		mRenderTargets[i]->setDestroyedListener(NULL);
	}
}

void TextureManager::destroy(){
	ResourceManager::destroy();

	int i;
	for(i=0;i<mRenderTargets.size();++i){
		PixelBufferRenderTarget::ptr renderTarget=mRenderTargets[i];
		renderTarget->setDestroyedListener(NULL);
		renderTarget->destroy();
	}
	mRenderTargets.clear();
}

Texture::ptr TextureManager::createTexture(TextureFormat::ptr format,tbyte *data){
	return createTexture(Texture::Usage_BIT_STATIC|Texture::Usage_BIT_AUTOGEN_MIPMAPS,format,data!=NULL?&data:NULL);
}

Texture::ptr TextureManager::createTexture(TextureFormat::ptr format,tbyte *mipDatas[]){
	return createTexture(Texture::Usage_BIT_STATIC,format,mipDatas);
}

Texture::ptr TextureManager::createTexture(int usage,TextureFormat::ptr format,tbyte *data){
	return createTexture(usage==0?(Texture::Usage_BIT_STATIC|Texture::Usage_BIT_AUTOGEN_MIPMAPS):usage,format,data!=NULL?&data:NULL);
}

Texture::ptr TextureManager::createTexture(int usage,TextureFormat::ptr format,tbyte *mipDatas[]){
	RenderDevice *renderDevice=mEngine->getRenderDevice();
	Texture::ptr texture;
#if defined(TOADLET_BACKABLE)
	if(mEngine->isBackable()){
		BackableTexture::ptr backableTexture=new BackableTexture();
		backableTexture->create(usage,format,mipDatas);
		if(renderDevice!=NULL){
			backableTexture->setBack(renderDevice->createTexture(),renderDevice);
		}
		texture=backableTexture;
	}
	else
#endif
	if(renderDevice!=NULL){
		TOADLET_TRY
			texture=renderDevice->createTexture();
		TOADLET_CATCH_ANONYMOUS(){}
		if(BackableTexture::convertCreate(texture,renderDevice,usage,format,mipDatas)==false){
			// See ShaderManager::createShader
			//Error::unknown(Categories::TOADLET_TADPOLE,"Error in texture convertCreate");
			return NULL;
		}
	}

	if(texture!=NULL){
		manage(shared_static_cast<Texture>(texture));
	}

	return texture;
}

PixelBufferRenderTarget::ptr TextureManager::createPixelBufferRenderTarget(){
	RenderDevice *renderDevice=mEngine->getRenderDevice();
	PixelBufferRenderTarget::ptr renderTarget;
#if defined(TOADLET_BACKABLE)
	if(mEngine->isBackable()){
		BackablePixelBufferRenderTarget::ptr backableRenderTarget=new BackablePixelBufferRenderTarget();
		backableRenderTarget->create();
		if(renderDevice!=NULL){
			PixelBufferRenderTarget::ptr back=renderDevice->createPixelBufferRenderTarget();
			backableRenderTarget->setBack(back);
		}
		renderTarget=backableRenderTarget;
	}
	else
#endif
	if(renderDevice!=NULL){
		renderTarget=renderDevice->createPixelBufferRenderTarget();
		if(renderTarget->create()==false){
			return NULL;
		}
	}

	if(renderTarget!=NULL){
		mRenderTargets.push_back(renderTarget);

		renderTarget->setDestroyedListener(this);
	}

	return renderTarget;
}

bool TextureManager::textureLoad(Texture::ptr texture,TextureFormat *format,tbyte *data){
	Log::debug(Categories::TOADLET_TADPOLE,"TextureManager::textureLoad");

	bool result=false;
	if(format->getPixelFormat()==texture->getFormat()->getPixelFormat()){
		result=texture->load(format,data);
	}
	else{
		TextureFormat::ptr newFormat=new TextureFormat(format);
		newFormat->setPixelFormat(texture->getFormat()->getPixelFormat());
		tbyte *newData=new tbyte[newFormat->getDataSize()];
		TextureFormatConversion::convert(data,format,newData,newFormat);
		result=texture->load(newFormat,newData);
		delete[] newData;
	}
	return result;
}

void TextureManager::contextActivate(RenderDevice *renderDevice){
	Log::debug(Categories::TOADLET_TADPOLE,"TextureManager::contextActivate");

#if defined(TOADLET_BACKABLE)
	int i;
	for(i=0;i<mResources.size();++i){
		Texture *texture=(Texture*)mResources[i];
		if(texture!=NULL && texture->getRootTexture()!=texture){
			Texture::ptr back=renderDevice->createTexture();
			((BackableTexture*)texture)->setBack(back,renderDevice);
		}
	}

	for(i=0;i<mRenderTargets.size();++i){
		PixelBufferRenderTarget::ptr renderTarget=mRenderTargets[i];
		if(renderTarget!=NULL && renderTarget->getRootRenderTarget()!=renderTarget){
			PixelBufferRenderTarget::ptr back=renderDevice->createPixelBufferRenderTarget();
			shared_static_cast<BackablePixelBufferRenderTarget>(renderTarget)->setBack(back);
		}
	}
#endif
}

void TextureManager::contextDeactivate(RenderDevice *renderDevice){
	Log::debug(Categories::TOADLET_TADPOLE,"TextureManager::contextDeactivate");

#if defined(TOADLET_BACKABLE)
	int i;
	for(i=0;i<mResources.size();++i){
		Texture *texture=(Texture*)mResources[i];
		if(texture!=NULL && texture->getRootTexture()!=texture){
			((BackableTexture*)texture)->setBack(NULL,NULL);
		}
	}

	for(i=0;i<mRenderTargets.size();++i){
		PixelBufferRenderTarget::ptr renderTarget=mRenderTargets[i];
		if(renderTarget!=NULL && renderTarget->getRootPixelBufferRenderTarget()!=renderTarget){
			shared_static_cast<BackablePixelBufferRenderTarget>(renderTarget)->setBack(NULL);
		}
	}
#endif
}

void TextureManager::preContextReset(peeper::RenderDevice *renderDevice){
	Log::debug(Categories::TOADLET_TADPOLE,"TextureManager::preContextReset");

	int i;
	for(i=0;i<mResources.size();++i){
		Texture *texture=(Texture*)mResources[i];
		if(texture!=NULL){
			texture->resetDestroy();
		}
	}

	for(i=0;i<mRenderTargets.size();++i){
		PixelBufferRenderTarget::ptr renderTarget=mRenderTargets[i];
		if(renderTarget!=NULL){
			renderTarget->resetDestroy();
		}
	}
}

void TextureManager::postContextReset(peeper::RenderDevice *renderDevice){
	Log::debug(Categories::TOADLET_TADPOLE,"TextureManager::postContextReset");

	int i;
	for(i=0;i<mResources.size();++i){
		Texture *texture=(Texture*)mResources[i];
		if(texture!=NULL){
			texture->resetCreate();
		}
	}

	for(i=0;i<mRenderTargets.size();++i){
		PixelBufferRenderTarget::ptr renderTarget=mRenderTargets[i];
		if(renderTarget!=NULL){
			renderTarget->resetCreate();
		}
	}
}

void TextureManager::resourceDestroyed(Resource *resource){
	RenderTargetCollection::iterator rit=stlit::find(mRenderTargets.begin(),mRenderTargets.end(),resource);
	if(rit!=mRenderTargets.end()){
		mRenderTargets.erase(rit);
	}
	else{
		ResourceManager::resourceDestroyed(resource);
	}
}

}
}

