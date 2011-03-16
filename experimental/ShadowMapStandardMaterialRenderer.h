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

#ifndef TOADLET_TADPOLE_MATERIAL_SHADOWMAPSTANDARDMATERIALRENDERER_H
#define TOADLET_TADPOLE_MATERIAL_SHADOWMAPSTANDARDMATERIALRENDERER_H

#include "StandardMaterial.h"
#include "MaterialRenderer.h"
#include "../Engine.h"
#include "../sg/LightEntity.h"
#include "../sg/CameraEntity.h"

namespace toadlet{
namespace tadpole{
namespace material{

class TOADLET_API ShadowMapStandardMaterialRenderer:public MaterialRenderer{
public:
	enum Path{
		PATH_NO_SHADERS_NO_SHADOWS,
		PATH_NO_SHADERS_SHADOWS,
		PATH_SHADERS_SHADOWS,
	};

	ShadowMapStandardMaterialRenderer();
	virtual ~ShadowMapStandardMaterialRenderer();

	Material::MaterialIdentifier *getMaterialIdentifier() const;

	void setPath(Path path);
	inline Path getPath() const{return mPath;}

	void startRendering(peeper::Renderer *renderer);
	int getNumRenderPasses(const Material *material);
	int preRenderPass(const Material *material,int pass,peeper::Renderer *renderer);
	void postRenderPass(const Material *material,int pass,peeper::Renderer *renderer);
	void stopRendering(peeper::Renderer *renderer);

	bool setShadowMapTexture(peeper::Texture::Ptr texture);
	void setLight(sg::LightEntity::Ptr light);
	void setLightCamera(sg::CameraEntity::Ptr lightCamera);
	void setViewCamera(sg::CameraEntity::Ptr viewCamera);

	void rendererRegistered(peeper::Renderer *r);
	void rendererUnregistered(peeper::Renderer *r);

	void internal_registered(Engine *engine);
	void internal_unregistered(Engine *engine);

protected:
	egg::String GLSL_VP1();
	egg::String GLSL_FP1();

	egg::String GLSL_VP2();
	egg::String GLSL_FP2();

	egg::String GLSL_VP3();
	egg::String GLSL_FP3();

	Path mPath;

	peeper::Texture::Ptr mWhiteTexture;

	peeper::Texture::Ptr mShadowMapTexture;
	sg::LightEntity::Ptr mLight;
	sg::CameraEntity::Ptr mLightCamera;
	sg::CameraEntity::Ptr mViewCamera;

	Engine *mEngine;
	SceneStates *mSceneStates;
	bool mRectangleShadows;
};

}
}
}

#endif
