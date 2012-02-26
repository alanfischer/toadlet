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

#ifndef TOADLET_TADPOLE_SHADOWMAPPEDSCENERENDERER_H
#define TOADLET_TADPOLE_SHADOWMAPPEDSCENERENDERER_H

#include <toadlet/peeper/PixelBufferRenderTarget.h>
#include <toadlet/tadpole/SceneRenderer.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API ShadowMappedSceneRenderer:public SceneRenderer{
public:
	TOADLET_SPTR(ShadowMappedSceneRenderer);

	ShadowMappedSceneRenderer(Scene *scene);
	virtual ~ShadowMappedSceneRenderer();

	void setLight(LightNode *light){mLight=light;}
	LightNode *getLight() const{return mLight;}

	void renderScene(RenderDevice *renderDevice,Node *node,CameraNode *camera);

protected:
	Texture::ptr mShadowTexture;
	PixelBufferRenderTarget::ptr mShadowTarget;
	RenderState::ptr mShadowState,mLightState;
	CameraNode::ptr mLightCamera;
	LightNode::ptr mLight;
};

}
}

#endif
