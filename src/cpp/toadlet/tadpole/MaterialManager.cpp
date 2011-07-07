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
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/material/RenderVariables.h>

namespace toadlet{
namespace tadpole{

MaterialManager::MaterialManager(Engine *engine,bool backable):ResourceManager(engine->getArchiveManager()),
	mEngine(NULL),
	mBackable(false)
{
	mEngine=engine;
	mBackable=backable;
}

void MaterialManager::destroy(){
	ResourceManager::destroy();

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
	Material::ptr material(new Material(this));

	RenderPath::ptr fixedPath=material->addPath();
	RenderPass::ptr fixedPass=fixedPath->addPass();
	{
		fixedPass->setBlendState(BlendState());
		fixedPass->setDepthState(DepthState());
		fixedPass->setRasterizerState(RasterizerState());
		fixedPass->setMaterialState(MaterialState(true,false,MaterialState::ShadeType_GOURAUD));

		if(texture!=NULL){
			SamplerState samplerState(mDefaultSamplerState);
			fixedPass->setSamplerState(0,samplerState);

			TextureState textureState;
			fixedPass->setTextureState(0,textureState);

			fixedPass->setTexture(0,texture);
		}
	}

	RenderPath::ptr shaderPath=material->addPath();
	RenderPass::ptr shaderPass=shaderPath->addPass();
	{
		shaderPass->setShader(Shader::ShaderType_VERTEX,mFixedVertexShader);
		shaderPass->setShader(Shader::ShaderType_FRAGMENT,mFixedFragmentShader);
		shaderPass->getVariables()->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
		shaderPass->getVariables()->addVariable("lightModelPosition",RenderVariable::ptr(new LightModelPositionVariable()),Material::Scope_RENDERABLE);
		shaderPass->getVariables()->addVariable("lightColor",RenderVariable::ptr(new LightDiffuseVariable()),Material::Scope_MATERIAL);
		shaderPass->getVariables()->addVariable("ambientColor",RenderVariable::ptr(new AmbientVariable()),Material::Scope_RENDERABLE);
		shaderPass->getVariables()->addVariable("materialDiffuseColor",RenderVariable::ptr(new MaterialDiffuseVariable()),Material::Scope_MATERIAL);
		shaderPass->getVariables()->addVariable("materialAmbientColor",RenderVariable::ptr(new MaterialAmbientVariable()),Material::Scope_MATERIAL);
	}

	manage(material);

	material->compile();

	return material;
}

Material::ptr MaterialManager::createSkyboxMaterial(Texture::ptr texture){
	Material::ptr material(new Material(this));

	RenderPath::ptr fixedPath=material->addPath();
	RenderPass::ptr fixedPass=fixedPath->addPass();
	fixedPass->setBlendState(BlendState());
	fixedPass->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	fixedPass->setRasterizerState(RasterizerState());
	fixedPass->setMaterialState(MaterialState(false));

	if(texture!=NULL){
		SamplerState samplerState(mDefaultSamplerState);
		samplerState.uAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
		samplerState.vAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
		samplerState.wAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
		fixedPass->setSamplerState(0,samplerState);

		TextureState textureState;
		fixedPass->setTextureState(0,textureState);

		fixedPass->setTexture(0,texture);
	}

	material->compile();

	manage(material);

	return material;
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
		if(renderState->create()==false){
			return NULL;
		}
	}

	renderState->setRenderStateDestroyedListener(this);
	mRenderStates.add(renderState);

