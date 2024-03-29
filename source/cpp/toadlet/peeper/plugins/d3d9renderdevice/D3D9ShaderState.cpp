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

#include "D3D9ShaderState.h"
#include "D3D9RenderDevice.h"
#include "D3D9Shader.h"
 
namespace toadlet{
namespace peeper{

class D3D9RenderDevice;

D3D9ShaderState::D3D9ShaderState(D3D9RenderDevice *renderDevice):
	mDevice(NULL)
	//mShaders
{
	mDevice=renderDevice;
}

void D3D9ShaderState::destroy(){
	mShaders.clear();

	BaseResource::destroy();
}

void D3D9ShaderState::setShader(Shader::ShaderType type,Shader::ptr shader){
	if(mShaders.size()<=type){
		mShaders.resize(type+1);
	}

	mShaders[type]=shader;
}

Shader::ptr D3D9ShaderState::getShader(Shader::ShaderType type){
	if(mShaders.size()<=type){
		return NULL;
	}

	return mShaders[type];
}

int D3D9ShaderState::getNumVariableBuffers(Shader::ShaderType type){
	if(mShaders.size()<=type){
		return 0;
	}

	D3D9Shader *d3dshader=mShaders[type]!=NULL?(D3D9Shader*)mShaders[type]->getRootShader():NULL;
	return d3dshader!=NULL?d3dshader->getNumVariableBuffers():0;
}

VariableBufferFormat::ptr D3D9ShaderState::getVariableBufferFormat(Shader::ShaderType type,int i){
	if(mShaders.size()<=type){
		return 0;
	}

	D3D9Shader *d3dshader=mShaders[type]!=NULL?(D3D9Shader*)mShaders[type]->getRootShader():NULL;
	return d3dshader!=NULL?d3dshader->getVariableBufferFormat(i):NULL;
}

bool D3D9ShaderState::activate(){
	bool result=true;

	IDirect3DDevice9 *device=mDevice->getDirect3DDevice9();

	int i;
	for(i=0;i<=Shader::ShaderType_FRAGMENT;++i){
		Shader *shader=i<mShaders.size()?mShaders[i]:NULL;
		D3D9Shader *d3dshader=(shader!=NULL)?(D3D9Shader*)shader->getRootShader():NULL;
		IUnknown *id3dshader=(d3dshader!=NULL)?d3dshader->mShader:NULL;
		switch(i){
			case Shader::ShaderType_VERTEX:
				device->SetVertexShader((IDirect3DVertexShader9*)id3dshader);
			break;
			case Shader::ShaderType_FRAGMENT:
				device->SetPixelShader((IDirect3DPixelShader9*)id3dshader);
			break;
		}
	}

	return result;
}

}
}
