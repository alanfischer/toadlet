#include "ARToadlet.h"

#pragma comment(lib,"libAR.lib")

extern "C" VideoDevice *new_MFVideoDevice();

ARToadlet::ARToadlet(Application *app){
	mApp=app;
}

bool ARToadlet::setupARCamera(const String &cameraParamFile,const String &config,ARParam *cameraParams){
	ARParam wparam;

	if(arParamLoad(cameraParamFile,1,&wparam)<0){
		Error::unknown("error loading parameter file");
		return false;
	}
	
	arParamChangeSize(&wparam,mTextureFormat->getWidth(),mTextureFormat->getHeight(),cameraParams);
	arParamDisp(cameraParams);
	arInitCparam(cameraParams);

	mVideoDevice->start();
	
	return true;
}
	
void ARToadlet::create(){
	mEngine=mApp->getEngine();
	mScene=Scene::ptr(new Scene(mEngine));

	mVideoDevice=new_MFVideoDevice();
	mVideoDevice->create();
	mVideoDevice->setListener(this);
	mTextureFormat=TextureFormat::ptr(new TextureFormat(mVideoDevice->getTextureFormat()));
	mTextureFormat->setPixelFormat(TextureFormat::Format_RGBA_8);
	mTextureData=new tbyte[mTextureFormat->getDataSize()];

	mCamera=mEngine->createNodeType(CameraNode::type(),mScene);
	// Mainly set our near & far distances here, so we can check them later on
	mCamera->setProjectionFovY(Math::HALF_PI,Math::ONE,1,1000);
	mCamera->setScope(1);
	mCamera->setClearFlags(RenderDevice::ClearType_BIT_DEPTH);
	mScene->getRoot()->attach(mCamera);

	mEngine->setDirectory("../data/");

	mElco=mEngine->createNodeType(ParentNode::type(),mScene);
	{
		MeshNode::ptr mesh=mEngine->createNodeType(MeshNode::type(),mScene);
		mesh->setMesh(mEngine->getMeshManager()->createAABoxMesh(AABox(-10,-10,-10,10,10,10)));//findMesh("elco.xmsh"));
		mesh->setTranslate(0,0,mesh->getMesh()->getBound().getSphere().radius/2);
//		mesh->getAnimationController()->setSequenceIndex(1);
//		mesh->getAnimationController()->setCycling(AnimationController::Cycling_LOOP);
//		mesh->getAnimationController()->start();
		mElco->attach(mesh);
	}
	mElco->setScope(2);
	mScene->getRoot()->attach(mElco);

	mMerv=mEngine->createNodeType(ParentNode::type(),mScene);
	{
		MeshNode::ptr mesh=mEngine->createNodeType(MeshNode::type(),mScene);
		mesh->setMesh(mEngine->getMeshManager()->createAABoxMesh(AABox(-10,-10,-10,10,10,10)));//findMesh("merv.xmsh"));
		mesh->setTranslate(0,0,mesh->getMesh()->getBound().getSphere().radius/2);
//		mesh->getAnimationController()->setSequenceIndex(1);
//		mesh->getAnimationController()->setCycling(AnimationController::Cycling_LOOP);
//		mesh->getAnimationController()->start();
		mMerv->attach(mesh);
	}
	mMerv->setScope(2);
	mScene->getRoot()->attach(mMerv);

	mLight=mEngine->createNodeType(LightNode::type(),mScene);
	LightState state;
	state.type=LightState::Type_DIRECTION;
	state.direction=Math::NEG_Z_UNIT_VECTOR3;
	mLight->setLightState(state);
	mScene->getRoot()->attach(mLight);

Logger::alert("Setting up camera");
	if (!setupARCamera("camera_para.dat", "", &mARTCparam)) {
		Error::unknown("error setting up camera");
		return;
	}
Logger::alert("Setting up done");

	mPatternIDs.resize(2);
	if((mPatternIDs[0]=arLoadPatt("../data/patt.sample1"))<0){
		Error::unknown("error loading pattern");
		return;
	}
	if((mPatternIDs[1]=arLoadPatt("../data/patt.sample2"))<0){
		Error::unknown("error loading pattern");
		return;
	}

	mOrthoCamera=mEngine->createNodeType(CameraNode::type(),mScene);
	mOrthoCamera->setProjectionOrtho(-1,1,-1,1,-10,10);
	mOrthoCamera->setScope(4);
	mOrthoCamera->setClearColor(Colors::RED);
	mCamera->setClearFlags(RenderDevice::ClearType_BIT_DEPTH);
	mScene->getRoot()->attach(mOrthoCamera);

	mBackgroundTexture=mEngine->getTextureManager()->createTexture(Texture::Usage_BIT_STREAM,mTextureFormat);

	mBackground=mEngine->createNodeType(MeshNode::type(),mScene);
	Material::ptr material=mEngine->getMaterialManager()->createDiffuseMaterial(mBackgroundTexture);
	Mesh::ptr mesh=mEngine->getMeshManager()->createGridMesh(2,2,2,2,material);
	mBackground->setMesh(mesh);
	mBackground->setScale(1,1,1);
	mOrthoCamera->attach(mBackground);
}

