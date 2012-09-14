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

#ifndef TOADLET_PEEPER_BACKABLESHADERSTATE_H
#define TOADLET_PEEPER_BACKABLESHADERSTATE_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/ShaderState.h>

namespace toadlet{
namespace peeper{

class TOADLET_API BackableShaderState:public BaseResource,public ShaderState{
public:
	TOADLET_RESOURCE(BackableShaderState,ShaderState);

	BackableShaderState();

	virtual ShaderState *getRootShaderState(){return mBack;}

	virtual bool create(){return true;}
	virtual void destroy();

	virtual void setShader(Shader::ShaderType type,Shader::ptr shader);
	virtual Shader::ptr getShader(Shader::ShaderType type);
	
	virtual int getNumVariableBuffers(Shader::ShaderType type){return mBack!=NULL?mBack->getNumVariableBuffers(type):0;}
	virtual VariableBufferFormat::ptr getVariableBufferFormat(Shader::ShaderType type,int i){return mBack!=NULL?mBack->getVariableBufferFormat(type,i):NULL;}

	virtual void setBack(ShaderState::ptr back);
	virtual ShaderState::ptr getBack(){return mBack;}

protected:
	Collection<Shader::ptr> mShaders;

	ShaderState::ptr mBack;
};

}
}

#endif
