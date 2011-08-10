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
#include <toadlet/tadpole/material/RenderVariables.h>

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

	RenderPath::ptr shaderPath=material->addPath();
	{
		RenderPass::ptr pass=shaderPath->addPass();

		pass->setBlendState(BlendState());
		pass->setDepthState(DepthState());
		pass->setRasterizerState(RasterizerState());
		pass->setMaterialState(MaterialState(true,false,MaterialState::ShadeType_GOURAUD));

		pass->setShader(Shader::ShaderType_VERTEX,mDiffuseVertexShader);
		pass->setShader(Shader::ShaderType_FRAGMENT,mDiffuseFragmentShader);
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
		pass->getVariables()->addVariable("textureMatrix",RenderVariable::ptr(new TextureMatrixVariable(0)),Material::Scope_MATERIAL);
		pass->getVariables()->addVariable("textureSet",RenderVariable::ptr(new TextureSetVariable(0)),Material::Scope_MATERIAL);

		pass->setSamplerState(0,mDefaultSamplerState);
		pass->setTexture(0,texture);
	}

	RenderPath::ptr fixedPath=material->addPath();
	{
		RenderPass::ptr pass=fixedPath->addPass();

		pass->setBlendState(BlendState());
		pass->setDepthState(DepthState());
		pass->setRasterizerState(RasterizerState());
		pass->setMaterialState(MaterialState(true,false,MaterialState::ShadeType_GOURAUD));

		pass->setSamplerState(0,mDefaultSamplerState);
		pass->setTexture(0,texture);
	}

	manage(material);

	material->compile();

	return material;
}

Material::ptr MaterialManager::createDiffusePointSpriteMaterial(Texture::ptr texture,scalar size,bool attenuated){
	Material::ptr material=createDiffuseMaterial(texture);

	RenderPath::ptr shaderPath=material->getPath(0);
	{
		RenderPass::ptr pass=shaderPath->getPass(0);

		pass->setShader(Shader::ShaderType_GEOMETRY,mPointSpriteGeometryShader);
		pass->getVariables()->addVariable("pointSize",RenderVariable::ptr(new PointSizeVariable()),Material::Scope_MATERIAL);
		pass->getVariables()->addVariable("viewport",RenderVariable::ptr(new ViewportVariable()),Material::Scope_MATERIAL);

		/// @todo: We need to sort out how to handle the case in GL where you can have Geometry Shaders and PointSprites both functional.
		/// Though I suppose that would be in the GLRenderDevice, it would deactivate PointSprites if Geometry Shaders are used.
		pass->setPointState(PointState(true,size,attenuated));
	}

	RenderPath::ptr fixedPath=material->getPath(1);
	{
		RenderPass::ptr pass=fixedPath->getPass(0);

		pass->setPointState(PointState(true,size,attenuated));
	}

	manage(material);

	material->compile();

	return material;
}

