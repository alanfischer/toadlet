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
		default:
			return false;
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
	if(handle<mLayouts.size() && mLayouts[handle]!=NULL){
		layout=mLayouts[handle];
	}
	else{
		Collection<D3D10_SIGNATURE_PARAMETER_DESC> missingParameters;
		int i,j;
		for(i=0;i<mInputParameters.size();++i){
			const D3D10_SIGNATURE_PARAMETER_DESC &parameterDesc=mInputParameters[i];
			for(j=0;j<vertexFormat->mElements.size();++j){
				const D3D10_INPUT_ELEMENT_DESC &elementDesc=vertexFormat->mElements[j];
				if(strcmp(elementDesc.SemanticName,parameterDesc.SemanticName)==0 && elementDesc.SemanticIndex==parameterDesc.SemanticIndex){
					break;
				}
			}
			if(j==vertexFormat->mElements.size()){
				missingParameters.add(parameterDesc);
			}
		}

		D3D10_INPUT_ELEMENT_DESC *elements=NULL;
		int numElements=0;
		if(missingParameters.size()==0){
			elements=vertexFormat->mElements;
			numElements=vertexFormat->mElements.size();
		}
		else{
			numElements=vertexFormat->mElements.size()+missingParameters.size();
			elements=new D3D10_INPUT_ELEMENT_DESC[numElements];
			for(i=0;i<vertexFormat->mElements.size();++i){
				elements[i]=vertexFormat->mElements[i];
			}
			for(i=0;i<missingParameters.size();++i){
				int elementFormat=0;
				switch(missingParameters[i].Mask){
					case 1:
						elementFormat|=VertexFormat::Format_COUNT_1;
					break;
					case 3:
						elementFormat|=VertexFormat::Format_COUNT_2;
					break;
					case 7:
						elementFormat|=VertexFormat::Format_COUNT_3;
					break;
					case 15:
						elementFormat|=VertexFormat::Format_COUNT_4;
					break;
				}
				switch(missingParameters[i].ComponentType){
					case D3D_REGISTER_COMPONENT_UINT32:
					case D3D_REGISTER_COMPONENT_SINT32:
						elementFormat|=VertexFormat::Format_TYPE_INT_32;
					break;
					case D3D_REGISTER_COMPONENT_FLOAT32:
						elementFormat|=VertexFormat::Format_TYPE_FLOAT_32;
					break;
				}
				DXGI_FORMAT dxgiFormat=D3D10RenderDevice::getVertexDXGI_FORMAT(elementFormat);
				D3D10_INPUT_ELEMENT_DESC element={
					missingParameters[i].SemanticName,missingParameters[i].SemanticIndex,dxgiFormat,0,0,D3D10_INPUT_PER_VERTEX_DATA,0
				};
				elements[vertexFormat->mElements.size()+i]=element;

				Logger::alert(String("D3D10Shader: adding missing vertex element:")+missingParameters[i].SemanticName);
			}
		}
		mDevice->getD3D10Device()->CreateInputLayout(elements,numElements,mBytecode->GetBufferPointer(),mBytecode->GetBufferSize(),&layout);
		if(elements!=vertexFormat->mElements){
			delete[] elements;
			elements=NULL;
		}
		if(mLayouts.size()<=handle){
			mLayouts.resize(handle+1,NULL);
		}
		mLayouts[handle]=layout;
	}
	return layout;
}

bool D3D10Shader::reflect(){
	D3D10_SHADER_DESC desc;
	HRESULT result=mReflector->GetDesc(&desc);
	if(FAILED(result)){
		Error::unknown(Categories::TOADLET_PEEPER,"unable to get reflection description");
	}

	int i,j;
	for(i=0;i<desc.InputParameters;++i){
		D3D10_SIGNATURE_PARAMETER_DESC parameterDesc;
		mReflector->GetInputParameterDesc(i,&parameterDesc);
		mInputParameters.add(parameterDesc);
	}

	mVariableBufferFormats.resize(desc.ConstantBuffers);

	int numResources=0;
	for(i=0;i<desc.BoundResources;++i){
		D3D10_SHADER_INPUT_BIND_DESC bindDesc;
		mReflector->GetResourceBindingDesc(i,&bindDesc);

		if(bindDesc.Type!=D3D10_SIT_CBUFFER){
			numResources++;
		}
	}

	VariableBufferFormat::ptr primaryFormat;
	for(i=0;i<desc.ConstantBuffers;++i){
		ID3D10ShaderReflectionConstantBuffer *buffer=mReflector->GetConstantBufferByIndex(i);
		D3D10_SHADER_BUFFER_DESC bufferDesc;
		buffer->GetDesc(&bufferDesc);

		int numVariables=bufferDesc.Variables;
		bool primary=(strcmp(bufferDesc.Name,"$Globals")==0);
		if(primary){
			numVariables+=numResources;
		}
		VariableBufferFormat::ptr format(new VariableBufferFormat(primary,bufferDesc.Name,bufferDesc.Size,numVariables));
		if(primary){
			primaryFormat=format;
		}

		for(j=0;j<bufferDesc.Variables;++j){
			ID3D10ShaderReflectionVariable *d3dvariable=buffer->GetVariableByIndex(j);
			D3D10_SHADER_VARIABLE_DESC variableDesc;
			d3dvariable->GetDesc(&variableDesc);
			ID3D10ShaderReflectionType *type=d3dvariable->GetType();
			D3D10_SHADER_TYPE_DESC typeDesc;
			type->GetDesc(&typeDesc);

			VariableBufferFormat::Variable::ptr variable(new VariableBufferFormat::Variable());
			variable->setName(variableDesc.Name);
			variable->setFormat(D3D10RenderDevice::getVariableFormat(typeDesc));
			variable->setOffset(variableDesc.StartOffset);
			variable->setSize(variableDesc.Size);
			variable->setIndex(j);
			format->setStructVariable(j,variable);
		}

		mVariableBufferFormats[i]=format;
	}

	if(primaryFormat==NULL && desc.BoundResources>0){
		primaryFormat=VariableBufferFormat::ptr(new VariableBufferFormat(true,"$Globals",0,desc.BoundResources));
	}

	for(i=0,j=0;i<desc.BoundResources;++i){
		D3D10_SHADER_INPUT_BIND_DESC bindDesc;
		mReflector->GetResourceBindingDesc(i,&bindDesc);

		if(bindDesc.Type!=D3D10_SIT_CBUFFER){
			VariableBufferFormat::Variable::ptr variable(new VariableBufferFormat::Variable());
			variable->setName(bindDesc.Name);
			variable->setFormat(VariableBufferFormat::Format_TYPE_RESOURCE);
			variable->setOffset(bindDesc.BindPoint);
			variable->setSize(bindDesc.BindCount);
			variable->setIndex(bindDesc.BindPoint);
			primaryFormat->setStructVariable(primaryFormat->getStructSize() - numResources + j,variable);
			j++;
		}
	}

	for(i=0;i<mVariableBufferFormats.size();++i){
		mVariableBufferFormats[i]->compile();
	}

	return true;
}

}
}
