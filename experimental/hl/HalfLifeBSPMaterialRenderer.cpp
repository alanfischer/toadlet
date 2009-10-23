 /********** Copyright header - do not remove **********
 *
 * The Toadlet Engine 
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 * 
 * All source code for the Toadlet Engine, including 
 * this file, is the sole property of Lightning Toads 
 * Productions, LLC. It has been developed on our own 
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed 
 * without our explicit permission. 
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#include "HalfLifeBSPMaterialRenderer.h"
#include <toadlet/peeper/BlendFunction.h>
#include <toadlet/peeper/CapabilitySet.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::material;

namespace hl{

Material::MaterialIdentifier *HalfLifeBSPMaterialRenderer::getMaterialIdentifier() const{
	return HalfLifeBSPMaterial::getClassMaterialIdentifier();
}

HalfLifeBSPMaterialRenderer::HalfLifeBSPMaterialRenderer():MaterialRenderer(){
	mPath=ShadowMapStandardMaterialRenderer::PATH_NO_SHADERS_NO_SHADOWS;
	mEngine=NULL;
}

HalfLifeBSPMaterialRenderer::~HalfLifeBSPMaterialRenderer(){
}

void HalfLifeBSPMaterialRenderer::setPath(ShadowMapStandardMaterialRenderer::Path path){
	mPath=path;
}

void HalfLifeBSPMaterialRenderer::startRendering(Renderer *renderer){
}

int HalfLifeBSPMaterialRenderer::getNumRenderPasses(const Material *material){
	return 1;
}

int HalfLifeBSPMaterialRenderer::preRenderPass(const Material *material,int pass,Renderer *renderer){
	HalfLifeBSPMaterial *hlMaterial=(HalfLifeBSPMaterial*)material;
renderer->setTextureStage(0,TextureStage(hlMaterial->getDiffuseTexture()));
return NOT_RENDERED_PER_LIGHT;

	if(mPath==ShadowMapStandardMaterialRenderer::PATH_NO_SHADERS_NO_SHADOWS){
		renderer->setTextureStage(0,TextureStage(hlMaterial->getDiffuseTexture()));

		return NOT_RENDERED_PER_LIGHT;
	}
	else if(mPath==ShadowMapStandardMaterialRenderer::PATH_NO_SHADERS_SHADOWS){
		renderer->setTextureStage(0,TextureStage(hlMaterial->getDiffuseTexture()));

		return NOT_RENDERED_PER_LIGHT;
	}
	else if(mPath==ShadowMapStandardMaterialRenderer::PATH_SHADERS_SHADOWS){
		renderer->setTextureStage(0,TextureStage(hlMaterial->getDiffuseTexture()));

		// TODO: Implement TextureStage 1 as the lightmap

//		Texture::Ptr bumpTexture=hlMaterial->getBumpmapTexture();
//		if(bumpTexture==NULL || bumpTexture->getTexturePeer()==NULL){
//			renderer->setTextureStage(2,TextureStage(bumpTexture));
//		}

		// TODO: Implement TextureStage 3 as the shadowmap
		/*
		Matrix4x4 biasMatrix;
		renderer->getShadowBiasMatrix(mShadowMapTexture,biasMatrix);

		// Calculate texture matrix for projection
		// This matrix takes us from eye space to the light's clip space
		Matrix4x4 textureMatrix=biasMatrix*
								mLightCamera->getCamera()->getFrustum().projectionMatrix*
								mLightCamera->getCamera()->getFrustum().viewMatrix*
								makeInverse(mViewCamera->getCamera()->getFrustum().viewMatrix);
		*/

//		if(bumpTexture!=NULL){
//			renderer->HACK_preShadowPass5(IDENTITY_MATRIX4X4/*textureMatrix*/,1.0f);
//		}
//		else{
			renderer->HACK_preShadowPass4(IDENTITY_MATRIX4X4/*textureMatrix*/);
//		}

		return NOT_RENDERED_PER_LIGHT;
	}
	else{
		return 0;
	}
}

void HalfLifeBSPMaterialRenderer::postRenderPass(const Material *material,int pass,Renderer *renderer){
renderer->disableTextureStage(0);
return;
	if(mPath==ShadowMapStandardMaterialRenderer::PATH_NO_SHADERS_NO_SHADOWS){
		renderer->disableTextureStage(0);
	}
	else if(mPath==ShadowMapStandardMaterialRenderer::PATH_NO_SHADERS_SHADOWS){
		renderer->disableTextureStage(0);
	}
	else if(mPath==ShadowMapStandardMaterialRenderer::PATH_SHADERS_SHADOWS){
		renderer->HACK_postShadowPass();
	}
}

void HalfLifeBSPMaterialRenderer::stopRendering(Renderer *renderer){
}

