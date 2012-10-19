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

package com.lightningtoads.toadlet.flick;

import javax.microedition.lcdui.*;
import javax.microedition.media.*;
import javax.microedition.media.control.*;
//#ifdef TOADLET_DEBUG
	import com.lightningtoads.toadlet.egg.*;
//#endif

// Standard setup
//	motionDetector=new ImageBlurDetector();
//	check motionDetector.available();
//	if(Nokia S40 3E){
//		motionDetector.setCaptureURL("capture://image");
//	}
//	if(Sony Ericsson JP3){
//		motionDetector.setKeepCamera(false);
//	}
//	if(microedition.platform=="SonyEricssonW850i"){
//		motionDetector.setSleepTimes(100,0);
//	}
//	else if(Samsung){
//		motionDetector.setSleepTimes(0,-400);
//	}
//	else{
//		motionDetector.setSleepTimes(200,325);
//	}
//	motionDetector.create();
//	detect motion
//	motionDetector.destroy();
//	motionDetector=null;

public final class JSR135ImageBlurDetector implements ImageBlurDetector, Runnable{
	// Status
	final static int STATUS_STOPPED=0;
	final static int STATUS_BUSY=1;

	public JSR135ImageBlurDetector(){}

	public boolean available(){
		//#ifdef TOADLET_DEBUG
			{
				String encodings=System.getProperty("video.snapshot.encodings");
				if(encodings!=null){
					Logger.getInstance().addLogString(Logger.LEVEL_ALERT,encodings);
				}
			}
		//#endif

		String platform=System.getProperty("microedition.platform");
		if(platform!=null){
			if(platform.indexOf("SonyEricssonK600i")!=-1){
				return false;
			}
		}

		String encodings=System.getProperty("video.snapshot.encodings");
		if(encodings==null || encodings.length()<=0){
			return false;
		}

		return true;
	}
	
	public boolean create(){
		if(mState==STATE_STOPPED){
			return true;
		}

		if(mState!=STATE_DESTROYED){
			mError=ERROR_SEQUENCE;
			return false;
		}

		mState=STATE_STOPPED;

		return true;
	}

