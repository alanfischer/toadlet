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

#include "GLRenderState.h"

namespace toadlet{
namespace peeper{

GLRenderState::GLRenderState(GLRenderDevice *renderDevice):
	mListener(NULL),
	mBlendState(NULL),
	mDepthState(NULL),
	mRasterizerState(NULL),
	mFogState(NULL),
	mPointState(NULL),
	mMaterialState(NULL)
{}

GLRenderState::~GLRenderState(){
	destroy();
}

void GLRenderState::destroy(){
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

}
}
