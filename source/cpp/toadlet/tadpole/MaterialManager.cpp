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

#include <toadlet/peeper/Texture.h>
#include <toadlet/tadpole/Colors.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/MaterialManager.h>
#if defined(TOADLET_BACKABLE)
#include <toadlet/peeper/BackableRenderState.h>
#include <toadlet/peeper/BackableShaderState.h>
#endif

namespace toadlet{
namespace tadpole{

MaterialManager::MaterialManager(Engine *engine):ResourceManager(engine),
	//mRenderStates,
	//mShaderStates,

	mRenderPathChooser(NULL)
{
}

void MaterialManager::destroy(){
	ResourceManager::destroy();

	int i;
	for(i=0;i<mRenderStates.size();++i){
		RenderState::ptr renderState=mRenderStates[i];
		renderState->setDestroyedListener(NULL);
		renderState->destroy();
	}
	mRenderStates.clear();

	for(i=0;i<mShaderStates.size();++i){
		ShaderState::ptr shaderState=mShaderStates[i];
		shaderState->setDestroyedListener(NULL);
		shaderState->destroy();
	}
	mShaderStates.clear();
}

Material::ptr MaterialManager::createMaterial(){
	Material::ptr material=new Material(this);
	material->compile();
	manage(material);
	return material;
}

Material::ptr MaterialManager::createSharedMaterial(Material::ptr source,RenderState::ptr renderState){
	Material::ptr material=new Material(this);

	tforeach(Material::PathCollection::iterator,srcPath,source->getPaths()){
		RenderPath *dstPath=material->addPath();
		tforeach(RenderPath::PassCollection::iterator,srcPass,srcPath->getPasses()){
			RenderPass *dstPass=new RenderPass(this,srcPass);

			// We need to copy the Texture & Sampler states, since the shared RenderState has no knowledge of Texture or Sampler states by default
			RenderState *srcState=srcPass->getRenderState();
			if(srcState!=NULL){
				for(int t=0;t<Shader::ShaderType_MAX;++t){
					for(int s=0;s<srcState->getNumTextureStates((Shader::ShaderType)t);++s){
						TextureState state;
						srcState->getTextureState((Shader::ShaderType)t,s,state);
						renderState->setTextureState((Shader::ShaderType)t,s,state);
					}
					for(int s=0;s<srcState->getNumSamplerStates((Shader::ShaderType)t);++s){
						SamplerState state;
						srcState->getSamplerState((Shader::ShaderType)t,s,state);
						renderState->setSamplerState((Shader::ShaderType)t,s,state);
					}
				}
			}

			dstPass->setRenderState(renderState);

			dstPath->addPass(dstPass);
		}
	}
	
	material->setName(source->getName());
	material->setSort(source->getSort());
	material->setLayer(source->getLayer());
	material->compile();
	manage(material);

	return material;
}

RenderState::ptr MaterialManager::createRenderState(){
	RenderDevice *renderDevice=mEngine->getRenderDevice();
	RenderState::ptr renderState;
#if defined(TOADLET_BACKABLE)
	if(mEngine->isBackable()){
		BackableRenderState::ptr backableRenderState=new BackableRenderState();
		backableRenderState->create();
		if(renderDevice!=NULL){
			RenderState::ptr back=renderDevice->createRenderState();
			backableRenderState->setBack(back);
		}
		renderState=backableRenderState;
	}
	else
#endif
	if(renderDevice!=NULL){
		renderState=renderDevice->createRenderState();
		if(renderState==NULL || renderState->create()==false){
			return NULL;
		}
	}

	if(renderState!=NULL){
		mRenderStates.add(renderState);

		renderState->setDestroyedListener(this);
	}

	return renderState;
}

ShaderState::ptr MaterialManager::createShaderState(){
	RenderDevice *renderDevice=mEngine->getRenderDevice();
	ShaderState::ptr shaderState;
#if defined(TOADLET_BACKABLE)
	if(mEngine->hasBackableShader(Shader::ShaderType_VERTEX)){
		BackableShaderState::ptr backableShaderState=new BackableShaderState();
		backableShaderState->create();
		if(renderDevice!=NULL){
			ShaderState::ptr back;
			TOADLET_TRY
				back=renderDevice->createShaderState();
			TOADLET_CATCH_ANONYMOUS(){}
			backableShaderState->setBack(back);
		}
		shaderState=backableShaderState;
	}
	else
#endif
	if(renderDevice!=NULL){
		TOADLET_TRY
			shaderState=renderDevice->createShaderState();
		TOADLET_CATCH_ANONYMOUS(){}
		if(shaderState==NULL || shaderState->create()==false){
			return NULL;
		}
	}

	if(shaderState!=NULL){
		mShaderStates.add(shaderState);

		shaderState->setDestroyedListener(this);
	}

	return shaderState;
}

void MaterialManager::modifyRenderState(RenderState::ptr dst,RenderState::ptr src){
	if(src==NULL || dst==NULL){
		return;
	}

	BlendState blendState;
	if(src->getBlendState(blendState)) dst->setBlendState(blendState);

	DepthState depthState;
	if(src->getDepthState(depthState)) dst->setDepthState(depthState);

	RasterizerState rasterizerState;
	if(src->getRasterizerState(rasterizerState)) dst->setRasterizerState(rasterizerState);

	FogState fogState;
	if(src->getFogState(fogState)) dst->setFogState(fogState);

	GeometryState geometryState;
	if(src->getGeometryState(geometryState)) dst->setGeometryState(geometryState);

	MaterialState materialState;
	if(src->getMaterialState(materialState)) dst->setMaterialState(materialState);
}

void MaterialManager::modifyShaderState(ShaderState::ptr dst,ShaderState::ptr src){
	if(src==NULL || dst==NULL){
		return;
	}

	int i;
	for(i=0;i<Shader::ShaderType_MAX;++i){
		Shader::ptr shader=src->getShader((Shader::ShaderType)i);
		dst->setShader((Shader::ShaderType)i,shader);
	}
}

void MaterialManager::setRenderPathChooser(RenderPathChooser *chooser){
	mRenderPathChooser=chooser;
	/// @todo: Recompile all materials
}

void MaterialManager::contextActivate(RenderDevice *renderDevice){
	Log::debug("MaterialManager::contextActivate");

	int i;
#if defined(TOADLET_BACKABLE)
	for(i=0;i<mRenderStates.size();++i){
		RenderState::ptr renderState=mRenderStates[i];
		if(renderState!=NULL && renderState->getRootRenderState()!=renderState){
			RenderState::ptr back;
			TOADLET_TRY
				back=renderDevice->createRenderState();
			TOADLET_CATCH_ANONYMOUS(){}
			if(back!=NULL){
				shared_static_cast<BackableRenderState>(renderState)->setBack(back);
			}
		}
	}

	for(i=0;i<mShaderStates.size();++i){
		ShaderState::ptr shaderState=mShaderStates[i];
		if(shaderState!=NULL && shaderState->getRootShaderState()!=shaderState){
			ShaderState::ptr back;
			TOADLET_TRY
				back=renderDevice->createShaderState();
			TOADLET_CATCH_ANONYMOUS(){}
			if(back!=NULL){
				shared_static_cast<BackableShaderState>(shaderState)->setBack(back);
			}
		}
	}
#endif

	for(i=0;i<mResources.size();++i){
		Material *material=(Material*)mResources[i];
		if(material!=NULL){
			compileMaterial(material);
		}
	}
}

void MaterialManager::contextDeactivate(RenderDevice *renderDevice){
	Log::debug("MaterialManager::contextDeactivate");

#if defined(TOADLET_BACKABLE)
	int i;
	for(i=0;i<mRenderStates.size();++i){
		RenderState::ptr renderState=mRenderStates[i];
		if(renderState!=NULL && renderState->getRootRenderState()!=renderState){
			shared_static_cast<BackableRenderState>(renderState)->setBack(NULL);
		}
	}

	for(i=0;i<mShaderStates.size();++i){
		ShaderState::ptr shaderState=mShaderStates[i];
		if(shaderState!=NULL && shaderState->getRootShaderState()!=shaderState){
			shared_static_cast<BackableShaderState>(shaderState)->setBack(NULL);
		}
	}
#endif
}

void MaterialManager::resourceDestroyed(Resource *resource){
	if(mRenderStates.remove(resource)==false){
		if(mShaderStates.remove(resource)==false){
			ResourceManager::resourceDestroyed(resource);
		}
	}
}

bool MaterialManager::isPathUseable(RenderPath *path,const RenderCaps &caps){
	tforeach(RenderPath::PassCollection::iterator,pass,path->getPasses()){
		ShaderState *state=pass->getShaderState();

		int j;
		for(j=0;j<Shader::ShaderType_MAX;++j){
			if(state!=NULL){
				/// @todo: I believe this wont currently detect a shader path that didn't load properly.  so FIX IT
				Shader *shader=state->getShader((Shader::ShaderType)j);
				if(shader!=NULL && (shader->getRootShader()==NULL || caps.hasShader[j]==false)){
					break;
				}
			}
			else if(caps.hasFixed[j]==false){
				break;
			}

			if(pass->getNumTextures((Shader::ShaderType)j)>caps.maxTextureStages){
				break;
			}
		}

		if(j!=Shader::ShaderType_MAX){
			return false;
		}
	}

	return true;
}

bool MaterialManager::compileMaterial(Material *material){
	RenderPath::ptr bestPath;
	if(mRenderPathChooser!=NULL){
		bestPath=mRenderPathChooser->chooseBestPath(material);
	}
	else if(mEngine->getRenderDevice()!=NULL){
		RenderCaps caps;
		mEngine->getRenderDevice()->getRenderCaps(caps);
		tforeach(Material::PathCollection::iterator,path,material->getPaths()){
			if(isPathUseable(path,caps)){
				bestPath=path;
				break;
			}
		}
	}

	material->setBestPath(bestPath);

	if(bestPath!=NULL){
		bestPath->compile();
	}

	return bestPath!=NULL;
}

BufferManager *MaterialManager::getBufferManager(){
	return mEngine->getBufferManager();
}

}
}

