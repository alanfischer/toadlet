#include <toadlet/egg/Error.h>
#include <toadlet/pad/Application.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/SceneNode.h>
#include <toadlet/tadpole/node/MeshNode.h>

#include <AR/config.h>
#include <AR/video.h>
#include <AR/param.h>			// arParamDisp()
#include <AR/ar.h>

#pragma comment(lib,"AR.lib")
#pragma comment(lib,"ARvideo.lib")

using namespace toadlet::egg;
using namespace toadlet::pad;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::animation;
using namespace toadlet::tadpole::node;

static double PATTERN_WIDTH=80;
static double PATTERN_CENTER[2]={0,0};
static int THRESHOLD=150;

class ARToadlet:public Application{
public:
	ARToadlet():Application(),
		mLastImage(NULL)
	{
		Math::setMatrix4x4FromOrtho(mProjectionMatrix,0,1,1,0,-1,1);
	}

	bool setupARCamera(const String &cameraParamFile,const String &config,ARParam *cameraParams){
		ARParam wparam;
		int width,height;
		
		if(arVideoOpen((char*)config.c_str())<0){
			Error::unknown("error opening camera connection");
			return false;
		}
		
		if(arVideoInqSize(&width,&height)<0){
			Error::unknown("error getting video size");
			return false;
		}
		
		if(arParamLoad(cameraParamFile,1,&wparam)<0){
			Error::unknown("error loading parameter file");
			return false;
		}
		
		arParamChangeSize(&wparam,width,height,cameraParams);
		arParamDisp(cameraParams);
		arInitCparam(cameraParams);

		if(arVideoCapStart()!=0){
			Error::unknown("unable to begin data capture");
			return false;
		}
		
		return true;
	}
	
