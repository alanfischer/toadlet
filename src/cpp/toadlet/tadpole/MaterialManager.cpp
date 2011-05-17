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
	for(i=0;i<mRenderStateSets.size();++i){
		RenderStateSet::ptr set=mRenderStateSets[i];
		set->setRenderStateSetDestroyedListener(NULL);
		set->destroy();
	}
	mRenderStateSets.clear();
}

Material::ptr MaterialManager::createMaterial(Texture::ptr texture,bool clamped,Material::ptr sharedSource){
	RenderStateSet::ptr renderStates;
	if(sharedSource==NULL){
		renderStates=createRenderStateSet();
	}
	else{
		renderStates=sharedSource->getRenderStateSet();
	}

	Material::ptr material(new Material(renderStates));

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
	RenderStateSet::ptr renderStates;
	if(sharedSource==NULL){
		renderStates=createRenderStateSet();
	}
	else{
		renderStates=sharedSource->getRenderStateSet();
	}

	Material::ptr clonedMaterial(new Material(renderStates));

	if(sharedSource==NULL){
		clonedMaterial->modifyWith(material);
	}
	else{
		int i;
		for(i=0;i<material->getNumTextures();++i){
			clonedMaterial->setTexture(i,material->getTexture(i));
			clonedMaterial->setTextureName(i,material->getTextureName(i));
		}
	}

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
		return createMaterial(texture);
	}
	else{
		return ResourceManager::unableToFindHandler(name,handlerData);
	}
}

}
}

