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

#include "WaterMaterialCreator.h"
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/material/RenderVariables.h>

namespace toadlet{
namespace tadpole{

WaterMaterialCreator::WaterMaterialCreator(Engine *engine){
	mEngine=engine;
	
	const char *profiles[]={
		"glsl",
		"hlsl"
	};

	const char *vertexCodes[]={
		"attribute vec4 POSITION;\n"
		"attribute vec3 NORMAL;\n"
		"attribute vec2 TEXCOORD0;\n"

		"varying float fog;\n"
		"varying vec4 reflectPosition;\n"
		"varying vec4 refractPosition;\n"
		"varying vec2 bumpPosition;\n"
		"varying vec4 positionFull;\n"

		"uniform mat4 modelViewProjectionMatrix;\n"
		"uniform mat4 modelMatrix;\n"
		"uniform mat4 waveMatrix;\n"
		"uniform vec2 fogDistance;\n"

		"void main(){\n"
			"gl_Position=modelViewProjectionMatrix * POSITION;\n"
			"positionFull=modelMatrix * POSITION;\n"

			"reflectPosition.x = 0.5 * (gl_Position.w - gl_Position.x);\n"
			"reflectPosition.y = 0.5 * (gl_Position.w - gl_Position.y);\n"
			"reflectPosition.z = gl_Position.w;\n"
			"reflectPosition.w = 1.0;\n"

			"refractPosition.x = 0.5 * (gl_Position.w + gl_Position.x);\n"
			"refractPosition.y = 0.5 * (gl_Position.w - gl_Position.y);\n"
			"refractPosition.z = gl_Position.w;\n"
			"refractPosition.w = 1.0;\n"

			"bumpPosition=(waveMatrix * vec4(TEXCOORD0,0.0,1.0)).xy;\n"

			"fog=clamp(1.0-(gl_Position.z-fogDistance.x)/(fogDistance.y-fogDistance.x),0.0,1.0);\n"
			// We calcaulate a separate refractFog, stored in positionFull.w, since the refractTex shows artifacts closer than the far plane
			//  due to not being able to use fog in an oblique frustum
			"positionFull.w=clamp(1.0-(gl_Position.z-fogDistance.x/4.0)/(fogDistance.y/4.0-fogDistance.x/4.0),0.0,1.0);\n"
		"}\n",


																	  
		"struct VIN{\n"
			"float4 position : POSITION;\n"
			"float3 normal : NORMAL;\n"
			"float2 texCoord: TEXCOORD0;\n"
		"};\n"
		"struct VOUT{\n"
			"float4 position : SV_POSITION;\n"
			"float fog: FOG;\n"
			"float4 reflectPosition: TEXCOORD0;\n"
			"float4 refractPosition: TEXCOORD1;\n"
			"float2 bumpPosition: TEXCOORD2;\n"
			"float4 positionFull: TEXCOORD3;\n"
		"};\n"

		"float4x4 modelViewProjectionMatrix;\n"
		"float4x4 modelMatrix;\n"
		"float4x4 waveMatrix;\n"
		"float2 fogDistance;\n"

		"VOUT main(VIN vin){\n"
			"VOUT vout;\n"
			"vout.position=mul(modelViewProjectionMatrix,vin.position);\n"
			"vout.positionFull=mul(modelMatrix,vin.position);\n"

			"vout.reflectPosition.x = 0.5 * (vout.position.w - vout.position.x);\n"
			"vout.reflectPosition.y = 0.5 * (vout.position.w - vout.position.y);\n"
			"vout.reflectPosition.z = vout.position.w;\n"
			"vout.reflectPosition.w=1.0;\n"

			"vout.refractPosition.x = 0.5 * (vout.position.w + vout.position.x);\n"
			"vout.refractPosition.y = 0.5 * (vout.position.w - vout.position.y);\n"
			"vout.refractPosition.z = vout.position.w;\n"
			"vout.refractPosition.w=1.0;\n"

			"vout.bumpPosition=mul(waveMatrix,float4(vin.texCoord,0.0,1.0));\n "

			"vout.fog=clamp(1.0-(vout.position.z-fogDistance.x)/(fogDistance.y-fogDistance.x),0.0,1.0);\n"
			// We calcaulate a separate refractFog, stored in positionFull.w, since the refractTex shows artifacts closer than the far plane
			//  due to not being able to use fog in an oblique frustum
			"vout.positionFull.w=clamp(1.0-(vout.position.z-fogDistance.x/4.0)/(fogDistance.y/4.0-fogDistance.x/4.0),0.0,1.0);\n"

			"return vout;\n"
		"}\n"
	};

	const char *fragmentCodes[]={
		"#if defined(GL_ES)\n"
			"precision mediump float;\n"
		"#endif\n"
		
		"varying float fog;\n"
		"varying vec4 reflectPosition;\n"
		"varying vec4 refractPosition;\n"
		"varying vec2 bumpPosition;\n"
		"varying vec4 positionFull;\n"

		"uniform vec4 fogColor;\n"
		"uniform vec4 lightPosition;\n"
		"uniform vec4 lightSpecular;\n"
		"uniform vec4 cameraPosition;\n"
		"uniform float waveHeight;\n"
		"uniform vec4 materialSpecular;\n"
		"uniform float materialShininess;\n"
		"uniform sampler2D reflectTex;\n"
		"uniform sampler2D refractTex;\n"
		"uniform sampler2D waveTex;\n"
		"uniform mat4 reflectMatrix;\n"
		"uniform mat4 refractMatrix;\n"

		"void main(){\n"
			"vec3 normalVector=vec3(0,0,1.0);\n"
			"vec4 bumpColor=texture2D(waveTex,bumpPosition);\n"
			"vec3 bumpVector=(bumpColor.xyz-0.5)*2.0;\n"

			"vec2 perturbation=waveHeight*bumpVector.xy;\n"

			"vec3 eyeVector = normalize(cameraPosition.xyz - positionFull.xyz);\n"
			"float fresnelTerm = 0.02+0.67*pow((1.0-dot(eyeVector, bumpVector)),5.0);\n"
			"fresnelTerm=clamp(fresnelTerm,0.0,1.0);\n"

			"vec4 reflectColor = texture2D(reflectTex,mod((reflectMatrix * vec4((reflectPosition.xy / reflectPosition.z) + perturbation,0,0)).xy,1.0));\n"
			"vec4 refractColor = texture2D(refractTex,mod((refractMatrix * vec4((refractPosition.xy / refractPosition.z) + perturbation,0,0)).xy,1.0));\n"
			"refractColor=mix(reflectColor,refractColor,positionFull.w);\n"

			"vec4 fragColor=mix(reflectColor,refractColor,fresnelTerm);\n"

			"vec4 dullColor=vec4(0.1,0.1,0.2,1.0);\n"
			"float dullBlendFactor=0.2;\n"
			"fragColor=mix(fragColor,dullColor,dullBlendFactor);\n"

			"float specPerturb=4.0;\n"
			"vec3 halfvec=normalize(eyeVector-lightPosition.xyz+vec3(perturbation.x*specPerturb,perturbation.y*specPerturb,0));\n"
			"float spec=pow(dot(halfvec,normalVector),materialShininess);\n"
			"fragColor=fragColor+mix(vec4(0),lightSpecular*materialSpecular,spec);\n"

			"fragColor.w=1.0;\n"
			"gl_FragColor=mix(fogColor,fragColor,fog);\n"
		"}\n",



		"struct PIN{\n"
			"float4 position: SV_POSITION;\n"
			"float fog: FOG;\n"
			"float4 reflectPosition: TEXCOORD0;\n"
			"float4 refractPosition: TEXCOORD1;\n"
			"float2 bumpPosition: TEXCOORD2;\n"
			"float4 positionFull: TEXCOORD3;\n"
		"};\n"

		"float4 fogColor;\n"
		"float4 lightPosition;\n"
		"float4 lightSpecular;\n"
		"float4 cameraPosition;\n"
		"float waveHeight;\n"
		"float4 materialSpecular;\n"
		"float materialShininess;\n"
		"Texture2D reflectTex;\n"
		"Texture2D refractTex;\n"
		"Texture2D waveTex;\n"
		"SamplerState reflectSamp;\n"
		"SamplerState refractSamp;\n"
		"SamplerState bumpSamp;\n"

		"float4 main(PIN pin): SV_TARGET{\n"
			"float3 normalVector=float3(0,0,1.0);\n"
			"float4 bumpColor=waveTex.Sample(bumpSamp,pin.bumpPosition);\n"
			"float3 bumpVector=(bumpColor.xyz-0.5)*2.0;\n"

			"float2 perturbation=waveHeight*bumpVector.xy;\n"

			"float3 eyeVector = normalize(cameraPosition.xyz - pin.positionFull.xyz);\n"
			"float fresnelTerm = 0.02+0.67*pow((1.0-dot(eyeVector, bumpVector)),5.0);\n"
			"fresnelTerm=clamp(fresnelTerm,0.0,1.0);\n"

			"float4 reflectColor = reflectTex.Sample(reflectSamp,(pin.reflectPosition.xy / pin.reflectPosition.z) + perturbation);\n"
			"float4 refractColor = refractTex.Sample(refractSamp,(pin.refractPosition.xy / pin.refractPosition.z) + perturbation);\n"
			"refractColor=lerp(reflectColor,refractColor,pin.positionFull.w);\n"

			"float4 fragColor=lerp(reflectColor,refractColor,fresnelTerm);\n"

			"float4 dullColor=float4(0.1,0.1,0.2,1.0);\n"
			"float dullBlendFactor=0.2;\n"
			"fragColor=lerp(fragColor,dullColor,dullBlendFactor);\n"

			"float specPerturb=4.0;\n"
			"float3 halfvec=normalize(eyeVector-lightPosition.xyz+float3(perturbation.x*specPerturb,perturbation.y*specPerturb,0));\n"
			"float spec=pow(dot(halfvec,normalVector),materialShininess);\n"
			"fragColor=fragColor+lerp(0.0,lightSpecular*materialSpecular,spec);\n"

			"fragColor.w=1.0;\n"
			"return lerp(fogColor,fragColor,pin.fog);\n"
		"}\n"
	};

	TOADLET_TRY
		mVertexShader=engine->getShaderManager()->createShader(Shader::ShaderType_VERTEX,profiles,vertexCodes,2);
	TOADLET_CATCH_ANONYMOUS(){}
	TOADLET_TRY
		mFragmentShader=engine->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,fragmentCodes,2);
	TOADLET_CATCH_ANONYMOUS(){}

