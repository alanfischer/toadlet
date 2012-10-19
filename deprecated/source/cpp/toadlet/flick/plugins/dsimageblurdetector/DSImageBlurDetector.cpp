/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the Toadlet Engine, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#include "DSImageBlurDetector.h"
#include <toadlet/egg/Error.h>

#pragma comment(lib,"dmoguids.lib")
#pragma comment(lib,"strmiids.lib")
#pragma comment(lib,"strmbase.lib")

using namespace toadlet::egg;

namespace toadlet{
namespace flick{

TOADLET_C_API ImageBlurDetector *new_DSImageBlurDetector(){
	return new DSImageBlurDetector();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API ImageBlurDetector* new_ImageBlurDetector(){
		return new DSImageBlurDetector();
	}
#endif

DSImageBlurDetector::DSImageBlurDetector(){
	mListener=NULL;
	mAngle=0;
	mAngleClusterSize=0;
	mAngleClusterWidth=0;
	mLighting=LR_GOOD;

	mCheckLighting=false;
	mLightingResult=LR_GOOD;
	mStartImageTaking=false;

	mImageData=NULL;
	mImageWidth=0;
	mImageHeight=0;

	mPrestartSleepTime=200;
	mPrecaptureSleepTime=250;

	mRun=false;
	mAlwaysRunGraph=false;
	mState=STATE_DESTROYED;
	mStatus=STATUS_STOPPED;
	mDetectionThread=NULL;

	CoInitializeEx(NULL,COINIT_MULTITHREADED);

	if(mCoreDLL.load("coredll")){
		mFindFirstDeviceFunc=(FindFirstDeviceFunc)mCoreDLL.getSymbol("FindFirstDevice");
	}
	else{
		mFindFirstDeviceFunc=NULL;
	}
}

DSImageBlurDetector::~DSImageBlurDetector(){
	TOADLET_ASSERT(mState==STATE_DESTROYED);

	mListener=NULL;
}

bool DSImageBlurDetector::available(){
	{
		wchar_t woem[1024];
		if(SystemParametersInfo(SPI_GETOEMINFO,sizeof(woem),woem,0)!=0){
			String oem(woem);
			Logger::log(Logger::TOADLET_FLICK,Logger::Level_ALERT,
				String("OEM:")+oem);

			if(oem=="Palm Treo 700w"){
				Logger::log(Logger::TOADLET_FLICK,Logger::Level_ALERT,
					"No camera available on Palm Treo 700w, due to bitrate too low");

				return false;
			}
		}
	}

	String camera=getFirstCameraDriver();

	Logger::log(Logger::TOADLET_FLICK,Logger::Level_ALERT,
		String("Camera driver: ")+camera);

	if(camera.length()==0){
		return false;
	}
	else{
		return true;
	}
}

bool DSImageBlurDetector::create(){
	if(mState==STATE_STOPPED){
		return true;
	}

	if(mState!=STATE_DESTROYED){
		Error::sequence(Logger::TOADLET_FLICK,
			"create called out of sequence");
		return false;
	}

    // Create and initialize the video capture filter
    CPropertyBag propertyBag;
    CComPtr<IPersistPropertyBag> capturePropertyBag;
    mVideoCaptureFilter.CoCreateInstance( CLSID_VideoCapture );
    mVideoCaptureFilter.QueryInterface( &capturePropertyBag );

    // We are loading the driver CAM1 in the video capture filter.
    String camera=getFirstCameraDriver();
    CComVariant varCamName=camera.wc_str();
	HRESULT hr=propertyBag.Write(L"VCapName",&varCamName);
	if(FAILED(hr)){
		Error::unknown(Logger::TOADLET_FLICK,
			"Error writing Property Bag");
		return false;
	}

	hr=capturePropertyBag->Load(&propertyBag,NULL);
	if(FAILED(hr)){
		Error::unknown(Logger::TOADLET_FLICK,
			"Error loading Property Bag");
		return false;
	}

	mState=STATE_STOPPED;

	return true;
}

bool DSImageBlurDetector::startup(){
	if(mState==STATE_RUNNING){
		return true;
	}

	if(mState!=STATE_STOPPED){
		Error::sequence(Logger::TOADLET_FLICK,
			"startup called out of sequence");
		return false;
	}

	mState=STATE_RUNNING;

	mRun=true;
	mAlwaysRunGraph=true;
	mStatus=STATUS_STARTING;
	mDetectionThread=new Thread(this);
	mDetectionThread->start();

	return true;
}

bool DSImageBlurDetector::shutdown(){
	if(mState==STATE_STOPPED){
		return true;
	}

	if(mState!=STATE_RUNNING){
		Error::sequence(Logger::TOADLET_FLICK,
			"shutdown called out of sequence");
		return false;
	}

	mState=STATE_STOPPED;

	mRun=false;

	mLock.lock();
		mLockWaitCondition.notify();
	mLock.unlock();

	while(mStatus!=STATUS_STOPPED){
		System::msleep(100);
	}

	return true;
}

bool DSImageBlurDetector::destroy(){
	if(mState==STATE_DESTROYED){
		return true;
	}

	if(mState!=STATE_STOPPED){
		Error::sequence(Logger::TOADLET_FLICK,
			"destroy called out of sequence");
		return false;
	}

	mVideoCaptureFilter=NULL;

	mState=STATE_DESTROYED;

	return true;
}

void DSImageBlurDetector::setSleepTimes(int prestart,int precapture){
	mPrestartSleepTime=prestart;
	mPrecaptureSleepTime=precapture;
}

void DSImageBlurDetector::setListener(ImageBlurDetectorListener *listener){
	mListener=listener;
}

bool DSImageBlurDetector::startDetection(){
	if(mStatus==STATUS_BUSY){
		return false; // Detection already running
	}

	while(mStatus==STATUS_STARTING){
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_ALERT,
			"Waiting for capture graph to be built");
		System::msleep(100);
	}

	mLock.lock();
		mCheckLighting=false;
		mStartImageTaking=true;
		mLockWaitCondition.notify();
	mLock.unlock();

	return true;
}

int DSImageBlurDetector::getDetectedAngle(){
	return mAngle;
}

int DSImageBlurDetector::getDetectedAngleClusterSize(){
	return mAngleClusterSize;
}

int DSImageBlurDetector::getDetectedAngleClusterWidth(){
	return mAngleClusterWidth;
}

bool DSImageBlurDetector::startLightingCheck(){
	if(mStatus==STATUS_BUSY){
		return false; // Detection already running
	}

	while(mStatus==STATUS_STARTING){
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,
			"Waiting for capture graph to be built");
		System::msleep(100);
	}

