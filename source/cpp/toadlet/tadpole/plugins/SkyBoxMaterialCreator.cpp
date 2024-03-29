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

#include "SkyBoxMaterialCreator.h"
#include <toadlet/tadpole/material/RenderVariables.h>

namespace toadlet{
namespace tadpole{

SkyBoxMaterialCreator::SkyBoxMaterialCreator(Engine *engine){
	mEngine=engine;
	createShaders();
}

void SkyBoxMaterialCreator::destroy(){
	destroyShaders();
}

void SkyBoxMaterialCreator::createShaders(){
	destroyShaders();

	const char *profiles[]={
		"glsl",
		"hlsl"
	};

	const char *skyBoxVertexCode[]={
		"attribute vec4 POSITION;\n"
		"attribute vec4 COLOR;\n"
		"attribute vec2 TEXCOORD0;\n"
		"varying vec4 color;\n"
		"varying vec2 texCoord;\n"

		"uniform mat4 modelViewProjectionMatrix;\n"
		"uniform mat4 textureMatrix;\n"
		"uniform float materialTrackColor;\n"

		"void main(){\n"
			"gl_Position=modelViewProjectionMatrix * POSITION;\n"
			"color=COLOR*materialTrackColor+(1.0-materialTrackColor);\n"
			"texCoord=(textureMatrix * vec4(TEXCOORD0,0.0,1.0)).xy;\n"
		"}",



		"struct VIN{\n"
			"float4 position : POSITION;\n"
			"float4 color : COLOR;\n"
			"float2 texCoord: TEXCOORD0;\n"
		"};\n"
		"struct VOUT{\n"
			"float4 position : SV_POSITION;\n"
			"float4 color : COLOR;\n"
			"float2 texCoord: TEXCOORD0;\n"
		"};\n"

		"float4x4 modelViewProjectionMatrix;\n"
		"float4x4 textureMatrix;\n"
		"float materialTrackColor;\n"

		"VOUT main(VIN vin){\n"
			"VOUT vout;\n"
			"vout.position=mul(modelViewProjectionMatrix,vin.position);\n"
			"vout.color=vin.color*materialTrackColor+(1.0-materialTrackColor);\n"
			"vout.texCoord=mul(textureMatrix,float4(vin.texCoord,0.0,1.0));\n"
			"return vout;\n"
		"}"
	};

	const char *skyBoxFragmentCode[]={
		"#if defined(GL_ES)\n"
			"precision mediump float;\n"
		"#endif\n"

		"varying vec4 color;\n"
		"varying vec2 texCoord;\n"
		
		"uniform sampler2D tex;\n"
		
		"void main(){\n"
			"gl_FragColor = color * texture2D(tex,texCoord);\n"
		"}",



		"struct PIN{\n"
			"float4 position: SV_POSITION;\n"
			"float4 color: COLOR;\n"
			"float2 texCoord: TEXCOORD0;\n"
		"};\n"

		"Texture2D tex;\n"
		"SamplerState samp;\n"

		"float4 main(PIN pin): SV_TARGET{\n"
			"return pin.color * tex.Sample(samp,pin.texCoord);\n"
		"}"
	};

	TOADLET_TRY
		mSkyBoxVertexShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_VERTEX,profiles,skyBoxVertexCode,2);
	TOADLET_CATCH_ANONYMOUS(){}
	TOADLET_TRY
		mSkyBoxFragmentShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,skyBoxFragmentCode,2);
	TOADLET_CATCH_ANONYMOUS(){}

	mSkyBoxShaderState=mEngine->getMaterialManager()->createShaderState();
	if(mSkyBoxShaderState!=NULL){
		mSkyBoxShaderState->setShader(Shader::ShaderType_VERTEX,mSkyBoxVertexShader);
		mSkyBoxShaderState->setShader(Shader::ShaderType_FRAGMENT,mSkyBoxFragmentShader);
	}
}

void SkyBoxMaterialCreator::destroyShaders(){
	if(mSkyBoxVertexShader!=NULL){
		mSkyBoxVertexShader->destroy();
		mSkyBoxVertexShader=NULL;
	}
	if(mSkyBoxFragmentShader!=NULL){
		mSkyBoxFragmentShader->destroy();
		mSkyBoxFragmentShader=NULL;
	}
}

Resource::ptr SkyBoxMaterialCreator::create(const String &name,ResourceData *data){
	Error::unimplemented(Categories::TOADLET_TADPOLE,"SkyBoxMaterialCreator::create unimplemented");
	return NULL;
}

Material::ptr SkyBoxMaterialCreator::createSkyBoxMaterial(Texture *texture,bool clamp,RenderState *state){
	Material::ptr material=new Material(mEngine->getMaterialManager());

	RenderState::ptr renderState=state;
	if(renderState==NULL){
		renderState=mEngine->getMaterialManager()->createRenderState();
		if(renderState!=NULL){
			renderState->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
			renderState->setMaterialState(MaterialState(false));
			renderState->setFogState(FogState());
		}
	}

	createPaths(material,renderState,texture,clamp);

	material->setLayer(-1);
	material->setSort(Material::SortType_MATERIAL);
	material->compile();

	mEngine->getMaterialManager()->manage(material);

	return material;
}

bool SkyBoxMaterialCreator::createPaths(Material *material,RenderState *renderState,Texture *texture,bool clamp){
	SamplerState samplerState(mEngine->getMaterialManager()->getDefaultSamplerState());
	if(clamp){
		samplerState.uAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
		samplerState.vAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
		samplerState.wAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
	}
	if(renderState){
		renderState->getSamplerState(Shader::ShaderType_FRAGMENT,0,samplerState);
	}

	TextureState textureState;
	textureState.calculation=TextureState::CalculationType_NORMAL;

	if(	mSkyBoxShaderState &&
		mEngine->hasShader(Shader::ShaderType_VERTEX) && 
		mEngine->hasShader(Shader::ShaderType_FRAGMENT)
	){
		RenderPath::ptr shaderPath=material->addPath("shader");

		RenderPass::ptr pass=shaderPath->addPass(renderState,mSkyBoxShaderState);

		pass->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
		pass->addVariable("textureMatrix",RenderVariable::ptr(new TextureMatrixVariable("tex")),Material::Scope_MATERIAL);
		pass->addVariable("materialTrackColor",RenderVariable::ptr(new MaterialTrackColorVariable()),Material::Scope_MATERIAL);

		pass->setTexture("tex",texture,"samp",samplerState,textureState);
	}

	if(	mEngine->hasFixed(Shader::ShaderType_VERTEX) &&
		mEngine->hasFixed(Shader::ShaderType_FRAGMENT)
	){
		RenderPath::ptr fixedPath=material->addPath("fixed");

		RenderPass::ptr pass=fixedPath->addPass(renderState);

		pass->setTexture(Shader::ShaderType_FRAGMENT,0,texture,samplerState,textureState);
		pass->setTextureLocationName(Shader::ShaderType_FRAGMENT,0,"tex");
	}

	return true;
}

}
}
