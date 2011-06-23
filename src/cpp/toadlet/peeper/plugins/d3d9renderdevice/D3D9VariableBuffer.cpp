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

#include "D3D9VariableBuffer.h"
#include "D3D9RenderDevice.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/peeper/BackableBuffer.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D9VariableBuffer::D3D9VariableBuffer(D3D9RenderDevice *renderDevice):
	mDevice(NULL),

	mListener(NULL),
	mUsage(0),
	mAccess(0),
	//mVariableFormat,
	mDataSize(0),
	mHasTranspose(false),

	mData(NULL)
{
	mDevice=renderDevice;
}

D3D9VariableBuffer::~D3D9VariableBuffer(){
	destroy();
}

bool D3D9VariableBuffer::create(int usage,int access,VariableBufferFormat::ptr variableFormat){
	mUsage=usage;
	mAccess=access;
	mVariableFormat=variableFormat;
	mDataSize=variableFormat->getDataSize();

	mData=new tbyte[mDataSize];

	mHasTranspose=false;
	int i;
	for(i=0;i<mVariableFormat->variableFormats.size();++i){
		mHasTranspose|=((mVariableFormat->variableFormats[i]&VariableBufferFormat::Format_BIT_TRANSPOSE)!=0);
	}

	return true;
}

void D3D9VariableBuffer::destroy(){
	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}

	if(mListener!=NULL){
		mListener->bufferDestroyed(this);
		mListener=NULL;
	}
}

tbyte *D3D9VariableBuffer::lock(int lockAccess){
	if(mVariableFormat!=NULL && mHasTranspose){
		BackableBuffer::transposeVariables(mVariableFormat,mData);
	}

	return mData;
}

bool D3D9VariableBuffer::unlock(){
	if(mVariableFormat!=NULL && mHasTranspose){
		BackableBuffer::transposeVariables(mVariableFormat,mData);
	}

	return true;
}

bool D3D9VariableBuffer::update(tbyte *data,int start,int size){
	memcpy(mData+start,data,size);

	if(mHasTranspose){
		int i;
		for(i=0;i<mVariableFormat->variableOffsets.size();++i){
			int offset=mVariableFormat->variableOffsets[i];
			if(offset>=start && offset<start+size){
				BackableBuffer::transposeVariable(mVariableFormat,mData,i);
			}
		}
	}

	return true;
}

void D3D9VariableBuffer::activateConstants(Shader::ShaderType type){
	IDirect3DDevice9 *device=mDevice->getDirect3DDevice9();

	int i;
	for(i=0;i<mVariableFormat->variableNames.size();++i){
		int format=mVariableFormat->variableFormats[i];
		int index=mVariableFormat->variableIndexes[i];
		int offset=mVariableFormat->variableOffsets[i];
		int count=0;
		switch(format&VariableBufferFormat::Format_MASK_COUNTS){
			case VariableBufferFormat::Format_COUNT_2X4:
			case VariableBufferFormat::Format_COUNT_4X2:
				count=2;
			break;
			case VariableBufferFormat::Format_COUNT_3X4:
			case VariableBufferFormat::Format_COUNT_4X3:
				count=3;
			break;
			case VariableBufferFormat::Format_COUNT_4X4:
				count=4;
			break;
			default:
				count=1;
		}

		switch(format&VariableBufferFormat::Format_MASK_TYPES){
			case VariableBufferFormat::Format_TYPE_UINT_8:
				if(type==Shader::ShaderType_VERTEX){
					device->SetVertexShaderConstantB(index,(BOOL*)(mData+offset),count);
				}
				else if(type==Shader::ShaderType_FRAGMENT){
					device->SetPixelShaderConstantB(index,(BOOL*)(mData+offset),count);
				}
			break;
			case VariableBufferFormat::Format_TYPE_INT_32:
				if(type==Shader::ShaderType_VERTEX){
					device->SetVertexShaderConstantI(index,(int*)(mData+offset),count);
				}
				else if(type==Shader::ShaderType_FRAGMENT){
					device->SetPixelShaderConstantI(index,(int*)(mData+offset),count);
				}
			break;
			case VariableBufferFormat::Format_TYPE_FLOAT_32:
				if(type==Shader::ShaderType_VERTEX){
					device->SetVertexShaderConstantF(index,(float*)(mData+offset),count);
				}
				else if(type==Shader::ShaderType_FRAGMENT){
					device->SetPixelShaderConstantF(index,(float*)(mData+offset),count);
				}
			break;
		}
	}
}

}
}
