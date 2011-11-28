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

#include <toadlet/tadpole/terrain/DiffuseTerrainMaterialSource.h>
#include <toadlet/tadpole/terrain/TerrainPatchNode.h>
#include <toadlet/tadpole/material/RenderVariables.h>

namespace toadlet{
namespace tadpole{
namespace terrain{

DiffuseTerrainMaterialSource::DiffuseTerrainMaterialSource(Engine *engine){
	mEngine=engine;


	String profiles[]={
		"glsl",
		"hlsl"
	};

	String diffuseVertexCode[]={
		"attribute vec4 POSITION;\n"
		"attribute vec3 NORMAL;\n"
		"attribute vec2 TEXCOORD0;\n"
		"varying vec4 color;\n"
		"varying float fog;\n"
		"varying vec2 texCoord,texCoord2;\n"

		"uniform mat4 modelViewProjectionMatrix;\n"
		"uniform mat4 normalMatrix;\n"
		"uniform vec4 materialDiffuseColor;\n"
		"uniform vec4 materialAmbientColor;\n"
		"uniform float materialTrackColor;\n"
		"uniform vec4 lightViewPosition;\n"
		"uniform vec4 lightColor;\n"
		"uniform vec4 ambientColor;\n"
		"uniform float materialLighting;\n"
		"uniform float fogDensity;\n"
		"uniform vec2 fogDistance;\n"

		"void main(){\n"
			"gl_Position=modelViewProjectionMatrix * POSITION;\n"
			"vec3 viewNormal=normalize(normalMatrix * vec4(NORMAL,0.0)).xyz;\n"
			"float lightIntensity=clamp(-dot(lightViewPosition.xyz,viewNormal),0.0,1.0);\n"
			"vec4 localLightColor=(lightIntensity*lightColor*materialLighting)+(1.0-materialLighting);\n"
			"color=clamp(localLightColor*materialDiffuseColor + ambientColor*materialAmbientColor,0.0,1.0);\n"
			"color=materialTrackColor+color*(1.0-materialTrackColor);\n"
			"texCoord=vec4(TEXCOORD0,0.0,1.0).xy * 16.0;\n"
			"texCoord2=vec4(TEXCOORD,0.0,1.0).xy;\n"
			"fog=clamp(1.0-fogDensity*(gl_Position.z-fogDistance.x)/(fogDistance.y-fogDistance.x),0.0,1.0);\n"
		"}",



		"struct VIN{\n"
			"float4 position : POSITION;\n"
			"float3 normal : NORMAL;\n"
			"float2 texCoord: TEXCOORD0;\n"
		"};\n"
		"struct VOUT{\n"
			"float4 position : SV_POSITION;\n"
			"float4 color : COLOR;\n"
			"float fog: FOG;\n"
			"float2 texCoord: TEXCOORD0;\n"
			"float2 texCoord2: TEXCOORD1;\n"
		"};\n"

		"float4x4 modelViewProjectionMatrix;\n"
		"float4x4 normalMatrix;\n"
		"float4 materialDiffuseColor;\n"
		"float4 materialAmbientColor;\n"
		"float materialTrackColor;\n"
		"float4 lightViewPosition;\n"
		"float4 lightColor;\n"
		"float4 ambientColor;\n"
		"float materialLighting;\n"
		"float fogDensity;\n"
		"float2 fogDistance;\n"

		"VOUT main(VIN vin){\n"
			"VOUT vout;\n"
			"vout.position=mul(modelViewProjectionMatrix,vin.position);\n"
			"float3 viewNormal=normalize(mul(normalMatrix,vin.normal));\n"
			"float lightIntensity=clamp(-dot(lightViewPosition,viewNormal),0.0,1.0);\n"
			"float4 localLightColor=lightIntensity*lightColor*materialLighting+(1.0-materialLighting);\n"
			"vout.color=clamp(localLightColor*materialDiffuseColor + ambientColor*materialAmbientColor,0.0,1.0);\n"
			"vout.color=materialTrackColor+vout.color*(1.0-materialTrackColor);\n"
			"vout.texCoord=float4(vin.texCoord,0.0,1.0) * 16.0;\n "
			"vout.texCoord2=float4(vin.texCoord,0.0,1.0);\n "
			"vout.fog=clamp(1.0-fogDensity*(vout.position.z-fogDistance.x)/(fogDistance.y-fogDistance.x),0.0,1.0);\n"
			"return vout;\n"
		"}"
	};

	String diffuseBaseFragmentCode[]={
		"varying vec4 color;\n"
		"varying vec2 texCoord;\n"
		"varying float fog;\n"

		"uniform vec4 fogColor;\n"
		"uniform sampler2D tex;\n"

		"void main(){\n"
			"vec4 fragColor=color*(texture2D(tex,texCoord));\n"
			"gl_FragColor=mix(fogColor,fragColor,fog);\n"
		"}",



		"struct PIN{\n"
			"float4 position: SV_POSITION;\n"
			"float4 color: COLOR;\n"
			"float fog: FOG;\n"
			"float2 texCoord: TEXCOORD0;\n"
		"};\n"

		"float4 fogColor;\n"
		"Texture2D tex;\n"
		"SamplerState samp;\n"

		"float4 main(PIN pin): SV_TARGET{\n"
			"float4 fragColor=pin.color*(tex.Sample(samp,pin.texCoord));\n"
			"return lerp(fogColor,fragColor,pin.fog);\n"
		"}"
	};

	String diffuseLayerFragmentCode[]={
		"varying vec4 color;\n"
		"varying vec2 texCoord,texCoord2;\n"
		"varying float fog;\n"

		"uniform vec4 fogColor;\n"
		"uniform sampler2D tex,layerTex;\n"

		"void main(){\n"
			"vec4 fragColor=color*(texture2D(tex,texCoord));\n"
			"fragColor.w=texture2D(layerTex,texCoord2).w;\n"
			"gl_FragColor=mix(fogColor,fragColor,fog);\n"
		"}",



		"struct PIN{\n"
			"float4 position: SV_POSITION;\n"
			"float4 color: COLOR;\n"
			"float fog: FOG;\n"
			"float2 texCoord: TEXCOORD0;\n"
			"float2 texCoord2: TEXCOORD1;\n"
		"};\n"

		"float4 fogColor;\n"
		"Texture2D tex,layerTex;\n"
		"SamplerState samp,layerSamp;\n"

		"float4 main(PIN pin): SV_TARGET{\n"
			"float4 fragColor=pin.color*(tex.Sample(samp,pin.texCoord));\n"
			"fragColor.w=layerTex.Sample(layerSamp,pin.texCoord2).w;\n"
			"return lerp(fogColor,fragColor,pin.fog);\n"
		"}"
	};

	mDiffuseVertexShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_VERTEX,profiles,diffuseVertexCode,2);
	mDiffuseBaseFragmentShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,diffuseBaseFragmentCode,2);
	mDiffuseLayerFragmentShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,diffuseLayerFragmentCode,2);
}

