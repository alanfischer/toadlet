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
#include <toadlet/tadpole/terrain/TerrainPatchComponent.h>
#include <toadlet/tadpole/material/RenderVariables.h>

namespace toadlet{
namespace tadpole{
namespace terrain{

DiffuseTerrainMaterialSource::DiffuseTerrainMaterialSource(Engine *engine):
	mEngine(engine),
	mDiffuseScale(1,1,1),mDetailScale(1,1,1)
{
	createShaders();
}

DiffuseTerrainMaterialSource::DiffuseTerrainMaterialSource(Engine *engine,const String &name,const Vector3 &scale,const Vector3 &offset):
	mEngine(engine),
	mDiffuseScale(scale),mDetailScale(scale),
	mDiffuseOffset(offset),mDetailOffset(offset)
{
	setDiffuseTexture(0,name);
	setDetailTexture(name);

	createShaders();
}

DiffuseTerrainMaterialSource::DiffuseTerrainMaterialSource(Engine *engine,Texture *texture,const Vector3 &scale,const Vector3 &offset):
	mEngine(engine),
	mDiffuseScale(scale),mDetailScale(scale),
	mDiffuseOffset(offset),mDetailOffset(offset)
{
	setDiffuseTexture(0,texture);
	setDetailTexture(texture);

	createShaders();
}

void DiffuseTerrainMaterialSource::createShaders(){
	const char *profiles[]={
		"glsl",
		"hlsl"
	};

	const char *diffuseVertexCode[]={
		"attribute vec4 POSITION;\n"
		"attribute vec3 NORMAL;\n"
		"attribute vec2 TEXCOORD0;\n"
		"varying vec4 color;\n"
		"varying float fog;\n"
		"varying vec2 texCoord,detailTexCoord,layerTexCoord;\n"

		"uniform mat4 modelViewProjectionMatrix;\n"
		"uniform mat4 normalMatrix;\n"
		"uniform vec4 materialDiffuseColor;\n"
		"uniform vec4 materialAmbientColor;\n"
		"uniform float materialTrackColor;\n"
		"uniform vec4 lightViewPosition;\n"
		"uniform vec4 lightColor;\n"
		"uniform vec4 ambientColor;\n"
		"uniform float materialLight;\n"
		"uniform mat4 textureMatrix;\n"
		"uniform mat4 detailTextureMatrix;\n"
		"uniform float fogDensity;\n"
		"uniform vec2 fogDistance;\n"

		"void main(){\n"
			"gl_Position=modelViewProjectionMatrix * POSITION;\n"
			"vec3 viewNormal=normalize(normalMatrix * vec4(NORMAL,0.0)).xyz;\n"
			"float lightIntensity=clamp(-dot(lightViewPosition.xyz,viewNormal),0.0,1.0);\n"
			"vec4 localLightColor=(lightIntensity*lightColor*materialLight)+(1.0-materialLight);\n"
			"color=clamp(localLightColor*materialDiffuseColor + ambientColor*materialAmbientColor,0.0,1.0);\n"
			"color=materialTrackColor+color*(1.0-materialTrackColor);\n"
			"texCoord=(textureMatrix * vec4(TEXCOORD0,0.0,1.0)).xy;\n"
			"detailTexCoord=(detailTextureMatrix * vec4(TEXCOORD0,0.0,1.0)).xy;\n"
			"layerTexCoord=vec4(TEXCOORD0,0.0,1.0).xy;\n"
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
			"float2 detailTexCoord: TEXCOORD1;\n"
			"float2 layerTexCoord: TEXCOORD2;\n"
		"};\n"

		"float4x4 modelViewProjectionMatrix;\n"
		"float4x4 normalMatrix;\n"
		"float4 materialDiffuseColor;\n"
		"float4 materialAmbientColor;\n"
		"float materialTrackColor;\n"
		"float4 lightViewPosition;\n"
		"float4 lightColor;\n"
		"float4 ambientColor;\n"
		"float materialLight;\n"
		"float4x4 textureMatrix;\n"
		"float4x4 detailTextureMatrix;\n"
		"float fogDensity;\n"
		"float2 fogDistance;\n"

		"VOUT main(VIN vin){\n"
			"VOUT vout;\n"
			"vout.position=mul(modelViewProjectionMatrix,vin.position);\n"
			"float3 viewNormal=normalize(mul(normalMatrix,vin.normal));\n"
			"float lightIntensity=clamp(-dot(lightViewPosition,viewNormal),0.0,1.0);\n"
			"float4 localLightColor=lightIntensity*lightColor*materialLight+(1.0-materialLight);\n"
			"vout.color=clamp(localLightColor*materialDiffuseColor + ambientColor*materialAmbientColor,0.0,1.0);\n"
			"vout.color=materialTrackColor+vout.color*(1.0-materialTrackColor);\n"
			"vout.texCoord=mul(textureMatrix,float4(vin.texCoord,0.0,1.0));\n "
			"vout.detailTexCoord=mul(detailTextureMatrix,float4(vin.texCoord,0.0,1.0));\n "
			"vout.layerTexCoord=float4(vin.texCoord,0.0,1.0);\n "
			"vout.fog=clamp(1.0-fogDensity*(vout.position.z-fogDistance.x)/(fogDistance.y-fogDistance.x),0.0,1.0);\n"
			"return vout;\n"
		"}"
	};

	const char *diffuseBaseFragmentCode[]={
		"#if defined(GL_ES)\n"
			"precision mediump float;\n"
		"#endif\n"

		"varying vec4 color;\n"
		"varying vec2 texCoord,detailTexCoord;\n"
		"varying float fog;\n"

		"uniform vec4 fogColor;\n"
		"uniform sampler2D tex,detailTex;\n"

		"void main(){\n"
			"vec4 fragColor=color*texture2D(tex,texCoord)*texture2D(detailTex,detailTexCoord);\n"
			"gl_FragColor=mix(fogColor,fragColor,fog);\n"
		"}",



		"struct PIN{\n"
			"float4 position: SV_POSITION;\n"
			"float4 color: COLOR;\n"
			"float fog: FOG;\n"
			"float2 texCoord: TEXCOORD0;\n"
			"float2 detailTexCoord: TEXCOORD1;\n"
		"};\n"

		"float4 fogColor;\n"
		"Texture2D tex,detailTex;\n"
		"SamplerState samp,detailSamp;\n"

		"float4 main(PIN pin): SV_TARGET{\n"
			"float4 fragColor=pin.color*tex.Sample(samp,pin.texCoord)*detailTex.Sample(detailSamp,pin.detailTexCoord);\n"
			"return lerp(fogColor,fragColor,pin.fog);\n"
		"}"
	};

	const char *diffuseLayerFragmentCode[]={
		"#if defined(GL_ES)\n"
			"precision mediump float;\n"
		"#endif\n"

		"varying vec4 color;\n"
		"varying vec2 texCoord,detailTexCoord,layerTexCoord;\n"
		"varying float fog;\n"

		"uniform vec4 fogColor;\n"
		"uniform sampler2D tex,detailTex,layerTex;\n"

		"void main(){\n"
			"vec4 fragColor=color*texture2D(tex,texCoord)*texture2D(detailTex,detailTexCoord);\n"
			"fragColor.w=texture2D(layerTex,layerTexCoord).w;\n"
			"gl_FragColor=mix(fogColor,fragColor,fog);\n"
		"}",



		"struct PIN{\n"
			"float4 position: SV_POSITION;\n"
			"float4 color: COLOR;\n"
			"float fog: FOG;\n"
			"float2 texCoord: TEXCOORD0;\n"
			"float2 detailTexCoord: TEXCOORD1;\n"
			"float2 layerTexCoord: TEXCOORD2;\n"
		"};\n"

		"float4 fogColor;\n"
		"Texture2D tex,detailTex,layerTex;\n"
		"SamplerState samp,detailSamp,layerSamp;\n"

		"float4 main(PIN pin): SV_TARGET{\n"
			"float4 fragColor=pin.color*tex.Sample(samp,pin.texCoord)*detailTex.Sample(detailSamp,pin.detailTexCoord);\n"
			"fragColor.w=layerTex.Sample(layerSamp,pin.layerTexCoord).w;\n"
			"return lerp(fogColor,fragColor,pin.fog);\n"
		"}"
	};

	TOADLET_TRY
		mDiffuseVertexShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_VERTEX,profiles,diffuseVertexCode,2);
	TOADLET_CATCH_ANONYMOUS(){}
	TOADLET_TRY
		mDiffuseBaseFragmentShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,diffuseBaseFragmentCode,2);
	TOADLET_CATCH_ANONYMOUS(){}
	TOADLET_TRY
		mDiffuseLayerFragmentShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,diffuseLayerFragmentCode,2);
	TOADLET_CATCH_ANONYMOUS(){}

