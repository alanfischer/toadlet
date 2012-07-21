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

#include <toadlet/peeper/BackableRenderState.h>
#include <toadlet/peeper/BackableShaderState.h>
#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/material/RenderPass.h>

namespace toadlet{
namespace tadpole{
namespace material{

RenderPass::RenderPass(MaterialManager *manager,RenderState *renderState,ShaderState *shaderState):
	mManager(NULL),
	//mRenderState,
	//mShaderState,
	//mTextures,
	mModelMatrixFlags(0)
	//mVariables,
{
	mManager=manager;
	if(renderState==NULL){
		if(manager!=NULL){
			mRenderState=manager->createRenderState();
		}
		else{
			mRenderState=new BackableRenderState();
		}
		mOwnRenderState=mRenderState;
	}
	else{
		mRenderState=renderState;
	}
	if(shaderState==NULL){
		if(manager!=NULL){
			mShaderState=manager->createShaderState();
		}
		else{
			mShaderState=new BackableShaderState();
		}
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

	int i;
	for(i=0;i<Shader::ShaderType_MAX;++i){
		mTextures[i].clear();
	}

	if(mVariables!=NULL){
		mVariables->destroy();
		mVariables=NULL;
	}
}

void RenderPass::setTexture(const String &name,Texture *texture,const String &samplerName,const SamplerState &samplerState,const TextureState &textureState){
	if(mVariables!=NULL){
		mVariables->addTexture(name,texture,samplerName,samplerState,textureState);
	}
	else{
		int i,j;
		for(j=0;j<Shader::ShaderType_MAX;++j){
			for(i=0;i<mTextureLocationNames[j].size();++i){
				if(mTextureLocationNames[j][i]==name){
					setTexture((Shader::ShaderType)j,i,texture,samplerState,textureState);
				}
			}
		}
	}
}

void RenderPass::setTexture(Shader::ShaderType type,int i,Texture *texture,const SamplerState &samplerState,const TextureState &textureState){
	if(i>=mTextures[type].size()){
		mTextures[type].resize(i+1);
	}
	mTextures[type][i]=texture;

	mRenderState->setSamplerState(type,i,samplerState);
	mRenderState->setTextureState(type,i,textureState);
}

void RenderPass::setShader(Shader::ShaderType type,Shader *shader){
	if(mShaderState==NULL){
		return;
	}
	
	mShaderState->setShader(type,shader);

	if(mVariables!=NULL){
		mVariables->buildBuffers(mManager->getBufferManager(),this);
	}
}

RenderVariableSet::ptr RenderPass::makeVariables(){
	if(mVariables==NULL){
		mVariables=RenderVariableSet::ptr(new RenderVariableSet());
		mVariables->buildBuffers(mManager->getBufferManager(),this);
	}

	return mVariables;
}

void RenderPass::updateVariables(int scope,SceneParameters *parameters){
	if(mVariables!=NULL){
		mVariables->update(scope,parameters);
	}
}

void RenderPass::setTextureLocationName(Shader::ShaderType type,int i,const String &name){
	if(i>=mTextureLocationNames[type].size()){
		mTextureLocationNames[type].resize(i+1);
	}
	mTextureLocationNames[type][i]=name;
}

bool RenderPass::isDepthSorted() const{
	DepthState state;
	return mRenderState->getDepthState(state) && state.write==false;
}

void RenderPass::compile(){
	if(mVariables!=NULL){
		mVariables->buildBuffers(mManager->getBufferManager(),this);
	}
}

}
}
}