void DiffuseTerrainMaterialSource::destroy(){
	int i;
	for(i=0;i<mDiffuseTextures.size();++i){
		if(mDiffuseTextures[i]!=NULL){
			mDiffuseTextures[i]->release();
			mDiffuseTextures[i]=NULL;
		}
	}

	if(mDiffuseVertexShader!=NULL){
		mDiffuseVertexShader->release();
		mDiffuseVertexShader=NULL;
	}
	if(mDiffuseBaseFragmentShader!=NULL){
		mDiffuseBaseFragmentShader->release();
		mDiffuseBaseFragmentShader=NULL;
	}
	if(mDiffuseLayerFragmentShader!=NULL){
		mDiffuseLayerFragmentShader->release();
		mDiffuseLayerFragmentShader=NULL;
	}
}

void DiffuseTerrainMaterialSource::setDiffuseTexture(int layer,Texture::ptr texture){
	if(mDiffuseTextures.size()<=layer){
		mDiffuseTextures.resize(layer+1);
	}

	if(mDiffuseTextures[layer]!=NULL){
		mDiffuseTextures[layer]->release();
	}
	mDiffuseTextures[layer]=texture;
	if(mDiffuseTextures[layer]!=NULL){
		mDiffuseTextures[layer]->retain();
	}
}

