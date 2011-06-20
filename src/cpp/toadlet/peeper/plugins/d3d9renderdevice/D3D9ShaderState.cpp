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
#include "D3D9Shader.h"
 
namespace toadlet{
namespace peeper{

class D3D9RenderDevice;

D3D9ShaderState::D3D9ShaderState(D3D9RenderDevice *renderDevice):
	mDevice(NULL),
	
	mListener(NULL)
	//mShaders
{
	mDevice=renderDevice;
}

D3D9ShaderState::~D3D9ShaderState(){
	destroy();
}

void D3D9ShaderState::destroy(){
	mShaders.clear();
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

bool D3D9ShaderState::activate(){
	bool result=true;

	int i;
	for(i=0;i<mShaders.size();++i){
		Shader *shader=mShaders[i];
		D3D9Shader *d3d9shader=(shader!=NULL)?(D3D9Shader*)shader->getRootShader():NULL;
		result|=d3d9shader->activate();
	}

	return result;
}

}
}
