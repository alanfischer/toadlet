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
#include <toadlet/peeper/BackableShader.h>
#include <toadlet/peeper/RenderCaps.h>

namespace toadlet{
namespace peeper{

BackableShader::BackableShader():BaseResource(),
	mShaderType((ShaderType)0)
	//mProfiles,
	//mCodes,
	//mBack
{}

BackableShader::~BackableShader(){
	destroy();
}

bool BackableShader::create(ShaderType shaderType,const String &profile,const String &code){
	mShaderType=shaderType;
	
	mProfiles.resize(1);
	mCodes.resize(1);

	mProfiles[0]=profile;
	mCodes[0]=code;

	return true;
}

bool BackableShader::create(ShaderType shaderType,const String profiles[],const String codes[],int numCodes){
	mShaderType=shaderType;
	
	mProfiles.resize(numCodes);
	mCodes.resize(numCodes);

	int i;
	for(i=0;i<numCodes;++i){
		mProfiles[i]=profiles[i];
		mCodes[i]=codes[i];
	}

	return true;
}

void BackableShader::destroy(){
	if(mBack!=NULL){
		mBack->destroy();
		mBack=NULL;
	}

	mProfiles.clear();
	mCodes.clear();
}

void BackableShader::setBack(Shader::ptr back,RenderDevice *renderDevice){
	if(back!=mBack && mBack!=NULL){
		mBack->destroy();
	}

	mBack=back;
	
	if(mBack!=NULL){
		convertCreate(mBack,renderDevice,mShaderType,mProfiles,mCodes,mProfiles.size());
	}
}

bool BackableShader::convertCreate(Shader::ptr shader,RenderDevice *renderDevice,Shader::ShaderType shaderType,const String profiles[],const String codes[],int numCodes){
	int i;
	for(i=0;i<numCodes;++i){
		if(renderDevice->getShaderProfileSupported(profiles[i])){
			if(codes[i]==(char*)NULL || shader==NULL || shader->create(shaderType,profiles[i],codes[i])==false){
				return false;
			}
		}
	}

	return true;
}

}
}
