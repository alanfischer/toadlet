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

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/ShaderManager.h>
#include <toadlet/peeper/BackableShader.h>

namespace toadlet{
namespace tadpole{

ShaderManager::ShaderManager(Engine *engine):ResourceManager(engine){
}

Shader::ptr ShaderManager::createShader(Shader::ShaderType type,const char *profiles[],const char *codes[],int numCodes){
	RenderDevice *renderDevice=mEngine->getRenderDevice();
	Shader::ptr shader;
#if defined(TOADLET_BACKABLE)
	if(mEngine->hasBackableShader(type)){
		BackableShader::ptr backableShader=new BackableShader();
		backableShader->create(type,profiles,codes,numCodes);
		if(renderDevice!=NULL && mEngine->hasShader(type)){
			Shader::ptr back;
			TOADLET_TRY
				back=renderDevice->createShader();
			TOADLET_CATCH_ANONYMOUS(){}
			if(back!=NULL){
				backableShader->setBack(back,renderDevice);
			}
		}
		shader=backableShader;
	}
	else
#endif
	if(renderDevice!=NULL && mEngine->hasShader(type)){
		TOADLET_TRY
			shader=renderDevice->createShader();
		TOADLET_CATCH_ANONYMOUS(){}
		if(BackableShader::convertCreate(shader,renderDevice,type,profiles,codes,numCodes)==false){
			// I'm not sure if this should be an Error or not.  Sometimes it's ok for createShader to just return NULL, if there are no supported profiles?
			//Error::unknown(Categories::TOADLET_TADPOLE,"Error in shader convertCreate");
			return NULL;
		}
	}

	if(shader!=NULL){
		manage(static_pointer_cast<Shader>(shader));
	}

	return shader;
}

Shader::ptr ShaderManager::createShader(Shader::ShaderType type,const char *profile,const char *code){
	return createShader(type,&profile,&code,1);
}

void ShaderManager::contextActivate(RenderDevice *renderDevice){
	Log::debug("ShaderManager::contextActivate");

#if defined(TOADLET_BACKABLE)
	int i;
	for(i=0;i<mResources.size();++i){
		Shader *shader=(Shader*)mResources[i];
		if(shader!=NULL && shader->getRootShader()!=shader){
			Shader::ptr back;
			TOADLET_TRY
				back=renderDevice->createShader();
			TOADLET_CATCH_ANONYMOUS(){}
			if(back!=NULL){
				((BackableShader*)shader)->setBack(back,renderDevice);
			}
		}
	}
#endif
}

void ShaderManager::contextDeactivate(RenderDevice *renderDevice){
	Log::debug("ShaderManager::contextDeactivate");

#if defined(TOADLET_BACKABLE)
	int i;
	for(i=0;i<mResources.size();++i){
		Shader *shader=(Shader*)mResources[i];
		if(shader!=NULL && shader->getRootShader()!=shader){
			((BackableShader*)shader)->setBack(NULL,NULL);
		}
	}
#endif
}

}
}

