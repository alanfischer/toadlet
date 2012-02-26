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

#include "D3D10ShaderState.h"
#include "D3D10RenderDevice.h"
#include "D3D10Shader.h"

namespace toadlet{
namespace peeper{

D3D10ShaderState::D3D10ShaderState(D3D10RenderDevice *renderDevice):
	mDevice(NULL)
	//mShaders
{
	mDevice=renderDevice;
	mShaders.resize(Shader::ShaderType_MAX);
}

D3D10ShaderState::~D3D10ShaderState(){
	destroy();
}

void D3D10ShaderState::destroy(){
	mShaders.clear();

	BaseResource::destroy();
}

void D3D10ShaderState::setShader(Shader::ShaderType type,Shader::ptr shader){
	if(mShaders.size()<=type){
		mShaders.resize(type+1);
	}

	mShaders[type]=shader;
}

Shader::ptr D3D10ShaderState::getShader(Shader::ShaderType type){
	if(mShaders.size()<=type){
		return NULL;
	}

	return mShaders[type];
}

int D3D10ShaderState::getNumVariableBuffers(Shader::ShaderType type){
	if(mShaders.size()<=type || mShaders[type]==NULL){
		return 0;
	}

	D3D10Shader *d3dshader=(D3D10Shader*)mShaders[type]->getRootShader();
	return d3dshader->getNumVariableBuffers();
}

VariableBufferFormat::ptr D3D10ShaderState::getVariableBufferFormat(Shader::ShaderType type,int i){
	if(mShaders.size()<=type || mShaders[type]==NULL){
		return 0;
	}

	D3D10Shader *d3dshader=(D3D10Shader*)mShaders[type]->getRootShader();
	return d3dshader->getVariableBufferFormat(i);
}

bool D3D10ShaderState::activate(){
	bool result=true;

	ID3D10Device *device=mDevice->getD3D10Device();

	int i;
	for(i=0;i<mShaders.size();++i){
		Shader *shader=mShaders[i];
		D3D10Shader *d3dshader=(shader!=NULL)?(D3D10Shader*)shader->getRootShader():NULL;
		ID3D10DeviceChild *id3dshader=(d3dshader!=NULL)?d3dshader->mShader:NULL;
		switch(i){
			case Shader::ShaderType_VERTEX:
				device->VSSetShader((ID3D10VertexShader*)id3dshader);
			break;
			case Shader::ShaderType_FRAGMENT:
				device->PSSetShader((ID3D10PixelShader*)id3dshader);
			break;
			case Shader::ShaderType_GEOMETRY:
				device->GSSetShader((ID3D10GeometryShader*)id3dshader);
			break;
		}
	}

	return result;
}

}
}