void HalfLifeBSPMaterialRenderer::rendererRegistered(Renderer *r){
return;
	if(r->getCapabilitySet().fragmentPrograms){
		r->HACK_loadShadowProgram4(GLSL_VP4(),GLSL_FP4());
//		r->HACK_loadShadowProgram5(GLSL_VP5(),GLSL_FP5());
	}
}

void HalfLifeBSPMaterialRenderer::rendererUnregistered(Renderer *r){
}

void HalfLifeBSPMaterialRenderer::internal_registered(Engine *engine){
	mEngine=engine;

	mEngine->registerMaterialIdentifier(HalfLifeBSPMaterial::getClassMaterialIdentifier());
	mMaterialIdentifierIndex=mEngine->getMaterialIdentifierIndex(HalfLifeBSPMaterial::getClassMaterialIdentifier());
}

void HalfLifeBSPMaterialRenderer::internal_unregistered(Engine *engine){
	mEngine->unregisterMaterialIdentifier(HalfLifeBSPMaterial::getClassMaterialIdentifier());
	mMaterialIdentifierIndex=NULL_MATERIAL_IDENTIFIER_INDEX;
}

#define IF(x) +(x?String(
#define ELSE  ):String(
#define ENDIF ))+

// Regular shading
String HalfLifeBSPMaterialRenderer::GLSL_VP4(){
	return

"attribute vec4 tangent;\n"

"const int diffuseTextureIndex=0;\n"
"const int lightmapTextureIndex=1;\n"
//"const int shadowTextureIndex=3;\n"
//"uniform mat4 shadowGenMatrix;\n"

"varying vec3 g_lightVec;\n"
"varying vec3 g_viewVec;\n"
"varying float g_distance;\n"

"void main(){\n"
"	vec3 binormal=cross(gl_Normal,tangent.xyz)*tangent.w;\n"

"	gl_Position=ftransform();\n"

"	mat3 tbnMatrix=mat3(gl_NormalMatrix*tangent.xyz,gl_NormalMatrix*binormal,gl_NormalMatrix*gl_Normal);\n"
"	vec4 mvVertex=gl_ModelViewMatrix * gl_Vertex;\n"

"	gl_TexCoord[0]=gl_TextureMatrix[diffuseTextureIndex] * gl_MultiTexCoord0;\n"
"	gl_TexCoord[1]=vec4(1,1,0,0);\n"//gl_TextureMatrix[lightmapTextureIndex] * gl_MultiTexCoord1;\n"
//"	gl_TexCoord[1]=gl_TextureMatrix[shadowTextureIndex] * (shadowGenMatrix * mvVertex);\n"

"	g_viewVec=-mvVertex.xyz * tbnMatrix;\n"

"	g_lightVec=(gl_LightSource[0].position.xyz - (mvVertex.xyz*gl_LightSource[0].position.w)) * tbnMatrix;\n"

"	g_distance=length(g_lightVec);\n"
"}\n";
}

String HalfLifeBSPMaterialRenderer::GLSL_FP4(){
	return

"uniform sampler2D diffuseTexture;\n"
"uniform sampler2D lightmapTexture;\n"
//IF(mRectangleShadows)
//"uniform sampler2DRectShadow shadowTexture;\n"
//ELSE
//"uniform sampler2DShadow shadowTexture;\n"
//ENDIF

"varying	vec3 g_lightVec;\n"
"varying	vec3 g_viewVec;\n"
"varying	float g_distance;\n"

"void main(){\n"
"	vec3 lightVec=normalize(g_lightVec);\n"
"	vec3 viewVec=normalize(g_viewVec);\n"

"	vec4 diffuseTexColor=texture2D(diffuseTexture,gl_TexCoord[0].xy);\n"
"	vec3 bump=vec3(0.0,0.0,1.0);\n"

"	vec4 lightmapTexColor=texture2D(lightmapTexture,gl_TexCoord[1].xy);\n"

//IF(mRectangleShadows)
//"	vec3 shadow=shadow2DRectProj(shadowTexture,gl_TexCoord[1]).rgb;\n"
//ELSE
//"	vec3 shadow=shadow2DProj(shadowTexture,gl_TexCoord[1]).rgb;\n"
//ENDIF

// Shadow falloff
//"	shadow.x=1.0;\n"//shadow.x * clamp(2.0 - 4.0*length((gl_TexCoord[1].st/gl_TexCoord[1].q) - 0.5),0.0,1.0);\n"

"	float attenuation=1.0 / (gl_LightSource[0].constantAttenuation + \n"
"					gl_LightSource[0].linearAttenuation * g_distance + \n"
"					gl_LightSource[0].quadraticAttenuation * g_distance * g_distance);\n"

"	float specular=clamp(pow(dot(reflect(-viewVec,bump),lightVec),gl_FrontMaterial.shininess),0.0,1.0);\n"

"	vec3 specularColor = lightmapTexColor.rgb * attenuation * gl_FrontMaterial.specular.rgb * gl_LightSource[0].specular.rgb * specular;\n"

"	gl_FragColor.xyz=clamp(diffuseTexColor.rgb * lightmapTexColor.rgb,0.0,1.0);\n"// + specularColor,0.0,1.0);\n"
"	gl_FragColor.w=diffuseTexColor.a * gl_FrontMaterial.diffuse.a;\n"
"}\n";
}

