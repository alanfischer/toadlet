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

namespace toadlet{
namespace peeper{

BackableRenderState::BackableRenderState():BaseResource(),
	mBlendState(NULL),
	mDepthState(NULL),
	mRasterizerState(NULL),
	mFogState(NULL),
	mGeometryState(NULL),
	mMaterialState(NULL)
#if !defined(FULLY_BACKED)
	,mSamplerState(NULL),
	mTextureState(NULL)
#endif
{}

void BackableRenderState::destroy(){
	if(mBack!=NULL){
		mBack->destroy();
		mBack=NULL;
	}

	if(mBlendState!=NULL){
		delete mBlendState;
		mBlendState=NULL;
	}
	if(mDepthState!=NULL){
		delete mDepthState;
		mDepthState=NULL;
	}
	if(mRasterizerState!=NULL){
		delete mRasterizerState;
		mRasterizerState=NULL;
	}
	if(mFogState!=NULL){
		delete mFogState;
		mFogState=NULL;
	}
	if(mGeometryState!=NULL){
		delete mGeometryState;
		mGeometryState=NULL;
	}
	if(mMaterialState!=NULL){
		delete mMaterialState;
		mMaterialState=NULL;
	}
#if defined(FULLY_BACKED)
	int i,j;
	for(j=0;j<Shader::ShaderType_MAX;++j){
		for(i=0;i<mSamplerStates[j].size();++i){
			delete mSamplerStates[j][i];
		}
		mSamplerStates[j].clear();
		for(i=0;i<mTextureStates[j].size();++i){
			delete mTextureStates[j][i];
		}
		mTextureStates[j].clear();
	}
#else
	if(mSamplerState!=NULL){
		delete mSamplerState;
		mSamplerState=NULL;
	}
	if(mTextureState!=NULL){
		delete mTextureState;
		mTextureState=NULL;
	}
#endif

	BaseResource::destroy();
}

void BackableRenderState::setBack(RenderState::ptr back){
	if(back!=mBack && mBack!=NULL){
		mBack->destroy();
	}

	mBack=back;
	
	if(mBack!=NULL){
		if(mBlendState!=NULL){mBack->setBlendState(*mBlendState);}
		if(mDepthState!=NULL){mBack->setDepthState(*mDepthState);}
		if(mRasterizerState!=NULL){mBack->setRasterizerState(*mRasterizerState);}
		if(mFogState!=NULL){mBack->setFogState(*mFogState);}
		if(mGeometryState!=NULL){mBack->setGeometryState(*mGeometryState);}
		if(mMaterialState!=NULL){mBack->setMaterialState(*mMaterialState);}
#if defined(FULLY_BACKED)
		int j,i;
		for(j=0;j<Shader::ShaderType_MAX;++j){
			for(i=0;i<mSamplerStates[j].size();++i){
				if(mSamplerStates[j][i]!=NULL){mBack->setSamplerState((Shader::ShaderType)j,i,*mSamplerStates[j][i]);}
			}
			for(i=0;i<mTextureStates[j].size();++i){
				if(mTextureStates[j][i]!=NULL){mBack->setTextureState((Shader::ShaderType)j,i,*mTextureStates[j][i]);}
			}
		}
#else
		if(mSamplerState!=NULL){mBack->setSamplerState(Shader::ShaderType_FRAGMENT,0,*mSamplerState);}
		if(mTextureState!=NULL){mBack->setTextureState(Shader::ShaderType_FRAGMENT,0,*mTextureState);}
#endif
	}
}

}
}
