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
 * it under the terms of the GNU 
 Lesser General Public License as published by
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

#include "DiffuseMaterialCreator.h"
#include <toadlet/tadpole/material/RenderVariables.h>

namespace toadlet{
namespace tadpole{

DiffuseMaterialCreator::DiffuseMaterialCreator(Engine *engine){
	mEngine=engine;

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
		"uniform float materialLight;\n"
		"uniform mat4 textureMatrix;\n"
		"uniform float fogDensity;\n"
		"uniform vec2 fogDistance;\n"

		"void main(){\n"
			"gl_Position=modelViewProjectionMatrix * POSITION;\n"
			"vec3 viewNormal=normalize(normalMatrix * vec4(NORMAL,0.0)).xyz;\n"
			"float lightIntensity=clamp(-dot(lightViewPosition.xyz,viewNormal),0.0,1.0);\n"
			"vec4 localLightColor=(lightIntensity*lightColor*materialLight)+(1.0-materialLight);\n"
			"color=clamp(localLightColor*materialDiffuseColor + ambientColor*materialAmbientColor,0.0,1.0);\n"
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
		"float materialLight;\n"
		"float4x4 textureMatrix;\n"
		"float fogDensity;\n"
		"float2 fogDistance;\n"

		"VOUT main(VIN vin){\n"
			"VOUT vout;\n"
			"vout.position=mul(modelViewProjectionMatrix,vin.position);\n"
			"float3 viewNormal=normalize(mul(normalMatrix,vin.normal));\n"
			"float lightIntensity=clamp(-dot(lightViewPosition,viewNormal),0.0,1.0);\n"
			"float4 localLightColor=lightIntensity*lightColor*materialLight+(1.0-materialLight);\n"
			"vout.color=clamp(localLightColor*materialDiffuseColor + ambientColor*materialAmbientColor,0.0,1.0);\n"
			"vout.color=vin.color*materialTrackColor+vout.color*(1.0-materialTrackColor);\n"
			"vout.texCoord=mul(textureMatrix,float4(vin.texCoord,0.0,1.0));\n "
			"vout.fog=clamp(1.0-fogDensity*(vout.position.z-fogDistance.x)/(fogDistance.y-fogDistance.x),0.0,1.0);\n"
			"return vout;\n"
		"}"
	};

	String diffuseFragmentCode[]={
		"#if defined(GL_ES)\n"
			"precision mediump float;\n"
		"#endif\n"
		
		"varying vec4 color;\n"
		"varying vec2 texCoord;\n"
		"varying float fog;\n"

		"uniform float textureSet;\n"
		"uniform vec4 fogColor;\n"
		"uniform sampler2D texture;\n"

		"void main(){\n"
			"vec4 fragColor=color*(texture2D(texture,texCoord)+(1.0-textureSet));\n"
			"gl_FragColor=mix(fogColor,fragColor,fog);\n"
		"}\n",



		"struct PIN{\n"
			"float4 position: SV_POSITION;\n"
			"float4 color: COLOR;\n"
			"float fog: FOG;\n"
			"float2 texCoord: TEXCOORD0;\n"
		"};\n"

		"float textureSet;\n"
		"float4 fogColor;\n"
		"Texture2D texture;\n"
		"SamplerState samp;\n"

		"float4 main(PIN pin): SV_TARGET{\n"
			"float4 fragColor=pin.color*(texture.Sample(samp,pin.texCoord)+(1.0-textureSet));\n"
			"return lerp(fogColor,fragColor,pin.fog);\n"
		"}\n"
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
		"}\n"
	};

	String pointSpriteFragmentCode[]={
		"#version 120\n"

		"#if defined(GL_ES)\n"
			"precision mediump float;\n"
		"#endif\n"
		
		"varying vec4 color;\n"
		"varying float fog;\n"

		"uniform float textureSet;\n"
		"uniform vec4 fogColor;\n"
		"uniform sampler2D texture;\n"

		"void main(){\n"
			"vec4 fragColor=color*(texture2D(texture,gl_PointCoord)+(1.0-textureSet));\n"
			"gl_FragColor=mix(fogColor,fragColor,fog);\n"
		"}\n",



		"struct PIN{\n"
			"float4 position: SV_POSITION;\n"
			"float4 color: COLOR;\n"
			"float fog: FOG;\n"
			"float2 texCoord: TEXCOORD0;\n"
		"};\n"

		"float textureSet;\n"
		"float4 fogColor;\n"
		"Texture2D texture;\n"
		"SamplerState samp;\n"

		"float4 main(PIN pin): SV_TARGET{\n"
			"float4 fragColor=pin.color*(texture.Sample(samp,pin.texCoord)+(1.0-textureSet));\n"
			"return lerp(fogColor,fragColor,pin.fog);\n"
		"}\n"
	};

