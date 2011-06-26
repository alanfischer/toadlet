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

#include <toadlet/egg/Error.h>
#include <toadlet/peeper/RenderCaps.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/ParentNode.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(CameraNode,Categories::TOADLET_TADPOLE_NODE+".CameraNode");

CameraNode::CameraNode():super(),
	mProjectionType(ProjectionType_FOVX),
	mFov(0),mAspect(0),
	mLeftDist(0),mRightDist(0),
	mBottomDist(0),mTopDist(0),
	mNearDist(0),mFarDist(0),
	mViewportSet(false),
	//mViewport,
	mClearFlags(0),
	//mClearColor,
	mSkipFirstClear(false),
	mAlignmentCalculationsUseOrigin(false),

	//mProjectionMatrix
	mProjectionRotation(0),
	mProjectionMirrorY(false),
	//mFinalProjectionMatrix
	//mViewMatrix,
	//mForward,mRight,

	//mOverlayMatrix,
	//mOverlayVertexData,
	//mOverlayIndexData,
	mGamma(0),
	//mGammaMaterial,

	mFPSLastTime(0),
	mFPSFrameCount(0),
	mFPS(0)
{}

Node *CameraNode::create(Scene *scene){
	super::create(scene);

	setProjectionFovX(Math::HALF_PI,Math::ONE,Math::ONE,Math::fromInt(1000));
	mViewportSet=false;
	mViewport.reset();
	mClearFlags=RenderDevice::ClearType_BIT_COLOR|RenderDevice::ClearType_BIT_DEPTH;
	mClearColor.reset();
	mSkipFirstClear=false;

	Math::setMatrix4x4FromOrtho(mOverlayMatrix,0,Math::ONE,0,Math::ONE,-Math::ONE,Math::ONE);

	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,4);
	VertexBufferAccessor vba(vertexBuffer,Buffer::Access_BIT_WRITE);
	vba.set3(0,0,0,0,0);
	vba.set3(0,1,0,0,0);
	vba.set2(0,2,0,1);
	vba.set3(1,0,1,0,0);
	vba.set3(1,1,0,0,0);
	vba.set2(1,2,1,1);
	vba.set3(2,0,0,1,0);
	vba.set3(2,1,0,0,0);
	vba.set2(2,2,0,0);
	vba.set3(3,0,1,1,0);
	vba.set3(3,1,0,0,0);
	vba.set2(3,2,1,0);
	vba.unlock();
	mOverlayVertexData=VertexData::ptr(new VertexData(vertexBuffer));
	mOverlayIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,vertexBuffer->getSize()));

	mGamma=Math::ONE;
	mGammaMaterial=mEngine->getMaterialManager()->createMaterial();
	mGammaMaterial->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	mGammaMaterial->setMaterialState(MaterialState(Math::ZERO_VECTOR4));
	mGammaMaterial->retain();

	mFPSLastTime=0;
	mFPSFrameCount=0;
	mFPS=0;

	return this;
}

void CameraNode::destroy(){
	mGammaMaterial->release();
	mOverlayVertexData->destroy();
	mOverlayIndexData->destroy();

	super::destroy();
}

Node *CameraNode::set(Node *node){
	super::set(node);
	
	CameraNode *cameraNode=(CameraNode*)node;
	switch(cameraNode->getProjectionType()){
		case ProjectionType_FOVX:
			setProjectionFovX(cameraNode->getFov(),cameraNode->getAspect(),cameraNode->getNearDist(),cameraNode->getFarDist());
		break;
		case ProjectionType_FOVY:
			setProjectionFovY(cameraNode->getFov(),cameraNode->getAspect(),cameraNode->getNearDist(),cameraNode->getFarDist());
		break;
		case ProjectionType_ORTHO:
			setProjectionOrtho(cameraNode->getLeftDist(),cameraNode->getRightDist(),cameraNode->getBottomDist(),cameraNode->getTopDist(),cameraNode->getNearDist(),cameraNode->getFarDist());
		break;
		case ProjectionType_FRUSTUM:
			setProjectionFrustum(cameraNode->getLeftDist(),cameraNode->getRightDist(),cameraNode->getBottomDist(),cameraNode->getTopDist(),cameraNode->getNearDist(),cameraNode->getFarDist());
		break;
		case ProjectionType_MATRIX:
			setProjectionMatrix(cameraNode->getProjectionMatrix());
		break;
	}
	setAlignmentCalculationsUseOrigin(cameraNode->getAlignmentCalculationsUseOrigin());
	setViewport(cameraNode->getViewport());
	setClearFlags(cameraNode->getClearFlags());
	setClearColor(cameraNode->getClearColor());
	setSkipFirstClear(cameraNode->getSkipFirstClear());
	setGamma(cameraNode->getGamma());

	return this;
}

