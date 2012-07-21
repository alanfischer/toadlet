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

#ifndef TOADLET_TADPOLE_ANAGLYPHCAMERA_H
#define TOADLET_TADPOLE_ANAGLYPHCAMERA_H

#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/PixelBufferRenderTarget.h>
#include <toadlet/tadpole/Camera.h>
#include <toadlet/tadpole/Engine.h>

namespace toadlet{
namespace tadpole{

class AnaglyphCamera:public Camera{
public:
	AnaglyphCamera(Engine *engine);
	virtual ~AnaglyphCamera();

	void destroy();

	void setSeparation(scalar separation){mSeparation=separation;}
	scalar getSeparation() const{return mSeparation;}

	void setLeftColor(const Vector4 &color);
	const Vector4 &getLeftColor() const{return mLeftColor;}

	void setRightColor(const Vector4 &color);
	const Vector4 &getRightColor() const{return mRightColor;}

	virtual void render(RenderDevice *device,Scene *scene);

protected:
	scalar mSeparation;

	Texture::ptr mLeftTexture;
	PixelBufferRenderTarget::ptr mLeftRenderTarget;
	Vector4 mLeftColor;
	Material::ptr mLeftMaterial;
	Texture::ptr mRightTexture;
	PixelBufferRenderTarget::ptr mRightRenderTarget;
	Vector4 mRightColor;
	Material::ptr mRightMaterial;
};

}
}

#endif
