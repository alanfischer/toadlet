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

import javax.microedition.sensor.*;
import javax.microedition.io.Connector;
import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;
//#ifdef TOADLET_DEBUG
	import com.lightningtoads.toadlet.egg.*;
//#endif

public final class JSR256MotionDetectorImpl implements MotionDetector, DataListener{
	// Status
	final static int STATUS_STOPPED=0;
	final static int STATUS_BUSY=1;

	public JSR256MotionDetectorImpl(){}

	public boolean available(){
		int i,j;
		String sv = System.getProperty("microedition.sensor.version");
		if(sv==null) {
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"No sensors available");
			//#endif
			return false;
		}
		else{
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Sensor version available:"+sv);
			//#endif
		}

		SensorInfo[] si = SensorManager.findSensors("acceleration",null);
		if(si==null || si.length<1){
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"No acceleration sensor available");
			//#endif
			return false;
		}
		for(i=0;i<si.length;i++){
			SensorInfo s=si[i];
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Accelerometer properties");
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"getDescription: "+s.getDescription());
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"isAvailable: "+s.isAvailable());
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"isAvailabilityPushSupported: "+s.isAvailabilityPushSupported());
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"isConditionPushSupported: "+s.isConditionPushSupported());
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"getConnectionType: "+s.getConnectionType());
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"getContextType: "+s.getContextType());
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"getMaxBufferSize: "+s.getMaxBufferSize());
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"getModel: "+s.getModel());
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"getQuantity: "+s.getQuantity());
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"getUrl: "+s.getUrl());
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Properties:");
				String[] pn=s.getPropertyNames();
				for(j=0;j<pn.length;j++){
					Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"  "+pn[j]+": "+s.getProperty(pn[j]));
				}
			//#endif

			ChannelInfo[] ci=s.getChannelInfos();
			if(ci==null || ci.length!=3){
				//#ifdef TOADLET_DEBUG
					Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Sensor doesn't support 3 channels, instead shows: "+ci.length);
				//#endif
				return false;
			}
			for(i=0;i<ci.length;++i){
				//#ifdef TOADLET_DEBUG
					Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Channel Info["+i+"]: name="+ci[i].getName()+" type="+ci[i].getDataType());
				//#endif
			}
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
		
		int bufferSize=-1;

		try{
			String sv = System.getProperty("microedition.sensor.version");
			if(sv==null) {
				//#ifdef TOADLET_DEBUG
					Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"JSR256MotionDetector.startup: No sensors present.");
				//#endif
				return false;
			}

			SensorInfo[] si = SensorManager.findSensors("acceleration",null);
			if(si==null || si.length<1){
				//#ifdef TOADLET_DEBUG
					Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"JSR256MotionDetector.startup: No acceleration sensor present.");
				//#endif
				return false;
			}

			//TODO: For now, we just use the first available acceleration sensor. Is there a way to decide among several if present?
			ChannelInfo[] ci=si[0].getChannelInfos();

			// We assume our sensor has at least 3 channels
			if(ci==null || ci.length<3){
				//#ifdef TOADLET_DEBUG
					Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"JSR256MotionDetector.startup: Acceleration sensor does not have at least 3 channels.");
				//#endif
				return false;
			}
			int i;
			for(i=0;i<3;++i){
				if(ci[i].getName().equals("axis_x")){
					mDataMapper[0]=i;
				}
				else if(ci[i].getName().equals("axis_y")){
					mDataMapper[1]=i;
				}
				else if(ci[i].getName().equals("axis_z")){
					mDataMapper[2]=i;
				}
			}
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"JSR256MotionDetector.startup: Acceleration sensor mapping: [0]="+mDataMapper[0]+" [1]="+mDataMapper[1]+" [2]="+mDataMapper[2]);
			//#endif
			
			// Get the channel data type
			// Assume all 3 accelerometer sensors give the same type
			// 1 = TYPE_DOUBLE
			// 2 = TYPE_INT
			// 4 = TYPE_OBJECT
			mDataType=ci[0].getDataType();

			mSensor=(SensorConnection)Connector.open(si[0].getUrl());

			// Compute the buffersize based on polling interval and sensor max rate
			// rate(Hz) * pollSleep(ms) / 1000(ms/s) = buffersize
			String[] pnames=si[0].getPropertyNames();
			bufferSize=1;
			if(pnames.length>0){
				Object rate=si[0].getProperty(SensorInfo.PROP_MAX_RATE);
				if(rate!=null){
					bufferSize=Math.fixedToInt(Math.mul(Math.floatToFixed(((Float)rate).floatValue()),Math.milliToFixed(mPollSleep)));
					if(bufferSize<1){
						bufferSize=1;
					}
				}
			}

			mSensor.setDataListener(this,bufferSize,0,true,false,false);
		}
		catch(SecurityException ex){
			mError=ERROR_SECURITY;
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"JSR256MotionDetector.startup:"+ex.toString());
			//#endif
			return false;
		}
		catch(Exception ex){
			mError=ERROR_SETUP;
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"JSR256MotionDetector.startup:"+ex.toString());
			//#endif
			return false;
		}
		
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"JSR256MotionDetector.startup: Successful with bufferSize="+bufferSize);
		//#endif

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

		try {
			mSensor.removeDataListener();
		}
		catch(Exception ex){
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"JSR256MotionDetector.shutdown:  sensor.removeDataListener():"+ex.toString());
			//#endif
			return false;
		}
		try{
			mSensor.close();
		}
		catch(Exception ex){
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"JSR256MotionDetector.shutdown: sensor.close():"+ex.toString());
			//#endif
			return false;
		}

		mSensor=null;

		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"JSR256MotionDetector.shutdown: Success!");
		//#endif

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

		return true;
	}

	public void setPollSleep(int ms){
		mPollSleep=ms;
	}

	public void setFilterAlpha(int alpha){
		mAlpha=alpha;
	}

	public void setNativeOrientation(boolean orientation){
		mNative=orientation;
	}

	public void setListener(MotionDetectorListener listener){
		mMotionData.time=0;
		mListener=listener;
	}

	public int getState(){
		return mState;
	}

	public int getError(){
		int error=mError;
		mError=ERROR_NONE;
		return error;
	}

	public void dataReceived(SensorConnection sensor,Data[] data,boolean dataLost){
		mCacheLastAcceleration.set(mMotionData.acceleration);
		mCacheLastVelocity.set(mMotionData.velocity);
		mCacheLastVelocityFiltered.set(mMotionData.velocityFiltered);

		// Get data from the acceleration sensor
		// HACK?: We here assume the data is always returned in the same order, eg: x then y then z, or whatever order we discovered in startup. 
		// HACK?: Also assume the same number of data objects are buffered for each direction
		long time=System.currentTimeMillis();
		int i;
		switch(mDataType){
			case ChannelInfo.TYPE_DOUBLE:
				for(i=0;i<data[0].getDoubleValues().length;++i){
					mMotionData.acceleration.x=convertDataDouble(data[mDataMapper[0]].getDoubleValues()[i],data[mDataMapper[0]].getChannelInfo().getScale());
					mMotionData.acceleration.y=convertDataDouble(data[mDataMapper[1]].getDoubleValues()[i],data[mDataMapper[1]].getChannelInfo().getScale());
					mMotionData.acceleration.z=convertDataDouble(data[mDataMapper[2]].getDoubleValues()[i],data[mDataMapper[2]].getChannelInfo().getScale());
				}
			break;
			case ChannelInfo.TYPE_INT:
				for(i=0;i<data[0].getIntValues().length;++i){
					mMotionData.acceleration.x=convertDataInt(data[mDataMapper[0]].getIntValues()[i],data[mDataMapper[0]].getChannelInfo().getScale());
					mMotionData.acceleration.y=convertDataInt(data[mDataMapper[1]].getIntValues()[i],data[mDataMapper[1]].getChannelInfo().getScale());
					mMotionData.acceleration.z=convertDataInt(data[mDataMapper[2]].getIntValues()[i],data[mDataMapper[2]].getChannelInfo().getScale());
				}
			break;
		}
	
		// No clue about orientation here yet
		if(mNative){
			//mMotionData.acceleration.set(x,y,z);
		}
		else{
			//mMotionData.acceleration.set(x,-y,z);
		}

		if(mMotionData.time==0){
			mMotionData.velocity.reset();
			mMotionData.velocityFiltered.reset();
		}
		else{
			// Improved euler integration
			// v = v + 0.5dt * (ao + a)
			Math.add(mCacheVector,mMotionData.acceleration,mCacheLastAcceleration);
			Math.mul(mCacheVector,Math.milliToFixed((int)((time-mMotionData.time)/2)));
			Math.add(mMotionData.velocity,mCacheVector);

			// Highpass filtering
			// vf = alpha * (vf0 + v - v0)
			Math.sub(mCacheVector,mMotionData.velocity,mCacheLastVelocity);
			Math.add(mCacheVector,mCacheLastVelocityFiltered);
			Math.mul(mMotionData.velocityFiltered,mCacheVector,mAlpha);

			if(mListener!=null){
				mListener.motionDetected(mMotionData);
			}
		}

		mMotionData.time=time;
	}

	// Convert accelerometer data to fixed point, and scale in as efficient a manner as possible, using stored scale values if unchanged
	// This is messy but necessary, as jsr256 sensor data always includes a scale and it makes no guarantee as to it's value.
	private final int convertDataDouble(double value,int scale) {
		int v=Math.floatToFixed((float)value);
		if(scale==0){
			return v;
		}
		else if(scale==mScale){
			v=(mScale>0)?Math.mul(v,mScaleValue):Math.div(v,mScaleValue);
			return v;
		}
		else{
			mScale=scale;
			mScaleValue=Math.ONE;
			for(int i=0;i<Math.abs(mScale);i++){
				mScaleValue*=10;
			}
			v=(mScale>0)?Math.mul(v,mScaleValue):Math.div(v,mScaleValue);
			return v;
		}
	}
	
	// Convert accelerometer data to fixed point, and scale in as efficient a manner as possible, using stored scale values if unchanged
	// This is messy but necessary, as jsr256 sensor data always includes a scale and it makes no guarantee as to it's value.
	private final int convertDataInt(int value,int scale) {
		int v=Math.intToFixed(value);
		if(scale==0){
			return v;
		}
		else if(scale==mScale){
			v=(mScale>0)?Math.mul(v,mScaleValue):Math.div(v,mScaleValue);
			return v;
		}
		else{
			mScale=scale;
			mScaleValue=Math.ONE;
			for(int i=0;i<Math.abs(mScale);i++){
				mScaleValue*=10;
			}
			v=(mScale>0)?Math.mul(v,mScaleValue):Math.div(v,mScaleValue);
			return v;
		}
	}

	protected int mError=ERROR_NONE;

	protected int mState=STATE_DESTROYED;

	protected int mPollSleep=50;
	protected int mAlpha=Math.milliToFixed(667);
	protected boolean mNative=false;
	protected MotionDetectorListener mListener;

	protected MotionData mMotionData=new MotionData();

	protected SensorConnection mSensor;
	protected String mSensorURL;
	protected String[] mSensorChannels=new String[3];

	protected Vector3 mCacheVector=new Vector3();
	protected Vector3 mCacheLastAcceleration=new Vector3();
	protected Vector3 mCacheLastVelocity=new Vector3();
	protected Vector3 mCacheLastVelocityFiltered=new Vector3();
	
	protected int mScale=0;
	protected int mScaleValue=Math.ONE;
	protected int mDataType=ChannelInfo.TYPE_DOUBLE;
	protected int[] mDataMapper=new int[3];
}