void CameraNode::setProjectionFovX(scalar fovx,scalar aspect,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_FOVX;
	mFov=fovx;mAspect=aspect;
	mLeftDist=0;mRightDist=0;
	mBottomDist=0;mTopDist=0;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromPerspectiveX(mProjectionMatrix,fovx,aspect,nearDist,farDist);

	projectionUpdated();
}

void CameraNode::setProjectionFovY(scalar fovy,scalar aspect,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_FOVY;
	mFov=fovy;mAspect=aspect;
	mLeftDist=0;mRightDist=0;
	mBottomDist=0;mTopDist=0;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromPerspectiveY(mProjectionMatrix,fovy,aspect,nearDist,farDist);

	projectionUpdated();
}

void CameraNode::setProjectionOrtho(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_ORTHO;
	mFov=0;mAspect=0;
	mLeftDist=leftDist;mRightDist=rightDist;
	mBottomDist=bottomDist;mTopDist=topDist;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromOrtho(mProjectionMatrix,leftDist,rightDist,bottomDist,topDist,nearDist,farDist);

	projectionUpdated();
}

void CameraNode::setProjectionFrustum(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_FRUSTUM;
	mFov=0;mAspect=0;
	mLeftDist=leftDist;mRightDist=rightDist;
	mBottomDist=bottomDist;mTopDist=topDist;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromFrustum(mProjectionMatrix,leftDist,rightDist,bottomDist,topDist,nearDist,farDist);

	projectionUpdated();
}

void CameraNode::setProjectionMatrix(const Matrix4x4 &matrix){
	mProjectionType=ProjectionType_MATRIX;
	mFov=0;mAspect=0;
	mLeftDist=0;mRightDist=0;
	mBottomDist=0;mTopDist=0;
	mNearDist=0;mFarDist=0;

	mProjectionMatrix.set(matrix);

	projectionUpdated();
}

void CameraNode::setProjectionRotation(scalar rotate){
	mProjectionRotation=rotate;

	projectionUpdated();
}

void CameraNode::setProjectionMirrorY(bool mirror){
	mProjectionMirrorY=mirror;

	projectionUpdated();
}

