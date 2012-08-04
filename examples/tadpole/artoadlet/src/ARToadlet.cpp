#include "ARToadlet.h"

#pragma comment(lib,"c://users//siralanf//lib//aruco124.lib")
#pragma comment(lib,"c://users//siralanf//lib//opencv_core242.lib")
#pragma comment(lib,"c://users//siralanf//lib//opencv_highgui242.lib")

extern "C" VideoDevice *new_MFVideoDevice();

ARToadlet::ARToadlet(Application *app){
	mApp=app;
}

void ARToadlet::create(){
	mEngine=mApp->getEngine();
	mScene=new Scene(mEngine);

	mVideoCapture=SharedPointer<cv::VideoCapture>(new cv::VideoCapture());
	mVideoCapture->open(0);
	if(mVideoCapture->isOpened()==false){
		Error::unknown("unable to open VideoCapture");
		return;
	}

	int width=mVideoCapture->get(CV_CAP_PROP_FRAME_WIDTH);
	int height=mVideoCapture->get(CV_CAP_PROP_FRAME_HEIGHT);
	int format=mVideoCapture->get(CV_CAP_PROP_FORMAT);

	mTextureFormat=TextureFormat::ptr(new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_BGR_8,width,height,1,1));
//	mTextureData=cv::Mat(mTextureFormat->getWidth(),mTextureFormat->getHeight(),CV_8UC3);

	mDetector=SharedPointer<aruco::MarkerDetector>(new aruco::MarkerDetector());
	mDetector->setCornerRefinementMethod(aruco::MarkerDetector::LINES);

	mCamera=new Camera();
	// Mainly set our near & far distances here, so we can check them later on
	mCamera->setProjectionFovY(Math::HALF_PI,Math::ONE,1,1000);
	mCamera->setScope(1);
	mCamera->setClearColor(Colors::RED);
	mCamera->setClearFlags(RenderDevice::ClearType_BIT_DEPTH);

	mEngine->getArchiveManager()->addDirectory("../data/");

	mElco=new Node(mScene);
	{
		MeshComponent::ptr mesh=new MeshComponent(mEngine);
		mesh->setMesh(mEngine->createAABoxMesh(AABox(-10,-10,-10,10,10,10)));//findMesh("elco.xmsh"));
//		mesh->getAnimationController()->setSequenceIndex(1);
//		mesh->getAnimationController()->setCycling(AnimationController::Cycling_LOOP);
//		mesh->getAnimationController()->start();
		mElco->attach(mesh);
	}
	mElco->setScope(2);
	mScene->getRoot()->attach(mElco);

	mMerv=new Node(mScene);
	{
		MeshComponent::ptr mesh=new MeshComponent(mEngine);
		mesh->setMesh(mEngine->createAABoxMesh(AABox(-10,-10,-10,10,10,10)));//findMesh("elco.xmsh"));
//		mesh->getAnimationController()->setSequenceIndex(1);
//		mesh->getAnimationController()->setCycling(AnimationController::Cycling_LOOP);
//		mesh->getAnimationController()->start();
		mMerv->attach(mesh);
	}
	mMerv->setScope(2);
	mScene->getRoot()->attach(mMerv);

	mLight=new LightComponent();
	LightState state;
	state.type=LightState::Type_DIRECTION;
	state.direction=Math::NEG_Z_UNIT_VECTOR3;
	mLight->setLightState(state);
	mScene->getRoot()->attach(mLight);

	mBackgroundCamera=new toadlet::tadpole::Camera();
	mBackgroundCamera->setProjectionOrtho(1,-1,1,-1,-10,10);
	mBackgroundCamera->setScope(4);
	mBackgroundCamera->setClearColor(Colors::RED);
	mBackgroundCamera->setClearFlags(RenderDevice::ClearType_BIT_DEPTH);

	mBackgroundTexture=mEngine->getTextureManager()->createTexture(Texture::Usage_BIT_STREAM,mTextureFormat);

	mBackground=new Node(mScene);
	{
		MeshComponent::ptr mesh=new MeshComponent(mEngine);
		Material::ptr material=mEngine->createDiffuseMaterial(mBackgroundTexture);
		mesh->setMesh(mEngine->createGridMesh(2,2,2,2,material));
		mBackground->attach(mesh);
	}
	mBackground->setScope(4);
	mScene->getRoot()->attach(mBackground);
}

void ARToadlet::destroy(){
	if(mScene!=NULL){
		mScene->destroy();
		mScene=NULL;
	}
}

void ARToadlet::update(int dt){
	if(mVideoCapture->grab()){
		mVideoCapture->retrieve(mTextureData);

		updateMarkers();
	}

	mScene->update(dt);
}

void ARToadlet::updateMarkers(){
	std::vector<aruco::Marker> markers;

	try{
		mDetector->detect(mTextureData,markers,mCameraParams);
	}catch(const std::exception &ex){
		Logger::alert(String("ERROR:")+ex.what());
	}
	Logger::alert(String("MARKERS:")+markers.size());

//    std::vector<int> markerId=mTracker->calc(mTextureData);
//    mTracker->selectBestMarkerByCf();
//    float conf=mTracker->getConfidence();

//	Logger::alert(String("confidence:")+conf);
/*
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
*/
}

void ARToadlet::render(){
	RenderDevice *device=mEngine->getRenderDevice();

	if(mTextureData.data!=NULL){
		mEngine->getTextureManager()->textureLoad(mBackgroundTexture,mTextureFormat,mTextureData.data);
	}

	device->beginScene();
		mBackgroundCamera->render(device,mScene);
		mCamera->render(device,mScene);
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

void ARToadlet::keyPressed(int key){
}

/*
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
*/

Applet *createApplet(Application *app){return new ARToadlet(app);}
