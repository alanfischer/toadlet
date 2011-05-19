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
#include <toadlet/peeper/BackableRenderState.h>
#include <toadlet/peeper/RendererCaps.h>
#include <toadlet/tadpole/Material.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

Material::Material(RenderState::ptr renderState):BaseResource(),
	//mFogState,
	//mBlendState,
	mDepthSorted(false),
	//mDepthState,
	//mPointState,
	//mRasterizerState
	mLayer(0)
{
	if(renderState==NULL){
		renderState=RenderState::ptr(new BackableRenderState());
	}

	mRenderState=renderState;
}

Material::~Material(){
	destroy();
}

void Material::destroy(){
	mRenderState->destroy();

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
		if(mRenderState->getSamplerState(i,samplerState)==false) mRenderState->setSamplerState(i,samplerState);

		TextureState textureState;
		if(mRenderState->getTextureState(i,textureState)==false) mRenderState->setTextureState(i,textureState);
	}
}

/// @todo: Optimize this so we're not resetting a ton of texture states, and not requesting the caps
void Material::setupRenderer(Renderer *renderer){
	renderer->setRenderState(mRenderState);

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
