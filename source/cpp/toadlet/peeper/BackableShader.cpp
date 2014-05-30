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

bool BackableShader::create(ShaderType shaderType,const char *profile,const char *code){
	mShaderType=shaderType;
	
	mProfiles.resize(1);
	mCodes.resize(1);
	mRawProfiles.resize(1);
	mRawCodes.resize(1);

	mProfiles[0]=profile;
	mCodes[0]=code;
	mRawProfiles[0]=mProfiles[0];
	mRawCodes[0]=mCodes[0];

	return true;
}

bool BackableShader::create(ShaderType shaderType,const char *profiles[],const char *codes[],int numCodes){
	mShaderType=shaderType;
	
	mProfiles.resize(numCodes);
	mCodes.resize(numCodes);
	mRawProfiles.resize(numCodes);
	mRawCodes.resize(numCodes);

	int i;
	for(i=0;i<numCodes;++i){
		mProfiles[i]=profiles[i];
		mCodes[i]=codes[i];
		mRawProfiles[i]=mProfiles[i];
		mRawCodes[i]=mCodes[i];
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
	mRawProfiles.clear();
	mRawCodes.clear();

	BaseResource::destroy();
}

void BackableShader::setBack(Shader::ptr back,RenderDevice *renderDevice){
	if(back!=mBack && mBack!=NULL){
		mBack->destroy();
	}

	mBack=back;
	
	if(mBack!=NULL){
		if(convertCreate(mBack,renderDevice,mShaderType,mRawProfiles,mRawCodes,mProfiles.size())==false){
			mBack=NULL;
		}
	}
}

bool BackableShader::convertCreate(Shader::ptr shader,RenderDevice *renderDevice,Shader::ShaderType shaderType,const char *profiles[],const char *codes[],int numCodes){
	if(shader==NULL){
		return false;
	}

	int i;
	for(i=0;i<numCodes;++i){
		if(renderDevice->getShaderProfileSupported(profiles[i])){
			bool result=false;
			TOADLET_TRY
				result=shader->create(shaderType,profiles[i],codes[i]);
			TOADLET_CATCH_ANONYMOUS(){result=false;}
			if(result){
				return true;
			}
		}
	}

	return false;
}

}
}
