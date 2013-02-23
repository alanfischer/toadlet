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
#include "D3D9WindowRenderTarget.h"
#include <toadlet/egg/Log.h>
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace peeper{

D3D9Shader::D3D9Shader(D3D9RenderDevice *renderDevice):BaseResource(),
	mDevice(NULL),
	mD3DDevice(NULL),

	mShaderType((ShaderType)0),
	//mProfile,
	//mCode,

	mShader(NULL),
	mBytecode(NULL),mLog(NULL),
	mConstantTable(NULL)
{
	mDevice=renderDevice;
	mD3DDevice=mDevice->getDirect3DDevice9();
}

bool D3D9Shader::create(ShaderType shaderType,const String &profile,const String &code){
	mShaderType=shaderType;
	mProfile=profile;
	mCode=code;

	bool result=createContext();

	return result;
}

void D3D9Shader::destroy(){
	destroyContext();

	BaseResource::destroy();
}

bool D3D9Shader::createContext(){
	IDirect3DDevice9 *device=mD3DDevice;

	String function="main";
	String targetProfile;
	switch(mShaderType){
		case ShaderType_VERTEX:
			targetProfile="vs_3_0";
		break;
		case ShaderType_FRAGMENT:
			targetProfile="ps_3_0";
		break;
		default:
			return false;
		break;
	}

	D3D9WindowRenderTarget *renderTarget=((D3D9WindowRenderTarget*)mDevice->getPrimaryRenderTarget()->getRootRenderTarget());
	if(renderTarget==NULL || renderTarget->CompileShaderSymbol==NULL){
		return false;
	}

	D3DXMACRO defines[1]={0};

	HRESULT result=renderTarget->CompileShaderSymbol(mCode,mCode.length(),defines,NULL,function,targetProfile,0,&mBytecode,&mLog,&mConstantTable);
	if(FAILED(result)){
		Log::alert(String("Code:")+mCode);
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
	if(FAILED(result)){
		Error::unknown(Categories::TOADLET_PEEPER,"unable to create shader");
		return false;
	}

	return reflect();
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

	if(mConstantTable!=NULL){
		mConstantTable->Release();
		mConstantTable=NULL;
	}

	return SUCCEEDED(result);
}

bool D3D9Shader::reflect(){
	D3DXCONSTANTTABLE_DESC tableDesc;
	HRESULT result=mConstantTable->GetDesc(&tableDesc);
	if(FAILED(result)){
		Error::unknown(Categories::TOADLET_PEEPER,"unable to get constant table description");
		return false;
	}

	VariableBufferFormat::ptr primaryFormat(new VariableBufferFormat(true,(char*)NULL,0,tableDesc.Constants));

	int dataSize=0;
	int i;
	for(i=0;i<tableDesc.Constants;++i){
		D3DXHANDLE handle=mConstantTable->GetConstant(NULL,i);
		D3DXCONSTANT_DESC constantDesc;
        unsigned int params=1;
        result=mConstantTable->GetConstantDesc(handle,&constantDesc,&params);

		VariableBufferFormat::Variable::ptr variable(new VariableBufferFormat::Variable());
		String name=constantDesc.Name;
		// Convert samp+tex name into just tex
		int plus=name.find('+');
		if(plus>=0){
			name=name.substr(plus+1,name.length());
		}
		variable->setName(name);
		int format=D3D9RenderDevice::getVariableFormat(constantDesc);
		variable->setFormat(format);
		variable->setOffset(dataSize);
		variable->setSize(constantDesc.Bytes);
		variable->setIndex(constantDesc.RegisterIndex);
		variable->setResourceIndex(constantDesc.RegisterIndex);
		primaryFormat->setStructVariable(i,variable);

		dataSize+=constantDesc.Bytes;
	}

	primaryFormat->setDataSize(dataSize);

	primaryFormat->compile();
	mVariableBufferFormats.add(primaryFormat);

	return true;
}

}
}
