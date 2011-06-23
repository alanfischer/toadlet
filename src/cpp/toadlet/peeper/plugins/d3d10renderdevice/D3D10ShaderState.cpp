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
	mDevice(NULL),

	mListener(NULL)
	//mShaders
{
	mDevice=renderDevice;
}

D3D10ShaderState::~D3D10ShaderState(){
	destroy();
}

void D3D10ShaderState::destroy(){
	mShaders.clear();

	if(mListener!=NULL){
		mListener->shaderStateDestroyed(this);
		mListener=NULL;
	}
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

	int i;
	for(i=0;i<mShaders.size();++i){
		Shader *shader=mShaders[i];
		D3D10Shader *d3d10shader=(shader!=NULL)?(D3D10Shader*)shader->getRootShader():NULL;
		result|=d3d10shader->activate();
	}

	return result;
}

}
}
