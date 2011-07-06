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

RenderPass::RenderPass(MaterialManager *manager,RenderPass *source,bool clone):
	mOwnsStates(false)
{
	mManager=manager;
	if(manager!=NULL && clone==false){
		mRenderState=mManager->createRenderState();
		mShaderState=mManager->createShaderState();
		mOwnsStates=true;

		if(source!=NULL){
			manager->modifyRenderState(mRenderState,source->getRenderState());
			manager->modifyRenderState(mRenderState,source->getRenderState());
		}
	}
	else if(clone){
		mRenderState=source->getRenderState();
		mShaderState=source->getShaderState();
		mOwnsStates=false;
	}
	else{
		mRenderState=RenderState::ptr(new BackableRenderState());
		mShaderState=ShaderState::ptr(new BackableShaderState());
		mOwnsStates=true;
	}
}

RenderPass::~RenderPass(){
	destroy();
}

void RenderPass::destroy(){
	if(mOwnsStates){
		if(mRenderState!=NULL){
			mRenderState->destroy();
		}
		if(mShaderState!=NULL){
			mShaderState->destroy();
		}
	}
	mRenderState=NULL;
	mShaderState=NULL;

	int i;
	for(i=0;i<mTextures.size();++i){
		if(mTextures[i]!=NULL){
			mTextures[i]->release();
		}
	}
	mTextures.clear();

	if(mVariables!=NULL){
		mVariables->destroy();
		mVariables=NULL;
	}
}

void RenderPass::setTexture(int i,Texture::ptr texture){
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

void RenderPass::setTextureName(int i,const String &name){
	if(i>=mTextures.size()){
		mTextures.resize(i+1);
		mTextureNames.resize(i+1);
	}
	mTextureNames[i]=name;
}

void RenderPass::shareStates(RenderPass *source){
	if(mOwnsStates){
		if(mRenderState!=NULL){
			mRenderState->destroy();
		}
		if(mShaderState!=NULL){
			mShaderState->destroy();
		}
	}
	mRenderState=source->getRenderState();
	mShaderState=source->getShaderState();
	mOwnsStates=false;
}

RenderVariableSet::ptr RenderPass::getVariables(){
	if(mVariables!=NULL){
		return mVariables;
	}

	mVariables=RenderVariableSet::ptr(new RenderVariableSet());

	int i;
	for(i=0;i<mShaderState->getNumVariableBuffers(Shader::ShaderType_VERTEX);++i){
		VariableBufferFormat::ptr format=mShaderState->getVariableBufferFormat(Shader::ShaderType_VERTEX,i);
		VariableBuffer::ptr buffer=mManager->getBufferManager()->createVariableBuffer(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,format);
		mVariables->addBuffer(buffer);
	}

	return mVariables;
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
			renderDevice->setBuffer(i,mVariables->getBuffer(i));
		}
	}
}

}
}
}