	public boolean startup(){
		if(mState==STATE_RUNNING){
			return true;
		}

		if(mState!=STATE_STOPPED){
			mError=ERROR_SEQUENCE;
			return false;
		}

		mState=STATE_RUNNING;

		if(mKeepCamera){
			try{
				createCamera();
			}
			catch(SecurityException ex){
				mError=ERROR_SECURITY;
				//#ifdef TOADLET_DEBUG
					Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"ImageBlurDetector::startup:"+ex.toString());
				//#endif
				return false;
			}
			catch(Exception ex){
				mError=ERROR_SETUP;
				//#ifdef TOADLET_DEBUG
					Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"ImageBlurDetector::startup:"+ex.toString());
				//#endif
				return false;
			}
		}
		return true;
	}

	public boolean shutdown(){
		if(mState==STATE_STOPPED){
			return true;
		}

		if(mState!=STATE_RUNNING){
			mError=ERROR_SEQUENCE;
			return false;
		}

		mState=STATE_STOPPED;

		while(mStatus!=STATUS_STOPPED){
			try{
				Thread.sleep(100);
			}
			catch(InterruptedException ex){}
		}

		try{
			destroyCamera();
		}
		catch(Exception ex){}

		return true;
	}
	
	public boolean destroy(){
		if(mState==STATE_DESTROYED){
			return true;
		}

		if(mState!=STATE_STOPPED){
			mError=ERROR_SEQUENCE;
			return false;
		}

		mState=STATE_DESTROYED;

		mListener=null;

		if(mImageDataCache!=null){
			mImageDataCache=null;
		}

		if(mResultsCache!=null){
			mResultsCache=null;
		}

		return true;
	}

	public void setCaptureURL(String url){
		mCaptureURL=url;
	}

	public void setKeepCamera(boolean keep){
		mKeepCamera=keep;
	}

	public void setSleepTimes(int prestart,int precapture){
		mPrestartSleepTime=prestart;
		mPrecaptureSleepTime=precapture;
	}

	public void setListener(ImageBlurDetectorListener listener){
		mListener=listener;
	}

	public boolean startDetection(){
		if(mStatus!=STATUS_STOPPED){
			return false; // Detection already running
		}

		mStatus=STATUS_BUSY;
		mCheckLighting=false;

		mDetectionThread=new Thread(this);
		mDetectionThread.start();

		return true;
	}

	public int getDetectedAngle(){
		return mAngle;
	}

	public int getDetectedAngleClusterSize(){
		return mAngleClusterSize;
	}

	public int getDetectedAngleClusterWidth(){
		return mAngleClusterWidth;
	}

	public int getState(){
		return mState;
	}

	public int getError(){
		int error=mError;
		mError=ERROR_NONE;
		return error;
	}

	public boolean startLightingCheck(){
		if(mStatus!=STATUS_STOPPED){
			return false;
		}

		mStatus=STATUS_BUSY;
		mCheckLighting=true;

		mDetectionThread=new Thread(this);
		mDetectionThread.start();

		return true;
	}

	public int getLightingResult(){
		return mLightingResult;
	}

	protected void createCamera() throws Exception{
		if(mCamera!=null){
			try{
				destroyCamera();
			}
			catch(Exception ex){}
		}

		try{
			mCamera=Manager.createPlayer(mCaptureURL);
		}
		catch(Exception ex){
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"ImageBlurDetector::createCamera::createPlayer:"+ex.toString());
			//#endif
			throw ex;
		}
		try{
			mCamera.realize();
		}
		catch(Exception ex){
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"ImageBlurDetector::createCamera::realize:"+ex.toString());
			//#endif
			throw ex;
		}

		try{
			mControl=(VideoControl)mCamera.getControl("VideoControl");
		}
		catch(Exception ex){
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"ImageBlurDetector::createCamera::getControl:"+ex.toString());
			//#endif
			throw ex;
		}
		mControl.initDisplayMode(VideoControl.USE_GUI_PRIMITIVE,null);
		mControl.setVisible(false);

		startCamera();
	}

	protected void destroyCamera() throws MediaException{
		stopCamera();

		if(mCamera!=null){
			mCamera.close();
			mCamera=null;
		}
	
		if(mControl!=null){
			mControl=null;
		}
	}

	protected void startCamera() throws MediaException{
		if(mCamera!=null){
			mCamera.start();
		}
	}
	
	protected void stopCamera() throws MediaException{
		if(mCamera!=null){
			mCamera.stop();
		}
	}
	
	public void run(){
		mError=ERROR_NONE;

		if(mKeepCamera==false){
			try{
				createCamera();
			}
			catch(SecurityException ex){
				mError=ERROR_SECURITY;
				//#ifdef TOADLET_DEBUG
					Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"ImageBlurDetector:run:"+ex.toString());
				//#endif
			}
			catch(Exception ex){
				mError=ERROR_SETUP;
				//#ifdef TOADLET_DEBUG
					Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"ImageBlurDetector::run:"+ex.toString());
				//#endif
			}
		}

		if(mError==ERROR_NONE){
			if(mCheckLighting==false){
				try{
					Thread.sleep(mPrestartSleepTime);
				}
				catch(InterruptedException ex){}

				if(mPrecaptureSleepTime<0){
					DelayNotify nofity=new DelayNotify(mListener,-mPrecaptureSleepTime);
				}
				else{
					if(mListener!=null){
						mListener.imageCaptureStarting();
					}

					try{
						Thread.sleep(mPrecaptureSleepTime);
					}
					catch(InterruptedException ex){}
				}

				captureStillImage();

				if(mListener!=null){
					mListener.imageCaptureFinished();
				}

				if(mError==ERROR_NONE){
					if(mKeepCamera==false){
						try{
							destroyCamera();
						}
						catch(Exception ex){}
					}

					Image image=null;
					try{
						image=Image.createImage(mRawImage,0,mRawImage.length);
					}
					catch(Exception ex){
						mError=ERROR_IMAGE_CREATION;
					}

					if(mError==ERROR_NONE){
						try{
							mRawImage=null;
							findAngleInImage(image);
						}
						catch(Exception ex){
							mError=ERROR_IMAGE_ANALYSIS;
							//#ifdef TOADLET_DEBUG
								Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"ImageBlurDetector::findAngleInImage:"+ex.toString());
							//#endif
						}
					}
				}
			}
			else{
				captureStillImage();

				if(mError==ERROR_NONE){
					Image image=null;
					try{
						image=Image.createImage(mRawImage,0,mRawImage.length);
					}
					catch(Exception ex){
						mError=ERROR_IMAGE_CREATION;
					}

					if(mError==ERROR_NONE){
						try{
							mRawImage=null;

							int width=image.getWidth();
							int height=image.getHeight();
							int[] data=new int[width*height];
							image.getRGB(data,0,width,0,0,width,height);

							int brightness=0;
							int advance=1;
							int i;
							for(i=0;i<data.length;i+=advance){
								brightness+=((0x00FF0000&data[i])>>16) + ((0x0000FF00&data[i])>>8) + (0x000000FF&data[i]);
							}
							brightness*=advance;
							brightness/=(data.length*3);

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
						catch(Exception ex){
							mError=ERROR_IMAGE_ANALYSIS;
							//#ifdef TOADLET_DEBUG
								Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"ImageBlurDetector::calculateLighting:"+ex.toString());
							//#endif
						}
					}
				}
			}
		}

		mDetectionThread=null;
		if(mListener!=null){
			mListener.detectionFinished();
		}

		mStatus=STATUS_STOPPED;		
	}

	protected void captureStillImage(){
		try{
			mRawImage=mControl.getSnapshot(null);
		}
		catch(SecurityException ex){
			mError=ERROR_SECURITY;
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"ImageBlurDetector::captureStillImage:"+ex.toString());
			//#endif
		}
		catch(Exception ex){
			mError=ERROR_IMAGE_ACQUISITION;
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"ImageBlurDetector::captureStillImage:"+ex.toString());
			//#endif
		}
	}

	protected final static float PI=3.1415926f;
	protected final static float QUARTER_PI=PI/4.0f;
	protected final static float THREE_QUARTER_PI=3.0f*PI/4.0f;

	protected void findAngleInImage(Image image){
		int width=image.getWidth();
		int height=image.getHeight();

		if(mImageDataCache==null){
			mImageDataCache=new int[width*height];
		}
		int[] data=mImageDataCache;
		image.getRGB(data,0,width,0,0,width,height);
		
		int[] bestAngle=new int[mTotalClusters*2];
		int[] bestPeakCount=new int[mTotalClusters*2];
		if(mResultsCache==null){
			mResultsCache=new int[61];
		}
		int[] results=mResultsCache;

		//#ifdef TOADLET_DEBUG
			int radonDataWidth=180;
			int radonDataHeight=results.length;
			int[] radonData=new int[radonDataWidth*radonDataHeight];
		//#endif

		int i,j,k,l,m;
		
		for(i=0;i<180;i+=2){
			radon(data,width,height,i*PI/180.0f,results,2);

			int max=0;
			int min=255;
			int peakCount=0;
			int direction=0;
			for(j=1;j<results.length;++j){
				int newDirection=results[j]-results[j-1];
				if(Math.abs(direction-newDirection)<2){
					//nothin
				}
				else{
					peakCount++;
					direction=newDirection;
				}
				if(results[j]<min){
					min=results[j];
				}
				if(results[j]>max){
					max=results[j];
				}
			}

			for(j=0;j<bestAngle.length;++j){
				if(peakCount>bestPeakCount[j]){
					bestAngle[j]=i;
					bestPeakCount[j]=peakCount;
					break;
				}
			}

			//#ifdef TOADLET_DEBUG	
				for(j=0;j<results.length;++j){
					radonData[j*radonDataWidth+i]=results[j] | (results[j]<<8) | (results[j]<<16);
				}
			//#endif	
		}

		//#ifdef TOADLET_DEBUG
			try{
		//#endif
		
		int[][] clusterPoints=new int[mTotalClusters][];
		for(i=0;i<mTotalClusters;++i){
			clusterPoints[i]=new int[mTotalClusters*2];
		}
		int[] clusterSize=new int[mTotalClusters];
		int[] clusterWidth=new int[mTotalClusters];
		float[] clusterCentroid=new float[mTotalClusters];
		float[] clusterInvDensity=new float[mTotalClusters];
		
		float distance=0;
		float minDistance=0;
		int minDistanceIndex=0;
		int clusterIndex=0;
		int minClusterIndex=0;
		boolean b;
		
		i=mTotalClusters*2;
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
		
		for(i=0;i<mTotalClusters;++i){
			minDistance=180;
			minDistanceIndex=-1;
			clusterIndex=0;
			minClusterIndex=-1;

			// Find the closest pair of points and/or clusters
			for(j=0;j<(mTotalClusters*2)-1;++j){
				if(bestAngle[j]==-1){
					if(clusterSize[clusterIndex]==0 || j+clusterSize[clusterIndex]>=mTotalClusters*2){
						continue;
					}
					j=j+clusterSize[clusterIndex]-1;
					// Compare a selected cluster with another cluster
					if(bestAngle[j+1]==-1){
						if(clusterIndex+1>=mTotalClusters){
							continue;
						}
						distance=Math.abs(clusterCentroid[clusterIndex+1]-clusterCentroid[clusterIndex]);
						if(distance<minDistance){
							minDistance=distance;
							minDistanceIndex=j;
							minClusterIndex=clusterIndex;
						}
					}
					// Compare a selected cluster with a point
					else{
						distance=Math.abs(bestAngle[j+1]-clusterCentroid[clusterIndex]);
						if(distance<minDistance){
							minDistance=distance;
							minDistanceIndex=j;
							minClusterIndex=clusterIndex;
						}
					}
				}
				// Compare the selected point with a cluster
				else if(bestAngle[j+1]==-1){
					distance=Math.abs(clusterCentroid[clusterIndex]-bestAngle[j]);
					if(distance<minDistance){
						minDistance=distance;
						minDistanceIndex=j;
						minClusterIndex=clusterIndex;
					}
				}
				// Compare the selected point with another point
				else{
					distance=Math.abs(bestAngle[j+1]-bestAngle[j]);
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

			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Loop="+i);
				for(j=0;j<mTotalClusters;++j){
					if(clusterSize[j]>0){
						Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Cluster #="+j);
						for(k=0;k<clusterSize[j];++k){
							Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"   point["+k+"]="+clusterPoints[j][k]);
						}
					}
				}
			//#endif
		}

		// Analyize the clusters
		float peakResult=INVALID_ANGLE_DETECTED;
		int bestCluster=-1;
		int[] pts=new int[mTotalClusters*2];
		b=false;
		for(i=0;i<mTotalClusters;++i){
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
					for(j=0; j<k; ++j){
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
			peakResult=INVALID_ANGLE_DETECTED;
		}

		//#ifdef TOADLET_DEBUG
			mDebugRadonImage=Image.createRGBImage(radonData,radonDataWidth,radonDataHeight,false);
			
			drawLineAtAngle(data,width,height,peakResult*PI/180.0f);
			mDebugImage=Image.createRGBImage(data,width,height,false);
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Cluster info: ");
			for(i=0;i<mTotalClusters;++i){
				if(clusterSize[i]>2){
					Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Cluster["+i+"] size="+clusterSize[i]+" width="+clusterWidth[i]+" center="+clusterCentroid[i]+" inv density="+clusterInvDensity[i]);
					for(j=0;j<clusterSize[i];++j){
						Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"   Point="+clusterPoints[i][j]);
					}
				}
			}
			if(peakResult==INVALID_ANGLE_DETECTED){
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Invalid angle detected!");
			}
			else{
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Best Cluster = Cluster["+bestCluster+"] size="+clusterSize[bestCluster]+" width="+clusterWidth[bestCluster]+" center="+clusterCentroid[bestCluster]+" inv density="+clusterInvDensity[bestCluster]);
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Using Angle = "+(int)peakResult);
			}
		//#endif

		mAngle=(int)peakResult;
		if(peakResult==INVALID_ANGLE_DETECTED){
			mAngleClusterSize=INVALID_ANGLE_DETECTED;
			mAngleClusterWidth=INVALID_ANGLE_DETECTED;
		}
		else{
			mAngleClusterSize=clusterSize[bestCluster];
			mAngleClusterWidth=clusterWidth[bestCluster];
		}

		//#ifdef TOADLET_DEBUG
			}
			catch(Exception ex){
				Logger.getInstance().addLogString(Logger.LEVEL_WARNING,ex.toString());
			}
		//#endif
	}
	
	// Bresenham's line drawing algorithm
	// Modified to avoid the inner multiplies
	// No boundary checking, since we rely on the calling function to clip the segment
	protected int sumLine(int x0,int y0,int x1,int y1,int[] data,int width,int height){
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

	protected void radon(int[] data,int width,int height,float theta,int[] results,int spacing){
		int halfAddedLines=(results.length-1)/2;

		int size=((width<height)?width:height);
		int hsize=size/2;
		int sizem1=size-1;

		int minX=-hsize;
		int maxX=hsize-1;
		int minY=-hsize;
		int maxY=hsize-1;

		if(theta>=QUARTER_PI && theta<=THREE_QUARTER_PI){
			float sintheta=(float)Math.sin(theta);
			if(sintheta<0){
				sintheta=-sintheta;
			}

			int mainEndY=maxY;
			int mainEndX=(int)(mainEndY/Math.tan(theta));
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
			float costheta=(float)Math.cos(theta);
			if(costheta<0){
				// This will cause the OUT_IMAGE of the radon transform to be non-continuous
				// but it saves us from having to do extra bounds checking, so we're ok with that
				costheta=-costheta;
			}

			int mainEndX=maxX;
			int mainEndY=(int)(mainEndX*Math.tan(theta));
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

	//#ifdef TOADLET_DEBUG
		// Bresenham's line drawing algorithm
		// Modified to avoid the inner multiplies
		// No boundary checking, since we rely on the calling function to clip the segment
		protected void drawLine(int x0,int y0,int x1,int y1,int[] data,int width,int height){
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

		protected void drawLineAtAngle(int[] data,int width,int height,float theta){
			int size=((width<height)?width:height);
			int hsize=size/2;

			int minX=-hsize;
			int maxX=hsize-1;
			int minY=-hsize;
			int maxY=hsize-1;

			if(theta>=QUARTER_PI && theta<=THREE_QUARTER_PI){
				float sintheta=(float)Math.sin(theta);
				if(sintheta<0){
					sintheta=-sintheta;
				}

				int mainEndY=maxY;
				int mainEndX=(int)(mainEndY/Math.tan(theta));
				int mainStartY=minY;
				int mainStartX=-mainEndX;

				drawLine(mainStartX - minX,mainStartY - minY,mainEndX - minX,mainEndY - minY,data,width,height);
			}
			else{
				float costheta=(float)Math.cos(theta);
				if(costheta<0){
					// This will cause the OUT_IMAGE of the random transform to be non-continuous
					// but it saves us from having to do extra bounds checking, so we're ok with that
					costheta=-costheta;
				}

				int mainEndX=maxX;
				int mainEndY=(int)(mainEndX*Math.tan(theta));
				int mainStartX=minX;
				int mainStartY=-mainEndY;

				drawLine(mainStartX - minX,mainStartY - minY,mainEndX - minX,mainEndY - minY,data,width,height);
			}
		}

		public Image getDebugImage(){
			return mDebugImage;
		}

		public Image getDebugRadonImage(){
			return mDebugRadonImage;
		}
	//#endif

	protected int mError=ERROR_NONE;

	protected int mState=STATE_DESTROYED;

	protected javax.microedition.media.Player mCamera;
	protected VideoControl mControl;

	protected int mStatus=STATUS_STOPPED;
	protected boolean mCheckLighting=false;
	protected int mLightingResult=LR_GOOD;
	protected Thread mDetectionThread=null;
	protected Object mMutex=new Object();

	protected byte[] mRawImage;

	protected int[] mImageDataCache;
	protected int[] mResultsCache;
	protected ImageBlurDetectorListener mListener;
	protected int mAngle=0;
	protected int mAngleClusterSize=0;
	protected int mAngleClusterWidth=0;

	protected int mTotalClusters=5;

	protected boolean mKeepCamera=true;
	protected String mCaptureURL="capture://video";
	protected int mPrestartSleepTime=200;
	protected int mPrecaptureSleepTime=325;

	//#ifdef TOADLET_DEBUG
		protected Image mDebugImage;
		protected Image mDebugRadonImage;
	//#endif

	final class DelayNotify extends Thread{
		public DelayNotify(ImageBlurDetectorListener listener,int delay){
			mListener=listener;
			mDelay=delay;
			start();
		}

		public void run(){
			try{
				Thread.sleep(mDelay);
			}
			catch(InterruptedException ex){}

			if(mListener!=null){
				mListener.imageCaptureStarting();
			}
		}

		protected int mDelay;
		protected ImageBlurDetectorListener mListener;
	}
}
