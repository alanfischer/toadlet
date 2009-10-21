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

#ifndef TOADLET_TADPOLE_ENTITY_SPRITEENTITY_H
#define TOADLET_TADPOLE_ENTITY_SPRITEENTITY_H

#include <toadlet/peeper/AnimatedTexture.h>
#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/entity/RenderableEntity.h>

namespace toadlet{
namespace tadpole{
namespace entity{

class Scene;

class TOADLET_API SpriteEntity:public RenderableEntity,public Renderable{
public:
	TOADLET_ENTITY(SpriteEntity,RenderableEntity);

#if defined(TOADLET_GCC_INHERITANCE_BUG)
	class RenderableWorkaround:public Renderable{
	public:
		RenderableWorkaround(SpriteEntity *entity):renderableEntity(entity){}
		SpriteEntity *renderableEntity;
		const Material::ptr &getRenderMaterial() const{return renderableEntity->getRenderMaterial();}
		const Matrix4x4 &getRenderTransform() const{return renderableEntity->getRenderTransform();}
		void render(peeper::Renderer *renderer) const{renderableEntity->render(renderer);}
	};
#endif

	SpriteEntity();
	virtual Entity *create(Engine *engine);
	virtual void destroy();

	void load(scalar width,scalar height,bool scaled,const egg::String &name);
	void load(scalar width,scalar height,bool scaled,peeper::Texture::ptr texture);

	void setFrame(int frame);
	int getNumFrames() const;

	virtual const Material::ptr &getRenderMaterial() const{return mMaterial;}
	virtual const Matrix4x4 &getRenderTransform() const{return super::getVisualWorldTransform();}

	void queueRenderables(Scene *scene);
	void render(peeper::Renderer *renderer) const;

	peeper::VertexBufferAccessor vba;

protected:
	void updateFrame();

#if defined(TOADLET_GCC_INHERITANCE_BUG)
	RenderableWorkaround renderable;
#endif

	peeper::Texture::ptr mTexture;
	peeper::AnimatedTexture::ptr mAnimatedTexture;
	Matrix4x4 mTextureMatrix;

	int mFrame;
	scalar mWidth;
	scalar mHeight;
	bool mScaled;

	Material::ptr mMaterial;
	peeper::VertexData::ptr mVertexData;
	peeper::IndexData::ptr mIndexData;

	Matrix4x4 cache_queueRenderables_scale;
	Vector4 cache_queueRenderables_point;
};

}
}
}

#endif

