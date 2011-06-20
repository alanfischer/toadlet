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
#include <toadlet/peeper/BackableShaderState.h>
#include <toadlet/peeper/RenderCaps.h>
#include <toadlet/tadpole/Material.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

Material::Material(RenderState::ptr renderState,ShaderState::ptr shaderState):BaseResource(),
	//mFogState,
	//mBlendState,
	mSort(SortType_MATERIAL),
	//mDepthState,
	//mPointState,
	//mRasterizerState
	mLayer(0)
{
	if(renderState==NULL){
		renderState=RenderState::ptr(new BackableRenderState());
	}

	mRenderState=renderState;

	if(shaderState==NULL){
		shaderState=ShaderState::ptr(new BackableShaderState());
	}

	mShaderState=shaderState;
}

Material::~Material(){
	destroy();
}

void Material::destroy(){
	mRenderState->destroy();
	mShaderState->destroy();

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
		mTextureNames.resize(i+1);
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

void Material::setTextureName(int i,const String &name){
	if(i>=mTextures.size()){
		mTextures.resize(i+1);
		mTextureNames.resize(i+1);
	}
	mTextureNames[i]=name;
}

void Material::setShader(Shader::ShaderType type,Shader::ptr shader){
	mShaderState->setShader(type,shader);
}

Shader::ptr Material::getShader(Shader::ShaderType type){
	return mShaderState->getShader(type);
}

bool Material::isDepthSorted() const{
	switch(mSort){
		case SortType_DEPTH:
			return true;
		case SortType_MATERIAL:
			return false;
		default:{
			DepthState depth;
			return mRenderState->getDepthState(depth) && depth.write==false;
		}
	}
}

/// @todo: Optimize this so we're not resetting a ton of texture states, and not requesting the caps
void Material::setupRenderDevice(RenderDevice *renderDevice){
	renderDevice->setRenderState(mRenderState);
	renderDevice->setShaderState(mShaderState);

	RenderCaps caps;
	renderDevice->getRenderCaps(caps);

	int i;
	for(i=0;i<getNumTextures();++i){
		renderDevice->setTexture(i,getTexture(i));
	}
	for(;i<caps.maxTextureStages;++i){
		renderDevice->setTexture(i,NULL);
	}
}

}
}