	TOADLET_TRY
		mDiffuseVertexShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_VERTEX,profiles,diffuseVertexCode,2);
	TOADLET_CATCH_ANONYMOUS(){}
	TOADLET_TRY
		mDiffuseFragmentShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,diffuseFragmentCode,2);
	TOADLET_CATCH_ANONYMOUS(){}
	TOADLET_TRY
		mPointSpriteGeometryShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_GEOMETRY,profiles,pointSpriteGeometryCode,2);
	TOADLET_CATCH_ANONYMOUS(){}
	TOADLET_TRY
		mPointSpriteFragmentShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,pointSpriteFragmentCode,2);
	TOADLET_CATCH_ANONYMOUS(){}

	mDiffuseShaderState=mEngine->getMaterialManager()->createShaderState();
	if(mDiffuseShaderState!=NULL){
		mDiffuseShaderState->setShader(Shader::ShaderType_VERTEX,mDiffuseVertexShader);
		mDiffuseShaderState->setShader(Shader::ShaderType_FRAGMENT,mDiffuseFragmentShader);
	}

	mPointShaderState=mEngine->getMaterialManager()->createShaderState();
	if(mPointShaderState!=NULL){
		mPointShaderState->setShader(Shader::ShaderType_VERTEX,mDiffuseVertexShader);
		mPointShaderState->setShader(Shader::ShaderType_GEOMETRY,mPointSpriteGeometryShader);
		mPointShaderState->setShader(Shader::ShaderType_FRAGMENT,mPointSpriteFragmentShader);
	}
}

void DiffuseMaterialCreator::destroy(){
	if(mDiffuseVertexShader!=NULL){
		mDiffuseVertexShader->destroy();
		mDiffuseVertexShader=NULL;
	}
	if(mDiffuseFragmentShader!=NULL){
		mDiffuseFragmentShader->destroy();
		mDiffuseFragmentShader=NULL;
	}
	if(mPointSpriteGeometryShader!=NULL){
		mPointSpriteGeometryShader->destroy();
		mPointSpriteGeometryShader=NULL;
	}
	if(mPointSpriteFragmentShader!=NULL){
		mPointSpriteFragmentShader->destroy();
		mPointSpriteFragmentShader=NULL;
	}

	if(mDiffuseShaderState!=NULL){
		mDiffuseShaderState->destroy();
		mDiffuseShaderState=NULL;
	}
	if(mPointShaderState!=NULL){
		mPointShaderState->destroy();
		mPointShaderState=NULL;
	}
}

Resource::ptr DiffuseMaterialCreator::create(const String &name,ResourceData *data,ProgressListener *listener){
	return mEngine->getMaterialManager()->find(name);
}

Material::ptr DiffuseMaterialCreator::createDiffuseMaterial(Texture *texture){
	Material::ptr material=new Material(mEngine->getMaterialManager());

	RenderState::ptr renderState=mEngine->getMaterialManager()->createRenderState();
	renderState->setBlendState(BlendState());
	renderState->setDepthState(DepthState());
	renderState->setRasterizerState(RasterizerState());
	renderState->setMaterialState(MaterialState(true,false,MaterialState::ShadeType_GOURAUD));

	if(mEngine->hasShader(Shader::ShaderType_VERTEX) && mEngine->hasShader(Shader::ShaderType_FRAGMENT)){
		RenderPath::ptr shaderPath=material->addPath();

		RenderPass::ptr pass=shaderPath->addPass(renderState,mDiffuseShaderState);

		RenderVariableSet::ptr variables=pass->makeVariables();
		variables->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
		variables->addVariable("normalMatrix",RenderVariable::ptr(new NormalMatrixVariable()),Material::Scope_RENDERABLE);
		variables->addVariable("lightViewPosition",RenderVariable::ptr(new LightViewPositionVariable()),Material::Scope_MATERIAL);
		variables->addVariable("lightColor",RenderVariable::ptr(new LightDiffuseVariable()),Material::Scope_MATERIAL);
		variables->addVariable("ambientColor",RenderVariable::ptr(new AmbientVariable()),Material::Scope_RENDERABLE);
		variables->addVariable("materialLight",RenderVariable::ptr(new MaterialLightVariable()),Material::Scope_MATERIAL);
		variables->addVariable("materialDiffuseColor",RenderVariable::ptr(new MaterialDiffuseVariable()),Material::Scope_MATERIAL);
		variables->addVariable("materialAmbientColor",RenderVariable::ptr(new MaterialAmbientVariable()),Material::Scope_MATERIAL);
		variables->addVariable("materialTrackColor",RenderVariable::ptr(new MaterialTrackColorVariable()),Material::Scope_MATERIAL);
		variables->addVariable("fogDensity",RenderVariable::ptr(new FogDensityVariable()),Material::Scope_MATERIAL);
		variables->addVariable("fogDistance",RenderVariable::ptr(new FogDistanceVariable()),Material::Scope_MATERIAL);
		variables->addVariable("fogColor",RenderVariable::ptr(new FogColorVariable()),Material::Scope_MATERIAL);
		variables->addVariable("textureMatrix",RenderVariable::ptr(new TextureMatrixVariable("tex")),Material::Scope_MATERIAL);
		variables->addVariable("textureSet",RenderVariable::ptr(new TextureSetVariable("tex")),Material::Scope_MATERIAL);

		if(texture!=NULL){
			variables->addTexture("texture",texture,"samp",mEngine->getMaterialManager()->getDefaultSamplerState(),TextureState());
		}
	}

	if(mEngine->hasFixed(Shader::ShaderType_VERTEX) && mEngine->hasFixed(Shader::ShaderType_FRAGMENT)){
		RenderPath::ptr fixedPath=material->addPath("fixed");

		RenderPass::ptr pass=fixedPath->addPass(renderState);

		if(texture!=NULL){
			pass->setTexture(Shader::ShaderType_FRAGMENT,0,texture,mEngine->getMaterialManager()->getDefaultSamplerState(),TextureState());
		}
	}

	material->compile();

	return material;
}