	mLock.lock();
		mCheckLighting=true;
		mStartImageTaking=true;
		mLockWaitCondition.notify();
	mLock.unlock();

	return true;
}

ImageBlurDetector::LightingResult DSImageBlurDetector::getLightingResult(){
	return mLightingResult;
}

ImageBlurDetector::State DSImageBlurDetector::getState(){
	return mState;
}

const static float PI=3.1415926f;
const static float QUARTER_PI=PI/4.0f;
const static float THREE_QUARTER_PI=3.0f*PI/4.0f;
const static int NUM_RESULTS=20;
const static int TOTAL_CLUSTERS=5;

void DSImageBlurDetector::run(){
	HRESULT hr=createCaptureGraph();

	if(mAlwaysRunGraph){
		runCaptureGraph();
	}

	while(mRun){
		mLock.lock();
			mStatus=STATUS_READY;

			if(mStartImageTaking==false){
				mLockWaitCondition.wait(&mLock);
				mStartImageTaking=false;
			}

			if(mRun==false){
				break;
			}

			mStatus=STATUS_BUSY;
		mLock.unlock();

		if(SUCCEEDED(hr)){
			if(mCheckLighting==false){
				System::msleep(mPrestartSleepTime);

				if(mListener!=NULL){
					mListener->imageCaptureStarting();
				}

				System::msleep(mPrecaptureSleepTime);

				hr=captureStillImage();

				if(mListener!=NULL){
					mListener->imageCaptureFinished();
				}

				findAngleInImage(mImageData,mImageWidth,mImageHeight,mAngle,mAngleClusterSize,mAngleClusterWidth);

				#if defined(TOADLET_DEBUG)
					drawLineAtAngle(mImageData,mImageWidth,mImageHeight,mAngle*PI/180.0f);
				#endif
			}
			else{
				hr=captureStillImage();

				int length=mImageWidth*mImageHeight;
				int brightness=0;
				int i;
				for(i=0;i<length;++i){
					brightness+=((0x00FF0000&mImageData[i])>>16) + ((0x0000FF00&mImageData[i])>>8) + (0x000000FF&mImageData[i]);
				}
				brightness/=(length*3);

				// Too bright for detection
				if(brightness>255-35){
					mLightingResult=LR_TOO_BRIGHT;
				}
				// Too dim for detection
				else if(brightness<35){
					mLightingResult=LR_TOO_DIM;
				}
				// Just right
				else{
					mLightingResult=LR_GOOD;
				}
			}
		}

		if(mListener!=NULL){
			mListener->detectionFinished();
		}
	}

	if(mAlwaysRunGraph){
		stopCaptureGraph();
	}

	destroyCaptureGraph();

	if(mDetectionThread!=NULL){
		delete mDetectionThread;
		mDetectionThread=NULL;
	}

	mStatus=STATUS_STOPPED;

	if(FAILED(hr)){
		Error::unknown(Logger::TOADLET_FLICK,
			"error running detector");
		return;
	}
}

void DSImageBlurDetector::findAngleInImage(int data[],int width,int height,int &angle,int &angleClusterSize,int &angleClusterWidth){
	int bestAngle[TOTAL_CLUSTERS*2];
	int bestPeakCount[TOTAL_CLUSTERS*2];
	int results[NUM_RESULTS];

	memset(bestAngle,0,sizeof(bestAngle));
	memset(bestPeakCount,0,sizeof(bestPeakCount));
	memset(results,0,sizeof(results));

	int i,j,k,l,m;
	
	for(i=0;i<180;i+=2){
		radon(data,width,height,i*PI/180.0f,results,4);

		int peakCount=0;
		int direction=0;
		for(j=1;j<NUM_RESULTS;++j){
			int newDirection=results[j]-results[j-1];
			if(abs(direction-newDirection)<2){
				//nothin
			}
			else{
				peakCount++;
				direction=newDirection;
			}
		}

		for(j=0;j<TOTAL_CLUSTERS*2;++j){
			if(peakCount>bestPeakCount[j]){
				bestAngle[j]=i;
				bestPeakCount[j]=peakCount;
				break;
			}
		}
	}
	
	int clusterPoints[TOTAL_CLUSTERS][TOTAL_CLUSTERS*2];
	int clusterSize[TOTAL_CLUSTERS];
	int clusterWidth[TOTAL_CLUSTERS];
	float clusterCentroid[TOTAL_CLUSTERS];
	float clusterInvDensity[TOTAL_CLUSTERS];
	
	for(i=0;i<TOTAL_CLUSTERS;++i){
		memset(clusterPoints[i],0,sizeof(clusterPoints[i]));
	}
	memset(clusterSize,0,sizeof(clusterSize));
	memset(clusterWidth,0,sizeof(clusterWidth));
	memset(clusterCentroid,0,sizeof(clusterCentroid));
	memset(clusterInvDensity,0,sizeof(clusterInvDensity));
	
	float distance=0;
	float minDistance=0;
	int minDistanceIndex=0;
	int clusterIndex=0;
	int minClusterIndex=0;
	bool b;
	
	i=TOTAL_CLUSTERS*2;
	b=true;
	while(b){
		--i;
		b=false;
		for(j=0;j<i;++j){
			if(bestAngle[j]>bestAngle[j+1]){
				k=bestAngle[j];
				bestAngle[j]=bestAngle[j+1];
				bestAngle[j+1]=k;
				b=true;
			}
		}
	}

	#if defined(TOADLET_DEBUG)
		for(i=0;i<TOTAL_CLUSTERS*2;++i){
			Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,
				String("bestAngle[")+i+String("]=")+bestAngle[i]);
		}
	#endif
	
