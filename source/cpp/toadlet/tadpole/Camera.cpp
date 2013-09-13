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

#include <toadlet/peeper/RenderCaps.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/Camera.h>

namespace toadlet{
namespace tadpole{

Camera::Camera(Engine *engine):
	mScope(-1),
	mProjectionType(ProjectionType_FOV),
	mFov(0),mAspect(0),
	mLeftDist(0),mRightDist(0),
	mBottomDist(0),mTopDist(0),
	mNearDist(0),mFarDist(0),
	mClearFlags(0),
	mSkipFirstClear(false),
	mAlignmentCalculationsUseOrigin(false),
	mAutoYHeight(false),

	mFPS(0),
	mVisibleCount(0)
{
	setAutoProjectionFov(Math::QUARTER_PI,false,Math::ONE,Math::fromInt(1000));
	mClearFlags=RenderDevice::ClearType_BIT_COLOR|RenderDevice::ClearType_BIT_DEPTH;
	if(engine!=NULL){
		mDefaultState=engine->getMaterialManager()->createRenderState();
		mDefaultState->setBlendState(BlendState());
		mDefaultState->setDepthState(DepthState());
		mDefaultState->setRasterizerState(RasterizerState());
		mDefaultState->setFogState(FogState());
		mDefaultState->setGeometryState(GeometryState());
	}

	mFPSData.resize(100);
}

void Camera::setAutoProjectionFov(scalar fov,bool yheight,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_FOV;
	mFov=fov;mAspect=Math::ONE;
	mLeftDist=0;mRightDist=0;
	mBottomDist=0;mTopDist=0;
	mNearDist=nearDist;mFarDist=farDist;
	mAutoYHeight=yheight;
}

void Camera::setProjectionFovX(scalar fovx,scalar aspect,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_FOVX;
	mFov=fovx;mAspect=aspect;
	mLeftDist=0;mRightDist=0;
	mBottomDist=0;mTopDist=0;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromPerspectiveX(mProjectionMatrix,fovx,aspect,nearDist,farDist);

	projectionUpdated();
}

void Camera::setProjectionFovY(scalar fovy,scalar aspect,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_FOVY;
	mFov=fovy;mAspect=aspect;
	mLeftDist=0;mRightDist=0;
	mBottomDist=0;mTopDist=0;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromPerspectiveY(mProjectionMatrix,fovy,aspect,nearDist,farDist);

	projectionUpdated();
}

void Camera::setProjectionOrtho(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_ORTHO;
	mFov=0;mAspect=0;
	mLeftDist=leftDist;mRightDist=rightDist;
	mBottomDist=bottomDist;mTopDist=topDist;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromOrtho(mProjectionMatrix,leftDist,rightDist,bottomDist,topDist,nearDist,farDist);

	projectionUpdated();
}

void Camera::setProjectionFrustum(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_FRUSTUM;
	mFov=0;mAspect=0;
	mLeftDist=leftDist;mRightDist=rightDist;
	mBottomDist=bottomDist;mTopDist=topDist;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromFrustum(mProjectionMatrix,leftDist,rightDist,bottomDist,topDist,nearDist,farDist);

	projectionUpdated();
}

void Camera::setProjectionMatrix(const Matrix4x4 &matrix){
	mProjectionType=ProjectionType_MATRIX;
	mFov=0;mAspect=0;
	mLeftDist=0;mRightDist=0;
	mBottomDist=0;mTopDist=0;
	mNearDist=0;mFarDist=0;

	mProjectionMatrix.set(matrix);

	projectionUpdated();
}

void Camera::setObliqueNearPlaneMatrix(const Matrix4x4 &oblique){
	mObliqueMatrix.set(oblique);
	
	projectionUpdated();
}

void Camera::setNearAndFarDist(scalar nearDist,scalar farDist){
	mNearDist=nearDist;
	mFarDist=farDist;

	switch(mProjectionType){
		case(ProjectionType_FOV):
			setAutoProjectionFov(mFov,mAutoYHeight,mNearDist,mFarDist);
		break;
		case(ProjectionType_FOVX):
			setProjectionFovX(mFov,mAspect,mNearDist,mFarDist);
		break;
		case(ProjectionType_FOVY):
			setProjectionFovY(mFov,mAspect,mNearDist,mFarDist);
		break;
		case(ProjectionType_ORTHO):
			setProjectionOrtho(mLeftDist,mRightDist,mBottomDist,mTopDist,mNearDist,mFarDist);
		break;
		case(ProjectionType_FRUSTUM):
			setProjectionFrustum(mLeftDist,mRightDist,mBottomDist,mTopDist,mNearDist,mFarDist);
		break;
		default:
			Error::unimplemented("projection type does not support adjusting near and far");
		break;
	}
}

void Camera::setLookAt(const Vector3 &eye,const Vector3 &point,const Vector3 &up){
	Math::setMatrix4x4FromLookAt(mWorldMatrix,eye,point,up,true);
	updateWorldTransform();
}

void Camera::setLookDir(const Vector3 &eye,const Vector3 &dir,const Vector3 &up){
	Math::setMatrix4x4FromLookDir(mWorldMatrix,eye,dir,up,true);
	updateWorldTransform();
}

void Camera::setWorldMatrix(const Matrix4x4 &matrix){
	mWorldMatrix.set(matrix);
	updateWorldTransform();
}

void Camera::setPosition(const Vector3 &position){
	Math::setMatrix4x4FromTranslate(mWorldMatrix,position);
	updateWorldTransform();
}

void Camera::setRenderTarget(RenderTarget *target){
	mRenderTarget=target;
	if(target==NULL){
		mViewport.set(0,0,0,0);
	}
	else{
		mViewport.set(0,0,target->getWidth(),target->getHeight());
	}
}

void Camera::projectionUpdated(){
	Matrix4x4 matrix;

	mFinalProjectionMatrix.set(mProjectionMatrix);

	if(mObliqueMatrix.equals(Math::IDENTITY_MATRIX4X4)==false){
		Matrix4x4 invtransProjMatrix;
		Math::mul(invtransProjMatrix,mFinalProjectionMatrix,mObliqueMatrix);
		Math::invert(matrix,invtransProjMatrix);
		Math::transpose(invtransProjMatrix,matrix);

		Vector4 oplane(0,0,-1,0),cplane;
		Math::mul(cplane,invtransProjMatrix,oplane);

		float inv=abs((int)cplane[2]);
		if(inv!=0){cplane/=inv;}// normalize such that depth is not scaled
		cplane[3] -= 1;

		if(cplane[2] < 0)
			cplane *= -1;


		Matrix4x4 suffix;
		suffix.setAt(2,0,cplane[0]);
		suffix.setAt(2,1,cplane[1]);
		suffix.setAt(2,2,cplane[2]);
		suffix.setAt(2,3,cplane[3]);

		Math::preMul(mFinalProjectionMatrix,suffix);
	}

	updateWorldTransform();
}

void Camera::render(RenderDevice *device,Scene *scene,Node *node){
	if(mProjectionType==ProjectionType_FOV){
		autoUpdateProjection(mRenderTarget!=NULL?mRenderTarget.get():device->getPrimaryRenderTarget());
	}

	updateFramesPerSecond(scene->getTime());

	mVisibleCount=0;

	scene->render(device,this,node);
}

/*
Texture::ptr Camera::renderToTexture(RenderDevice *device,int format,int width,int height){
	Engine *engine=mEngine;

	TextureFormat::ptr textureFormat=new TextureFormat(TextureFormat::Dimension_D2,format,width,height,1,1);
	Texture::ptr renderTexture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_RENDERTARGET,textureFormat);
	PixelBufferRenderTarget::ptr renderTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
	renderTarget->attach(renderTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

	RenderTarget *oldTarget=device->getRenderTarget();
	Viewport oldViewport=getViewport();

	device->setRenderTarget(renderTarget);
	setViewport(Viewport(0,0,width,height));

	device->beginScene();
		render(device);
	device->endScene();

	setViewport(oldViewport);
	device->setRenderTarget(oldTarget);

	renderTarget->destroy();

	return renderTexture;
}

Mesh::ptr Camera::renderToSkyBox(RenderDevice *device,int format,int size,scalar scale){
	Engine *engine=mParent->getEngine();

	TextureFormat::ptr textureFormat=new TextureFormat(TextureFormat::Dimension_D2,format,size,size,1,1);
	bool rtt=engine->getRenderCaps().renderToTexture;
	int flags=Texture::Usage_BIT_RENDERTARGET;
	Texture::ptr skyBoxTexture[6];
	Material::ptr skyBoxMaterial[6];
	skyBoxTexture[0]=engine->getTextureManager()->createTexture(flags,textureFormat);
	skyBoxTexture[1]=engine->getTextureManager()->createTexture(flags,textureFormat);
	skyBoxTexture[2]=engine->getTextureManager()->createTexture(flags,textureFormat);
	skyBoxTexture[3]=engine->getTextureManager()->createTexture(flags,textureFormat);
	skyBoxTexture[4]=engine->getTextureManager()->createTexture(flags,textureFormat);
	skyBoxTexture[5]=engine->getTextureManager()->createTexture(flags,textureFormat);
	PixelBufferRenderTarget::ptr renderTarget=rtt?engine->getTextureManager()->createPixelBufferRenderTarget():NULL;

	Vector3 forward[6],up[6];
	forward[0]=Math::NEG_Z_UNIT_VECTOR3;	up[0]=Math::Y_UNIT_VECTOR3;
	forward[1]=Math::Z_UNIT_VECTOR3;		up[1]=Math::Y_UNIT_VECTOR3;
	forward[2]=Math::NEG_X_UNIT_VECTOR3;	up[2]=Math::Z_UNIT_VECTOR3;
	forward[3]=Math::X_UNIT_VECTOR3;		up[3]=Math::Z_UNIT_VECTOR3;
	forward[4]=Math::NEG_Y_UNIT_VECTOR3;	up[4]=Math::Z_UNIT_VECTOR3;
	forward[5]=Math::Y_UNIT_VECTOR3;		up[5]=Math::Z_UNIT_VECTOR3;

	RenderTarget *oldTarget=device->getRenderTarget();
	Transform oldTransform=mParent->getTransform();
	Viewport oldViewport=getViewport();
	bool oldAlignment=getAlignmentCalculationsUseOrigin();
	setAlignmentCalculationsUseOrigin(true);

	int i;
	for(i=0;i<6;++i){
		if(rtt){
			renderTarget->attach(skyBoxTexture[i]->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);
			device->setRenderTarget(renderTarget);
		}

		setViewport(Viewport(0,0,skyBoxTexture[i]->getFormat()->getWidth(),skyBoxTexture[i]->getFormat()->getHeight()));
		setLookDir(mParent->getTranslate(),forward[i],up[i]);
		mParent->logicUpdate(0,-1);
		device->beginScene();
			render(device);
		device->endScene();

		if(rtt){
			device->swap();
			renderTarget->remove(skyBoxTexture[i]->getMipPixelBuffer(0,0));
			device->setRenderTarget(oldTarget);
 		}
		else{ 
			device->copyFrameBufferToPixelBuffer(skyBoxTexture[i]->getMipPixelBuffer(0,0));
		}

		skyBoxMaterial[i]=engine->createSkyBoxMaterial(skyBoxTexture[i]);
	}

	setAlignmentCalculationsUseOrigin(oldAlignment);
	setViewport(oldViewport);
	mParent->setTransform(oldTransform);
	device->setRenderTarget(oldTarget);

	if(renderTarget!=NULL){
		renderTarget->destroy();
	}

	return engine->createSkyBoxMesh(scale,false,true,skyBoxMaterial[0],skyBoxMaterial[1],skyBoxMaterial[2],skyBoxMaterial[3],skyBoxMaterial[4],skyBoxMaterial[5]);
}
*/

bool Camera::culled(Bound *bound) const{
	bool result=false;
	switch(bound->getType()){
		case Bound::Type_EMPTY:
			result=true;
		break;
		case Bound::Type_INFINITE:
			result=false;
		break;
		case Bound::Type_SPHERE:
			result=!Math::testIntersection(bound->getSphere(),getClipPlanes(),getNumClipPlanes());
		break;
		case Bound::Type_AABOX:
			result=!Math::testIntersection(bound->getAABox(),getClipPlanes(),getNumClipPlanes());
		break;
		default:
			Error::unimplemented("unknown bound type");
			return false;
	}
	if(result==false){
		mVisibleCount++;
	}
	return result;
}

bool Camera::culled(const AABox &box) const{
	return !Math::testIntersection(box,getClipPlanes(),getNumClipPlanes());
}

void Camera::updateFramesPerSecond(uint64 time){
	FPSData &oldf=mFPSData[0];

	FPSData newf;
	newf.time=time;
	newf.count=mFPSData[mFPSData.size()-1].count+1;

	int dt=newf.time-oldf.time;
	if(dt>0){
		mFPS=Math::div(Math::fromInt(newf.count-oldf.count),Math::fromMilli(newf.time-oldf.time));
	}

	mFPSData.removeAt(0);
	mFPSData.add(newf);
}

void Camera::updateWorldTransform(){
	Math::mulVector3(mForward,mWorldMatrix,Math::NEG_Z_UNIT_VECTOR3);
	Math::mulVector3(mRight,mWorldMatrix,Math::X_UNIT_VECTOR3);
	Math::mulVector3(mUp,mWorldMatrix,Math::Y_UNIT_VECTOR3);
	Math::setTranslateFromMatrix4x4(mPosition,mWorldMatrix);

	Math::invert(mViewMatrix,mWorldMatrix);

 	Math::mul(mViewProjectionMatrix,mFinalProjectionMatrix,mViewMatrix);

	updateClippingPlanes();
}

void Camera::updateClippingPlanes(){
	scalar *vpt=mViewProjectionMatrix.data;
	// Right clipping plane.
	Math::normalize(mClipPlanes[0].set(vpt[3]-vpt[0], vpt[7]-vpt[4], vpt[11]-vpt[8], vpt[15]-vpt[12]));
	// Left clipping plane.
	Math::normalize(mClipPlanes[1].set(vpt[3]+vpt[0], vpt[7]+vpt[4], vpt[11]+vpt[8], vpt[15]+vpt[12]));
	// Bottom clipping plane.
	Math::normalize(mClipPlanes[2].set(vpt[3]+vpt[1], vpt[7]+vpt[5], vpt[11]+vpt[9], vpt[15]+vpt[13]));
	// Top clipping plane.
	Math::normalize(mClipPlanes[3].set(vpt[3]-vpt[1], vpt[7]-vpt[5], vpt[11]-vpt[9], vpt[15]-vpt[13]));
	// Far clipping plane.
	Math::normalize(mClipPlanes[4].set(vpt[3]-vpt[2], vpt[7]-vpt[6], vpt[11]-vpt[10], vpt[15]-vpt[14]));
	// Near clipping plane.
	Math::normalize(mClipPlanes[5].set(vpt[3]+vpt[2], vpt[7]+vpt[6], vpt[11]+vpt[10], vpt[15]+vpt[14]));
}

void Camera::autoUpdateProjection(RenderTarget *target){
	int width=target->getWidth(),height=target->getHeight();
	setViewport(Viewport(0,0,width,height));
	if((width>=height && !mAutoYHeight) || (height>=width && mAutoYHeight)){
		Math::setMatrix4x4FromPerspectiveY(mProjectionMatrix,mFov,Math::div(Math::fromInt(width),Math::fromInt(height)),mNearDist,mFarDist);
	}
	else{
		Math::setMatrix4x4FromPerspectiveX(mProjectionMatrix,mFov,Math::div(Math::fromInt(height),Math::fromInt(width)),mNearDist,mFarDist);
	}
	projectionUpdated();
}

}
}
