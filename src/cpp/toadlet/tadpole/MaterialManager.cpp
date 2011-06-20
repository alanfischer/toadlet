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
#include <toadlet/peeper/BackableRenderState.h>
#include <toadlet/peeper/BackableShaderState.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/MaterialManager.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

MaterialManager::MaterialManager(Engine *engine,bool backable):ResourceManager(engine->getArchiveManager()),
	mEngine(NULL),
	mBackable(false)
{
	mEngine=engine;
	mBackable=backable;
}

void MaterialManager::destroy(){
	ResourceManager::destroy();

	int i;
	for(i=0;i<mRenderStates.size();++i){
		RenderState::ptr renderState=mRenderStates[i];
		renderState->setRenderStateDestroyedListener(NULL);
		renderState->destroy();
	}
	mRenderStates.clear();
}

Material::ptr MaterialManager::createMaterial(Texture::ptr texture,bool clamped,Material::ptr sharedSource){
	RenderState::ptr renderState;
	ShaderState::ptr shaderState;

	if(sharedSource==NULL){
		renderState=createRenderState();
	}
	else{
		renderState=sharedSource->getRenderState();
	}

	shaderState=createShaderState();

	Material::ptr material(new Material(renderState,shaderState));

	if(sharedSource==NULL){
		material->setBlendState(BlendState());
		material->setDepthState(DepthState());
		material->setRasterizerState(RasterizerState());
		material->setMaterialState(MaterialState(true,false,MaterialState::ShadeType_GOURAUD));
	}

	if(texture!=NULL){
		material->setTexture(0,texture);

		if(sharedSource==NULL){
			SamplerState samplerState(mDefaultSamplerState);
			if(texture->getNumMipLevels()==1){
				samplerState.mipFilter=SamplerState::FilterType_NONE;
			}
			if(clamped || texture->getDimension()==Texture::Dimension_CUBE){
				samplerState.uAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
				samplerState.vAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
				samplerState.wAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
			}
			material->setSamplerState(0,samplerState);

			TextureState textureState;
			if((texture->getFormat()&Texture::Format_BIT_DEPTH)>0){
				textureState.shadowResult=TextureState::ShadowResult_L;
			}
			material->setTextureState(0,textureState);
		}
	}

	manage(material);

	return material;
}

Material::ptr MaterialManager::cloneMaterial(Material::ptr material,bool managed,Material::ptr sharedSource){
	RenderState::ptr renderState;
	if(sharedSource==NULL){
		renderState=createRenderState();
	}
	else{
		renderState=sharedSource->getRenderState();
	}

	Material::ptr clonedMaterial(new Material(renderState));

	if(sharedSource==NULL){
		modifyRenderState(clonedMaterial->getRenderState(),material->getRenderState());
	}

	int i;
	for(i=0;i<material->getNumTextures();++i){
		clonedMaterial->setTexture(i,material->getTexture(i));
		clonedMaterial->setTextureName(i,material->getTextureName(i));
	}

	if(managed){
		manage(clonedMaterial);
	}

	return clonedMaterial;
}

RenderState::ptr MaterialManager::createRenderState(){
	RenderState::ptr renderState;

	if(mBackable || mEngine->getRenderDevice()==NULL){
		Logger::debug(Categories::TOADLET_TADPOLE,"creating BackableRenderState");

		BackableRenderState::ptr backableRenderState(new BackableRenderState());
		backableRenderState->create();
		if(mEngine->getRenderDevice()!=NULL){
			RenderState::ptr back(mEngine->getRenderDevice()->createRenderState());
			backableRenderState->setBack(back);
		}
		renderState=backableRenderState;
	}
	else{
		Logger::debug(Categories::TOADLET_TADPOLE,"creating RenderState");

		renderState=RenderState::ptr(mEngine->getRenderDevice()->createRenderState());
		if(renderState->create()==false){
			return NULL;
		}
	}

	renderState->setRenderStateDestroyedListener(this);
	mRenderStates.add(renderState);

	return renderState;
}

ShaderState::ptr MaterialManager::createShaderState(){
	ShaderState::ptr shaderState;

	if(mBackable || mEngine->getRenderDevice()==NULL){
		Logger::debug(Categories::TOADLET_TADPOLE,"creating BackableShaderState");

		BackableShaderState::ptr backableShaderState(new BackableShaderState());
		backableShaderState->create();
		if(mEngine->getRenderDevice()!=NULL){
			ShaderState::ptr back(mEngine->getRenderDevice()->createShaderState());
			backableShaderState->setBack(back);
		}
		shaderState=backableShaderState;
	}
	else{
		Logger::debug(Categories::TOADLET_TADPOLE,"creating ShaderState");

		shaderState=ShaderState::ptr(mEngine->getRenderDevice()->createShaderState());
		if(shaderState->create()==false){
			return NULL;
		}
	}

	shaderState->setShaderStateDestroyedListener(this);
	mShaderStates.add(shaderState);

	return shaderState;
}

void MaterialManager::modifyRenderState(RenderState::ptr dst,RenderState::ptr src){
	BlendState blendState;
	if(src->getBlendState(blendState)) dst->setBlendState(blendState);

	DepthState depthState;
	if(src->getDepthState(depthState)) dst->setDepthState(depthState);

	RasterizerState rasterizerState;
	if(src->getRasterizerState(rasterizerState)) dst->setRasterizerState(rasterizerState);

	FogState fogState;
	if(src->getFogState(fogState)) dst->setFogState(fogState);

	PointState pointState;
	if(src->getPointState(pointState)) dst->setPointState(pointState);

	MaterialState materialState;
	if(src->getMaterialState(materialState)) dst->setMaterialState(materialState);

	int i;
	for(i=0;i<src->getNumSamplerStates();++i){
		SamplerState samplerState;
		if(src->getSamplerState(i,samplerState)) dst->setSamplerState(i,samplerState);
	}
	for(i=0;i<src->getNumTextureStates();++i){
		TextureState textureState;
		if(src->getTextureState(i,textureState)) dst->setTextureState(i,textureState);
	}
}

void MaterialManager::contextActivate(RenderDevice *renderDevice){
	int i;
	for(i=0;i<mRenderStates.size();++i){
		RenderState::ptr renderState=mRenderStates[i];
		if(renderState!=NULL && renderState->getRootRenderState()!=renderState){
			RenderState::ptr back(renderDevice->createRenderState());
			shared_static_cast<BackableRenderState>(renderState)->setBack(back);
		}
	}
}

void MaterialManager::contextDeactivate(RenderDevice *renderDevice){
	int i;
	for(i=0;i<mRenderStates.size();++i){
		RenderState::ptr renderState=mRenderStates[i];
		if(renderState!=NULL && renderState->getRootRenderState()!=renderState){
			shared_static_cast<BackableRenderState>(renderState)->setBack(NULL);
		}
	}
}

void MaterialManager::renderStateDestroyed(RenderState *renderState){
	mRenderStates.remove(renderState);
}

void MaterialManager::shaderStateDestroyed(ShaderState *shaderState){
	mShaderStates.remove(shaderState);
}

Resource::ptr MaterialManager::unableToFindHandler(const egg::String &name,const ResourceHandlerData *handlerData){
	Texture::ptr texture=mEngine->getTextureManager()->findTexture(name);
	if(texture!=NULL){
		return createMaterial(texture);
	}
	else{
		return ResourceManager::unableToFindHandler(name,handlerData);
	}
}

}
}