	for(i=0;i<TOTAL_CLUSTERS;++i){
		minDistance=180;
		minDistanceIndex=-1;
		clusterIndex=0;
		minClusterIndex=-1;
		
		// Find the closest pair of points and/or clusters
		for(j=0;j<(TOTAL_CLUSTERS*2)-1;++j){
			if(bestAngle[j]==-1){
				if(clusterSize[clusterIndex]==0 || j+clusterSize[clusterIndex]>=TOTAL_CLUSTERS*2){
					continue;
				}
				j=j+clusterSize[clusterIndex]-1;
				// Compare a selected cluster with another cluster
				if(bestAngle[j+1]==-1){
					if(clusterIndex+1>=TOTAL_CLUSTERS){
						continue;
					}
					distance=fabs(clusterCentroid[clusterIndex+1]-clusterCentroid[clusterIndex]);
					if(distance<minDistance){
						minDistance=distance;
						minDistanceIndex=j;
						minClusterIndex=clusterIndex;
					}
				}
				// Compare a selected cluster with a point
				else{
					distance=fabs(bestAngle[j+1]-clusterCentroid[clusterIndex]);
					if(distance<minDistance){
						minDistance=distance;
						minDistanceIndex=j;
						minClusterIndex=clusterIndex;
					}
				}
				
			}
			// Compare the selected point with a cluster
			else if(bestAngle[j+1]==-1){
				distance=fabs(clusterCentroid[clusterIndex]-bestAngle[j]);
				if(distance<minDistance){
					minDistance=distance;
					minDistanceIndex=j;
					minClusterIndex=clusterIndex;
				}
			}
			// Compare the selected point with another point
			else{
				distance=fabs(bestAngle[j+1]-bestAngle[j]);
				if(distance<minDistance){
					minDistance=distance;
					minDistanceIndex=j;
					minClusterIndex=-1;
				}
			}
			
			if(bestAngle[j]==-1){
				++clusterIndex;
			}
		}
		
		// Cluster together the closest pair of points and/or clusters
		j=minDistanceIndex+1;
		if(bestAngle[minDistanceIndex]==-1){
			// Join 2 clusters and ensure all cluster data fields are next to each other in their respective arrays
			if(bestAngle[j]==-1){
				for(k=0;k<clusterSize[minClusterIndex+1];++k){
					clusterPoints[minClusterIndex][clusterSize[minClusterIndex]]=clusterPoints[minClusterIndex+1][k];
					clusterSize[minClusterIndex]+=1;
				}
				clusterSize[minClusterIndex+1]=0;
				clusterCentroid[minClusterIndex+1]=0;
				k=minClusterIndex+1;
				for(l=k;l<mTotalClusters;++l){
					if(clusterSize[l]>0){
						for(m=0;m>clusterSize[l];++m){
							clusterPoints[k][m]=clusterPoints[l][m];
						}
						clusterSize[k]=clusterSize[l];
						clusterCentroid[k]=clusterCentroid[l];
						clusterSize[l]=0;
						++k;
					}
				}
			}
			// Add a single point onto the end of an existing cluster
			else{
				clusterPoints[minClusterIndex][clusterSize[minClusterIndex]]=bestAngle[j];
				clusterSize[minClusterIndex]+=1;
				bestAngle[j]=-1;
			}
		}
		// Add a point onto the upcoming cluster
		else if(bestAngle[j]==-1){
			for(k=clusterSize[minClusterIndex];k>0;--k){
				clusterPoints[minClusterIndex][k]=clusterPoints[minClusterIndex][k-1];
			}
			clusterPoints[minClusterIndex][0]=bestAngle[minDistanceIndex];
			clusterSize[minClusterIndex]+=1;
			bestAngle[minDistanceIndex]=-1;
		}
		// Join 2 points together into a new cluster
		else{
			for(k=0;k<mTotalClusters;++k){
				if(clusterSize[k]==0){
					break;
				}
			}
			clusterPoints[k][clusterSize[k]]=bestAngle[minDistanceIndex];
			clusterPoints[k][clusterSize[k]+1]=bestAngle[j];
			clusterSize[k]=2;
			minClusterIndex=k;
			bestAngle[minDistanceIndex]=-1;
			bestAngle[j]=-1;
		}
		
		// Update the centroid of any altered clusters
		if(minClusterIndex!=-1){
			k=0;
			for(j=0;j<clusterSize[minClusterIndex];++j){
				k+=clusterPoints[minClusterIndex][j];
			}
			clusterCentroid[minClusterIndex]=(float)k/(float)clusterSize[minClusterIndex];
		}
		
		#if defined(TOADLET_DEBUG)
			Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,
				String("Cluster loop:")+i);
			for(j=0;j<mTotalClusters;++j){
				if(clusterSize[j]>0){
					Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,
						String("Cluster #=")+j);
					for(k=0;k<clusterSize[j];++k){
						Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,
							String("   point[")+k+String("]=")+clusterPoints[j][k]);
					}
				}
			}
		#endif
	}
	
	// Analyize the clusters
	float peakResult=INVALID_ANGLE_DETECTED;
	int bestCluster=-1;
	int pts[TOTAL_CLUSTERS*2];
	b=false;
	for(i=0;i<TOTAL_CLUSTERS;++i){
		if(clusterSize[i]>2){
			for(j=0;j<clusterSize[i];++j){
				pts[j]=clusterPoints[i][j];
			}
			
			// Bubble sort pts
			k=clusterSize[i];
			b=true;
			while(b){
				--k;
				b=false;
				for(j=0;j<k;++j){
					if(pts[j]>pts[j+1]){
						l=pts[j];
						pts[j]=pts[j+1];
						pts[j+1]=l;
						b=true;
					}
				}
			}
			b=true;

			for(j=0;j<clusterSize[i];++j){
				clusterPoints[i][j]=pts[j];
			}

			clusterWidth[i]=pts[clusterSize[i]-1]-pts[0];
			clusterInvDensity[i]=(float)clusterWidth[i]/(float)clusterSize[i];

			if(bestCluster==-1){
				bestCluster=i;
			}
			else if(clusterInvDensity[i]<clusterInvDensity[bestCluster]){
				bestCluster=i;
			}
		}
	}
	
	// If we never found a cluster > 2 elements, it's trash; likewise if our found cluster's density is too low
	if(b==true && clusterInvDensity[bestCluster]<11){
		peakResult=clusterCentroid[bestCluster];
	}
	
	// If peak result is outside a reasonable range, again it's probably trash
	if(peakResult<30 || peakResult>150){
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,
			"Invalid Angle");

		peakResult=INVALID_ANGLE_DETECTED;
	}
	else{
		// For some reason the angle is mirrored
		peakResult=180-peakResult;
	}
	
	#if defined(TOADLET_DEBUG)
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,"Analyzed clusters:");
		for(i=0;i<mTotalClusters;++i){
			if(clusterSize[j]>2){
				Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,String("Cluster #=")+i);
				Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,String("   size #=")+clusterSize[i]);
				Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,String("   width #=")+clusterWidth[i]);
				Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,String("   center #=")+clusterCentroid[i]);
				Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,String("   invDensity #=")+clusterInvDensity[i]);
				for(j=0;j<clusterSize[i];++j){
					Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,String("      point[")+k+String("]=")+clusterPoints[i][j]);
				}
			}
		}
		if(peakResult==INVALID_ANGLE_DETECTED){
			Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,String("Invalid angle detected!"));
		}
		else{
			Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,String("Best Cluster = Cluster[")+bestCluster+String("] size=")+clusterSize[bestCluster]+String(" width=")+clusterWidth[bestCluster]+String(" center=")+clusterCentroid[bestCluster]+String(" inv density=")+clusterInvDensity[bestCluster]);
			Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,String("Selected peak result=")+(int)peakResult);
		}
	#endif

	mAngle=(int)peakResult;
	if(peakResult==INVALID_ANGLE_DETECTED){
		mAngleClusterSize=INVALID_ANGLE_DETECTED;
		mAngleClusterWidth=INVALID_ANGLE_DETECTED;
	}
	else{
		mAngleClusterSize=clusterSize[bestCluster];
		mAngleClusterWidth=clusterWidth[bestCluster];
	}
}

