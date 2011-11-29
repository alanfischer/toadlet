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

#include <toadlet/peeper/BackableShader.h>
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
	Shader::ptr shader;
	if(mBackable || renderDevice==NULL){
		BackableShader::ptr backableShader(new BackableShader());
		backableShader->create(type,profiles,codes,numCodes);
		if(renderDevice!=NULL){
			TOADLET_TRY
				backableShader->setBack(Shader::ptr(renderDevice->createShader()),renderDevice);
			TOADLET_CATCH(Exception &){}
		}
		shader=backableShader;
	}
	else{
		TOADLET_TRY
			shader=Shader::ptr(renderDevice->createShader());
		TOADLET_CATCH(Exception &){shader=NULL;}
		if(BackableShader::convertCreate(shader,renderDevice,type,profiles,codes,numCodes)==false){
			Logger::error(Categories::TOADLET_TADPOLE,"Error in shader convertCreate");
			return NULL;
		}
	}

	manage(shared_static_cast<Shader>(shader));

	Logger::debug(Categories::TOADLET_TADPOLE,"shader created");

	return shader;
}

Shader::ptr ShaderManager::createShader(Shader::ShaderType type,const String &profile,const String &code){
	return createShader(type,&profile,&code,1);
}

void ShaderManager::contextActivate(RenderDevice *renderDevice){
	int i;
	for(i=0;i<mResources.size();++i){
		Shader::ptr shader=shared_static_cast<Shader>(mResources[i]);
		if(shader!=NULL && shader->getRootShader()!=shader){
			Shader::ptr back(renderDevice->createShader());
			shared_static_cast<BackableShader>(shader)->setBack(back,renderDevice);
		}
	}
}

void ShaderManager::contextDeactivate(RenderDevice *renderDevice){
	int i;
	for(i=0;i<mResources.size();++i){
		Shader::ptr shader=shared_static_cast<Shader>(mResources[i]);
		if(shader!=NULL && shader->getRootShader()!=shader){
			shared_static_cast<BackableShader>(shader)->setBack(NULL,NULL);
		}
	}
}

}
}

