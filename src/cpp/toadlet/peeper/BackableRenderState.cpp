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

BackableRenderState::BackableRenderState():
	mListener(NULL),
	mBlendState(NULL),
	mDepthState(NULL),
	mRasterizerState(NULL),
	mFogState(NULL),
	mPointState(NULL),
	mMaterialState(NULL)
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
	if(mPointState!=NULL){
		delete mPointState;
		mPointState=NULL;
	}
	if(mMaterialState!=NULL){
		delete mMaterialState;
		mMaterialState=NULL;
	}
	int i;
	for(i=0;i<mSamplerStates.size();++i){
		delete mSamplerStates[i];
	}
	mSamplerStates.clear();
	for(i=0;i<mTextureStates.size();++i){
		delete mTextureStates[i];
	}
	mTextureStates.clear();

	if(mListener!=NULL){
		mListener->renderStateDestroyed(this);
		mListener=NULL;
	}
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
		if(mPointState!=NULL){mBack->setPointState(*mPointState);}
		if(mMaterialState!=NULL){mBack->setMaterialState(*mMaterialState);}
		int i;
		for(i=0;i<mSamplerStates.size();++i){
			if(mSamplerStates[i]!=NULL){mBack->setSamplerState(i,*mSamplerStates[i]);}
		}
		for(i=0;i<mTextureStates.size();++i){
			if(mTextureStates[i]!=NULL){mBack->setTextureState(i,*mTextureStates[i]);}
		}
	}
}

}
}