	void create(){
		Application::create();

		mEngine->setScene(mEngine->createNodeType(SceneNode::type()));

		mCamera=mEngine->createNodeType(CameraNode::type());
		// Mainly set our near & far distances here, so we can check them later on
		mCamera->setProjectionFovY(Math::HALF_PI,Math::ONE,1,1000);
		mCamera->setScope(1);
		mCamera->setClearFlags(Renderer::ClearFlag_DEPTH);
		getEngine()->getScene()->getRootNode()->attach(mCamera);

		getEngine()->setDirectory("../data/");

		mElco=mEngine->createNodeType(ParentNode::type());
		{
			MeshNode::ptr mesh=mEngine->createNodeType(MeshNode::type());
			mesh->setMesh(mEngine->getMeshManager()->findMesh("elco.xmsh"));
			mesh->setTranslate(0,0,mesh->getMesh()->boundingRadius/2);
			mesh->getAnimationController()->setSequenceIndex(1);
			mesh->getAnimationController()->setCycling(AnimationController::Cycling_LOOP);
			mesh->getAnimationController()->start();
			mElco->attach(mesh);
		}
		mEngine->getScene()->getRootNode()->attach(mElco);

		mMerv=mEngine->createNodeType(ParentNode::type());
		{
			MeshNode::ptr mesh=mEngine->createNodeType(MeshNode::type());
			mesh->setMesh(mEngine->getMeshManager()->findMesh("merv.xmsh"));
			mesh->setTranslate(0,0,mesh->getMesh()->boundingRadius/2);
			mesh->getAnimationController()->setSequenceIndex(1);
			mesh->getAnimationController()->setCycling(AnimationController::Cycling_LOOP);
			mesh->getAnimationController()->start();
			mMerv->attach(mesh);
		}
		mEngine->getScene()->getRootNode()->attach(mMerv);

		mLight=mEngine->createNodeType(LightNode::type());
		mLight->setLightType(Light::Type_DIRECTION);
		mLight->setDirection(Math::NEG_Z_UNIT_VECTOR3);
		mEngine->getScene()->getRootNode()->attach(mLight);

		if (!setupARCamera("../data/camera_para.dat", "", &mARTCparam)) {
			Error::unknown("error setting up camera");
			return;
		}
	
		mPatternIDs.resize(2);
		if((mPatternIDs[0]=arLoadPatt("../data/patt.sample1"))<0){
			Error::unknown("error loading pattern");
			return;
		}
		if((mPatternIDs[1]=arLoadPatt("../data/patt.sample2"))<0){
			Error::unknown("error loading pattern");
			return;
		}

		mBackgroundTexture=mEngine->getTextureManager()->createTexture(Texture::UsageFlags_NONE,Texture::Dimension_D2,Texture::Format_BGR_8,mARTCparam.xsize,mARTCparam.ysize,1,1);
		mBackgroundTextureStage=TextureStage::ptr(new TextureStage(mBackgroundTexture));

		VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_WRITE_ONLY,mEngine->getVertexFormats().POSITION_TEX_COORD,4);
		VertexBufferAccessor vba;
		{
			vba.lock(vertexBuffer);
			
			vba.set3(0,0,0,0,0);
			vba.set2(0,1,0,0);

			vba.set3(1,0,0,1,0);
			vba.set2(1,1,0,1);

			vba.set3(2,0,1,0,0);
			vba.set2(2,1,1,0);

			vba.set3(3,0,1,1,0);
			vba.set2(3,1,1,1);
			
			vba.unlock();
		}
		mBackgroundVertexData=VertexData::ptr(new VertexData(vertexBuffer));
		mBackgroundIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,IndexBuffer::ptr(),0,4));
	}
	
	void destroy(){
		mBackgroundTexture=Texture::ptr();
		mBackgroundTextureStage=TextureStage::ptr();
		mBackgroundVertexData=VertexData::ptr();
		mBackgroundIndexData=IndexData::ptr();

		mCamera=NULL;
		mElco=NULL;
		mMerv=NULL;
		mLight=NULL;

		arVideoCapStop();
		arVideoClose();

		Application::destroy();
	}
	
	void update(int dt){
		Application::update(dt);

		if((mLastImage=arVideoGetImage())!=NULL){
			double patternTransform[3][4];
			Matrix4x4 transform;
			int numMarkers=0;
			ARMarkerInfo *markerInfo=NULL;

			// Detect the markers in the video frame.
			if (arDetectMarker(mLastImage, THRESHOLD, &markerInfo, &numMarkers) < 0) {
				return;
			}
			
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
			mCamera->setProjectionTransform(transform);

			// Calculate the camera position relative to the marker.
			if(bestMarkers[0]!=-1){
				arGetTransMatCont(&(markerInfo[bestMarkers[0]]), patternTransform, PATTERN_CENTER, PATTERN_WIDTH, patternTransform);
				setMatrix4x4FromARMatrix(transform,patternTransform);
				mElco->setTransform(transform);
				mElco->setScope(1);
			}
			else{
				mElco->setScope(2);
			}

			if(bestMarkers[1]!=-1){
				arGetTransMatCont(&(markerInfo[bestMarkers[1]]), patternTransform, PATTERN_CENTER, PATTERN_WIDTH, patternTransform);
				setMatrix4x4FromARMatrix(transform,patternTransform);
				mMerv->setTransform(transform);
				mMerv->setScope(1);
			}
			else{
				mMerv->setScope(2);
			}
		}

		getEngine()->getScene()->update(dt);
	}

	void render(Renderer *renderer){
		if(mLastImage!=NULL){
			mBackgroundTexture->load(fromARFormat(AR_DEFAULT_PIXEL_FORMAT),mARTCparam.xsize,mARTCparam.ysize,1,0,mLastImage);
		}

		arVideoCapNext(); // Get next video frame, we're done with the previous one

		renderer->setDefaultStates();

		renderer->beginScene();
			renderer->clear(Renderer::ClearFlag_DEPTH,Colors::BLACK);

			renderer->setProjectionMatrix(mProjectionMatrix);
			renderer->setModelMatrix(Math::IDENTITY_MATRIX4X4);
			renderer->setViewMatrix(Math::IDENTITY_MATRIX4X4);
			renderer->setTextureStage(0,mBackgroundTextureStage);
			renderer->renderPrimitive(mBackgroundVertexData,mBackgroundIndexData);

			getEngine()->getScene()->render(renderer,mCamera,NULL);
		renderer->endScene();
		renderer->swap();
	}

	void resized(int width,int height){
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

	static int fromARFormat(int format){
		switch(format){
			case AR_PIXEL_FORMAT_RGBA:
				return Texture::Format_RGBA_8;
			case AR_PIXEL_FORMAT_ABGR: // SGI
				return Texture::Format_UNKNOWN;
			case AR_PIXEL_FORMAT_BGRA: // Windows
				return Texture::Format_BGRA_8;
			case AR_PIXEL_FORMAT_ARGB:	// Mac
				return Texture::Format_UNKNOWN;
			case AR_PIXEL_FORMAT_RGB:
				return Texture::Format_RGB_8;
			case AR_PIXEL_FORMAT_BGR:
				return Texture::Format_BGR_8;
			case AR_PIXEL_FORMAT_MONO:
				return Texture::Format_L_8;
			case AR_PIXEL_FORMAT_2vuy:
				return Texture::Format_UNKNOWN;
			case AR_PIXEL_FORMAT_yuvs:
				return Texture::Format_UNKNOWN;
			default:
				return Texture::Format_UNKNOWN;
		}
	}

	static void setMatrix4x4FromARProjection(Matrix4x4 &r,ARParam *cparam,const double minDistance,const double maxDistance){
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

	static void setMatrix4x4FromARMatrix(Matrix4x4 &r,const double para[3][4]){
		r.setAt(0,0,para[0][0]);	r.setAt(0,1,para[0][1]);	r.setAt(0,2,para[0][2]);	r.setAt(0,3,para[0][3]);
		r.setAt(1,0,-para[1][0]);	r.setAt(1,1,-para[1][1]);	r.setAt(1,2,-para[1][2]);	r.setAt(1,3,-para[1][3]);
		r.setAt(2,0,-para[2][0]);	r.setAt(2,1,-para[2][1]);	r.setAt(2,2,-para[2][2]);	r.setAt(2,3,-para[2][3]);
		r.setAt(3,0,0);				r.setAt(3,1,0);				r.setAt(3,2,0);				r.setAt(3,3,Math::ONE);
	}

	ARParam mARTCparam;
	Collection<int> mPatternIDs;
	ARUint8 *mLastImage;

	Matrix4x4 mProjectionMatrix;
	Texture::ptr mBackgroundTexture;
	TextureStage::ptr mBackgroundTextureStage;
	VertexData::ptr mBackgroundVertexData;
	IndexData::ptr mBackgroundIndexData;

	CameraNode::ptr mCamera;
	ParentNode::ptr mElco;
	ParentNode::ptr mMerv;
	LightNode::ptr mLight;
};

int main(int argc, char** argv){
	ARToadlet app;
	app.setSize(800,1024);
	app.create();
	app.start();
	app.destroy();
	return 0;
}