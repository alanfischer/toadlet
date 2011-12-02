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
#include <toadlet/peeper/BackableRenderState.h>
#include <toadlet/peeper/BackableShaderState.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/tadpole/Colors.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/creator/DiffuseMaterialCreator.h>
#include <toadlet/tadpole/creator/SkyBoxMaterialCreator.h>

namespace toadlet{
namespace tadpole{

MaterialManager::MaterialManager(Engine *engine,bool backable):ResourceManager(engine->getArchiveManager()),
	mEngine(NULL),
	mBackable(false),

	//mRenderStates,
	//mShaderStates,

	mRenderPathChooser(NULL)
{
	mEngine=engine;
	mBackable=backable;
}

void MaterialManager::destroy(){
	ResourceManager::destroy();

	if(mDiffuseCreator!=NULL){
		mDiffuseCreator->destroy();
		mDiffuseCreator=NULL;
	}

	int i;
	for(i=0;i<mRenderStates.size();++i){
		RenderState::ptr renderState=mRenderStates[i];
		renderState->setRenderStateDestroyedListener(NULL);
		renderState->destroy();
	}
	mRenderStates.clear();
}

/// @todo: Should these two just be merged?
Material::ptr MaterialManager::createMaterial(Material::ptr source){
	Material::ptr material(new Material(this,source,false));
	material->compile();
	manage(material);
	return material;
}

Material::ptr MaterialManager::cloneMaterial(Material::ptr source){
	Material::ptr material(new Material(this,source,false));
	material->compile();
	manage(material);
	return material;
}

Material::ptr MaterialManager::createDiffuseMaterial(Texture::ptr texture){
	return shared_static_cast<DiffuseMaterialCreator>(mDiffuseCreator)->createDiffuseMaterial(texture);
}

Material::ptr MaterialManager::createPointSpriteMaterial(Texture::ptr texture,scalar size,bool attenuated){
	return shared_static_cast<DiffuseMaterialCreator>(mDiffuseCreator)->createPointSpriteMaterial(texture,size,attenuated);
}

Material::ptr MaterialManager::createSkyBoxMaterial(Texture::ptr texture){
	return shared_static_cast<SkyBoxMaterialCreator>(mSkyBoxCreator)->createSkyBoxMaterial(texture);
}

RenderState::ptr MaterialManager::createRenderState(){
	RenderState::ptr renderState;

	if(mBackable || mEngine->getRenderDevice()==NULL){
		Logger::debug(Categories::TOADLET_TADPOLE,"creating BackableRenderState");

		BackableRenderState::ptr backableRenderState(new BackableRenderState());
		backableRenderState->create();
		if(mEngine->getRenderDevice()!=NULL){
			RenderState::ptr back(mEngine->getRenderDevice()->createRenderState());
			backableRenderState->setBack(back);
		}
		renderState=backableRenderState;
	}
	else{
		Logger::debug(Categories::TOADLET_TADPOLE,"creating RenderState");

		renderState=RenderState::ptr(mEngine->getRenderDevice()->createRenderState());
		if(renderState==NULL || renderState->create()==false){
			return NULL;
		}
	}

	renderState->setRenderStateDestroyedListener(this);
	mRenderStates.add(renderState);

	return renderState;
}

ShaderState::ptr MaterialManager::createShaderState(){
	ShaderState::ptr shaderState;

	if(mEngine->hasShader(Shader::ShaderType_VERTEX)==false){
		return shaderState;
	}

	if(mBackable || mEngine->getRenderDevice()==NULL){
		Logger::debug(Categories::TOADLET_TADPOLE,"creating BackableShaderState");

		BackableShaderState::ptr backableShaderState(new BackableShaderState());
		backableShaderState->create();
		if(mEngine->getRenderDevice()!=NULL){
			ShaderState::ptr back;
			TOADLET_TRY
				back=ShaderState::ptr(mEngine->getRenderDevice()->createShaderState());
			TOADLET_CATCH(const Exception &){}
			backableShaderState->setBack(back);
		}
		shaderState=backableShaderState;
	}
	else{
		Logger::debug(Categories::TOADLET_TADPOLE,"creating ShaderState");

		TOADLET_TRY
			shaderState=ShaderState::ptr(mEngine->getRenderDevice()->createShaderState());
		TOADLET_CATCH(const Exception &){}
		if(shaderState==NULL || shaderState->create()==false){
			return NULL;
		}
	}

	shaderState->setShaderStateDestroyedListener(this);
	mShaderStates.add(shaderState);

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

	PointState pointState;
	if(src->getPointState(pointState)) dst->setPointState(pointState);

	MaterialState materialState;
	if(src->getMaterialState(materialState)) dst->setMaterialState(materialState);

	int i;
	for(i=0;i<src->getNumSamplerStates();++i){
		SamplerState samplerState;
		if(src->getSamplerState(i,samplerState)) dst->setSamplerState(i,samplerState);
	}
	for(i=0;i<src->getNumTextureStates();++i){
		TextureState textureState;
		if(src->getTextureState(i,textureState)) dst->setTextureState(i,textureState);
	}
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
	int i;
	for(i=0;i<mRenderStates.size();++i){
		RenderState::ptr renderState=mRenderStates[i];
		if(renderState!=NULL && renderState->getRootRenderState()!=renderState){
			RenderState::ptr back;
			TOADLET_TRY
				back=RenderState::ptr(renderDevice->createRenderState());
			TOADLET_CATCH(const Exception &){}
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
				back=ShaderState::ptr(renderDevice->createShaderState());
			TOADLET_CATCH(const Exception &){}
			if(back!=NULL){
				shared_static_cast<BackableShaderState>(shaderState)->setBack(back);
			}
		}
	}

	for(i=0;i<mResources.size();++i){
		Material *material=(Material*)mResources.at(i).get();
		if(material!=NULL){
			compileMaterial(material);
		}
	}
}

void MaterialManager::contextDeactivate(RenderDevice *renderDevice){
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
}

void MaterialManager::renderStateDestroyed(RenderState *renderState){
	mRenderStates.remove(renderState);
}

void MaterialManager::shaderStateDestroyed(ShaderState *shaderState){
	mShaderStates.remove(shaderState);
}

Resource::ptr MaterialManager::unableToFindStreamer(const String &name,ResourceData *data){
	Texture::ptr texture=mEngine->getTextureManager()->findTexture(name);
	if(texture!=NULL){
		return createDiffuseMaterial(texture);
	}
	else{
		return ResourceManager::unableToFindStreamer(name,data);
	}
}

bool MaterialManager::isPathUseable(RenderPath *path,const RenderCaps &caps){
	int i,j;
	for(i=0;i<path->getNumPasses();++i){
		RenderPass *pass=path->getPass(i);
		ShaderState *state=pass->getShaderState();

		if(state!=NULL){
			for(j=0;j<Shader::ShaderType_MAX;++j){
				if(state->getShader((Shader::ShaderType)j)!=NULL && caps.hasShader[j]==false){
					break;
				}
			}

			if(j!=Shader::ShaderType_MAX){
				break;
			}
		}
	}

	return i==path->getNumPasses();
}

bool MaterialManager::compileMaterial(Material *material){
	RenderPath::ptr bestPath;

	if(mRenderPathChooser!=NULL){
		bestPath=mRenderPathChooser->chooseBestPath(material);
	}
	else{
		int i;
		for(i=0;i<material->getNumPaths();++i){
			RenderPath::ptr path=material->getPath(i);
			if(isPathUseable(path,mEngine->getRenderCaps())){
				bestPath=path;
				break;
			}
		}
	}

	material->setBestPath(bestPath);
	return bestPath!=NULL;
}

BufferManager *MaterialManager::getBufferManager(){
	return mEngine->getBufferManager();
}

}
}