// Bresenham's line drawing algorithm
// Modified to avoid the inner multiplies
// No boundary checking, since we rely on the calling function to clip the segment
int DSImageBlurDetector::sumLine(int x0,int y0,int x1,int y1,int data[],int width,int height){
	int dy=y1-y0;
	int dx=x1-x0;
	int stepx=0,stepy=0;

	if(dy<0){
		dy=-dy;
		stepy=-width;
	}
	else{
		stepy=width;
	}

	if(dx<0){
		dx=-dx;
		stepx=-1;
	}
	else{
		stepx=1;
	}

	dy<<=1;
	dx<<=1;

	y0*=width;
	y1*=width;
	
	int sum=0;
	int count=0;
	int val=0;

	val=data[x0+y0];
	sum+=((0x00FF0000&val)>>16) + ((0x0000FF00&val)>>8) + (0x000000FF&val);
	count++;

	if(dx>dy){
		int fraction=dy-(dx>>1);
		while(x0!=x1){
			if(fraction>=0){
				y0+=stepy;
				fraction-=dx;
			}
			x0+=stepx;
			fraction+=dy;

			val=data[x0+y0];
			sum+=((0x00FF0000&val)>>16) + ((0x0000FF00&val)>>8) + (0x000000FF&val);
			count++;
		}
	}
	else{
		int fraction=dx-(dy>>1);
		while(y0!=y1){
			if(fraction>=0){
				x0+=stepx;
				fraction-=dy;
			}
			y0+=stepy;
			fraction+=dx;

			val=data[x0+y0];
			sum+=((0x00FF0000&val)>>16) + ((0x0000FF00&val)>>8) + (0x000000FF&val);
			count++;
		}
	}

	if(count==0){
		return 0xFF;
	}
	else{
		return sum/(count*3);
	}
}

