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
#include <toadlet/peeper/BackableShaderState.h>

namespace toadlet{
namespace peeper{

BackableShaderState::BackableShaderState():BaseResource()
	//mShaders
{}

void BackableShaderState::destroy(){
	if(mBack!=NULL){
		mBack->destroy();
		mBack=NULL;
	}

	mShaders.clear();
	
	BaseResource::destroy();
}

void BackableShaderState::setShader(Shader::ShaderType type,Shader::ptr shader){
	if(mShaders.size()<=type){
		mShaders.resize(type+1);
	}
	
	mShaders[type]=shader;

	if(mBack!=NULL){
		mBack->setShader(type,shader);
	}
}

Shader::ptr BackableShaderState::getShader(Shader::ShaderType type){
	if(mShaders.size()<=type){
		return NULL;
	}

	return mShaders[type];
}

void BackableShaderState::setBack(ShaderState::ptr back){
	if(back!=mBack && mBack!=NULL){
		mBack->destroy();
	}

	mBack=back;
	
	if(mBack!=NULL){
		mBack->create();

		int i;
		for(i=0;i<mShaders.size();++i){
			mBack->setShader((Shader::ShaderType)i,mShaders[i]);
		}
	}
}

}
}
