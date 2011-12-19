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
#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/material/RenderPass.h>

namespace toadlet{
namespace tadpole{
namespace material{

RenderPass::RenderPass(MaterialManager *manager,RenderState::ptr renderState,ShaderState::ptr shaderState):
	mManager(NULL)
	//mRenderState,
	//mShaderState,
	//mTextures,
	//mTextureNames,
	//mVariables
{
	mManager=manager;
	if(renderState==NULL){
		mRenderState=manager->createRenderState();
		mOwnRenderState=mRenderState;
	}
	else{
		mRenderState=renderState;
	}
	if(shaderState==NULL){
		mShaderState=manager->createShaderState();
		mOwnShaderState=mShaderState;
	}
	else{
		mShaderState=shaderState;
	}
}

RenderPass::~RenderPass(){
	destroy();
}

void RenderPass::destroy(){
	if(mOwnRenderState!=NULL){
		mOwnRenderState->destroy();
		mOwnRenderState=NULL;
	}
	if(mOwnShaderState!=NULL){
		mOwnShaderState->destroy();
		mOwnShaderState=NULL;
	}
	mRenderState=NULL;
	mShaderState=NULL;

	int i,j;
	for(j=0;j<Shader::ShaderType_MAX;++j){
		for(i=0;i<mTextures[j].size();++i){
			if(mTextures[j][i]!=NULL){
				mTextures[j][i]->release();
			}
		}
		mTextures[j].clear();
	}

	if(mVariables!=NULL){
		mVariables->destroy();
		mVariables=NULL;
	}
}

void RenderPass::setTexture(Shader::ShaderType type,int i,Texture::ptr texture,const SamplerState &samplerState,const TextureState &textureState){
	if(i>=mTextures[type].size()){
		mTextures[type].resize(i+1);
	}
	if(mTextures[type][i]!=NULL){
		mTextures[type][i]->release();
	}
	mTextures[type][i]=texture;
	if(mTextures[type][i]!=NULL){
		mTextures[type][i]->retain();
	}

	mRenderState->setSamplerState(type,i,samplerState);
	mRenderState->setTextureState(type,i,textureState);
}

void RenderPass::setShader(Shader::ShaderType type,Shader::ptr shader){
	if(mShaderState==NULL){
		return;
	}
	
	mShaderState->setShader(type,shader);

	if(mVariables!=NULL){
		mVariables->buildBuffers(mManager->getBufferManager(),this);
	}
}

RenderVariableSet::ptr RenderPass::getVariables(){
	if(mVariables==NULL){
		mVariables=RenderVariableSet::ptr(new RenderVariableSet());
		mVariables->buildBuffers(mManager->getBufferManager(),this);
	}

	return mVariables;
}

bool RenderPass::isDepthSorted() const{
	DepthState state;
	return mRenderState->getDepthState(state) && state.write==false;
}

void RenderPass::setupRenderVariables(RenderDevice *renderDevice,int scope,SceneParameters *parameters){
	if(mVariables==NULL){
		return;
	}

	mVariables->update(scope,parameters);

	int i;
	for(i=0;i<mVariables->getNumBuffers();++i){
		int bufferScope=mVariables->getBufferScope(i);
		// If the buffer applies to this scope, and it is the highest bit scope in the buffer
		if((bufferScope&scope)!=0 && (bufferScope&~scope)<=scope){
			renderDevice->setBuffer(mVariables->getBufferShaderType(i),mVariables->getBufferIndex(i),mVariables->getBuffer(i));
		}
	}
}

}
}
}