Material::ptr DiffuseMaterialCreator::createPointSpriteMaterial(Texture *texture,scalar size,bool attenuated){
	Material::ptr material(new Material(mEngine->getMaterialManager()));

	RenderState::ptr renderState=mEngine->getMaterialManager()->createRenderState();
	renderState->setBlendState(BlendState());
	renderState->setDepthState(DepthState());
	renderState->setRasterizerState(RasterizerState());
	renderState->setMaterialState(MaterialState(true,false,MaterialState::ShadeType_GOURAUD));
	/// @todo: We need to sort out how to handle the case in GL where you can have Geometry Shaders and PointSprites both functional.
	/// Though I suppose that would be in the GLRenderDevice, it would deactivate PointSprites if Geometry Shaders are used.
	renderState->setPointState(PointState(true,size,attenuated));

	if(	mEngine->hasShader(Shader::ShaderType_VERTEX) &&
		mEngine->hasShader(Shader::ShaderType_FRAGMENT)	&& 
		mEngine->hasShader(Shader::ShaderType_GEOMETRY))
	{
		RenderPath::ptr shaderPath=material->addPath("shader");

		RenderPass::ptr pass=shaderPath->addPass(renderState,mPointShaderState);

		RenderVariableSet::ptr variables=pass->makeVariables();
		variables->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
		variables->addVariable("normalMatrix",RenderVariable::ptr(new NormalMatrixVariable()),Material::Scope_RENDERABLE);
		variables->addVariable("lightViewPosition",RenderVariable::ptr(new LightViewPositionVariable()),Material::Scope_MATERIAL);
		variables->addVariable("lightColor",RenderVariable::ptr(new LightDiffuseVariable()),Material::Scope_MATERIAL);
		variables->addVariable("ambientColor",RenderVariable::ptr(new AmbientVariable()),Material::Scope_RENDERABLE);
		variables->addVariable("materialLight",RenderVariable::ptr(new MaterialLightVariable()),Material::Scope_MATERIAL);
		variables->addVariable("materialDiffuseColor",RenderVariable::ptr(new MaterialDiffuseVariable()),Material::Scope_MATERIAL);
		variables->addVariable("materialAmbientColor",RenderVariable::ptr(new MaterialAmbientVariable()),Material::Scope_MATERIAL);
		variables->addVariable("materialTrackColor",RenderVariable::ptr(new MaterialTrackColorVariable()),Material::Scope_MATERIAL);
		variables->addVariable("fogDensity",RenderVariable::ptr(new FogDensityVariable()),Material::Scope_MATERIAL);
		variables->addVariable("fogDistance",RenderVariable::ptr(new FogDistanceVariable()),Material::Scope_MATERIAL);
		variables->addVariable("fogColor",RenderVariable::ptr(new FogColorVariable()),Material::Scope_MATERIAL);
		variables->addVariable("textureMatrix",RenderVariable::ptr(new TextureMatrixVariable("tex")),Material::Scope_MATERIAL);
		variables->addVariable("textureSet",RenderVariable::ptr(new TextureSetVariable("tex")),Material::Scope_MATERIAL);
		variables->addVariable("pointSize",RenderVariable::ptr(new PointSizeVariable()),Material::Scope_MATERIAL);
		variables->addVariable("viewport",RenderVariable::ptr(new ViewportVariable()),Material::Scope_MATERIAL);

		if(texture!=NULL){
			variables->addTexture("texture",texture,"samp",mEngine->getMaterialManager()->getDefaultSamplerState(),TextureState());
		}
	}

	if(	mEngine->hasFixed(Shader::ShaderType_VERTEX) &&
		mEngine->hasFixed(Shader::ShaderType_FRAGMENT) &&
		mEngine->hasFixed(Shader::ShaderType_GEOMETRY))
	{
		RenderPath::ptr fixedPath=material->addPath("fixed");

		RenderPass::ptr pass=fixedPath->addPass(renderState);

		if(texture!=NULL){
			pass->setTexture(Shader::ShaderType_FRAGMENT,0,texture,mEngine->getMaterialManager()->getDefaultSamplerState(),TextureState());
		}
	}

	material->compile();

	return material;
}