void ARToadlet::destroy(){
	if(mScene!=NULL){
		mScene->destroy();
		mScene=NULL;
	}

	if(mVideoDevice!=NULL){
		mVideoDevice->destroy();
		delete mVideoDevice;
		mVideoDevice=NULL;
	}
}

void ARToadlet::frameReceived(TextureFormat::ptr format,tbyte *data){
	mMutex.lock();

	TextureFormatConversion::convert(data,format,mTextureData,mTextureFormat);

	mMutex.unlock();
}
	
void ARToadlet::update(int dt){
	mMutex.lock();

	updateMarkers();

	mMutex.unlock();

	mScene->update(dt);
}

void ARToadlet::updateMarkers(){
		double patternTransform[3][4];
		Matrix4x4 transform;
		int numMarkers=0;
		ARMarkerInfo *markerInfo=NULL;

		// Detect the markers in the video frame.
		Logger::alert("Markers?");
		if (arDetectMarker(mTextureData, THRESHOLD, &markerInfo, &numMarkers) < 0) {
		Logger::alert("Markers? no");
			return;
		}
		Logger::alert(String("Markers:")+numMarkers);
		
		// Check through the marker_info array for highest confidence
		// visible marker matching our preferred pattern.
		Collection<int> bestMarkers(mPatternIDs.size(),-1);
		int i,j;
		for(i=0;i<numMarkers;i++){
			for(j=0;j<mPatternIDs.size();++j){
				if(markerInfo[i].id=mPatternIDs[j]){
					if(bestMarkers[j]==-1){bestMarkers[j]=i;}
					else if(markerInfo[i].cf>markerInfo[bestMarkers[j]].cf){bestMarkers[j]=i;}
				}
			}
		}
			
		// Projection transformation.
		setMatrix4x4FromARProjection(transform,&mARTCparam,mCamera->getNearDist(),mCamera->getFarDist());
		mCamera->setProjectionMatrix(transform);

		// Calculate the camera position relative to the marker.
		if(bestMarkers[0]!=-1){
			arGetTransMatCont(&(markerInfo[bestMarkers[0]]), patternTransform, PATTERN_CENTER, PATTERN_WIDTH, patternTransform);
			setMatrix4x4FromARMatrix(transform,patternTransform);
			mElco->setMatrix4x4(transform);
			mElco->setScope(1);
		}
		else{
			mElco->setScope(2);
		}

		if(bestMarkers[1]!=-1){
			arGetTransMatCont(&(markerInfo[bestMarkers[1]]), patternTransform, PATTERN_CENTER, PATTERN_WIDTH, patternTransform);
			setMatrix4x4FromARMatrix(transform,patternTransform);
			mMerv->setMatrix4x4(transform);
			mMerv->setScope(1);
		}
		else{
			mMerv->setScope(2);
		}
}

void ARToadlet::render(RenderDevice *device){
	mMutex.lock();

	mEngine->getTextureManager()->textureLoad(mBackgroundTexture,mTextureFormat,mTextureData);

	mMutex.unlock();

	device->beginScene();
		mOrthoCamera->render(device);
		mCamera->render(device);
	device->endScene();
	device->swap();
}

void ARToadlet::resized(int width,int height){
	if(mCamera!=NULL && width>0 && height>0){
		if(width>=height){
			mCamera->setProjectionFovY(Math::degToRad(Math::fromInt(60)),Math::div(Math::fromInt(width),Math::fromInt(height)),mCamera->getNearDist(),mCamera->getFarDist());
		}
		else{
			mCamera->setProjectionFovX(Math::degToRad(Math::fromInt(60)),Math::div(Math::fromInt(height),Math::fromInt(width)),mCamera->getNearDist(),mCamera->getFarDist());
		}
		mCamera->setViewport(Viewport(0,0,width,height));
	}
}

