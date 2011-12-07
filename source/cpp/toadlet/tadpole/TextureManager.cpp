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
#include <toadlet/peeper/TextureFormatConversion.h>
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/Engine.h>

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

Texture::ptr TextureManager::createTexture(TextureFormat::ptr format,tbyte *data){
	return createTexture(Texture::Usage_BIT_STATIC|Texture::Usage_BIT_AUTOGEN_MIPMAPS,format,data!=NULL?&data:NULL);
}

Texture::ptr TextureManager::createTexture(TextureFormat::ptr format,tbyte *mipDatas[]){
	return createTexture(Texture::Usage_BIT_STATIC|Texture::Usage_BIT_AUTOGEN_MIPMAPS,format,mipDatas);
}

Texture::ptr TextureManager::createTexture(int usage,TextureFormat::ptr format,tbyte *data){
	return createTexture(usage,format,data!=NULL?&data:NULL);
}

Texture::ptr TextureManager::createTexture(int usage,TextureFormat::ptr format,tbyte *mipDatas[]){
	RenderDevice *renderDevice=mEngine->getRenderDevice();
	Texture::ptr texture;
	if(mBackable || renderDevice==NULL){
		BackableTexture::ptr backableTexture(new BackableTexture());
		backableTexture->create(usage,format,mipDatas);
		if(renderDevice!=NULL){
			backableTexture->setBack(Texture::ptr(renderDevice->createTexture()),renderDevice);
		}
		texture=backableTexture;
	}
	else{
		texture=Texture::ptr(renderDevice->createTexture());
		if(BackableTexture::convertCreate(texture,renderDevice,usage,format,mipDatas)==false){
			Logger::error(Categories::TOADLET_TADPOLE,"Error in texture convertCreate");
			return NULL;
		}
	}

	manage(shared_static_cast<Texture>(texture));

	Logger::debug(Categories::TOADLET_TADPOLE,"texture created");

	return texture;
}

PixelBufferRenderTarget::ptr TextureManager::createPixelBufferRenderTarget(){
	Logger::debug(Categories::TOADLET_TADPOLE,"TextureManager::createPixelBufferRenderTarget");

	PixelBufferRenderTarget::ptr renderTarget;
	if(mBackable || mEngine->getRenderDevice()==NULL){
		BackablePixelBufferRenderTarget::ptr backableRenderTarget(new BackablePixelBufferRenderTarget());
		backableRenderTarget->create();
		if(mEngine->getRenderDevice()!=NULL){
			PixelBufferRenderTarget::ptr back(mEngine->getRenderDevice()->createPixelBufferRenderTarget());
			backableRenderTarget->setBack(back);
		}
		renderTarget=backableRenderTarget;
	}
	else{
		renderTarget=PixelBufferRenderTarget::ptr(mEngine->getRenderDevice()->createPixelBufferRenderTarget());
		if(renderTarget->create()==false){
			return NULL;
		}
	}

	mRenderTargets.add(renderTarget);

	renderTarget->setRenderTargetDestroyedListener(this);

	return renderTarget;
}

bool TextureManager::textureLoad(Texture::ptr texture,TextureFormat *format,tbyte *data){
	bool result=false;
	if(format->getPixelFormat()==texture->getFormat()->getPixelFormat()){
		result=texture->load(format,data);
	}
	else{
		TextureFormat::ptr newFormat(new TextureFormat(format));
		newFormat->setPixelFormat(texture->getFormat()->getPixelFormat());
		tbyte *newData=new tbyte[newFormat->getDataSize()];
		TextureFormatConversion::convert(data,format,newData,newFormat);
		result=texture->load(newFormat,newData);
		delete[] newData;
	}
	return result;
}

void TextureManager::contextActivate(RenderDevice *renderDevice){
	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		if(texture!=NULL && texture->getRootTexture()!=texture){
			Texture::ptr back(renderDevice->createTexture());
			shared_static_cast<BackableTexture>(texture)->setBack(back,renderDevice);
		}
	}

	for(i=0;i<mRenderTargets.size();++i){
		PixelBufferRenderTarget::ptr renderTarget=mRenderTargets[i];
		if(renderTarget!=NULL && renderTarget->getRootRenderTarget()!=renderTarget){
			PixelBufferRenderTarget::ptr back(renderDevice->createPixelBufferRenderTarget());
			shared_static_cast<BackablePixelBufferRenderTarget>(renderTarget)->setBack(back);
		}
	}
}

void TextureManager::contextDeactivate(RenderDevice *renderDevice){
	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		if(texture!=NULL && texture->getRootTexture()!=texture){
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

void TextureManager::preContextReset(peeper::RenderDevice *renderDevice){
	Logger::debug("TextureManager::preContextReset");

	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
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
	Logger::debug("TextureManager::postContextReset");

	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
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

void TextureManager::renderTargetDestroyed(RenderTarget *renderTarget){
	mRenderTargets.remove(renderTarget);
}

Texture::ptr TextureManager::createNormalization(int size){
	TextureFormat::ptr format(new TextureFormat(TextureFormat::Dimension_CUBE,TextureFormat::Format_RGB_8,size,size,TextureFormat::CubeSide_MAX,1));
	tbyte *data=new tbyte[format->getDataSize()];

	Vector3 HALF_VECTOR3(Math::HALF,Math::HALF,Math::HALF);

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

	return createTexture(Texture::Usage_BIT_STATIC,format,data);
}

}
}