void DSImageBlurDetector::radon(int data[],int width,int height,float theta,int results[],int spacing){
	int halfAddedLines=(NUM_RESULTS-1)/2;

	int size=((width<height)?width:height);
	int hsize=size/2;
	int sizem1=size-1;

	int minX=-hsize;
	int maxX=hsize-1;
	int minY=-hsize;
	int maxY=hsize-1;

	if(theta>=QUARTER_PI && theta<=THREE_QUARTER_PI){
		float sintheta=(float)sin(theta);
		if(sintheta<0){
			sintheta=-sintheta;
		}

		int mainEndY=maxY;
		int mainEndX=(int)(mainEndY/tan(theta));
		int mainStartY=minY;
		int mainStartX=-mainEndX;

		results[halfAddedLines]=sumLine(mainStartX - minX,mainStartY - minY,mainEndX - minX,mainEndY - minY,data,width,height);

		int startX=0,startY=0,endX=0,endY=0;
		int i;
		for(i=0;i<halfAddedLines;++i){
			int offset=(int)(i*spacing/sintheta);

			// Add line to the "left" of the line
			startX=mainStartX - offset - minX;
			startY=minY - minY;
			endX=mainEndX - offset - minX;
			endY=maxY - minY;

			if(startX<0){
				if(endX<0){
					results[halfAddedLines+i]=0;
					continue;
				}

				startY=(endY-startY)*(0-startX)/(endX-startX);
				startX=0;
			}
			else if(endX<0){
				endY=(endY-startY)*(0-startX)/(endX-startX);
				endX=0;
			}

			results[halfAddedLines+i]=sumLine(startX,startY,endX,endY,data,width,height);

			// TODO: Instead of re-clipping this line, can we do the initial clipping operations in 'center space'
			//  and then just flip?
			// Add line to the "right" of the line
			startX=mainStartX + offset - minX;
			startY=minY - minY;
			endX=mainEndX + offset - minX;
			endY=maxY - minY;

			if(startX>sizem1){
				if(endX>sizem1){
					results[halfAddedLines-i]=0;
					continue;
				}

				startY=(endY-startY)*(sizem1-startX)/(endX-startX);
				startX=sizem1;
			}
			else if(endX>sizem1){
				endY=(endY-startY)*(sizem1-startX)/(endX-startX);
				endX=sizem1;
			}

			results[halfAddedLines-i]=sumLine(startX,startY,endX,endY,data,width,height);
		}
	}
	else{
		float costheta=(float)cos(theta);
		if(costheta<0){
			// This will cause the OUT_IMAGE of the radon transform to be non-continuous
			// but it saves us from having to do extra bounds checking, so we're ok with that
			costheta=-costheta;
		}

		int mainEndX=maxX;
		int mainEndY=(int)(mainEndX*tan(theta));
		int mainStartX=minX;
		int mainStartY=-mainEndY;

		results[halfAddedLines]=sumLine(mainStartX - minX,mainStartY - minY,mainEndX - minX,mainEndY - minY,data,width,height);

		int startX=0,startY=0,endX=0,endY=0;
		int i;
		for(i=0;i<halfAddedLines;++i){
			int offset=(int)(i*spacing/costheta);

			// Add line to the "left" of the line
			startY=mainStartY - offset - minY;
			startX=minX - minX;
			endY=mainEndY - offset - minY;
			endX=maxX - minX;

			if(startY<0){
				if(endY<0){
					results[halfAddedLines-i]=0;
					continue;
				}

				startX=(endX-startX)*(0-startY)/(endY-startY);
				startY=0;
			}
			else if(endY<0){
				endX=(endX-startX)*(0-startY)/(endY-startY);
				endY=0;
			}

			results[halfAddedLines-i]=sumLine(startX,startY,endX,endY,data,width,height);

			// TODO: Instead of re-clipping this line, can we do the initial clipping operations in 'center space'
			//  and then just flip?
			// Add lines to the "right" of the line
			startY=mainStartY + offset - minY;
			startX=minX - minX;
			endY=mainEndY + offset - minY;
			endX=maxX - minX;

			if(startY>sizem1){
				if(endY>sizem1){
					results[halfAddedLines+i]=0;
					continue;
				}

				startX=(endX-startX)*(sizem1-startY)/(endY-startY);
				startY=sizem1;
			}
			else if(endY>sizem1){
				endX=(endX-startX)*(sizem1-startY)/(endY-startY);
				endY=sizem1;
			}

			results[halfAddedLines+i]=sumLine(startX,startY,endX,endY,data,width,height);
		}
	}
}

