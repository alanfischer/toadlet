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

#include "BSP30MaterialCreator.h"
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/material/RenderVariables.h>

namespace toadlet{
namespace tadpole{

BSP30MaterialCreator::BSP30MaterialCreator(Engine *engine){
	mEngine=engine;
	
	String profiles[]={
		"glsl",
		"hlsl"
	};

	String vertexCodes[]={
		"attribute vec4 POSITION;\n"
		"attribute vec2 TEXCOORD0;\n"
		"attribute vec2 TEXCOORD1;\n"
		"varying vec2 texCoord0;\n"
		"varying vec2 texCoord1;\n"

		"uniform mat4 modelViewProjectionMatrix;\n"
		"uniform mat4 textureMatrix;\n"

		"void main(){\n"
			"gl_Position=modelViewProjectionMatrix * POSITION;\n"
			"texCoord0=(textureMatrix * vec4(TEXCOORD0,0.0,1.0)).xy;\n"
			"texCoord1=TEXCOORD1;\n"
		"}",


		
		"struct VIN{\n"
			"float4 position : POSITION;\n"
			"float2 texCoord0: TEXCOORD0;\n"
			"float2 texCoord1: TEXCOORD1;\n"
		"};\n"
		"struct VOUT{\n"
			"float4 position : SV_POSITION;\n"
			"float2 texCoord0: TEXCOORD0;\n"
			"float2 texCoord1: TEXCOORD1;\n"
		"};\n"

		"float4x4 modelViewProjectionMatrix;\n"
		"float4x4 textureMatrix;\n"

		"VOUT main(VIN vin){\n"
			"VOUT vout;\n"
			"vout.position=mul(modelViewProjectionMatrix,vin.position);\n"
			"vout.texCoord0=mul(textureMatrix,float4(vin.texCoord0,0.0,1.0));\n "
			"vout.texCoord1=vin.texCoord1;\n "
			"return vout;\n"
		"}"
	};

	String fragmentCodes[]={
		"#if defined(GL_ES)\n"
			"precision mediump float;\n"
		"#endif\n"
		
		"varying vec2 texCoord0;\n"
		"varying vec2 texCoord1;\n"

		"uniform sampler2D diffuseTex;\n"
		"uniform sampler2D lightmapTex;\n"

		"void main(){\n"
			"vec4 fragColor=texture2D(diffuseTex,texCoord0)*texture2D(lightmapTex,texCoord1);\n"
			"gl_FragColor=fragColor;\n"
		"}",



		"struct PIN{\n"
			"float4 position : SV_POSITION;\n"
			"float2 texCoord0: TEXCOORD0;\n"
			"float2 texCoord1: TEXCOORD1;\n"
		"};\n"

		"Texture2D diffuseTex;\n"
		"SamplerState diffuseSamp;\n"
		"Texture2D lightmapTex;\n"
		"SamplerState lightmapSamp;\n"

		"float4 main(PIN pin): SV_TARGET{\n"
			"float4 fragColor=diffuseTex.Sample(diffuseSamp,pin.texCoord0)*lightmapTex.Sample(lightmapSamp,pin.texCoord1);\n"
			"return fragColor;\n"
		"}"
	};

	mVertexShader=engine->getShaderManager()->createShader(Shader::ShaderType_VERTEX,profiles,vertexCodes,2);
	mFragmentShader=engine->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,fragmentCodes,2);
}

Material::ptr BSP30MaterialCreator::createBSP30Material(Texture *diffuseTexture){
	Material::ptr material=mEngine->getMaterialManager()->createMaterial();

	RenderState::ptr renderState=mEngine->getMaterialManager()->createRenderState();
	renderState->setBlendState(BlendState());
	renderState->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,true));
	renderState->setMaterialState(MaterialState(false,false,MaterialState::ShadeType_FLAT));
	renderState->setRasterizerState(RasterizerState(RasterizerState::CullType_FRONT));

	if(mEngine->hasShader(Shader::ShaderType_VERTEX) && mEngine->hasShader(Shader::ShaderType_FRAGMENT)){
		RenderPath::ptr shaderPath=material->addPath();
		RenderPass::ptr pass=shaderPath->addPass(renderState);

		pass->setShader(Shader::ShaderType_VERTEX,mVertexShader);
		pass->setShader(Shader::ShaderType_FRAGMENT,mFragmentShader);

		pass->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
		pass->addVariable("textureMatrix",RenderVariable::ptr(new TextureMatrixVariable("diffuseTex")),Material::Scope_MATERIAL);

		pass->setTexture("diffuseTex",diffuseTexture,"diffuseSamp",mEngine->getMaterialManager()->getDefaultSamplerState(),TextureState());
		pass->setTexture("lightmapTex",NULL,"lightmapSamp",mEngine->getMaterialManager()->getDefaultSamplerState(),TextureState());
	}

	if(mEngine->hasFixed(Shader::ShaderType_VERTEX) && mEngine->hasFixed(Shader::ShaderType_FRAGMENT)){
		RenderPath::ptr fixedPath=material->addPath();
		RenderPass::ptr pass=fixedPath->addPass(renderState);

		TextureState lightmapState;
		lightmapState.texCoordIndex=1;
		lightmapState.colorOperation=TextureState::Operation_MODULATE;
		lightmapState.colorSource1=TextureState::Source_PREVIOUS;
		lightmapState.colorSource2=TextureState::Source_TEXTURE;

		pass->setTexture(Shader::ShaderType_FRAGMENT,0,diffuseTexture,mEngine->getMaterialManager()->getDefaultSamplerState(),TextureState());
		pass->setTexture(Shader::ShaderType_FRAGMENT,1,NULL,mEngine->getMaterialManager()->getDefaultSamplerState(),lightmapState);
	}
	
	material->compile();
	
	return material;
}

}
}
