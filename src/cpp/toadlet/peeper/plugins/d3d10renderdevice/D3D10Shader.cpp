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

#include "D3D10Shader.h"
#include "D3D10RenderDevice.h"
#include "D3D10VertexFormat.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D10Shader::D3D10Shader(D3D10RenderDevice *renderDevice):BaseResource(),
	mDevice(NULL),
	mD3DDevice(NULL),

	mBytecode(NULL),mLog(NULL),
	mShader(NULL)
{
	mDevice=renderDevice;
	mD3DDevice=mDevice->getD3D10Device();
}

D3D10Shader::~D3D10Shader(){
	if(mShader!=NULL){
		destroy();
	}
}

bool D3D10Shader::create(ShaderType shaderType,const String &code){
	mShaderType=shaderType;
	mCode=code;

	bool result=createContext();

	if(mDevice!=NULL){
		mDevice->shaderCreated(this);
	}

	return result;
}

void D3D10Shader::destroy(){
	destroyContext();

	if(mDevice!=NULL){
		mDevice->shaderDestroyed(this);
	}
}

bool D3D10Shader::createContext(){
	ID3D10Device *device=mD3DDevice;

	String function="main";
	String profile;
	if(mShaderType==ShaderType_VERTEX){
		profile="vs_4_0";
	}
	else if(mShaderType==ShaderType_FRAGMENT){
		profile="ps_4_0";
	}
	
	HRESULT result=E_FAIL;
	#if defined(TOADLET_SET_D3D10)
		result=D3D10CompileShader(mCode,mCode.length(),NULL,NULL,NULL,function,profile,0,&mBytecode,&mLog);
	#endif
	if(result==E_FAIL){
		Error::unknown(Categories::TOADLET_PEEPER,(LPCSTR)mLog->GetBufferPointer());
		return false;
	}
	
	switch(mShaderType){
		case ShaderType_VERTEX:
			result=device->CreateVertexShader(mBytecode->GetBufferPointer(),mBytecode->GetBufferSize(),(ID3D10VertexShader**)&mShader);
		break;
		case ShaderType_FRAGMENT:
			result=device->CreatePixelShader(mBytecode->GetBufferPointer(),mBytecode->GetBufferSize(),(ID3D10PixelShader**)&mShader);
		break;
		case ShaderType_GEOMETRY:
			result=device->CreateGeometryShader(mBytecode->GetBufferPointer(),mBytecode->GetBufferSize(),(ID3D10GeometryShader**)&mShader);
		break;
		default:
			result=E_FAIL;
	}

	return SUCCEEDED(result);
}

bool D3D10Shader::destroyContext(){
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

ID3D10InputLayout *D3D10Shader::findInputLayout(D3D10VertexFormat *vertexFormat){
	int handle=vertexFormat->mUniqueHandle;
	ID3D10InputLayout *layout=NULL;
	if(handle<mLayouts.size()){
		layout=mLayouts[handle];
	}
	else{
		mDevice->getD3D10Device()->CreateInputLayout(
			vertexFormat->mElements,vertexFormat->mElements.size(),mBytecode->GetBufferPointer(),mBytecode->GetBufferSize(),&layout
		);
		mLayouts.resize(handle+1,NULL);
		mLayouts[handle]=layout;
	}
	return layout;
}

}
}