// Bresenham's line drawing algorithm
// Modified to avoid the inner multiplies
// No boundary checking, since we rely on the calling function to clip the segment
void DSImageBlurDetector::drawLine(int x0,int y0,int x1,int y1,int data[],int width,int height){
	int dy=y1-y0;
	int dx=x1-x0;
	int stepx=0,stepy=0;

	if(dy<0){
		dy=-dy;
		stepy=-width;
	}
	else{
		stepy=width;
	}

	if(dx<0){
		dx=-dx;
		stepx=-1;
	}
	else{
		stepx=1;
	}

	dy<<=1;
	dx<<=1;

    y0*=width;
    y1*=width;
	
	data[x0+y0]=0x00FF0000;

	if(dx>dy){
		int fraction=dy-(dx>>1);
		while(x0!=x1){
			if(fraction>=0){
				y0+=stepy;
				fraction-=dx;
			}

			x0+=stepx;
			fraction+=dy;

			data[x0+y0]=0x00FF0000;
		}
	}
	else{
		int fraction=dx-(dy>>1);
		while(y0!=y1){
			if(fraction>=0){
				x0+=stepx;
				fraction-=dy;
			}

			y0+=stepy;
			fraction+=dx;

			data[x0+y0]=0x00FF0000;
		}
	}
}

