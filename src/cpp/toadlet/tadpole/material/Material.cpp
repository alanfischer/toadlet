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
#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/material/Material.h>

namespace toadlet{
namespace tadpole{
namespace material{

Material::Material(MaterialManager *manager,Material *source,bool clone):BaseResource(),
	mSort(SortType_MATERIAL),
	mOwnsStates(false),
	mLayer(0)
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

Material::~Material(){
	destroy();
}

void Material::destroy(){
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

void Material::shareStates(Material::ptr material){
	if(mOwnsStates){
		if(mRenderState!=NULL){
			mRenderState->destroy();
		}
		if(mShaderState!=NULL){
			mShaderState->destroy();
		}
	}
	mRenderState=material->getRenderState();
	mShaderState=material->getShaderState();
	mOwnsStates=false;
}

RenderVariableSet::ptr Material::getVariables(){
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

	mParameters=SceneParameters::ptr(new SceneParameters());

	return mVariables;
}

/// @todo: This whole setup should be moved to the SceneRenderers, let them handle it
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

void Material::setupRenderVariables(RenderDevice *renderDevice,int scope,Scene *scene,Renderable *renderable){
	mParameters->setScene(scene);
	mParameters->setRenderable(renderable);
	mVariables->update(scope,mParameters);
	int i;
	for(i=0;i<mVariables->getNumBuffers();++i){
		// If the buffer only changes for this scope
		if(mVariables->getBufferScope(i)==scope){
			renderDevice->setBuffer(i,mVariables->getBuffer(i));
		}
	}
}

}
}
}
