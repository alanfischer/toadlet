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

#ifndef TOADLET_TADPOLE_CAMERA_H
#define TOADLET_TADPOLE_CAMERA_H

#include <toadlet/peeper/Viewport.h>
#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/peeper/RenderTarget.h>
#include <toadlet/peeper/RenderState.h>
#include <toadlet/tadpole/Bound.h>

namespace toadlet{
namespace tadpole{

class Scene;
class Node;

class TOADLET_API Camera:public Object{
public:
	TOADLET_OBJECT(Camera);

	enum ProjectionType{
		ProjectionType_FOV=0,
		ProjectionType_FOVX,
		ProjectionType_FOVY,
		ProjectionType_ORTHO,
		ProjectionType_FRUSTUM,
		ProjectionType_MATRIX
	};

	Camera();
	virtual ~Camera(){}

	virtual void setScope(int scope){mScope=scope;}
	inline int getScope() const{return mScope;}

	inline ProjectionType getProjectionType() const{return mProjectionType;}

	// Automatically adjust fov for the RenderTarget size
	void setAutoProjectionFov(scalar fov,scalar nearDist,scalar farDist);

	void setProjectionFovX(scalar fovx,scalar aspect,scalar nearDist,scalar farDist);
	void setProjectionFovY(scalar fovy,scalar aspect,scalar nearDist,scalar farDist);
	void setProjectionOrtho(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist);
	void setProjectionFrustum(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist);
	inline scalar getFov() const{return mFov;}
	inline scalar getAspect() const{return mAspect;}
	inline scalar getLeftDist() const{return mLeftDist;}
	inline scalar getRightDist() const{return mRightDist;}
	inline scalar getBottomDist() const{return mBottomDist;}
	inline scalar getTopDist() const{return mTopDist;}
	inline scalar getNearDist() const{return mNearDist;}
	inline scalar getFarDist() const{return mFarDist;}
	
	void setProjectionMatrix(const Matrix4x4 &matrix);
	inline const Matrix4x4 &getProjectionMatrix() const{return mFinalProjectionMatrix;}

	void setPosition(const Vector3 &position);
	inline const Vector3 &getPosition() const{return mPosition;}

	void setNearAndFarDist(scalar nearDist,scalar farDist);

	void setAlignmentCalculationsUseOrigin(bool origin){mAlignmentCalculationsUseOrigin=origin;}
	bool getAlignmentCalculationsUseOrigin() const{return mAlignmentCalculationsUseOrigin;}

	void setLookAt(const Vector3 &eye,const Vector3 &point,const Vector3 &up);
	void setLookDir(const Vector3 &eye,const Vector3 &dir,const Vector3 &up);
	void setWorldMatrix(const Matrix4x4 &matrix);

	virtual void setRenderTarget(RenderTarget *target);
	RenderTarget *getRenderTarget(){return mRenderTarget;}

	bool ready() const{return mRenderTarget==NULL || mRenderTarget->getRootRenderTarget()!=NULL;}

	virtual void setViewport(const Viewport &viewport){mViewport.set(viewport);}
	inline const Viewport &getViewport() const{return mViewport;}

	virtual void setClearFlags(int clearFlags){mClearFlags=clearFlags;}
	inline int getClearFlags() const{return mClearFlags;}

	virtual void setClearColor(const Vector4 &clearColor){mClearColor.set(clearColor);}
	inline const Vector4 &getClearColor() const{return mClearColor;}

	virtual void setSkipFirstClear(bool skip){mSkipFirstClear=skip;}
	inline bool getSkipFirstClear() const{return mSkipFirstClear;}

	virtual void setDefaultState(RenderState *renderState){mDefaultState=renderState;}
	inline RenderState* getDefaultState() const{return mDefaultState;}

	void setObliqueNearPlaneMatrix(const Matrix4x4 &oblique);
	void getObliqueNearPlaneMatrix(Matrix4x4 &oblique){oblique.set(mObliqueMatrix);}

	inline const Matrix4x4 &getWorldMatrix() const{return mWorldMatrix;}
	inline const Matrix4x4 &getViewMatrix() const{return mViewMatrix;}
	inline const Vector3 &getForward() const{return mForward;}
	inline const Vector3 &getRight() const{return mRight;}
	inline const Vector3 &getUp() const{return mUp;}

	inline int getNumClipPlanes() const{return 6;}
	inline const Plane &getClipPlane(int i) const{return mClipPlanes[i];}
	inline const Plane *getClipPlanes() const{return mClipPlanes;}

	virtual void render(RenderDevice *device,Scene *scene,Node *node=NULL);

	bool culled(Bound *bound) const;
	bool culled(const AABox &box) const;

	void updateFramesPerSecond(uint64 time);
	inline scalar getFramesPerSecond() const{return mFPS;}
	inline int getVisibleCount() const{return mVisibleCount;}

protected:
	virtual void projectionUpdated();
	virtual void updateWorldTransform();
	virtual void updateClippingPlanes();
	virtual void autoUpdateProjection(RenderTarget *target);

	int mScope;
	ProjectionType mProjectionType;
	scalar mFov,mAspect;
	scalar mLeftDist,mRightDist;
	scalar mBottomDist,mTopDist;
	scalar mNearDist,mFarDist;

	RenderTarget::ptr mRenderTarget;
	Viewport mViewport;
	int mClearFlags;
	Vector4 mClearColor;
	bool mSkipFirstClear;
	bool mAlignmentCalculationsUseOrigin;
	RenderState::ptr mDefaultState;

	Matrix4x4 mProjectionMatrix;
	Matrix4x4 mObliqueMatrix;
	Matrix4x4 mFinalProjectionMatrix;
	Matrix4x4 mViewMatrix;
	Matrix4x4 mViewProjectionMatrix;
	Matrix4x4 mInverseProjectionMatrix;
	Matrix4x4 mInverseViewMatrix;
	Matrix4x4 mWorldMatrix;
	Plane mClipPlanes[6];
	Vector3 mForward,mRight,mUp;
	Vector3 mPosition;

	mutable int mFPSLastTime;
	mutable int mFPSFrameCount;
	mutable scalar mFPS;
	mutable int mVisibleCount;
};

}
}

#endif

