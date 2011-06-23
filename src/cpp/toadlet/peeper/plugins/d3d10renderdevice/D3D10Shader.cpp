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
#include "D3D10WindowRenderTarget.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D10Shader::D3D10Shader(D3D10RenderDevice *renderDevice):BaseResource(),
	mDevice(NULL),
	mD3DDevice(NULL),

	mShaderType((ShaderType)0),
	//mProfile,
	//mCode,

	mBytecode(NULL),mLog(NULL),
	mShader(NULL),
	mReflector(NULL)
{
	mDevice=renderDevice;
	mD3DDevice=mDevice->getD3D10Device();
}

D3D10Shader::~D3D10Shader(){
	if(mShader!=NULL){
		destroy();
	}
}

bool D3D10Shader::create(ShaderType shaderType,const String &profile,const String &code){
	mShaderType=shaderType;
	mProfile=profile;
	mCode=code;

	bool result=createContext();

	if(result && mDevice!=NULL){
		mDevice->shaderCreated(this);
	}

	return result;
}

void D3D10Shader::destroy(){
	destroyContext();

	if(mDevice!=NULL){
		mDevice->shaderDestroyed(this);
	}

	int i;
	for(i=0;i<mLayouts.size();++i){
		if(mLayouts[i]!=NULL){
			mLayouts[i]->Release();
			mLayouts[i]=NULL;
		}
	}
	mLayouts.clear();
}

bool D3D10Shader::createContext(){
	ID3D10Device *device=mD3DDevice;

	String function="main";
	String targetProfile;
	switch(mShaderType){
		case ShaderType_VERTEX:
			targetProfile="vs_4_0";
		break;
		case ShaderType_FRAGMENT:
			targetProfile="ps_4_0";
		break;
		case ShaderType_GEOMETRY:
			targetProfile="gs_4_0";
		break;
	}

	HRESULT result=((D3D10WindowRenderTarget*)mDevice->getPrimaryRenderTarget()->getRootRenderTarget())->CompileFromMemorySymbol(mCode,mCode.length(),NULL,NULL,NULL,function,targetProfile,0,0,NULL,&mBytecode,&mLog,NULL);
	if(FAILED(result)){
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
	if(FAILED(result)){
		Error::unknown(Categories::TOADLET_PEEPER,"unable to create shader");
		return false;
	}

	result=((D3D10WindowRenderTarget*)mDevice->getPrimaryRenderTarget()->getRootRenderTarget())->ReflectShaderSymbol(mBytecode->GetBufferPointer(),mBytecode->GetBufferSize(),&mReflector);
	if(FAILED(result)){
		Error::unknown(Categories::TOADLET_PEEPER,"unable to reflect shader");
		return false;
	}

	return reflect();
}

bool D3D10Shader::destroyContext(){
	HRESULT result=S_OK;

	if(mReflector!=NULL){
		result=mReflector->Release();
		mReflector=NULL;
	}

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

bool D3D10Shader::activate(){
	ID3D10Device *device=mD3DDevice;

	switch(mShaderType){
		case ShaderType_VERTEX:
			device->VSSetShader((ID3D10VertexShader*)mShader);
		break;
		case ShaderType_FRAGMENT:
			device->PSSetShader((ID3D10PixelShader*)mShader);
		break;
		case ShaderType_GEOMETRY:
			device->GSSetShader((ID3D10GeometryShader*)mShader);
		break;
	}

	return true;
}

bool D3D10Shader::reflect(){
	D3D10_SHADER_DESC desc;
	HRESULT result=mReflector->GetDesc(&desc);
	if(FAILED(result)){
		Error::unknown(Categories::TOADLET_PEEPER,"unable to get reflection description");
	}

	mVariableBufferFormats.resize(desc.ConstantBuffers);

	int i,j;
	for(i=0;i<desc.ConstantBuffers;++i){
		ID3D10ShaderReflectionConstantBuffer *buffer=mReflector->GetConstantBufferByIndex(i);
		D3D10_SHADER_BUFFER_DESC bufferDesc;
		buffer->GetDesc(&bufferDesc);

		VariableBufferFormat::ptr format(new VariableBufferFormat(bufferDesc.Name,bufferDesc.Size,bufferDesc.Variables));

		if(format->getName()=="$Globals"){
			format->default=true;
		}

		for(j=0;j<bufferDesc.Variables;++j){
			ID3D10ShaderReflectionVariable *variable=buffer->GetVariableByIndex(j);
			D3D10_SHADER_VARIABLE_DESC variableDesc;
			variable->GetDesc(&variableDesc);
			ID3D10ShaderReflectionType *type=variable->GetType();
			D3D10_SHADER_TYPE_DESC typeDesc;
			type->GetDesc(&typeDesc);

			format->variableNames[j]=variableDesc.Name;
			format->variableFormats[j]=D3D10RenderDevice::getVariableFormat(typeDesc) | VariableBufferFormat::Format_BIT_TRANSPOSE; // Only applies to the matrixes
			format->variableOffsets[j]=variableDesc.StartOffset;
			format->variableIndexes[j]=j;
		}

		mVariableBufferFormats[i]=format;
	}

	Logger::alert(String("Num Resources:")+desc.BoundResources);
	for(i=0;i<desc.BoundResources;++i){
		D3D10_SHADER_INPUT_BIND_DESC bindDesc;
		mReflector->GetResourceBindingDesc(i,&bindDesc);
		Logger::alert(String("Resource Name:")+bindDesc.Name+" :"+bindDesc.BindPoint);
	}

	return true;
}

}
}
