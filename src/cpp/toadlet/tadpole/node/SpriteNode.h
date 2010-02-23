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

#ifndef TOADLET_TADPOLE_NODE_SPRITENODE_H
#define TOADLET_TADPOLE_NODE_SPRITENODE_H

#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/animation/AnimationController.h>
#include <toadlet/tadpole/animation/TextureStageAnimation.h>
#include <toadlet/tadpole/node/Node.h>
#include <toadlet/tadpole/node/Renderable.h>
#include <toadlet/tadpole/node/Sizeable.h>

namespace toadlet{
namespace tadpole{
namespace node{

class Scene;

class TOADLET_API SpriteNode:public Node,public Renderable,public Sizeable{
public:
	TOADLET_NODE(SpriteNode,Node);

	/// Specialization of the AnimationController that allows for easy access to playing sprites.
	class TOADLET_API SpriteAnimationController:public animation::AnimationController{
	public:
		TOADLET_SHARED_POINTERS(SpriteAnimationController);

		SpriteAnimationController(SpriteNode *node);

		virtual void start();
		virtual void stop();

		virtual void logicUpdate(int dt);
		virtual void renderUpdate(int dt);

		void materialChanged();

	protected:
		SpriteNode *mSpriteNode;
		animation::TextureStageAnimation::ptr mAnimation;
		int mStartingFrame;
	};
	
	SpriteNode();
	virtual Node *create(Engine *engine);
	virtual void destroy();

	Renderable *isRenderable(){return this;}
	Sizeable *isSizeable(){return this;}

	void setMaterial(const egg::String &name);
	void setMaterial(Material::ptr material);
	Material::ptr getMaterial() const{return mMaterial;}

	void setPerspective(bool perspective);
	bool getPerspective() const{return mPerspective;}

	void setAlignment(int alignment);
	int getAlignment() const{return mAlignment;}

	void setPixelSpace(bool pixelSpace);
	bool getPixelSpace() const{return mPixelSpace;}

	void setSize(scalar x,scalar y,scalar z);
	void setSize(const Vector3 &size);
	const Vector3 &getSize() const{return mSize;}
	const Vector3 &getDesiredSize(){return Math::ZERO_VECTOR3;}

	SpriteAnimationController::ptr getAnimationController();

	void logicUpdate(int dt);
	void renderUpdate(int dt);
	void queueRenderable(SceneNode *scene,CameraNode *camera);
	Material *getRenderMaterial() const{return mMaterial;}
	const Matrix4x4 &getRenderTransform() const{return mWorldSpriteTransform;}
	void render(peeper::Renderer *renderer) const;

	peeper::VertexBufferAccessor vba;

protected:
	TOADLET_GIB_DEFINE(SpriteNode);

	void updateSprite();
	void updateBound();

	bool mPerspective;
	int mAlignment;
	bool mPixelSpace;
	Vector3 mSize;
	SpriteAnimationController::ptr mAnimationController;

	Material::ptr mMaterial;
	peeper::VertexData::ptr mVertexData;
	peeper::IndexData::ptr mIndexData;
	Matrix4x4 mSpriteTransform;
	Matrix4x4 mWorldSpriteTransform;

	Matrix4x4 cache_queueRenderable_scale;
	Vector4 cache_queueRenderable_point;
};

}
}
}

#endif

