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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/ShaderManager.h>

namespace toadlet{
namespace tadpole{

ShaderManager::ShaderManager(Engine *engine,bool backable):ResourceManager(engine->getArchiveManager()),
	mEngine(NULL),
	mBackable(false)
{
	mEngine=engine;
	mBackable=backable;
}

Shader::ptr ShaderManager::createShader(Shader::ShaderType type,const String profiles[],const String codes[],int numCodes){
	RenderDevice *renderDevice=mEngine->getRenderDevice();

	int i;
	for(i=0;i<numCodes;++i){
		if(renderDevice->getShaderProfileSupported(profiles[i])){
			Shader::ptr shader=createShader(type,profiles[i],codes[i]);
			if(shader!=NULL){
				return shader;
			}
		}
	}

	return NULL;
}

Shader::ptr ShaderManager::createShader(Shader::ShaderType type,const String &profile,const String &code){
	Logger::debug(Categories::TOADLET_TADPOLE,"ShaderManager::createShader");

	if(code.length()==0){
		return NULL;
	}

	Shader::ptr shader=Shader::ptr(mEngine->getRenderDevice()->createShader());
	if(shader==NULL || shader->create(type,profile,code)==false){
		return NULL;
	}

	manage(shader);

	return shader;
}

}
}

