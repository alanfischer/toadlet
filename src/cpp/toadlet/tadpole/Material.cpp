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

#include <toadlet/egg/Logger.h>
#include <toadlet/peeper/BackableRenderStateSet.h>
#include <toadlet/peeper/RendererCaps.h>
#include <toadlet/tadpole/Material.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

Material::Material(RenderStateSet::ptr renderStateSet):BaseResource(),
	//mFogState,
	//mBlendState,
	mDepthSorted(false),
	//mDepthState,
	//mPointState,
	//mRasterizerState
	mLayer(0)
{
	if(renderStateSet==NULL){
		renderStateSet=RenderStateSet::ptr(new BackableRenderStateSet());
	}

	mRenderStateSet=renderStateSet;
}

Material::~Material(){
	destroy();
}

void Material::destroy(){
	mRenderStateSet->destroy();

	int i;
	for(i=0;i<mTextures.size();++i){
		if(mTextures[i]!=NULL){
			mTextures[i]->release();
		}
	}
	mTextures.clear();
}

void Material::setTexture(int i,Texture::ptr texture){
	if(i>=mTextures.size()){
		mTextures.resize(i+1);
	}
	if(mTextures[i]!=NULL){
		mTextures[i]->release();
	}
	mTextures[i]=texture;
	if(mTextures[i]!=NULL){
		mTextures[i]->retain();
	}

	// Always add a default SamplerState and TextureState if non exists
	{
		SamplerState samplerState;
		if(mRenderStateSet->getSamplerState(i,samplerState)==false) mRenderStateSet->setSamplerState(i,samplerState);

		TextureState textureState;
		if(mRenderStateSet->getTextureState(i,textureState)==false) mRenderStateSet->setTextureState(i,textureState);
	}
}

void Material::modifyWith(Material *material){
	RenderStateSet::ptr src=material->mRenderStateSet;
	RenderStateSet::ptr dst=mRenderStateSet;

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
	for(i=0;i<material->getNumTextures();++i){
		setTexture(i,material->getTexture(i));
		setTextureName(i,material->getTextureName(i));
	}

	setDepthSorted(material->getDepthSorted());
}

/// @todo: Optimize this so we're not resetting a ton of texture states, and not requesting the caps
void Material::setupRenderer(Renderer *renderer){
	renderer->setRenderStateSet(mRenderStateSet);

	RendererCaps caps;
	renderer->getRendererCaps(caps);

	int i;
	for(i=0;i<getNumTextures();++i){
		renderer->setTexture(i,getTexture(i));
	}
	for(;i<caps.maxTextureStages;++i){
		renderer->setTexture(i,NULL);
	}
}

}
}