	mDiffuseBaseState=mEngine->getMaterialManager()->createShaderState();
	if(mDiffuseBaseState!=NULL){
		mDiffuseBaseState->setShader(Shader::ShaderType_VERTEX,mDiffuseVertexShader);
		mDiffuseBaseState->setShader(Shader::ShaderType_FRAGMENT,mDiffuseBaseFragmentShader);
	}

	mDiffuseLayerState=mEngine->getMaterialManager()->createShaderState();
	if(mDiffuseLayerState!=NULL){
		mDiffuseLayerState->setShader(Shader::ShaderType_VERTEX,mDiffuseVertexShader);
		mDiffuseLayerState->setShader(Shader::ShaderType_FRAGMENT,mDiffuseLayerFragmentShader);
	}
}

void DiffuseTerrainMaterialSource::destroy(){
	mDiffuseTextures.clear();

	if(mDiffuseVertexShader!=NULL){
		mDiffuseVertexShader->destroy();
		mDiffuseVertexShader=NULL;
	}
	if(mDiffuseBaseFragmentShader!=NULL){
		mDiffuseBaseFragmentShader->destroy();
		mDiffuseBaseFragmentShader=NULL;
	}
	if(mDiffuseLayerFragmentShader!=NULL){
		mDiffuseLayerFragmentShader->destroy();
		mDiffuseLayerFragmentShader=NULL;
	}

	if(mDiffuseBaseState!=NULL){
		mDiffuseBaseState->destroy();
		mDiffuseBaseState=NULL;
	}
	if(mDiffuseLayerState!=NULL){
		mDiffuseLayerState->destroy();
		mDiffuseLayerState=NULL;
	}
}

