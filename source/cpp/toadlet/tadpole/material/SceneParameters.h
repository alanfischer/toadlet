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

#ifndef TOADLET_TADPOLE_MATERIAL_SCENEPARAMETERS_H
#define TOADLET_TADPOLE_MATERIAL_SCENEPARAMETERS_H

#include <toadlet/peeper/Viewport.h>
#include <toadlet/peeper/LightState.h>
#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{

class Renderable;
class Scene;
namespace node{class CameraNode;}

namespace material{

class RenderPass;

class SceneParameters{
public:
	TOADLET_SHARED_POINTERS(SceneParameters);

	SceneParameters():
		mScene(NULL),
		mCamera(NULL),
		mRenderable(NULL)
	{
		mTextureState.resize(8);
	}

	inline void setScene(Scene *scene){mScene=scene;}
	inline Scene *getScene() const{return mScene;}

	inline void setCamera(CameraNode *camera){mCamera=camera;}
	inline CameraNode *getCamera() const{return mCamera;}

	inline void setViewport(Viewport viewport){mViewport=viewport;}
	inline const Viewport &getViewport() const{return mViewport;}

	inline void setRenderable(Renderable *renderable){mRenderable=renderable;}
	inline Renderable *getRenderable() const{return mRenderable;}

	inline void setRenderPass(RenderPass *renderPass){mRenderPass=renderPass;}
	inline RenderPass *getRenderPass() const{return mRenderPass;}

	inline void setRenderState(RenderState *renderState){
		renderState->getMaterialState(mMaterialState);
		renderState->getPointState(mPointState);
		renderState->getFogState(mFogState);

		if(mTextureState.size()<renderState->getNumTextureStates()){
			mTextureState.resize(renderState->getNumTextureStates());
		}

		int i;
		for(i=0;i<renderState->getNumTextureStates();++i){
			renderState->getTextureState(i,mTextureState[i]);
		}
	}

	inline const MaterialState &getMaterialState() const{return mMaterialState;}
	inline const TextureState &getTextureState(int i) const{return mTextureState[i];}
	inline const PointState &getPointState() const{return mPointState;}
	inline const FogState &getFogState() const{return mFogState;}

	inline void setLightState(const LightState &state){mLightState.set(state);}
	inline const LightState &getLightState() const{return mLightState;}

	inline void setAmbient(const Vector4 &ambient){mAmbient.set(ambient);}
	inline const Vector4 &getAmbient() const{return mAmbient;}

protected:
	Scene *mScene;
	CameraNode *mCamera;
	Viewport mViewport;
	Renderable *mRenderable;
	RenderPass *mRenderPass;

	MaterialState mMaterialState;
	PointState mPointState;
	FogState mFogState;
	Collection<TextureState> mTextureState;

	LightState mLightState;
	Vector4 mAmbient;
};

}
}
}

#endif
