#ifndef SHADOWMAPPING_H
#define SHADOWMAPPING_H

#include <toadlet/peeper/RenderTexture.h>
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/sg/CameraEntity.h>
#include <toadlet/tadpole/material/ShadowMapStandardMaterialRenderer.h>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::sg;
using namespace toadlet::tadpole::material;

class ShadowMapping{
public:
	enum Method{
		METHOD_PLAIN,
		METHOD_SHADOWS,
		METHOD_BUMP_SHADOWS,
	};

	ShadowMapping(){
		mEngine=NULL;
		mConstantBias=0.001f;
		mSlopeBias=1.0f;
	}

	void setup(Engine *engine,RenderWindow *renderWindow,peeper::Renderer *renderer,CameraEntity::Ptr camera,LightEntity::Ptr light,float lightCameraNear,float lightCameraFar){
		mEngine=engine;

		const CapabilitySet &set=renderer->getCapabilitySet();

		unsigned int windowWidth,windowHeight;
		renderWindow->getSize(windowWidth,windowHeight);

		mLightCamera=CameraEntity::make(mEngine);
		if(set.renderToDepthTexture){
#if 1
			unsigned int size=1024;
#elif 1
			unsigned int m=minVal(mWidth,mHeight);
			unsigned int size;
			for(size=1;size<=m;size<<=1);
			size>>=1;
#else
			unsigned int size=set.maxTextureSize;
#endif

			mShadowMapTarget=shared_dynamic_cast<RenderTexture>(mEngine->getTextureManager()->load(new RenderTexture(RenderTexture::FORMAT_DEPTH,size,size)));
			mShadowMapTarget->setMipMap(TMM_NO);
			mShadowMapTarget->setClamp(TC_CLAMP_X|TC_CLAMP_Y);
			mLightCamera->setAutogenViewport();
			mLightCamera->setRenderTarget(mShadowMapTarget);
		}
		else{
			unsigned int width=0,height=0;

			if(set.textureNonPowerOf2==true){
				width=windowWidth;
				height=windowHeight;
			}
			else{
				unsigned int m=minVal(windowWidth,windowHeight);
				for(width=1;width<=m;width<<=1);
				width>>=1;
				height=width;
			}

			mShadowMapTexture=mEngine->getTextureManager()->load(new Texture(Texture::DIMENSION_2D,Texture::FORMAT_DEPTH,width,height));
			mShadowMapTexture->setMipMap(TMM_NO);
			mShadowMapTexture->setClamp(TC_CLAMP_X|TC_CLAMP_Y);
			mLightCamera->setOriginalViewport(Viewport(0,0,width,height));
			mLightCamera->setRenderTarget(renderWindow);
		}

		mLight=light;

		if(mLight->getType()!=peeper::Light::LT_POSITION){
			TOADLET_LOG(NULL,Logger::LEVEL_WARNING) <<
				"Light type not LT_POSITION, shadow mapping will not work";
		}

		mLightCamera->setOriginalFrustum(Frustum(90,1,lightCameraNear,lightCameraFar));
		mLightCamera->setClearColor(Color::COLOR_WHITE);
		mLightCamera->setDepthOnly(true);
		mEngine->getSceneManager()->attach(mLightCamera);

		ShadowMapStandardMaterialRenderer *standard=(ShadowMapStandardMaterialRenderer*)mEngine->getMaterialRenderer(StandardMaterial::getClassMaterialIdentifier());
		if(mShadowMapTarget!=NULL){
			standard->setShadowMapTexture(mShadowMapTarget);
		}
		else{
			standard->setShadowMapTexture(mShadowMapTexture);
		}
		standard->setLightCamera(mLightCamera);
		standard->setViewCamera(camera);
		setMethod(METHOD_PLAIN);
	}

	void render(peeper::Renderer *renderer){
		if(mLight->getType()==peeper::Light::LT_POSITION){
			mLightCamera->setLookAt(mLight->getTranslate(),mLight->getTranslate()-Vector3(0,0,1),Vector3(1,0,0));
		}

		if(mMethod!=METHOD_PLAIN){
			renderer->setDefaultStates();

			renderer->setNormalizeNormals(NT_RESCALE);

			int i;
			for(i=renderer->getCapabilitySet().maxTextureStages-1;i>=0;--i){
				renderer->disableTextureStage(i);
			}
			renderer->setFaceCulling(FC_FRONT);
			renderer->setShading(SHADING_FLAT);
			renderer->setLighting(false);
			renderer->setColorWrite(false);
			renderer->setDepthBias(mConstantBias,mSlopeBias);

			mEngine->getSceneManager()->setActiveCamera(mLightCamera);
			mEngine->getSceneManager()->render(renderer);

			renderer->setDepthBias(0,0);
			renderer->setColorWrite(true);
			renderer->setLighting(true);
			renderer->setShading(SHADING_SMOOTH);
			renderer->setFaceCulling(FC_BACK);

			if(mShadowMapTarget==NULL){
				renderer->copyBufferToTexture(mShadowMapTexture);
			}
		}
	}

	void setDepthBias(float constant,float slope){
		mConstantBias=constant;
		mSlopeBias=slope;
	}

	void setMethod(Method method){
		mMethod=method;
		ShadowMapStandardMaterialRenderer *standard=(ShadowMapStandardMaterialRenderer*)mEngine->getMaterialRenderer(StandardMaterial::getClassMaterialIdentifier());
		if(method==METHOD_PLAIN){
			standard->setPath(ShadowMapStandardMaterialRenderer::PATH_NO_SHADERS_NO_SHADOWS);
		}
		else if(method==METHOD_SHADOWS){
			standard->setPath(ShadowMapStandardMaterialRenderer::PATH_NO_SHADERS_SHADOWS);
		}
		else if(method==METHOD_BUMP_SHADOWS){
			standard->setPath(ShadowMapStandardMaterialRenderer::PATH_SHADERS_SHADOWS);
		}
	}

protected:
	Engine *mEngine;
	RenderTexture::Ptr mShadowMapTarget;
	Texture::Ptr mShadowMapTexture;
	CameraEntity::Ptr mLightCamera;
	float mConstantBias;
	float mSlopeBias;
	Method mMethod;
	LightEntity::Ptr mLight;
};

#endif
