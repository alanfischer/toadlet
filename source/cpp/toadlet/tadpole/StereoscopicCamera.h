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

#ifndef TOADLET_TADPOLE_STEREOSCOPICCAMERA_H
#define TOADLET_TADPOLE_STEREOSCOPICCAMERA_H

#include <toadlet/tadpole/Camera.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API StereoscopicCamera:public Camera{
public:
	StereoscopicCamera(Engine *engine);

	void destroy();

	void setScope(int scope);

	void setClearFlags(int clearFlags);

	void setClearColor(const Vector4 &clearColor);

	void setSkipFirstClear(bool skip);

	void setDefaultState(RenderState *renderState);

	void setRenderTarget(RenderTarget *target);

	void setViewport(const Viewport &viewport);

	void setSeparation(scalar separation);
	scalar getSeparation() const{return mSeparation;}

	void setCrossEyed(bool crossEyed);
	bool getCrossEyed() const{return mCrossEyed;}

	void render(RenderDevice *device,Scene *scene,Node *node);

protected:
	void projectionUpdated();
	void updateWorldTransform();

	scalar mSeparation;
	bool mCrossEyed;

	Camera::ptr mLeftCamera;
	Camera::ptr mRightCamera;
};

}
}

#endif
