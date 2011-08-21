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

#include <toadlet/tadpole/handler/SkyboxMaterialCreator.h>
#include <toadlet/tadpole/material/RenderVariables.h>

namespace toadlet{
namespace tadpole{
namespace handler{

SkyboxMaterialCreator::SkyboxMaterialCreator(Engine *engine){
	mEngine=engine;
}

void SkyboxMaterialCreator::destroy(){
	destroyShaders();
}

void SkyboxMaterialCreator::createShaders(){
	destroyShaders();

	String profiles[]={
		"glsl",
		"hlsl"
	};

	String skyboxVertexCode[]={
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

	String skyboxFragmentCode[]={
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

	mSkyboxVertexShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_VERTEX,profiles,skyboxVertexCode,2);
	mSkyboxFragmentShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,skyboxFragmentCode,2);
}

void SkyboxMaterialCreator::destroyShaders(){
	if(mSkyboxVertexShader!=NULL){
		mSkyboxVertexShader->release();
		mSkyboxVertexShader=NULL;
	}
	if(mSkyboxFragmentShader!=NULL){
		mSkyboxFragmentShader->release();
		mSkyboxFragmentShader=NULL;
	}
}

Resource::ptr SkyboxMaterialCreator::create(const String &name,ResourceData *data,ProgressListener *listener){
	Error::unimplemented(Categories::TOADLET_TADPOLE,"SkyboxMaterialCreator::create unimplemented");
	return NULL;
}

Material::ptr SkyboxMaterialCreator::createSkyboxMaterial(Texture::ptr texture){
	Material::ptr material(new Material(mEngine->getMaterialManager()));

	RenderPath::ptr shaderPath=material->addPath();
	{
		RenderPass::ptr pass=shaderPath->addPass();

		pass->setBlendState(BlendState());
		pass->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
		pass->setRasterizerState(RasterizerState());
		pass->setMaterialState(MaterialState());
		pass->setFogState(FogState());

		pass->setShader(Shader::ShaderType_VERTEX,mSkyboxVertexShader);
		pass->setShader(Shader::ShaderType_FRAGMENT,mSkyboxFragmentShader);
		pass->getVariables()->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
		pass->getVariables()->addVariable("textureMatrix",RenderVariable::ptr(new TextureMatrixVariable(0)),Material::Scope_MATERIAL);
		pass->getVariables()->addVariable("materialTrackColor",RenderVariable::ptr(new MaterialTrackColorVariable()),Material::Scope_MATERIAL);

		SamplerState samplerState(mEngine->getMaterialManager()->getDefaultSamplerState());
		samplerState.uAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
		samplerState.vAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
		samplerState.wAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
		pass->setSamplerState(0,samplerState);
		pass->setTextureState(0,TextureState());
		pass->setTexture(0,texture);
	}

	RenderPath::ptr fixedPath=material->addPath();
	{
		RenderPass::ptr pass=fixedPath->addPass();

		pass->setBlendState(BlendState());
		pass->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
		pass->setRasterizerState(RasterizerState());
		pass->setMaterialState(MaterialState());
		pass->setFogState(FogState());

		SamplerState samplerState(mEngine->getMaterialManager()->getDefaultSamplerState());
		samplerState.uAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
		samplerState.vAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
		samplerState.wAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
		pass->setSamplerState(0,samplerState);
		pass->setTextureState(0,TextureState());
		pass->setTexture(0,texture);
	}

	material->setLayer(-1);
	material->compile();

	mEngine->getMaterialManager()->manage(material);

	return material;
}

}
}
}