Material::ptr DiffuseMaterialCreator::createFontMaterial(Font *font){
	if(font==NULL){
		return NULL;
	}

	Material::ptr material(new Material(mEngine->getMaterialManager()));

	Texture::ptr texture=font->getTexture();

	RenderState::ptr renderState=mEngine->getMaterialManager()->createRenderState();
	renderState->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
	renderState->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
	renderState->setMaterialState(MaterialState(true,true,MaterialState::ShadeType_FLAT));
	if(TextureFormat::getAlphaBits(texture->getFormat()->getPixelFormat())>0){
		renderState->setBlendState(BlendState::Combination_ALPHA);
	}
	else{
		renderState->setBlendState(BlendState::Combination_COLOR);
	}

	if(mEngine->hasShader(Shader::ShaderType_VERTEX) &&
		mEngine->hasShader(Shader::ShaderType_FRAGMENT)
	){
		RenderPath::ptr shaderPath=material->addPath("shader");
	
		RenderPass::ptr pass=shaderPath->addPass(renderState,mDiffuseShaderState);

		RenderVariableSet::ptr variables=pass->makeVariables();
		variables->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
		variables->addVariable("normalMatrix",RenderVariable::ptr(new NormalMatrixVariable()),Material::Scope_RENDERABLE);
		variables->addVariable("lightViewPosition",RenderVariable::ptr(new LightViewPositionVariable()),Material::Scope_MATERIAL);
		variables->addVariable("lightColor",RenderVariable::ptr(new LightDiffuseVariable()),Material::Scope_MATERIAL);
		variables->addVariable("ambientColor",RenderVariable::ptr(new AmbientVariable()),Material::Scope_RENDERABLE);
		variables->addVariable("materialLight",RenderVariable::ptr(new MaterialLightVariable()),Material::Scope_MATERIAL);
		variables->addVariable("materialDiffuseColor",RenderVariable::ptr(new MaterialDiffuseVariable()),Material::Scope_MATERIAL);
		variables->addVariable("materialAmbientColor",RenderVariable::ptr(new MaterialAmbientVariable()),Material::Scope_MATERIAL);
		variables->addVariable("materialTrackColor",RenderVariable::ptr(new MaterialTrackColorVariable()),Material::Scope_MATERIAL);
		variables->addVariable("fogDensity",RenderVariable::ptr(new FogDensityVariable()),Material::Scope_MATERIAL);
		variables->addVariable("fogDistance",RenderVariable::ptr(new FogDistanceVariable()),Material::Scope_MATERIAL);
		variables->addVariable("fogColor",RenderVariable::ptr(new FogColorVariable()),Material::Scope_MATERIAL);
		variables->addVariable("textureMatrix",RenderVariable::ptr(new TextureMatrixVariable("tex")),Material::Scope_MATERIAL);
		variables->addVariable("textureSet",RenderVariable::ptr(new TextureSetVariable("tex")),Material::Scope_MATERIAL);

		variables->addTexture("texture",texture,"samp",mEngine->getMaterialManager()->getDefaultSamplerState(),TextureState());
	}

	if(	mEngine->hasFixed(Shader::ShaderType_VERTEX) &&
		mEngine->hasFixed(Shader::ShaderType_FRAGMENT)
	){
		RenderPath::ptr fixedPath=material->addPath("fixed");

		RenderPass::ptr pass=fixedPath->addPass(renderState);

		pass->setTexture(Shader::ShaderType_FRAGMENT,0,texture,mEngine->getMaterialManager()->getDefaultSamplerState(),TextureState());
	}

	mEngine->getMaterialManager()->manage(material);

	material->compile();

	return material;
}

}
}