Material::ptr MaterialManager::createSkyboxMaterial(Texture::ptr texture){
	Material::ptr material(new Material(this));

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

		SamplerState samplerState(mDefaultSamplerState);
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

		SamplerState samplerState(mDefaultSamplerState);
		samplerState.uAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
		samplerState.vAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
		samplerState.wAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
		pass->setSamplerState(0,samplerState);
		pass->setTextureState(0,TextureState());
		pass->setTexture(0,texture);
	}

	material->setLayer(-1);
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
	String profiles[]={
		"glsl",
		"hlsl"
	};

	String diffuseVertexCode[]={
		"attribute vec4 POSITION;\n"
		"attribute vec3 NORMAL;\n"
		"attribute vec4 COLOR;\n"
		"attribute vec2 TEXCOORD0;\n"
		"varying vec4 color;\n"
		"varying float fog;\n"
		"varying vec2 texCoord;\n"

		"uniform mat4 modelViewProjectionMatrix;\n"
		"uniform mat4 normalMatrix;\n"
		"uniform vec4 materialDiffuseColor;\n"
		"uniform vec4 materialAmbientColor;\n"
		"uniform float materialTrackColor;\n"
		"uniform vec4 lightViewPosition;\n"
		"uniform vec4 lightColor;\n"
		"uniform vec4 ambientColor;\n"
		"uniform float materialLighting;\n"
		"uniform mat4 textureMatrix;\n"
		"uniform float fogDensity;\n"
		"uniform vec2 fogDistance;\n"

		"void main(){\n"
			"gl_Position=modelViewProjectionMatrix * POSITION;\n"
			"vec3 viewNormal=normalize(normalMatrix * vec4(NORMAL,0.0)).xyz;\n"
			"float lightIntensity=clamp(-dot(lightViewPosition.xyz,viewNormal),0.0,1.0);\n"
			"vec4 localLightColor=(lightIntensity*lightColor*materialLighting)+(1.0-materialLighting);\n"
			"color=localLightColor*materialDiffuseColor + ambientColor*materialAmbientColor;\n"
			"color=COLOR*materialTrackColor+color*(1.0-materialTrackColor);\n"
			"texCoord=(textureMatrix * vec4(TEXCOORD0,0.0,1.0)).xy;\n"
			"fog=clamp(1.0-fogDensity*(gl_Position.z-fogDistance.x)/(fogDistance.y-fogDistance.x),0.0,1.0);\n"
		"}",



		"struct VIN{\n"
			"float4 position : POSITION;\n"
			"float3 normal : NORMAL;\n"
			"float4 color : COLOR;\n"
			"float2 texCoord: TEXCOORD0;\n"
		"};\n"
		"struct VOUT{\n"
			"float4 position : SV_POSITION;\n"
			"float4 color : COLOR;\n"
			"float fog: FOG;\n"
			"float2 texCoord: TEXCOORD0;\n"
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
		"float4x4 textureMatrix;\n"
		"float fogDensity;\n"
		"float2 fogDistance;\n"

		"VOUT main(VIN vin){\n"
			"VOUT vout;\n"
			"vout.position=mul(modelViewProjectionMatrix,vin.position);\n"
			"float3 viewNormal=normalize(mul(normalMatrix,vin.normal));\n"
			"float lightIntensity=clamp(-dot(lightViewPosition,viewNormal),0.0,1.0);\n"
			"float4 localLightColor=lightIntensity*lightColor*materialLighting+(1.0-materialLighting);\n"
			"vout.color=localLightColor*materialDiffuseColor + ambientColor*materialAmbientColor;\n"
			"vout.color=vin.color*materialTrackColor+vout.color*(1.0-materialTrackColor);\n"
			"vout.texCoord=mul(textureMatrix,float4(vin.texCoord,0.0,1.0));\n "
			"vout.fog=clamp(1.0-fogDensity*(vout.position.z-fogDistance.x)/(fogDistance.y-fogDistance.x),0.0,1.0);\n"
			"return vout;\n"
		"}"
	};

	String diffuseFragmentCode[]={
		"varying vec4 color;\n"
		"varying vec2 texCoord;\n"
		"varying float fog;\n"

		"uniform float textureSet;\n"
		"uniform vec4 fogColor;\n"
		"uniform sampler2D tex;\n"

		"void main(){\n"
			"vec4 fragColor=color*(texture2D(tex,texCoord)+(1.0-textureSet));\n"
			"gl_FragColor=mix(fogColor,fragColor,fog);\n"
		"}",



		"struct PIN{\n"
			"float4 position: SV_POSITION;\n"
			"float4 color: COLOR;\n"
			"float fog: FOG;\n"
			"float2 texCoord: TEXCOORD0;\n"
		"};\n"

		"float textureSet;\n"
		"float4 fogColor;\n"
		"Texture2D tex;\n"
		"SamplerState samp;\n"

		"float4 main(PIN pin): SV_TARGET{\n"
			"float4 fragColor=pin.color*(tex.Sample(samp,pin.texCoord)+(1.0-textureSet));\n"
			"return lerp(fogColor,fragColor,pin.fog);\n"
		"}"
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

	String pointSpriteGeometryCode[]={
		(char*)NULL,

		"struct GIN{\n"
			"float4 position: SV_POSITION;\n"
			"float4 color : COLOR;\n"
			"float fog : FOG;\n"
			"float2 texCoord: TEXCOORD0;\n"
		"};\n"
		"struct GOUT{\n"
			"float4 position: SV_POSITION;\n"
			"float4 color : COLOR;\n"
			"float fog : FOG;\n"
			"float2 texCoord: TEXCOORD0;\n"
		"};\n"

		"float pointSize;\n"
		"float4 viewport;\n"

		"[maxvertexcount(4)]\n"
		"void main(point GIN gin[1],inout TriangleStream<GOUT> stream){\n"
			"float aspect=viewport.w/viewport.z;\n"
			"float w=aspect*pointSize/4.0,h=pointSize/4.0;\n"
			"const float3 positions[4]={\n"
				"float3(-w,-h,0.0),\n"
				"float3(w,-h,0.0),\n"
				"float3(-w,h,0.0),\n"
				"float3(w,h,0.0),\n"
			"};\n"
			"const float2 texCoords[4]={\n"
				"float2(0.0,0.0),\n"
				"float2(1.0,0.0),\n"
				"float2(0.0,1.0),\n"
				"float2(1.0,1.0),\n"
			"};\n"
			"GOUT gout;\n"
			"for(int i=0;i<4;i++){\n"
				"gout.position=gin[0].position+float4(positions[i],0.0);\n"
				"gout.color=gin[0].color;\n"
				"gout.fog=gin[0].fog;\n"
				"gout.texCoord=texCoords[i];\n"
				"stream.Append(gout);\n"
			"}\n"
			"stream.RestartStrip();\n"
		"}"
	};

	mDiffuseVertexShader=getEngine()->getShaderManager()->createShader(Shader::ShaderType_VERTEX,profiles,diffuseVertexCode,2);
	mDiffuseFragmentShader=getEngine()->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,diffuseFragmentCode,2);
	mSkyboxVertexShader=getEngine()->getShaderManager()->createShader(Shader::ShaderType_VERTEX,profiles,skyboxVertexCode,2);
	mSkyboxFragmentShader=getEngine()->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,skyboxFragmentCode,2);
	mPointSpriteGeometryShader=getEngine()->getShaderManager()->createShader(Shader::ShaderType_GEOMETRY,profiles,pointSpriteGeometryCode,2);

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
	if(mDiffuseVertexShader!=NULL){
		mDiffuseVertexShader->release();
		mDiffuseVertexShader=NULL;
	}
	if(mDiffuseFragmentShader!=NULL){
		mDiffuseFragmentShader->release();
		mDiffuseFragmentShader=NULL;
	}
	if(mSkyboxVertexShader!=NULL){
		mSkyboxVertexShader->release();
		mSkyboxVertexShader=NULL;
	}
	if(mSkyboxFragmentShader!=NULL){
		mSkyboxFragmentShader->release();
		mSkyboxFragmentShader=NULL;
	}
	if(mPointSpriteGeometryShader!=NULL){
		mPointSpriteGeometryShader->release();
		mPointSpriteGeometryShader=NULL;
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

bool MaterialManager::isPathUseable(RenderPath *path,const RenderCaps &caps){
	int i;
	for(i=0;i<path->getNumPasses();++i){
		RenderPass *pass=path->getPass(i);
		ShaderState *state=pass->getShaderState();

		/// @todo: Use a new RenderCaps interface to check all types of shader programs
		if(state!=NULL && state->getShader(Shader::ShaderType_VERTEX)!=NULL){
			if(caps.vertexShaders==false){
				break;
			}
		}
		else{
			if(caps.vertexFixedFunction==false){
				break;
			}
		}

		if(state!=NULL && state->getShader(Shader::ShaderType_FRAGMENT)!=NULL){
			if(caps.fragmentShaders==false){
				break;
			}
		}
		else{
			if(caps.fragmentFixedFunction==false){
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