void DSImageBlurDetector::drawLineAtAngle(int data[],int width,int height,float theta){
	int size=((width<height)?width:height);
	int hsize=size/2;
	int sizem1=size-1;

	int minX=-hsize;
	int maxX=hsize-1;
	int minY=-hsize;
	int maxY=hsize-1;

	if(theta>=QUARTER_PI && theta<=THREE_QUARTER_PI){
		float sintheta=(float)sin(theta);
		if(sintheta<0){
			sintheta=-sintheta;
		}

		int mainEndY=maxY;
		int mainEndX=(int)(mainEndY/tan(theta));
		int mainStartY=minY;
		int mainStartX=-mainEndX;

		drawLine(mainStartX - minX,mainStartY - minY,mainEndX - minX,mainEndY - minY,data,width,height);
	}
	else{
		float costheta=(float)cos(theta);
		if(costheta<0){
			// This will cause the OUT_IMAGE of the random transform to be non-continuous
			// but it saves us from having to do extra bounds checking, so we're ok with that
			costheta=-costheta;
		}

		int mainEndX=maxX;
		int mainEndY=(int)(mainEndX*tan(theta));
		int mainStartX=minX;
		int mainStartY=-mainEndY;

		drawLine(mainStartX - minX,mainStartY - minY,mainEndX - minX,mainEndY - minY,data,width,height);
	}
}

String DSImageBlurDetector::getFirstCameraDriver(){
	HRESULT hr=S_OK;
	HANDLE handle=NULL;
	DEVMGR_DEVICE_INFORMATION di;
	GUID guidCamera = { 0xCB998A05, 0x122C, 0x4166, 0x84, 0x6A, 0x93, 0x3E, 0x4D, 0x7E, 0x3C, 0x86 };
	// Note about the above: The driver material doesn't ship as part of the SDK. This GUID is hardcoded
	// here to be able to enumerate the camera drivers and pass the name of the driver to the video capture filter

	memset(&di,0,sizeof(di));
	di.dwSize=sizeof(di);

	if(mFindFirstDeviceFunc==NULL){
		return String();
	}
	else{
		handle=(*mFindFirstDeviceFunc)(DeviceSearchByGuid,&guidCamera,&di);
		if((handle==INVALID_HANDLE_VALUE) || (di.hDevice==NULL)){
			return String();
		}
	}

	String name=di.szLegacyName;

	FindClose(handle);

	return name;
}

HRESULT DSImageBlurDetector::createCaptureGraph(){
    HRESULT       hr=S_OK;

    CComPtr<IGraphBuilder>          pFilterGraph;
    CComPtr<IBaseFilter>			pImageSinkFilter;

	Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,"createCaptureGraph:begin");

	// Create capture graph
	hr=mCaptureGraphBuilder.CoCreateInstance( CLSID_CaptureGraphBuilder );
	if(FAILED(hr) || mCaptureGraphBuilder.p==NULL){
		Error::unknown(Logger::TOADLET_FLICK,
			"Error creating capture graph builder");
		return E_FAIL;
	}

	// Register the filtergraph manager. 
	pFilterGraph.CoCreateInstance( CLSID_FilterGraph );
	mCaptureGraphBuilder->SetFiltergraph( pFilterGraph );

    // Everything succeeded, the video capture filter is added to the filtergraph
    hr=pFilterGraph->AddFilter( mVideoCaptureFilter, L"Video Capture Filter Source" );
	if(FAILED(hr)){
		Error::unknown(Logger::TOADLET_FLICK,
			"Error adding filter");
		return hr;
	}

	// Create the still image filter
	CDump *dump=(CDump*)CDump::CreateInstance(NULL,&hr);
	dump->AddRef();
	mDump.Attach(dump);
	mDump->QueryInterface(IID_IBaseFilter,(void**)&pImageSinkFilter);

    // Added the still image filter to the filtergraph
    pFilterGraph->AddFilter( pImageSinkFilter, L"Still image filter" );

	// Connect the filters together
	IPin *pInputPin=NULL;
	IPin *pOutputPin=NULL;
	AM_MEDIA_TYPE mediaType;
	ZeroMemory(&mediaType,sizeof(mediaType));

	pInputPin=mDump->GetInputPin();
	mVideoCaptureFilter->FindPin(L"Still",&pOutputPin);

	//mDump->GetInputPin()->GetMediaType(0,(CMediaType*)&mediaType);
	pOutputPin->ConnectionMediaType(&mediaType);

	hr=pFilterGraph->ConnectDirect(pOutputPin,pInputPin,&mediaType);
	if(FAILED(hr)){
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_WARNING,"Error connecting output to input, going to try to RenderStream");

		hr=mCaptureGraphBuilder->RenderStream(&PIN_CATEGORY_STILL,&MEDIATYPE_Video,mVideoCaptureFilter,NULL,pImageSinkFilter);
		if(FAILED(hr)){
			Error::unknown(Logger::TOADLET_FLICK,
				"Error rendering stream");
			return hr;
		}
	}
	else{
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,"Pin connection successful");
	}

	// MEDIASUBTYPE_IJPG seems to hang sometimes if we keep the filtergraph running
	if(IsEqualGUID(mDump->GetConnectedSubtype(),MEDIASUBTYPE_IJPG)){
		mAlwaysRunGraph=false;
	}

    // Prevent the data from flowing into the capture stream
    mCaptureGraphBuilder->ControlStream( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, mVideoCaptureFilter, 0, 0, 0, 0 );

    // Let's get the handle for DShow events. The main loop will listen to both notifications from 
    // the UI thread and for DShow notifications
    pFilterGraph->QueryInterface( IID_IMediaEvent, (void**) &mMediaEvent );

	Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,"createCaptureGraph:end");

    return hr;
}

