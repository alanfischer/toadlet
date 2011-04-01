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
#include <toadlet/peeper/BackableRenderStateSet.h>
#include <toadlet/peeper/CapabilityState.h>
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

	mDefaultMinFilter=TextureStage::Filter_LINEAR;
	mDefaultMagFilter=TextureStage::Filter_LINEAR;
	mDefaultMipFilter=TextureStage::Filter_LINEAR;
}

void MaterialManager::destroy(){
	ResourceManager::destroy();

	int i;
	for(i=0;i<mRenderStateSets.size();++i){
		RenderStateSet::ptr set=mRenderStateSets[i];
		set->setRenderStateSetDestroyedListener(NULL);
		set->destroy();
	}
	mRenderStateSets.clear();
}

Material::ptr MaterialManager::createMaterial(){
	Material::ptr material(new Material(createRenderStateSet()));

	material->setBlendState(BlendState());
	material->setDepthState(DepthState());
	material->setRasterizerState(RasterizerState());
	material->setMaterialState(MaterialState(true,false,MaterialState::ShadeType_GOURAUD));

	manage(material);

	return material;
}

Material::ptr MaterialManager::cloneMaterial(Material::ptr material,bool managed){
	Material::ptr clonedMaterial(new Material(createRenderStateSet()));

	clonedMaterial->modifyWith(material);

	if(managed){
		manage(clonedMaterial);
	}

	return clonedMaterial;
}

RenderStateSet::ptr MaterialManager::createRenderStateSet(){
	RenderStateSet::ptr renderStateSet;

	if(mBackable || mEngine->getRenderer()==NULL){
		Logger::debug(Categories::TOADLET_TADPOLE,"creating BackableRenderStateSet");

		BackableRenderStateSet::ptr backableRenderStateSet(new BackableRenderStateSet());
		backableRenderStateSet->create();
		if(mEngine->getRenderer()!=NULL){
			RenderStateSet::ptr back(mEngine->getRenderer()->createRenderStateSet());
			backableRenderStateSet->setBack(back);
		}
		renderStateSet=backableRenderStateSet;
	}
	else{
		Logger::debug(Categories::TOADLET_TADPOLE,"creating RenderStateSet");

		renderStateSet=RenderStateSet::ptr(mEngine->getRenderer()->createRenderStateSet());
		if(renderStateSet->create()==false){
			return NULL;
		}
	}

	renderStateSet->setRenderStateSetDestroyedListener(this);
	mRenderStateSets.add(renderStateSet);

	return renderStateSet;
}

TextureStage::ptr MaterialManager::createTextureStage(Texture::ptr texture,bool clamped){
	TextureStage::ptr textureStage(new TextureStage(texture));
	textureStage->setMinFilter(mDefaultMinFilter);
	textureStage->setMagFilter(mDefaultMagFilter);
	if(texture!=NULL && texture->getNumMipLevels()!=1){ // Only enable mipmap filtering if we have 0 for autogen, or > 1 mipmap levels
		textureStage->setMipFilter(mDefaultMipFilter);
	}
	else{
		textureStage->setMipFilter(TextureStage::Filter_NONE);
	}
	if(clamped || (texture!=NULL && texture->getDimension()==Texture::Dimension_CUBE)){
		textureStage->setUAddressMode(TextureStage::AddressMode_CLAMP_TO_EDGE);
		textureStage->setVAddressMode(TextureStage::AddressMode_CLAMP_TO_EDGE);
		textureStage->setWAddressMode(TextureStage::AddressMode_CLAMP_TO_EDGE);
	}
	if(texture!=NULL && (texture->getFormat()&Texture::Format_BIT_DEPTH)>0){
		textureStage->setShadowComparison(TextureStage::ShadowComparison_L);
	}

	return textureStage;
}

void MaterialManager::contextActivate(Renderer *renderer){
	int i;
	for(i=0;i<mRenderStateSets.size();++i){
		RenderStateSet::ptr set=mRenderStateSets[i];
		if(set!=NULL && set->getRootRenderStateSet()!=set){
			RenderStateSet::ptr back(renderer->createRenderStateSet());
			shared_static_cast<BackableRenderStateSet>(set)->setBack(back);
		}
	}
}

void MaterialManager::contextDeactivate(Renderer *renderer){
	int i;
	for(i=0;i<mRenderStateSets.size();++i){
		RenderStateSet::ptr set=mRenderStateSets[i];
		if(set!=NULL && set->getRootRenderStateSet()!=set){
			shared_static_cast<BackableRenderStateSet>(set)->setBack(NULL);
		}
	}
}

void MaterialManager::renderStateSetDestroyed(RenderStateSet *renderStateSet){
	mRenderStateSets.remove(renderStateSet);
}

Resource::ptr MaterialManager::unableToFindHandler(const egg::String &name,const ResourceHandlerData *handlerData){
	Texture::ptr texture=mEngine->getTextureManager()->findTexture(name);
	if(texture!=NULL){
		Material::ptr material=createMaterial();
		material->setTextureStage(0,createTextureStage(texture));
		return material;
	}
	else{
		return ResourceManager::unableToFindHandler(name,handlerData);
	}
}

}
}

