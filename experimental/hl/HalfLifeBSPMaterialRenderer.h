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

#ifndef HL_HALFLIFEBSPMATERIALRENDERER_H
#define HL_HALFLIFEBSPMATERIALRENDERER_H

#include "HalfLifeBSPMaterial.h"
#include <toadlet/tadpole/material/MaterialRenderer.h>
#include <toadlet/tadpole/material/ShadowMapStandardMaterialRenderer.h>
#include <toadlet/tadpole/Engine.h>

namespace hl{

class TOADLET_API HalfLifeBSPMaterialRenderer:public toadlet::tadpole::material::MaterialRenderer{
public:
	HalfLifeBSPMaterialRenderer();
	virtual ~HalfLifeBSPMaterialRenderer();

	toadlet::tadpole::material::Material::MaterialIdentifier *getMaterialIdentifier() const;

	void setPath(toadlet::tadpole::material::ShadowMapStandardMaterialRenderer::Path path);
	toadlet::tadpole::material::ShadowMapStandardMaterialRenderer::Path getPath() const{return mPath;}

	void startRendering(toadlet::peeper::Renderer *renderer);
	int getNumRenderPasses(const toadlet::tadpole::material::Material *material);
	int preRenderPass(const toadlet::tadpole::material::Material *material,int pass,toadlet::peeper::Renderer *renderer);
	void postRenderPass(const toadlet::tadpole::material::Material *material,int pass,toadlet::peeper::Renderer *renderer);
	void stopRendering(toadlet::peeper::Renderer *renderer);

	void rendererRegistered(toadlet::peeper::Renderer *r);
	void rendererUnregistered(toadlet::peeper::Renderer *r);

	void internal_registered(toadlet::tadpole::Engine *engine);
	void internal_unregistered(toadlet::tadpole::Engine *engine);

protected:
	toadlet::egg::String GLSL_VP4();
	toadlet::egg::String GLSL_FP4();
	toadlet::egg::String GLSL_VP5();
	toadlet::egg::String GLSL_FP5();

	toadlet::tadpole::Engine *mEngine;

	toadlet::tadpole::material::ShadowMapStandardMaterialRenderer::Path mPath;
};

}

#endif