void CameraNode::setNearAndFarDist(scalar nearDist,scalar farDist){
	mNearDist=nearDist;
	mFarDist=farDist;

	switch(mProjectionType){
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

void CameraNode::setLookAt(const Vector3 &eye,const Vector3 &point,const Vector3 &up){
	Matrix4x4 matrix;
	Vector3 translate;
	Quaternion rotate;

	Math::setMatrix4x4FromLookAt(matrix,eye,point,up,true);
	Math::setTranslateFromMatrix4x4(translate,matrix);
	Math::setQuaternionFromMatrix4x4(rotate,matrix);

	setTranslate(translate);
	setRotate(rotate);
}

/// @todo: Unit test
void CameraNode::setWorldLookAt(const Vector3 &eye,const Vector3 &point,const Vector3 &up){
	Matrix4x4 matrix;
	Vector3 translate,worldTranslate;
	Quaternion rotate,worldRotate;

	Math::setMatrix4x4FromLookAt(matrix,eye,point,up,true);
	Math::setTranslateFromMatrix4x4(worldTranslate,matrix);
	Math::setQuaternionFromMatrix4x4(worldRotate,matrix);
	if(mParent!=NULL){
		Quaternion invrot;
		Math::invert(invrot,mParent->getWorldRotate());
		Math::sub(worldTranslate,mParent->getWorldTranslate());
		Math::div(worldTranslate,mParent->getWorldScale());
		Math::mul(translate,invrot,worldTranslate);
		Math::mul(rotate,invrot,worldRotate);
	}
	setTranslate(translate);
	setRotate(rotate);
}

void CameraNode::setLookDir(const Vector3 &eye,const Vector3 &dir,const Vector3 &up){
	Matrix4x4 matrix;
	Vector3 translate;
	Quaternion rotate;

	Math::setMatrix4x4FromLookDir(matrix,eye,dir,up,true);
	Math::setTranslateFromMatrix4x4(translate,matrix);
	Math::setQuaternionFromMatrix4x4(rotate,matrix);

	setTranslate(translate);
	setRotate(rotate);
}

void CameraNode::setWorldLookDir(const Vector3 &eye,const Vector3 &dir,const Vector3 &up){
	Matrix4x4 matrix;
	Vector3 translate,worldTranslate;
	Quaternion rotate,worldRotate;

	Math::setMatrix4x4FromLookDir(matrix,eye,dir,up,true);
	Math::setTranslateFromMatrix4x4(worldTranslate,matrix);
	Math::setQuaternionFromMatrix4x4(worldRotate,matrix);
	if(mParent!=NULL){
		Quaternion invrot;
		Math::invert(invrot,mParent->getWorldRotate());
		Math::sub(worldTranslate,mParent->getWorldTranslate());
		Math::div(worldTranslate,mParent->getWorldScale());
		Math::mul(translate,invrot,worldTranslate);
		Math::mul(rotate,invrot,worldRotate);
	}
	setTranslate(translate);
	setRotate(rotate);
}

void CameraNode::setViewport(const Viewport &viewport){
	mViewportSet=true;
	mViewport.set(viewport);
}

void CameraNode::setViewport(int x,int y,int width,int height){
	mViewportSet=true;
	mViewport.set(x,y,width,height);
}

void CameraNode::setGamma(scalar gamma){
	mGamma=gamma;
	if(gamma>=Math::ONE){
		mGammaMaterial->setBlendState(BlendState::Combination_ALPHA_ADDITIVE);
		mGammaMaterial->setMaterialState(MaterialState(Vector4(Math::ONE,Math::ONE,Math::ONE,gamma-Math::ONE)));
	}
	else{
		mGammaMaterial->setBlendState(BlendState::Combination_ALPHA);
		mGammaMaterial->setMaterialState(MaterialState(Vector4(0,0,0,Math::ONE-gamma)));
	}
}

void CameraNode::projectionUpdated(){
	Matrix4x4 matrix;

/// @todo: Make some test cases for the projection rotation, and get this debugged
	scalar x=0;//mViewport.x+mViewport.width/2;
	scalar y=0;//mViewport.y+mViewport.height/2;
	mFinalProjectionMatrix.set(mProjectionMatrix);

	matrix.reset();
	Math::setMatrix4x4FromTranslate(matrix,x,y,0);
	Math::preMul(matrix,mFinalProjectionMatrix);

	matrix.reset();
	Math::setMatrix4x4FromZ(matrix,mProjectionRotation);
	Math::postMul(mFinalProjectionMatrix,matrix);

	matrix.reset();
	Math::setMatrix4x4FromTranslate(matrix,-x,-y,0);
	Math::postMul(mFinalProjectionMatrix,matrix);

	matrix.reset();
	Math::setMatrix4x4FromScale(matrix,Math::ONE,mProjectionMirrorY?-Math::ONE:Math::ONE,Math::ONE);
	Math::preMul(mFinalProjectionMatrix,matrix);

	updateViewTransform();
}

void CameraNode::updateWorldTransform(){
	super::updateWorldTransform();

	Math::mul(mForward,mWorldTransform.getRotate(),Math::NEG_Z_UNIT_VECTOR3);
	Math::mul(mRight,mWorldTransform.getRotate(),Math::NEG_Z_UNIT_VECTOR3);

	updateViewTransform();
}

void CameraNode::render(RenderDevice *device,Node *node){
	updateFramesPerSecond();

	mVisibleCount=0;

	mScene->render(device,this,node);
	renderOverlayGamma(device);
}

Image::ptr CameraNode::renderToImage(RenderDevice *device,int format,int width,int height){
	Texture::ptr renderTexture=mEngine->getTextureManager()->createTexture(Texture::Usage_BIT_RENDERTARGET,Texture::Dimension_D2,format,width,height,0,1);
	PixelBufferRenderTarget::ptr renderTarget=mEngine->getTextureManager()->createPixelBufferRenderTarget();
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

	Image::ptr image=mEngine->getTextureManager()->createImage(renderTexture);

	renderTarget->destroy();
	renderTexture->release();

	return image;
}

Mesh::ptr CameraNode::renderToSkyBox(RenderDevice *device,int format,int size,scalar scale){
	bool rtt=mEngine->getRenderCaps().renderToTexture;
	int flags=Texture::Usage_BIT_RENDERTARGET;
	Texture::ptr skyboxTexture[6];
	Material::ptr skyboxMaterial[6];
	skyboxTexture[0]=mEngine->getTextureManager()->createTexture(flags,Texture::Dimension_D2,format,size,size,0,1);
	skyboxTexture[1]=mEngine->getTextureManager()->createTexture(flags,Texture::Dimension_D2,format,size,size,0,1);
	skyboxTexture[2]=mEngine->getTextureManager()->createTexture(flags,Texture::Dimension_D2,format,size,size,0,1);
	skyboxTexture[3]=mEngine->getTextureManager()->createTexture(flags,Texture::Dimension_D2,format,size,size,0,1);
	skyboxTexture[4]=mEngine->getTextureManager()->createTexture(flags,Texture::Dimension_D2,format,size,size,0,1);
	skyboxTexture[5]=mEngine->getTextureManager()->createTexture(flags,Texture::Dimension_D2,format,size,size,0,1);
	PixelBufferRenderTarget::ptr renderTarget=rtt?mEngine->getTextureManager()->createPixelBufferRenderTarget():NULL;

	Vector3 forward[6],up[6];
	forward[0]=Math::NEG_Z_UNIT_VECTOR3;	up[0]=Math::Y_UNIT_VECTOR3;
	forward[1]=Math::Z_UNIT_VECTOR3;		up[1]=Math::Y_UNIT_VECTOR3;
	forward[2]=Math::NEG_X_UNIT_VECTOR3;	up[2]=Math::Z_UNIT_VECTOR3;
	forward[3]=Math::X_UNIT_VECTOR3;		up[3]=Math::Z_UNIT_VECTOR3;
	forward[4]=Math::NEG_Y_UNIT_VECTOR3;	up[4]=Math::Z_UNIT_VECTOR3;
	forward[5]=Math::Y_UNIT_VECTOR3;		up[5]=Math::Z_UNIT_VECTOR3;

	RenderTarget *oldTarget=device->getRenderTarget();
	Viewport oldViewport=getViewport();
	Transform oldTransform=getTransform();
	bool oldAlignment=getAlignmentCalculationsUseOrigin();
	setAlignmentCalculationsUseOrigin(true);

	int i;
	for(i=0;i<6;++i){
		if(rtt){
			renderTarget->attach(skyboxTexture[i]->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);
			device->setRenderTarget(renderTarget);
		}

		setViewport(Viewport(0,0,skyboxTexture[i]->getWidth(),skyboxTexture[i]->getHeight()));
		setLookDir(getTranslate(),forward[i],up[i]);
		mScene->update(0);
		device->beginScene();
			render(device);
		device->endScene();

		if(rtt){
			device->swap();
			renderTarget->remove(skyboxTexture[i]->getMipPixelBuffer(0,0));
			device->setRenderTarget(oldTarget);
 		}
		else{ 
			device->copyFrameBufferToPixelBuffer(skyboxTexture[i]->getMipPixelBuffer(0,0));
		}

		skyboxMaterial[i]=mEngine->getMaterialManager()->createMaterial(skyboxTexture[i],true);
		skyboxMaterial[i]->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	}

	setAlignmentCalculationsUseOrigin(oldAlignment);
	setTransform(oldTransform);
	setViewport(oldViewport);
	device->setRenderTarget(oldTarget);

	if(renderTarget!=NULL){
		renderTarget->destroy();
	}

	return getEngine()->getMeshManager()->createSkyBox(scale,false,true,skyboxMaterial[0],skyboxMaterial[1],skyboxMaterial[2],skyboxMaterial[3],skyboxMaterial[4],skyboxMaterial[5]);
}

bool CameraNode::culled(Node *node) const{
	bool c=(node->getScope()&getScope())==0 || culled(node->getWorldBound());
	if(c==false) mVisibleCount++;
	return c;
}

bool CameraNode::culled(const Bound &bound) const{
	switch(bound.getType()){
		case Bound::Type_INFINITE:	return false;
		case Bound::Type_SPHERE:	return culled(bound.getSphere());
		case Bound::Type_AABOX:		return culled(bound.getAABox());
		default:
			Error::unimplemented("unknown bound type");
			return false;
	}
}

bool CameraNode::culled(const Sphere &sphere) const{
	if(sphere.radius<0) return false;
	scalar distance=0;
	int i;
	for(i=0;i<6;++i){
		distance=Math::dot(mClipPlanes[i].normal,sphere.origin)+mClipPlanes[i].distance;
		if(distance<-sphere.radius){
			return true;
		}
	}
	return false;
}

bool CameraNode::culled(const AABox &box) const{
	int i;
	for(i=0;i<6;i++){
		if(box.findPVertexLength(mClipPlanes[i])<0){
			return true;
		}
	}
	return false;
}

void CameraNode::updateFramesPerSecond(){
	mFPSFrameCount++;
	int fpsTime=mScene->getTime();
	if(mFPSLastTime==0 || fpsTime-mFPSLastTime>5000){
		scalar fps=0;
		if(mFPSLastTime>0){
			fps=Math::div(Math::fromInt(mFPSFrameCount),Math::fromMilli(fpsTime-mFPSLastTime));
		}
		mFPSLastTime=fpsTime;
		mFPSFrameCount=0;
		mFPS=fps;
	}
}

void CameraNode::updateViewTransform(){
	const Vector3 worldTranslate=mWorldTransform.getTranslate();
	Matrix4x4 worldMatrix;
	mWorldTransform.getMatrix(worldMatrix);
	scalar wt00=worldMatrix.at(0,0),wt01=worldMatrix.at(0,1),wt02=worldMatrix.at(0,2);
	scalar wt10=worldMatrix.at(1,0),wt11=worldMatrix.at(1,1),wt12=worldMatrix.at(1,2);
	scalar wt20=worldMatrix.at(2,0),wt21=worldMatrix.at(2,1),wt22=worldMatrix.at(2,2);

	mViewMatrix.setAt(0,0,wt00);
	mViewMatrix.setAt(0,1,wt10);
	mViewMatrix.setAt(0,2,wt20);
	mViewMatrix.setAt(1,0,wt01);
	mViewMatrix.setAt(1,1,wt11);
	mViewMatrix.setAt(1,2,wt21);
	mViewMatrix.setAt(2,0,wt02);
	mViewMatrix.setAt(2,1,wt12);
	mViewMatrix.setAt(2,2,wt22);

	mViewMatrix.setAt(0,3,-(Math::mul(wt00,worldTranslate.x) + Math::mul(wt10,worldTranslate.y) + Math::mul(wt20,worldTranslate.z)));
	mViewMatrix.setAt(1,3,-(Math::mul(wt01,worldTranslate.x) + Math::mul(wt11,worldTranslate.y) + Math::mul(wt21,worldTranslate.z)));
	mViewMatrix.setAt(2,3,-(Math::mul(wt02,worldTranslate.x) + Math::mul(wt12,worldTranslate.y) + Math::mul(wt22,worldTranslate.z)));

	// Update frustum planes
 	Math::mul(mViewProjectionMatrix,mFinalProjectionMatrix,mViewMatrix);
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

void CameraNode::renderOverlayGamma(RenderDevice *device){
	if(mGamma!=Math::ONE){
		device->setAmbientColor(Math::ONE_VECTOR4);
		mGammaMaterial->setupRenderDevice(device);
		device->setMatrix(RenderDevice::MatrixType_PROJECTION,mOverlayMatrix);
		device->setMatrix(RenderDevice::MatrixType_VIEW,Math::IDENTITY_MATRIX4X4);
		device->setMatrix(RenderDevice::MatrixType_MODEL,Math::IDENTITY_MATRIX4X4);
		device->renderPrimitive(mOverlayVertexData,mOverlayIndexData);
	}
}

}
}
}
