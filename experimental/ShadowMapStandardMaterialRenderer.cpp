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

#include "ShadowMapStandardMaterialRenderer.h"
#include "../../peeper/BlendFunction.h"
#include "../../peeper/CapabilitySet.h"

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace material{

Renderer *HACK_renderer;

Material::MaterialIdentifier *ShadowMapStandardMaterialRenderer::getMaterialIdentifier() const{
	return StandardMaterial::getClassMaterialIdentifier();
}

ShadowMapStandardMaterialRenderer::ShadowMapStandardMaterialRenderer():MaterialRenderer(){
	mEngine=NULL;
	mPath=PATH_NO_SHADERS_NO_SHADOWS;
	mRectangleShadows=false;
	mSceneStates=NULL;
}

ShadowMapStandardMaterialRenderer::~ShadowMapStandardMaterialRenderer(){
}

void ShadowMapStandardMaterialRenderer::setPath(Path path){
	mPath=path;
}

void ShadowMapStandardMaterialRenderer::startRendering(Renderer *renderer){
	mSceneStates=mEngine->getSceneManager()->getSceneStates();
}

int ShadowMapStandardMaterialRenderer::getNumRenderPasses(const Material *material){
	StandardMaterial *standard=(StandardMaterial*)material;

	if(mLightCamera==NULL || mPath==PATH_NO_SHADERS_NO_SHADOWS){
		return 1;
	}
	else if(mPath==PATH_NO_SHADERS_SHADOWS){
		if(standard->isTransparent()){
			return 1;
		}
		else{
			return 2;
		}
	}
	else if(mPath==PATH_SHADERS_SHADOWS){
		return 1;
	}

	return 0;
}

int ShadowMapStandardMaterialRenderer::preRenderPass(const Material *material,int pass,Renderer *renderer){
	StandardMaterial *standard=(StandardMaterial*)material;

	renderer->setLighting(standard->getLightingEnabled());

	if(mSceneStates!=NULL){
		renderer->setAmbientLight(mSceneStates->ambientLight);
	}

	if(mLightCamera==NULL || mPath==PATH_NO_SHADERS_NO_SHADOWS){
		renderer->setLightEffect(standard->getLightEffectWithOpacity());
		renderer->setFaceCulling(standard->getFaceCulling());
		renderer->setTextureStage(0,TextureStage(standard->getDiffuseMap()));
		renderer->disableTextureStage(1);

		if(standard->isTransparent()){
			renderer->setBlendFunction(BC_ALPHA);
			renderer->setColor(Vector4(1,1,1,standard->getOpacity()));
		}
		else{
			renderer->setBlendFunction(DISABLED_BLENDFUNCTION);
		}

		return NOT_RENDERED_PER_LIGHT;
	}
	else if(mPath==PATH_NO_SHADERS_SHADOWS){
		if(standard->isTransparent()){
			renderer->setLightEffect(standard->getLightEffectWithOpacity());
			renderer->setFaceCulling(standard->getFaceCulling());
			renderer->setTextureStage(0,TextureStage(standard->getDiffuseMap()));
			renderer->disableTextureStage(1);

			renderer->setBlendFunction(BC_ALPHA);
			renderer->setColor(Vector4(1,1,1,standard->getOpacity()));

			return NOT_RENDERED_PER_LIGHT;
		}
		else{
			if(pass==0){
				renderer->setLightEffect(standard->getLightEffectWithOpacity());
				renderer->setFaceCulling(standard->getFaceCulling());
				renderer->setTextureStage(0,TextureStage(standard->getDiffuseMap()));
				renderer->disableTextureStage(1);

				renderer->disableAllLights();

				renderer->setBlendFunction(DISABLED_BLENDFUNCTION);

				return NOT_RENDERED_PER_LIGHT;
			}
			else if(pass==1){
				//Bind & enable shadow map texture
				renderer->setTextureStage(0,TextureStage(mShadowMapTexture));

				Matrix4x4 biasMatrix;
				renderer->getShadowBiasMatrix(mShadowMapTexture,biasMatrix);

				// Calculate texture matrix for projection
				// This matrix takes us from eye space to the light's clip space
				Matrix4x4 textureMatrix=biasMatrix*
										mLightCamera->getCamera()->getFrustum().projectionMatrix*
										mLightCamera->getCamera()->getFrustum().viewMatrix*
										makeInverse(mViewCamera->getCamera()->getFrustum().viewMatrix);

				renderer->setTextureCoordinateGeneration(0,TCG_CAMERASPACE,textureMatrix);

				renderer->setLightEffect(standard->getLightEffectWithOpacity());
				renderer->setShadowComparisonFunction(true);
				renderer->setFaceCulling(standard->getFaceCulling());

				renderer->setTextureStage(1,TextureStage(standard->getDiffuseMap()));

				renderer->enableAllLights();

				return NOT_RENDERED_PER_LIGHT;
			}
		}
	}
	else if(mPath==PATH_SHADERS_SHADOWS){
		renderer->setLightEffect(standard->getLightEffectWithOpacity());
		renderer->setFaceCulling(standard->getFaceCulling());		

		renderer->setTextureStage(0,TextureStage(mShadowMapTexture));

		Texture::Ptr texture=standard->getNormalHeightMap();
		if(texture==NULL || texture->getTexturePeer()==NULL){
			texture=mWhiteTexture;
		}
		renderer->setTextureStage(1,TextureStage(texture));

		texture=standard->getDiffuseMap();
		if(texture==NULL || texture->getTexturePeer()==NULL){
			texture=mWhiteTexture;
		}
		renderer->setTextureStage(2,TextureStage(texture));

		texture=standard->getMap(StandardMaterial::MAP_SPECULAR);
		if(texture==NULL || texture->getTexturePeer()==NULL){
			texture=mWhiteTexture;
		}
		renderer->setTextureStage(3,TextureStage(texture));

		Matrix4x4 biasMatrix;
		renderer->getShadowBiasMatrix(mShadowMapTexture,biasMatrix);

		// Calculate texture matrix for projection
		// This matrix takes us from eye space to the light's clip space
		Matrix4x4 textureMatrix=biasMatrix*
								mLightCamera->getCamera()->getFrustum().projectionMatrix*
								mLightCamera->getCamera()->getFrustum().viewMatrix*
								makeInverse(mViewCamera->getCamera()->getFrustum().viewMatrix);

		if(standard->getMethod()==StandardMaterial::METHOD_STANDARD){
			renderer->HACK_preShadowPass1(textureMatrix);
		}
		else if(standard->getMethod()==StandardMaterial::METHOD_BUMP){
			renderer->HACK_preShadowPass2(textureMatrix,standard->getMapAmount(StandardMaterial::MAP_BUMP));
		}
		else if(standard->getMethod()==StandardMaterial::METHOD_PARALLAX){
			renderer->HACK_preShadowPass3(textureMatrix,standard->getMapAmount(StandardMaterial::MAP_BUMP));
		}

		if(standard->isTransparent()){
			renderer->setBlendFunction(BC_ALPHA);
			renderer->setColor(Vector4(1,1,1,standard->getOpacity()));
		}
		else{
			renderer->setBlendFunction(DISABLED_BLENDFUNCTION);
		}

		return NOT_RENDERED_PER_LIGHT;
	}

	return 0;
}

void ShadowMapStandardMaterialRenderer::postRenderPass(const Material *material,int pass,Renderer *renderer){
	StandardMaterial *standard=(StandardMaterial*)material;

	if(mPath==PATH_NO_SHADERS_SHADOWS){
		if(pass==1){
			renderer->setTextureCoordinateGeneration(0,TCG_DISABLED,IDENTITY_MATRIX4X4);

			renderer->setShadowComparisonFunction(false);

			renderer->disableTextureStage(0);
			renderer->disableTextureStage(1);
		}
	}
	else if(mPath==PATH_SHADERS_SHADOWS){
		renderer->HACK_postShadowPass();

		renderer->disableTextureStage(0);
		renderer->disableTextureStage(1);
		renderer->disableTextureStage(2);
		renderer->disableTextureStage(3);
	}
}

void ShadowMapStandardMaterialRenderer::stopRendering(Renderer *renderer){
	mSceneStates=NULL;
}

bool ShadowMapStandardMaterialRenderer::setShadowMapTexture(peeper::Texture::Ptr texture){
	mShadowMapTexture=texture;

	unsigned int x,y;
	mShadowMapTexture->getSize(x,y);
	if((x&(x-1))!=0 || (y&(y-1))!=0){
		// RectangleShadows do not seem to be widely supported
		return false;
		//mRectangleShadows=true;
	}
	else{
		mRectangleShadows=false;
	}

	if(HACK_renderer->getCapabilitySet().fragmentPrograms){
		HACK_renderer->HACK_loadShadowProgram1(GLSL_VP1(),GLSL_FP1());
		HACK_renderer->HACK_loadShadowProgram2(GLSL_VP2(),GLSL_FP2());
		HACK_renderer->HACK_loadShadowProgram3(GLSL_VP3(),GLSL_FP3());
	}

	return false;
}

void ShadowMapStandardMaterialRenderer::setLight(sg::LightEntity::Ptr light){
	mLight=light;
}

void ShadowMapStandardMaterialRenderer::setLightCamera(sg::CameraEntity::Ptr lightCamera){
	mLightCamera=lightCamera;
}

void ShadowMapStandardMaterialRenderer::setViewCamera(sg::CameraEntity::Ptr viewCamera){
	mViewCamera=viewCamera;
}

void ShadowMapStandardMaterialRenderer::rendererRegistered(peeper::Renderer *r){
	const CapabilitySet &caps=r->getCapabilitySet();

	HACK_renderer=r;
}

void ShadowMapStandardMaterialRenderer::rendererUnregistered(peeper::Renderer *r){
}

void ShadowMapStandardMaterialRenderer::internal_registered(Engine *engine){
	mEngine=engine;

	mEngine->registerMaterialIdentifier(StandardMaterial::getClassMaterialIdentifier());
	mMaterialIdentifierIndex=mEngine->getMaterialIdentifierIndex(StandardMaterial::getClassMaterialIdentifier());

	Image *image=new Image(Image::DIMENSION_2D,Image::FORMAT_RGB,Image::TYPE_BYTE,1,1,1);
	image->setPixel(Pixel<uint8>(255,255,255),0,0);
	mWhiteTexture=mEngine->getTextureManager()->load(new Texture(image));
}

void ShadowMapStandardMaterialRenderer::internal_unregistered(Engine *engine){
	mEngine->unregisterMaterialIdentifier(StandardMaterial::getClassMaterialIdentifier());
	mMaterialIdentifierIndex=NULL_MATERIAL_IDENTIFIER_INDEX;
}

#define IF(x) +(x?String(
#define ELSE  ):String(
#define ENDIF ))+

// Regular shading
String ShadowMapStandardMaterialRenderer::GLSL_VP1(){
	return

"attribute vec4 tangent;\n"

"const int shadowTextureIndex=0;\n"
"const int diffuseTextureIndex=2;\n"
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

String ShadowMapStandardMaterialRenderer::GLSL_FP1(){
	return

"uniform sampler2D normalTexture;\n"
"uniform sampler2D diffuseTexture;\n"
"uniform sampler2D specularTexture;\n"
IF(mRectangleShadows)
"uniform sampler2DRectShadow shadowTexture;\n"
ELSE
"uniform sampler2DShadow shadowTexture;\n"
ENDIF

"varying	vec3 g_lightVec;\n"
"varying	vec3 g_viewVec;\n"
"varying	float g_distance;\n"

"void main(){\n"
"	vec3 lightVec=normalize(g_lightVec);\n"
"	vec3 viewVec=normalize(g_viewVec);\n"

"	vec4 diffuseTexColor=texture2D(diffuseTexture,gl_TexCoord[0].xy);\n"
"	vec3 bump=vec3(0.0,0.0,1.0);\n"

"	vec4 specularTexColor=texture2D(specularTexture,gl_TexCoord[0].xy);\n"

IF(mRectangleShadows)
"	vec3 shadow=shadow2DRectProj(shadowTexture,gl_TexCoord[1]).rgb;\n"
ELSE
"	vec3 shadow=shadow2DProj(shadowTexture,gl_TexCoord[1]).rgb;\n"
ENDIF

// Shadow falloff
"	shadow.x=1.0;\n"//shadow.x * clamp(2.0 - 4.0*length((gl_TexCoord[1].st/gl_TexCoord[1].q) - 0.5),0.0,1.0);\n"

"	float attenuation=1.0 / (gl_LightSource[0].constantAttenuation + \n"
"					gl_LightSource[0].linearAttenuation * g_distance + \n"
"					gl_LightSource[0].quadraticAttenuation * g_distance * g_distance);\n"

"	float diffuse=clamp(dot(lightVec,bump),0.0,1.0);\n"

"	float specular=clamp(pow(dot(reflect(-viewVec,bump),lightVec),gl_FrontMaterial.shininess),0.0,1.0);\n"

"	vec3 ambientDiffuseColor = gl_FrontMaterial.ambient.rgb * gl_LightModel.ambient.rgb +\n"
"		shadow.x * attenuation * gl_FrontMaterial.diffuse.rgb * gl_LightSource[0].diffuse.xyz * diffuse;\n"

"	vec3 specularColor = shadow.x * attenuation * gl_FrontMaterial.specular.rgb * gl_LightSource[0].specular.rgb * specular * specularTexColor.rgb;\n"

"	gl_FragColor.xyz=clamp(diffuseTexColor.rgb * ambientDiffuseColor + specularColor,0.0,1.0);\n"
"	gl_FragColor.w=diffuseTexColor.a * gl_FrontMaterial.diffuse.a;\n"
"}\n";
}

// Bumpmaps only
String ShadowMapStandardMaterialRenderer::GLSL_VP2(){
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

String ShadowMapStandardMaterialRenderer::GLSL_FP2(){
	return

"uniform sampler2D normalTexture;\n"
"uniform sampler2D diffuseTexture;\n"
"uniform sampler2D specularTexture;\n"
"uniform float bumpScale;\n"
IF(mRectangleShadows)
"uniform sampler2DRectShadow shadowTexture;\n"
ELSE
"uniform sampler2DShadow shadowTexture;\n"
ENDIF

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

IF(mRectangleShadows)
"	vec3 shadow=shadow2DRectProj(shadowTexture,gl_TexCoord[1]).rgb;\n"
ELSE
"	vec3 shadow=shadow2DProj(shadowTexture,gl_TexCoord[1]).rgb;\n"
ENDIF

// Shadow falloff
"	shadow.x=1.0;\n"//shadow.x * clamp(2.0 - 4.0*length((gl_TexCoord[1].st/gl_TexCoord[1].q) - 0.5),0.0,1.0);\n"

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

// Parallax mapping
String ShadowMapStandardMaterialRenderer::GLSL_VP3(){
	return

"attribute vec4 tangent;\n"

"const int shadowTextureIndex=0;\n"
"const int diffuseTextureIndex=2;\n"
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

String ShadowMapStandardMaterialRenderer::GLSL_FP3(){
	return

"uniform sampler2D normalTexture;\n"
"uniform sampler2D diffuseTexture;\n"
"uniform sampler2D specularTexture;\n"
"uniform float bumpScale;\n"
IF(mRectangleShadows)
"uniform sampler2DRectShadow shadowTexture;\n"
ELSE
"uniform sampler2DShadow shadowTexture;\n"
ENDIF

"uniform vec2 bumpSize;\n"

"varying	vec3 g_lightVec;\n"
"varying	vec3 g_viewVec;\n"
"varying	float g_distance;\n"

"void main(){\n"
"	vec3 lightVec=normalize(g_lightVec);\n"
"	vec3 viewVec=normalize(g_viewVec);\n"
	
"	float height=texture2D(normalTexture,gl_TexCoord[0].xy).a;\n"
"	height=height*bumpSize.x+bumpSize.y;\n"

"	vec2 newUV=gl_TexCoord[0].xy + viewVec.xy * height;\n"
"	vec4 diffuseTexColor=texture2D(diffuseTexture,newUV);\n"
"	vec3 bump=texture2D(normalTexture,newUV.xy).rgb * 2.0 - 1.0;\n"
"	bump.xy*=bumpScale;\n"
"	bump.z+=0.00001;\n"
"	bump=normalize(bump);\n"

"	vec4 specularTexColor=texture2D(specularTexture,gl_TexCoord[0].xy);\n"

IF(mRectangleShadows)
"	vec3 shadow=shadow2DRectProj(shadowTexture,gl_TexCoord[1]).rgb;\n"
ELSE
"	vec3 shadow=shadow2DProj(shadowTexture,gl_TexCoord[1]).rgb;\n"
ENDIF

// Shadow falloff
"	shadow.x=1.0;\n"//shadow.x * clamp(2.0 - 4.0*length((gl_TexCoord[1].st/gl_TexCoord[1].q) - 0.5),0.0,1.0);\n"

"	float attenuation=1.0 / (gl_LightSource[0].constantAttenuation + \n"
"					gl_LightSource[0].linearAttenuation * g_distance + \n"
"					gl_LightSource[0].quadraticAttenuation * g_distance * g_distance);\n"

"	float diffuse=clamp(dot(lightVec,bump),0.0,1.0);\n"

"	float specular=clamp(pow(dot(reflect(-viewVec,bump),lightVec),gl_FrontMaterial.shininess),0.0,1.0);\n"

"	vec3 ambientDiffuseColor = gl_FrontMaterial.ambient.rgb * gl_LightModel.ambient.rgb +\n"
"		shadow.x * attenuation * gl_FrontMaterial.diffuse.rgb * gl_LightSource[0].diffuse.xyz * diffuse;\n"

"	vec3 specularColor = shadow.x * attenuation * gl_FrontMaterial.specular.rgb * gl_LightSource[0].specular.rgb * specular * specularTexColor.rgb;\n"

"	gl_FragColor.xyz=clamp(diffuseTexColor.rgb * ambientDiffuseColor + specularColor,0.0,1.0);\n"
"	gl_FragColor.w=diffuseTexColor.a * gl_FrontMaterial.diffuse.a;\n"
"}\n";
}

}
}
}