// Bumpmaps only
String HalfLifeBSPMaterialRenderer::GLSL_VP5(){
	return

"attribute vec4 tangent;\n"

"const int shadowTextureIndex=0;\n"
"const int diffuseTextureIndex=1;\n"
"uniform mat4 shadowGenMatrix;\n"

"varying vec3 g_lightVec;\n"
"varying vec3 g_viewVec;\n"
"varying float g_distance;\n"

"void main(){\n"
"	vec3 binormal=cross(gl_Normal,tangent.xyz)*tangent.w;\n"

"	gl_Position=ftransform();\n"

"	mat3 tbnMatrix=mat3(gl_NormalMatrix*tangent.xyz,gl_NormalMatrix*binormal,gl_NormalMatrix*gl_Normal);\n"
"	vec4 mvVertex=gl_ModelViewMatrix * gl_Vertex;\n"

"	gl_TexCoord[0]=gl_TextureMatrix[diffuseTextureIndex] * gl_MultiTexCoord0;\n"
"	gl_TexCoord[1]=gl_TextureMatrix[shadowTextureIndex] * (shadowGenMatrix * mvVertex);\n"

"	g_viewVec=-mvVertex.xyz * tbnMatrix;\n"

"	g_lightVec=(gl_LightSource[0].position.xyz - (mvVertex.xyz*gl_LightSource[0].position.w)) * tbnMatrix;\n"

"	g_distance=length(g_lightVec);\n"
"}\n";
}

String HalfLifeBSPMaterialRenderer::GLSL_FP5(){
	return

"uniform sampler2D normalTexture;\n"
"uniform sampler2D diffuseTexture;\n"
"uniform sampler2D specularTexture;\n"
"uniform float bumpScale;\n"
//IF(mRectangleShadows)
//"uniform sampler2DRectShadow shadowTexture;\n"
//ELSE
//"uniform sampler2DShadow shadowTexture;\n"
//ENDIF

"varying	vec3 g_lightVec;\n"
"varying	vec3 g_viewVec;\n"
"varying	float g_distance;\n"

"void main(){\n"
"	vec3 lightVec=normalize(g_lightVec);\n"
"	vec3 viewVec=normalize(g_viewVec);\n"

"	vec4 diffuseTexColor=texture2D(diffuseTexture,gl_TexCoord[0].xy);\n"
"	vec3 bump=texture2D(normalTexture,gl_TexCoord[0].xy).rgb * 2.0 - 1.0;\n"
"	bump.xy*=bumpScale;\n"
"	bump.z+=0.00001;\n"
"	bump=normalize(bump);\n"

"	vec4 specularTexColor=texture2D(specularTexture,gl_TexCoord[0].xy);\n"

//IF(mRectangleShadows)
//"	vec3 shadow=shadow2DRectProj(shadowTexture,gl_TexCoord[1]).rgb;\n"
//ELSE
//"	vec3 shadow=shadow2DProj(shadowTexture,gl_TexCoord[1]).rgb;\n"
//ENDIF

// Shadow falloff
//"	shadow.x=1.0;\n"//shadow.x * clamp(2.0 - 4.0*length((gl_TexCoord[1].st/gl_TexCoord[1].q) - 0.5),0.0,1.0);\n"

"	float attenuation=1.0 / (gl_LightSource[0].constantAttenuation + \n"
"					gl_LightSource[0].linearAttenuation * g_distance + \n"
"					gl_LightSource[0].quadraticAttenuation * g_distance * g_distance);\n"

"	float diffuse=clamp(dot(lightVec,bump),0.0,1.0);\n"

"	float specular=clamp(pow(dot(reflect(-viewVec,bump),lightVec),gl_FrontMaterial.shininess),0.0,1.0);\n"

"	vec3 ambientDiffuseColor = gl_FrontMaterial.ambient.rgb * gl_LightModel.ambient.rgb +\n"
"		shadow.x * attenuation * gl_FrontMaterial.diffuse.rgb * gl_LightSource[0].diffuse.xyz * diffuse;\n"

"	vec3 specularColor = shadow.x * attenuation * gl_FrontMaterial.specular.rgb * gl_LightSource[0].specular.rgb * specular * specularTexColor.rgb;\n"

"	gl_FragColor.rgb=clamp(diffuseTexColor.rgb * ambientDiffuseColor + specularColor,0.0,1.0);\n"
"	gl_FragColor.a=diffuseTexColor.a * gl_FrontMaterial.diffuse.a;\n"
"}\n";
}

}
