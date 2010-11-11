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

#ifndef TOADLET_TADPOLE_NODE_CAMERANODE_H
#define TOADLET_TADPOLE_NODE_CAMERANODE_H

#include <toadlet/peeper/Color.h>
#include <toadlet/peeper/Viewport.h>
#include <toadlet/peeper/Renderer.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/node/ParentNode.h>

namespace toadlet{
namespace tadpole{
namespace node{

class TOADLET_API CameraNode:public ParentNode{
public:
	TOADLET_NODE(CameraNode,ParentNode);

	enum ProjectionType{
		ProjectionType_FOVX=0,
		ProjectionType_FOVY,
		ProjectionType_ORTHO,
		ProjectionType_FRUSTUM,
		ProjectionType_MATRIX
	};

	CameraNode();
	virtual Node *create(Scene *scene);
	virtual void destroy();

	inline ProjectionType getProjectionType() const{return mProjectionType;}

	virtual void setProjectionFovX(scalar fovx,scalar aspect,scalar nearDist,scalar farDist);
	virtual void setProjectionFovY(scalar fovy,scalar aspect,scalar nearDist,scalar farDist);
	virtual void setProjectionOrtho(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist);
	virtual void setProjectionFrustum(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist);
	inline scalar getFov() const{return mFov;}
	inline scalar getAspect() const{return mAspect;}
	inline scalar getLeftDist() const{return mLeftDist;}
	inline scalar getRightDist() const{return mRightDist;}
	inline scalar getBottomDist() const{return mBottomDist;}
	inline scalar getTopDist() const{return mTopDist;}
	inline scalar getNearDist() const{return mNearDist;}
	inline scalar getFarDist() const{return mFarDist;}

	virtual void setProjectionTransform(const Matrix4x4 &transform);
	inline const Matrix4x4 &getProjectionTransform() const{return mFinalProjectionTransform;}

	virtual void setProjectionRotation(scalar rotate);
	virtual void setNearAndFarDist(scalar nearDist,scalar farDist);

	virtual void setAlignmentCalculationsUseOrigin(bool origin){mAlignmentCalculationsUseOrigin=origin;}
	virtual bool getAlignmentCalculationsUseOrigin() const{return mAlignmentCalculationsUseOrigin;}

	virtual void setLookAt(const Vector3 &eye,const Vector3 &point,const Vector3 &up);
	virtual void setWorldLookAt(const Vector3 &eye,const Vector3 &point,const Vector3 &up);
	virtual void setLookDir(const Vector3 &eye,const Vector3 &dir,const Vector3 &up);
	virtual void setWorldLookDir(const Vector3 &eye,const Vector3 &dir,const Vector3 &up);

	virtual void setViewport(const peeper::Viewport &viewport);
	virtual void setViewport(int x,int y,int width,int height);
	inline bool getViewportSet() const{return mViewportSet;}
	inline const peeper::Viewport &getViewport() const{return mViewport;}
	inline int getViewportX() const{return mViewport.x;}
	inline int getViewportY() const{return mViewport.y;}
	inline int getViewportWidth() const{return mViewport.width;}
	inline int getViewportHeight() const{return mViewport.height;}

	virtual void setClearFlags(int clearFlags){mClearFlags=clearFlags;}
	inline int getClearFlags() const{return mClearFlags;}

	virtual void setClearColor(peeper::Color clearColor){mClearColor.set(clearColor);}
	inline peeper::Color getClearColor() const{return mClearColor;}

	virtual void setSkipFirstClear(bool skip){mSkipFirstClear=skip;}
	inline bool getSkipFirstClear() const{return mSkipFirstClear;}

	virtual void setGamma(scalar gamma);
	virtual scalar getGamma() const{return mGamma;}

	inline const Matrix4x4 &getViewTransform() const{return mViewTransform;}
	inline const Vector3 &getForward() const{return mForward;}
	inline const Vector3 &getRight() const{return mRight;}

	virtual void render(peeper::Renderer *renderer,Node *node=NULL);

	virtual bool culled(Node *node) const;
	virtual bool culled(const Bound &bound) const;
	/// @todo: These should probably be moved into the Math library, and passing in a list of planes
	virtual bool culled(const Sphere &sphere) const;
	virtual bool culled(const AABox &box) const;

	virtual void updateFramesPerSecond();
	inline scalar getFramesPerSecond() const{return mFPS;}

	virtual void updateWorldTransform();

protected:
	virtual void projectionUpdated();
	virtual void updateViewTransform();
	virtual void renderOverlayGamma(peeper::Renderer *renderer);

	ProjectionType mProjectionType;
	scalar mFov,mAspect;
	scalar mLeftDist,mRightDist;
	scalar mBottomDist,mTopDist;
	scalar mNearDist,mFarDist;
	bool mViewportSet;
	peeper::Viewport mViewport;
	int mClearFlags;
	peeper::Color mClearColor;
	bool mSkipFirstClear;
	bool mAlignmentCalculationsUseOrigin;

	Matrix4x4 mProjectionTransform;
	scalar mProjectionRotation;
	Matrix4x4 mFinalProjectionTransform;
	Matrix4x4 mViewTransform;
	Matrix4x4 mViewProjectionTransform;
	Plane mClipPlanes[6];
	Vector3 mForward,mRight;

	Matrix4x4 mOverlayMatrix;
	peeper::VertexData::ptr mOverlayVertexData;
	peeper::IndexData::ptr mOverlayIndexData;
	scalar mGamma;
	Material::ptr mGammaMaterial;

	int mFPSLastTime;
	int mFPSFrameCount;
	scalar mFPS;

	Vector3 cache_culled_vertex;
};

}
}
}

#endif

