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

#include "D3D9Shader.h"
#include "D3D9RenderDevice.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D9Shader::D3D9Shader(D3D9RenderDevice *renderDevice):BaseResource(),
	mDevice(NULL),
	mD3DDevice(NULL),

	mBytecode(NULL),mLog(NULL),
	mShader(NULL)
{
	mDevice=renderDevice;
	mD3DDevice=mDevice->getDirect3DDevice9();
}

D3D9Shader::~D3D9Shader(){
	if(mShader!=NULL){
		destroy();
	}
}

bool D3D9Shader::create(ShaderType shaderType,const String &code){
	mShaderType=shaderType;
	mCode=code;

	bool result=createContext();

	return result;
}

void D3D9Shader::destroy(){
	destroyContext();
}

bool D3D9Shader::createContext(){
	IDirect3DDevice9 *device=mD3DDevice;

	String function="main";
	String profile;
	if(mShaderType==ShaderType_VERTEX){
		profile="vs_2_0";
	}
	else if(mShaderType==ShaderType_FRAGMENT){
		profile="ps_2_0";
	}
	
	HRESULT result=E_FAIL;
	result=D3DXCompileShader(mCode,mCode.length(),NULL,NULL,function,profile,0,&mBytecode,&mLog,NULL);
	if(FAILED(result)){
		Error::unknown(Categories::TOADLET_PEEPER,(LPCSTR)mLog->GetBufferPointer());
		return false;
	}
	
	switch(mShaderType){
		case ShaderType_VERTEX:
			result=device->CreateVertexShader((DWORD*)mBytecode->GetBufferPointer(),(IDirect3DVertexShader9**)&mShader);
		break;
		case ShaderType_FRAGMENT:
			result=device->CreatePixelShader((DWORD*)mBytecode->GetBufferPointer(),(IDirect3DPixelShader9**)&mShader);
		break;
		default:
			result=E_FAIL;
	}

	return SUCCEEDED(result);
}

bool D3D9Shader::destroyContext(){
	HRESULT result=S_OK;

	if(mShader!=NULL){
		result=mShader->Release();
		mShader=NULL;
	}
	
	if(mBytecode!=NULL){
		mBytecode->Release();
		mBytecode=NULL;
	}


	if(mLog!=NULL){
		mLog->Release();
		mLog=NULL;
	}

	return SUCCEEDED(result);
}

}
}
