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
class Camera;

namespace material{

class RenderPass;

class SceneParameters:public Object{
public:
	TOADLET_OBJECT(SceneParameters);

	SceneParameters():
		mScene(NULL),
		mCamera(NULL),
		mRenderable(NULL)
	{}

	inline void setScene(Scene *scene){mScene=scene;}
	inline Scene *getScene() const{return mScene;}

	inline void setCamera(Camera *camera){mCamera=camera;}
	inline Camera *getCamera() const{return mCamera;}

	inline void setViewport(Viewport viewport){mViewport=viewport;}
	inline const Viewport &getViewport() const{return mViewport;}

	inline void setMatrix(RenderDevice::MatrixType type,const Matrix4x4 &matrix){mMatrixes[type].set(matrix);}
	inline const Matrix4x4 &getMatrix(RenderDevice::MatrixType type) const{return mMatrixes[type];}

	inline void setRenderable(Renderable *renderable){mRenderable=renderable;}
	inline Renderable *getRenderable() const{return mRenderable;}

	inline void setRenderPass(RenderPass *renderPass){mRenderPass=renderPass;}
	inline RenderPass *getRenderPass() const{return mRenderPass;}

	inline void setRenderState(RenderState *renderState){
		renderState->getMaterialState(mMaterialState);
		renderState->getPointState(mPointState);
		renderState->getFogState(mFogState);
	}

	///@todo: Texture states!

	inline const MaterialState &getMaterialState() const{return mMaterialState;}
	inline const PointState &getPointState() const{return mPointState;}
	inline const FogState &getFogState() const{return mFogState;}

	inline void setLightState(const LightState &state){mLightState.set(state);}
	inline const LightState &getLightState() const{return mLightState;}

	inline void setAmbientColor(const Vector4 &color){mAmbientColor.set(color);}
	inline const Vector4 &getAmbientColor() const{return mAmbientColor;}

protected:
	Scene *mScene;
	Camera *mCamera;
	Viewport mViewport;
	Matrix4x4 mMatrixes[RenderDevice::MatrixType_MAX];
	Renderable *mRenderable;
	RenderPass *mRenderPass;

	MaterialState mMaterialState;
	PointState mPointState;
	FogState mFogState;

	LightState mLightState;
	Vector4 mAmbientColor;
};

}
}
}

#endif