	mShaderState=mEngine->getMaterialManager()->createShaderState();
	if(mShaderState!=NULL){
		mShaderState->setShader(Shader::ShaderType_VERTEX,mVertexShader);
		mShaderState->setShader(Shader::ShaderType_FRAGMENT,mFragmentShader);
	}
}

Material::ptr WaterMaterialCreator::createWaterMaterial(Texture *reflectTexture,Texture *refractTexture,Texture *waveTexture,const Vector4 &color){
	Material::ptr material=mEngine->getMaterialManager()->createMaterial();

	RenderState::ptr renderState=mEngine->getMaterialManager()->createRenderState();
	if(renderState!=NULL){
		renderState->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
		renderState->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
		renderState->setMaterialState(MaterialState(Math::ZERO_VECTOR4,Math::ONE_VECTOR4,Math::ONE_VECTOR4,128));
	}

	createPaths(material,renderState,reflectTexture,refractTexture,waveTexture);
	
	material->compile();
	
	return material;
}

bool WaterMaterialCreator::createPaths(Material *material,RenderState *renderState,Texture *reflectTexture,Texture *refractTexture,Texture *waveTexture){
	if(	mShaderState && 
		mEngine->hasShader(Shader::ShaderType_VERTEX) &&
		mEngine->hasShader(Shader::ShaderType_FRAGMENT)
	){
		RenderPath::ptr shaderPath=material->addPath();
		RenderPass::ptr pass=shaderPath->addPass(renderState,mShaderState);

		pass->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
		pass->addVariable("modelMatrix",RenderVariable::ptr(new ModelMatrixVariable()),Material::Scope_RENDERABLE);
		pass->addVariable("fogDistance",RenderVariable::ptr(new FogDistanceVariable()),Material::Scope_MATERIAL);
		pass->addVariable("fogColor",RenderVariable::ptr(new FogColorVariable()),Material::Scope_MATERIAL);
		pass->addVariable("cameraPosition",RenderVariable::ptr(new CameraPositionVariable()),Material::Scope_MATERIAL);
		pass->addVariable("lightPosition",RenderVariable::ptr(new LightPositionVariable()),Material::Scope_RENDERABLE);
		pass->addVariable("lightSpecular",RenderVariable::ptr(new LightSpecularVariable()),Material::Scope_RENDERABLE);
		pass->addVariable("waveHeight",RenderVariable::ptr(new ScalarVariable(0.25)),Material::Scope_MATERIAL);
		pass->addVariable("materialSpecular",RenderVariable::ptr(new MaterialSpecularVariable()),Material::Scope_RENDERABLE);
		pass->addVariable("materialShininess",RenderVariable::ptr(new MaterialShininessVariable()),Material::Scope_RENDERABLE);

		pass->setTexture("reflectTex",reflectTexture,"reflectSamp",SamplerState(SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE),TextureState());
		pass->setTexture("refractTex",refractTexture,"refractSamp",SamplerState(SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE),TextureState());
		pass->setTexture("waveTex",waveTexture,"bumpSamp",SamplerState(),TextureState());
		pass->addVariable("reflectMatrix",RenderVariable::ptr(new TextureMatrixVariable("reflectTex")),Material::Scope_MATERIAL);
		pass->addVariable("refractMatrix",RenderVariable::ptr(new TextureMatrixVariable("refractTex")),Material::Scope_MATERIAL);
		pass->addVariable("waveMatrix",RenderVariable::ptr(new TextureMatrixVariable("waveTex")),Material::Scope_MATERIAL);
	}

	if(	mEngine->hasFixed(Shader::ShaderType_VERTEX) &&
		mEngine->hasFixed(Shader::ShaderType_FRAGMENT)
	){
		RenderPath::ptr fixedPath=material->addPath();
		RenderPass::ptr pass=fixedPath->addPass(renderState);

		pass->setTexture(Shader::ShaderType_FRAGMENT,0,waveTexture,SamplerState(),TextureState());
		pass->setTextureLocationName(Shader::ShaderType_FRAGMENT,0,"waveTex");
	}

	return true;
}

}
}