HRESULT DSImageBlurDetector::runCaptureGraph(){
    HRESULT hr=S_OK;

    CComPtr<IGraphBuilder> pGraphBuilder;
    CComPtr<IMediaControl> pMediaControl;

	Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,"runCaptureGraph:begin");

    // Let's make sure that the graph has been initialized
	if(mCaptureGraphBuilder==NULL){
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_WARNING,"Graph not built");
		return E_FAIL;
	}

	// Retrieve the filtergraph off the capture graph builder
    hr=mCaptureGraphBuilder->GetFiltergraph( &pGraphBuilder );
	if(FAILED(hr)){
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_WARNING,"Error getting filtergraph");
		return hr;
	}

    // Get the media control interface, and run the graph
    pGraphBuilder.QueryInterface( &pMediaControl );
    hr=pMediaControl->Run();
	if(FAILED(hr)){
		if(hr==E_OUTOFMEMORY){
			Logger::log(Logger::TOADLET_FLICK,Logger::Level_WARNING,"Out of memory for Media Control");
		}
		else{
			Logger::log(Logger::TOADLET_FLICK,Logger::Level_WARNING,"Error running Media Control");
		}
		return hr;
	}
	else{
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,"Media Control running");
	}

	Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,"runCaptureGraph:end");

	return hr;
}

HRESULT DSImageBlurDetector::captureStillImage(){
	HRESULT hr=S_OK;

	CComPtr<IUnknown>		 pUnkCaptureFilter;
	CComPtr<IPin>			 pStillPin;
	CComPtr<IAMVideoControl> pVideoControl;

	if(mCaptureGraphBuilder==NULL){
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_WARNING,"Graph not built");
		return E_FAIL;
	}

	if(mAlwaysRunGraph==false){
		runCaptureGraph();
	}

	mVideoCaptureFilter.QueryInterface( &pUnkCaptureFilter );
	mCaptureGraphBuilder->FindPin( pUnkCaptureFilter, PINDIR_OUTPUT, &PIN_CATEGORY_STILL, &MEDIATYPE_Video, FALSE, 0, &pStillPin );
	mVideoCaptureFilter.QueryInterface( &pVideoControl );

	hr=pVideoControl->SetMode( pStillPin, VideoControlFlag_Trigger );
	if(FAILED(hr)){
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_WARNING,"Error setting mode");
		return hr;
	}

	long lEventCode;
	LONG_PTR lParam1, lParam2;
	while(true){
		HRESULT hr=mMediaEvent->GetEvent(&lEventCode, &lParam1, &lParam2, 50);
		if(!FAILED(hr)){
			mMediaEvent->FreeEventParams(lEventCode, lParam1, lParam2);
			if(lEventCode==EC_CAP_FILE_COMPLETED || lEventCode==EC_CAP_FILE_WRITE_ERROR){
				break;
			}
		}
		System::msleep(50);
	}

	mImageWidth=mDump->GetImageWidth();
	mImageHeight=mDump->GetImageHeight();
	mImageData=mDump->GetImageData();

	if(mAlwaysRunGraph==false){
		stopCaptureGraph();
	}

	return hr;
}

HRESULT DSImageBlurDetector::stopCaptureGraph(){
    HRESULT hr=S_OK;

    CComPtr<IGraphBuilder> pGraphBuilder;
    CComPtr<IMediaControl> pMediaControl;

	Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,"stopCaptureGraph:begin");

    // Let's make sure that the graph has been initialized
	if(mCaptureGraphBuilder==NULL){
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_WARNING,"Graph not built");
		return E_FAIL;
	}

	// Retrieve the filtergraph off the capture graph builder
    hr=mCaptureGraphBuilder->GetFiltergraph( &pGraphBuilder );
	if(FAILED(hr)){
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_WARNING,"Error getting filtergraph");
		return hr;
	}

    // Get the media control interface, and run the graph
    pGraphBuilder.QueryInterface( &pMediaControl );
    hr=pMediaControl->Stop();
	if(FAILED(hr)){
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_WARNING,"Error stopping");
		return hr;
	}

	mStatus=STATUS_STOPPED;

	Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,"stopCaptureGraph:end");

	return hr;
}

HRESULT DSImageBlurDetector::destroyCaptureGraph(){
	Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,"destroyCaptureGraph:begin");

	mMediaEvent=NULL;
	mCaptureGraphBuilder=NULL;

	Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,"destroyCaptureGraph:end");

	return S_OK;
}

}
}