	return renderState;
}

ShaderState::ptr MaterialManager::createShaderState(){
	ShaderState::ptr shaderState;

	if(mBackable || mEngine->getRenderDevice()==NULL){
		Logger::debug(Categories::TOADLET_TADPOLE,"creating BackableShaderState");

		BackableShaderState::ptr backableShaderState(new BackableShaderState());
		backableShaderState->create();
		if(mEngine->getRenderDevice()!=NULL){
			ShaderState::ptr back(mEngine->getRenderDevice()->createShaderState());
			backableShaderState->setBack(back);
		}
		shaderState=backableShaderState;
	}
	else{
		Logger::debug(Categories::TOADLET_TADPOLE,"creating ShaderState");

		shaderState=ShaderState::ptr(mEngine->getRenderDevice()->createShaderState());
		if(shaderState->create()==false){
			return NULL;
		}
	}

	shaderState->setShaderStateDestroyedListener(this);
	mShaderStates.add(shaderState);

	return shaderState;
}

void MaterialManager::modifyRenderState(RenderState::ptr dst,RenderState::ptr src){
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
	int i;
	for(i=0;i<Shader::ShaderType_MAX;++i){
		Shader::ptr shader=src->getShader((Shader::ShaderType)i);
		dst->setShader((Shader::ShaderType)i,shader);
	}
}

void MaterialManager::contextActivate(RenderDevice *renderDevice){
	String fixedProfiles[]={
		"glsl",
		"hlsl"
	};

	String fixedVertexCode[]={
		"#version 150\n" \
		"in vec4 POSITION;\n" \
		"in vec3 NORMAL;\n" \
		"out vec4 color;\n" \

		"uniform mat4 modelViewProjectionMatrix;\n" \
		"uniform vec4 materialDiffuseColor;\n" \
		"uniform vec4 materialAmbientColor;\n" \
		"uniform vec4 lightModelPosition;\n" \
		"uniform vec4 lightColor;\n" \
		"uniform vec4 ambientColor;\n" \

		"void main(){\n" \
			"gl_Position=modelViewProjectionMatrix * POSITION;\n" \
			"color=-dot(lightModelPosition,vec4(NORMAL,1.0))*lightColor*materialDiffuseColor + ambientColor*materialAmbientColor;\n" \
		"}",

		"struct VIN{\n" \
			"float4 position : POSITION;\n" \
			"float3 normal : NORMAL;\n" \
		"};\n" \
		"struct VOUT{\n" \
			"float4 position : SV_POSITION;\n" \
			"float4 color : COLOR;\n" \
		"};\n" \

		"float4x4 modelViewProjectionMatrix;\n" \
		"float4 materialDiffuseColor;\n" \
		"float4 materialAmbientColor;\n" \
		"float4 lightModelPosition;\n" \
		"float4 lightColor;\n" \
		"float4 ambientColor;\n" \

		"VOUT main(VIN vin){\n" \
		"	VOUT vout;\n" \
		"	vout.position=mul(vin.position,modelViewProjectionMatrix);\n" \
		"	vout.color=-dot(lightModelPosition,vin.normal)*lightColor*materialDiffuseColor + ambientColor*materialAmbientColor;\n" \
		"	return vout;\n" \
		"}"
	};

	String fixedFragmentCode[]={
		"in vec4 color;\n" \
		"void main(){\n" \
			"gl_FragColor = color;\n" \
		"}",

		"struct PIN{\n" \
			"float4 position : SV_POSITION;\n" \
			"float4 color: COLOR;\n" \
		"};\n" \
		"float4 main(PIN pin): SV_TARGET{" \
		"	return pin.color;\n" \
		"}"
	};

	mFixedVertexShader=getEngine()->getShaderManager()->createShader(Shader::ShaderType_VERTEX,fixedProfiles,fixedVertexCode,2);
	mFixedFragmentShader=getEngine()->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,fixedProfiles,fixedFragmentCode,2);

	int i;
	for(i=0;i<mRenderStates.size();++i){
		RenderState::ptr renderState=mRenderStates[i];
		if(renderState!=NULL && renderState->getRootRenderState()!=renderState){
			RenderState::ptr back(renderDevice->createRenderState());
			shared_static_cast<BackableRenderState>(renderState)->setBack(back);
		}
	}
}

void MaterialManager::contextDeactivate(RenderDevice *renderDevice){
	if(mFixedVertexShader!=NULL){
		mFixedVertexShader->release();
		mFixedVertexShader=NULL;
	}
	if(mFixedFragmentShader!=NULL){
		mFixedFragmentShader->release();
		mFixedFragmentShader=NULL;
	}

	int i;
	for(i=0;i<mRenderStates.size();++i){
		RenderState::ptr renderState=mRenderStates[i];
		if(renderState!=NULL && renderState->getRootRenderState()!=renderState){
			shared_static_cast<BackableRenderState>(renderState)->setBack(NULL);
		}
	}
}

void MaterialManager::renderStateDestroyed(RenderState *renderState){
	mRenderStates.remove(renderState);
}

void MaterialManager::shaderStateDestroyed(ShaderState *shaderState){
	mShaderStates.remove(shaderState);
}

Resource::ptr MaterialManager::unableToFindHandler(const String &name,const ResourceHandlerData *handlerData){
	Texture::ptr texture=mEngine->getTextureManager()->findTexture(name);
	if(texture!=NULL){
		return createDiffuseMaterial(texture);
	}
	else{
		return ResourceManager::unableToFindHandler(name,handlerData);
	}
}

BufferManager *MaterialManager::getBufferManager(){
	return mEngine->getBufferManager();
}

}
}