int ARToadlet::getPixelFormatFromARPixelFormat(int format){
	switch(format){
		case AR_PIXEL_FORMAT_RGBA:
			return TextureFormat::Format_RGBA_8;
		case AR_PIXEL_FORMAT_ABGR: // SGI
			return TextureFormat::Format_UNKNOWN;
		case AR_PIXEL_FORMAT_BGRA: // Windows
			return TextureFormat::Format_BGRA_8;
		case AR_PIXEL_FORMAT_ARGB:	// Mac
			return TextureFormat::Format_UNKNOWN;
		case AR_PIXEL_FORMAT_RGB:
			return TextureFormat::Format_RGB_8;
		case AR_PIXEL_FORMAT_BGR:
			return TextureFormat::Format_BGR_8;
		case AR_PIXEL_FORMAT_MONO:
			return TextureFormat::Format_L_8;
		case AR_PIXEL_FORMAT_2vuy:
			return TextureFormat::Format_UNKNOWN;
		case AR_PIXEL_FORMAT_yuvs:
			return TextureFormat::Format_UNKNOWN;
		default:
			return TextureFormat::Format_UNKNOWN;
	}
}

void ARToadlet::setMatrix4x4FromARProjection(Matrix4x4 &r,ARParam *cparam,const double minDistance,const double maxDistance){
	double   icpara[3][4];
	double   trans[3][4];
	double   p[3][3], q[4][4];
	int i=0,j=0;
	int width=cparam->xsize;
	int height=cparam->ysize;

	if(arParamDecompMat(cparam->mat,icpara,trans)<0){
		Error::unknown("arParamDecompMat error");
		return;
	}

	for(i=0;i<4;i++){
		icpara[1][i] = (height - 1)*(icpara[2][i]) - icpara[1][i];
	}

	for(i=0;i<3;i++) {
		for(j=0;j<3;j++) {
			p[i][j] = icpara[i][j] / icpara[2][2];
		}
	}

	q[0][0] = (2.0 * p[0][0] / (width - 1));
	q[0][1] = (2.0 * p[0][1] / (width - 1));
	q[0][2] = -((2.0 * p[0][2] / (width - 1))  - 1.0);
	q[0][3] = 0.0;
	
	q[1][0] = 0.0;
	q[1][1] = -(2.0 * p[1][1] / (height - 1));
	q[1][2] = -((2.0 * p[1][2] / (height - 1)) - 1.0);
	q[1][3] = 0.0;
	
	q[2][0] = 0.0;
	q[2][1] = 0.0;
	q[2][2] = (maxDistance + minDistance)/(minDistance - maxDistance);
	q[2][3] = 2.0 * maxDistance * minDistance / (minDistance - maxDistance);
	
	q[3][0] = 0.0;
	q[3][1] = 0.0;
	q[3][2] = -1.0;
	q[3][3] = 0.0;

	for(i=0;i<4;i++){
		for(j=0;j<3;j++){
			r.setAt(i,j, q[i][0] * trans[0][j] + q[i][1] * trans[1][j] + q[i][2] * trans[2][j] );
		}
		r.setAt(i,3, q[i][0] * trans[0][3] + q[i][1] * trans[1][3] + q[i][2] * trans[2][3] + q[i][3] );
    }	
}

void ARToadlet::setMatrix4x4FromARMatrix(Matrix4x4 &r,const double para[3][4]){
	r.setAt(0,0,para[0][0]);	r.setAt(0,1,para[1][0]);	r.setAt(0,2,para[2][0]);	r.setAt(0,3,para[0][3]);
	r.setAt(1,0,para[0][1]);	r.setAt(1,1,para[1][1]);	r.setAt(1,2,para[2][1]);	r.setAt(1,3,para[1][3]);
	r.setAt(2,0,para[0][2]);	r.setAt(2,1,para[1][2]);	r.setAt(2,2,para[2][2]);	r.setAt(2,3,-para[2][3]);
	r.setAt(3,0,0);				r.setAt(3,1,0);				r.setAt(3,2,0);				r.setAt(3,3,Math::ONE);
}

Applet *createApplet(Application *app){return new ARToadlet(app);}
void destroyApplet(Applet *applet){delete applet;}