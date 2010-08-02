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

#ifndef TOADLET_TADPOLE_ANAGLYPHCAMERANODE_H
#define TOADLET_TADPOLE_ANAGLYPHCAMERANODE_H

#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/SurfaceRenderTarget.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/node/CameraNode.h>

namespace toadlet{
namespace tadpole{
namespace node{

class AnaglyphCameraNode:public CameraNode{
public:
	TOADLET_NODE(AnaglyphCameraNode,CameraNode);

	AnaglyphCameraNode();

	Node *create(Scene *scene);
	void destroy();

	void setSeparation(scalar separation){mSeparation=separation;}
	scalar getSeparation() const{return mSeparation;}

	void setLeftColor(const peeper::Color &color){mLeftMaterial->setLightEffect(peeper::LightEffect(color));}
	const peeper::Color &getLeftColor() const{return mLeftMaterial->getLightEffect().diffuse;}

	void setRightColor(const peeper::Color &color){mRightMaterial->setLightEffect(peeper::LightEffect(color));}
	const peeper::Color &getRightColor() const{return mLeftMaterial->getLightEffect().diffuse;}

	virtual void render(peeper::Renderer *renderer,Node *node);

protected:
	scalar mSeparation;

	peeper::VertexData::ptr mVertexData;
	peeper::IndexData::ptr mIndexData;
	peeper::Texture::ptr mLeftTexture;
	peeper::SurfaceRenderTarget::ptr mLeftRenderTarget;
	Material::ptr mLeftMaterial;
	peeper::Texture::ptr mRightTexture;
	peeper::SurfaceRenderTarget::ptr mRightRenderTarget;
	Material::ptr mRightMaterial;
};

}
}
}

#endif