Material::ptr DiffuseTerrainMaterialSource::getMaterial(TerrainPatchNode *patch){
	Material::ptr material(new Material(mEngine->getMaterialManager()));

	TextureState scaleState;
	scaleState.calculation=TextureState::CalculationType_NORMAL;
	Math::setMatrix4x4FromScale(scaleState.matrix,16,16,16);

	TextureState layerState;
	layerState.colorOperation=TextureState::Operation_REPLACE;
	layerState.colorSource1=TextureState::Source_PREVIOUS;
	layerState.colorSource2=TextureState::Source_PREVIOUS;
	layerState.alphaOperation=TextureState::Operation_REPLACE;
	layerState.alphaSource1=TextureState::Source_TEXTURE;
	layerState.alphaSource2=TextureState::Source_TEXTURE;

	SamplerState clampState;
	clampState.uAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
	clampState.vAddress=SamplerState::AddressType_CLAMP_TO_EDGE;

	int numLayers=patch->getNumLayers();

	if(mEngine->hasShader(Shader::ShaderType_VERTEX) && mEngine->hasShader(Shader::ShaderType_FRAGMENT)){
		RenderPath::ptr shaderPath=material->addPath();
		
		{
			RenderPass::ptr pass=shaderPath->addPass();

			pass->setBlendState(BlendState());
			pass->setDepthState(DepthState());
			pass->setRasterizerState(RasterizerState());
			pass->setMaterialState(MaterialState(true,false,MaterialState::ShadeType_GOURAUD));
			pass->setShader(Shader::ShaderType_VERTEX,mDiffuseVertexShader);
			pass->setShader(Shader::ShaderType_FRAGMENT,mDiffuseBaseFragmentShader);
			pass->getVariables()->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
			pass->getVariables()->addVariable("normalMatrix",RenderVariable::ptr(new NormalMatrixVariable()),Material::Scope_RENDERABLE);
			pass->getVariables()->addVariable("lightViewPosition",RenderVariable::ptr(new LightViewPositionVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("lightColor",RenderVariable::ptr(new LightDiffuseVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("ambientColor",RenderVariable::ptr(new AmbientVariable()),Material::Scope_RENDERABLE);
			pass->getVariables()->addVariable("materialLighting",RenderVariable::ptr(new MaterialLightingVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("materialDiffuseColor",RenderVariable::ptr(new MaterialDiffuseVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("materialAmbientColor",RenderVariable::ptr(new MaterialAmbientVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("materialTrackColor",RenderVariable::ptr(new MaterialTrackColorVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("fogDensity",RenderVariable::ptr(new FogDensityVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("fogDistance",RenderVariable::ptr(new FogDistanceVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("fogColor",RenderVariable::ptr(new FogColorVariable()),Material::Scope_MATERIAL);

			pass->setTexture(0,mDiffuseTextures.size()>0?mDiffuseTextures[0]:NULL);
			pass->setTextureState(0,scaleState);
		}

		int i;
		for(i=1;i<numLayers;++i){
			if(patch->getLayerTexture(i)==NULL) continue;

			RenderPass::ptr pass=shaderPath->addPass();

			pass->setBlendState(BlendState::Combination_ALPHA);
			pass->setDepthState(DepthState());
			pass->setRasterizerState(RasterizerState());
			pass->setMaterialState(MaterialState(true,false,MaterialState::ShadeType_GOURAUD));
			pass->setShader(Shader::ShaderType_VERTEX,mDiffuseVertexShader);
			pass->setShader(Shader::ShaderType_FRAGMENT,mDiffuseLayerFragmentShader);
			pass->getVariables()->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
			pass->getVariables()->addVariable("normalMatrix",RenderVariable::ptr(new NormalMatrixVariable()),Material::Scope_RENDERABLE);
			pass->getVariables()->addVariable("lightViewPosition",RenderVariable::ptr(new LightViewPositionVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("lightColor",RenderVariable::ptr(new LightDiffuseVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("ambientColor",RenderVariable::ptr(new AmbientVariable()),Material::Scope_RENDERABLE);
			pass->getVariables()->addVariable("materialLighting",RenderVariable::ptr(new MaterialLightingVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("materialDiffuseColor",RenderVariable::ptr(new MaterialDiffuseVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("materialAmbientColor",RenderVariable::ptr(new MaterialAmbientVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("materialTrackColor",RenderVariable::ptr(new MaterialTrackColorVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("fogDensity",RenderVariable::ptr(new FogDensityVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("fogDistance",RenderVariable::ptr(new FogDistanceVariable()),Material::Scope_MATERIAL);
			pass->getVariables()->addVariable("fogColor",RenderVariable::ptr(new FogColorVariable()),Material::Scope_MATERIAL);

			pass->setTexture(0,mDiffuseTextures.size()>i?mDiffuseTextures[i]:NULL);
			pass->setTextureState(0,scaleState);
			pass->setTexture(1,patch->getLayerTexture(i));
			pass->setTextureState(1,layerState);
			pass->setSamplerState(1,clampState);
		}
	}

	if(mEngine->hasFixed(Shader::ShaderType_VERTEX) && mEngine->hasFixed(Shader::ShaderType_FRAGMENT)){
		RenderPath::ptr fixedPath=material->addPath();

		{
			RenderPass::ptr pass=fixedPath->addPass();

			pass->setBlendState(BlendState());
			pass->setDepthState(DepthState());
			pass->setRasterizerState(RasterizerState());
			pass->setMaterialState(MaterialState(true,false,MaterialState::ShadeType_GOURAUD));

			pass->setTexture(0,mDiffuseTextures.size()>0?mDiffuseTextures[0]:NULL);
			pass->setTextureState(0,scaleState);
		}

		int i;
		for(i=1;i<numLayers;++i){
			if(patch->getLayerTexture(i)==NULL) continue;

			RenderPass::ptr pass=fixedPath->addPass();

			pass->setBlendState(BlendState::Combination_ALPHA);
			pass->setDepthState(DepthState());
			pass->setRasterizerState(RasterizerState());
			pass->setMaterialState(MaterialState(true,false,MaterialState::ShadeType_GOURAUD));

			pass->setTexture(0,mDiffuseTextures.size()>i?mDiffuseTextures[i]:NULL);
			pass->setTextureState(0,scaleState);
			pass->setTexture(1,patch->getLayerTexture(i));
			pass->setTextureState(1,layerState);
			pass->setSamplerState(1,clampState);
		}
	}

	mEngine->getMaterialManager()->manage(material);

	material->setLayer(-1);

	material->compile();

	return material;
}

}
}
}