bool DiffuseTerrainMaterialSource::setDiffuseTexture(int layer,const String &name){
	return mEngine->getTextureManager()->find(name,new TextureRequest(this,layer));
}

bool DiffuseTerrainMaterialSource::setDiffuseTexture(int layer,Texture *texture){
	if(mDiffuseTextures.size()<=layer){
		mDiffuseTextures.resize(layer+1);
	}

	mDiffuseTextures[layer]=texture;

	return true;
}

bool DiffuseTerrainMaterialSource::setDetailTexture(const String &name){
	return mEngine->getTextureManager()->find(name,new TextureRequest(this));
}

bool DiffuseTerrainMaterialSource::setDetailTexture(Texture *texture){
	mDetailTexture=texture;

	return true;
}

Material::ptr DiffuseTerrainMaterialSource::getMaterial(TerrainPatchComponent *patch){
	Material::ptr material=new Material(mEngine->getMaterialManager());

	TextureState diffuseState;
	diffuseState.calculation=TextureState::CalculationType_NORMAL;
	getTextureMatrix(diffuseState.matrix,patch,false);

	TextureState detailState;
	detailState.calculation=TextureState::CalculationType_NORMAL;
	getTextureMatrix(detailState.matrix,patch,true);

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

	RenderState::ptr baseRenderState=mEngine->getMaterialManager()->createRenderState();
	baseRenderState->setMaterialState(MaterialState(true,false,MaterialState::ShadeType_GOURAUD));

	RenderState::ptr layerRenderState=mEngine->getMaterialManager()->createRenderState();
	layerRenderState->setBlendState(BlendState::Combination_ALPHA);
	layerRenderState->setMaterialState(MaterialState(true,false,MaterialState::ShadeType_GOURAUD));

	if(	mDiffuseBaseState &&
		mEngine->hasShader(Shader::ShaderType_VERTEX) &&
		mEngine->hasShader(Shader::ShaderType_FRAGMENT)
	){
		RenderPath::ptr shaderPath=material->addPath();
		
		{
			RenderPass::ptr pass=shaderPath->addPass(baseRenderState,mDiffuseBaseState);

			pass->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
			pass->addVariable("normalMatrix",RenderVariable::ptr(new NormalMatrixVariable()),Material::Scope_RENDERABLE);
			pass->addVariable("lightViewPosition",RenderVariable::ptr(new LightViewPositionVariable()),Material::Scope_MATERIAL);
			pass->addVariable("lightColor",RenderVariable::ptr(new LightDiffuseVariable()),Material::Scope_MATERIAL);
			pass->addVariable("ambientColor",RenderVariable::ptr(new AmbientVariable()),Material::Scope_RENDERABLE);
			pass->addVariable("materialLight",RenderVariable::ptr(new MaterialLightVariable()),Material::Scope_MATERIAL);
			pass->addVariable("materialDiffuseColor",RenderVariable::ptr(new MaterialDiffuseVariable()),Material::Scope_MATERIAL);
			pass->addVariable("materialAmbientColor",RenderVariable::ptr(new MaterialAmbientVariable()),Material::Scope_MATERIAL);
			pass->addVariable("materialTrackColor",RenderVariable::ptr(new MaterialTrackColorVariable()),Material::Scope_MATERIAL);
			pass->addVariable("textureMatrix",RenderVariable::ptr(new TextureMatrixVariable("tex")),Material::Scope_MATERIAL);
			pass->addVariable("detailTextureMatrix",RenderVariable::ptr(new TextureMatrixVariable("detailTex")),Material::Scope_MATERIAL);
			pass->addVariable("fogDensity",RenderVariable::ptr(new FogDensityVariable()),Material::Scope_MATERIAL);
			pass->addVariable("fogDistance",RenderVariable::ptr(new FogDistanceVariable()),Material::Scope_MATERIAL);
			pass->addVariable("fogColor",RenderVariable::ptr(new FogColorVariable()),Material::Scope_MATERIAL);

			pass->setTexture("tex",mDiffuseTextures.size()>0?mDiffuseTextures[0]:NULL,"samp",SamplerState(),diffuseState);
			pass->setTexture("detailTex",mDetailTexture,"detailSamp",SamplerState(),detailState);
		}

		int i;
		for(i=1;i<numLayers;++i){
			if(patch->getLayerTexture(i)==NULL) continue;

			RenderPass::ptr pass=shaderPath->addPass(layerRenderState,mDiffuseLayerState);

			pass->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
			pass->addVariable("normalMatrix",RenderVariable::ptr(new NormalMatrixVariable()),Material::Scope_RENDERABLE);
			pass->addVariable("lightViewPosition",RenderVariable::ptr(new LightViewPositionVariable()),Material::Scope_MATERIAL);
			pass->addVariable("lightColor",RenderVariable::ptr(new LightDiffuseVariable()),Material::Scope_MATERIAL);
			pass->addVariable("ambientColor",RenderVariable::ptr(new AmbientVariable()),Material::Scope_RENDERABLE);
			pass->addVariable("materialLight",RenderVariable::ptr(new MaterialLightVariable()),Material::Scope_MATERIAL);
			pass->addVariable("materialDiffuseColor",RenderVariable::ptr(new MaterialDiffuseVariable()),Material::Scope_MATERIAL);
			pass->addVariable("materialAmbientColor",RenderVariable::ptr(new MaterialAmbientVariable()),Material::Scope_MATERIAL);
			pass->addVariable("materialTrackColor",RenderVariable::ptr(new MaterialTrackColorVariable()),Material::Scope_MATERIAL);
			pass->addVariable("textureMatrix",RenderVariable::ptr(new TextureMatrixVariable("tex")),Material::Scope_MATERIAL);
			pass->addVariable("detailTextureMatrix",RenderVariable::ptr(new TextureMatrixVariable("detailTex")),Material::Scope_MATERIAL);
			pass->addVariable("fogDensity",RenderVariable::ptr(new FogDensityVariable()),Material::Scope_MATERIAL);
			pass->addVariable("fogDistance",RenderVariable::ptr(new FogDistanceVariable()),Material::Scope_MATERIAL);
			pass->addVariable("fogColor",RenderVariable::ptr(new FogColorVariable()),Material::Scope_MATERIAL);

			pass->setTexture("tex",mDiffuseTextures[i],"samp",SamplerState(),diffuseState);
			pass->setTexture("detailTex",mDetailTexture,"detailSamp",SamplerState(),detailState);
			pass->setTexture("layerTex",patch->getLayerTexture(i),"layerSamp",clampState,layerState);
		}
	}

	if(	mEngine->hasFixed(Shader::ShaderType_VERTEX) &&
		mEngine->hasFixed(Shader::ShaderType_FRAGMENT)
	){
		RenderPath::ptr fixedPath=material->addPath();

		{
			RenderPass::ptr pass=fixedPath->addPass(baseRenderState);

			pass->setTexture(Shader::ShaderType_FRAGMENT,0,mDiffuseTextures.size()>0?mDiffuseTextures[0]:NULL,SamplerState(),diffuseState);
		}

		int i;
		for(i=1;i<numLayers;++i){
			if(patch->getLayerTexture(i)==NULL) continue;

			RenderPass::ptr pass=fixedPath->addPass(layerRenderState);

			pass->setTexture(Shader::ShaderType_FRAGMENT,0,mDiffuseTextures[i],SamplerState(),diffuseState);
			pass->setTexture(Shader::ShaderType_FRAGMENT,1,patch->getLayerTexture(i),clampState,layerState);
		}
	}

	mEngine->getMaterialManager()->manage(material);

	material->setLayer(-1);

	material->compile();

	return material;
}

void DiffuseTerrainMaterialSource::getTextureMatrix(Matrix4x4 &result,TerrainPatchComponent *patch,bool detail){
	if(!detail){
		Math::setMatrix4x4FromScale(result,mDiffuseScale);
		Math::setMatrix4x4FromTranslate(result,Vector3(mDiffuseOffset.x + mDiffuseScale.x * patch->getTerrainX(),mDiffuseOffset.y + mDiffuseScale.y * patch->getTerrainY(),mDiffuseOffset.z));
	}
	else{
		Math::setMatrix4x4FromScale(result,mDetailScale);
		Math::setMatrix4x4FromTranslate(result,Vector3(mDetailOffset.x + mDetailScale.x * patch->getTerrainX(),mDetailOffset.y + mDetailScale.y * patch->getTerrainY(),mDetailOffset.z));
	}
}

}
}
}
