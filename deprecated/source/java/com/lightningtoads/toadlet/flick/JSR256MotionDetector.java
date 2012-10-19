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

//#ifdef TOADLET_DEBUG
	import com.lightningtoads.toadlet.egg.*;
//#endif

public final class JSR256MotionDetector implements MotionDetector{
	public JSR256MotionDetector(){
		try{
			if(Class.forName("javax.microedition.sensor.SensorManager")!=null){
				Class c=Class.forName("com.lightningtoads.toadlet.flick.JSR256MotionDetectorImpl");
				mImplementation=(MotionDetector)c.newInstance();
			}
		}
		catch(Exception ex){
			mImplementation=null;
		}
	}

	public boolean available(){
		return mImplementation!=null && mImplementation.available();
	}
	
	public boolean create(){
		return mImplementation!=null && mImplementation.create();
	}

	public boolean startup(){
		return mImplementation!=null && mImplementation.startup();
	}

	public boolean shutdown(){
		return mImplementation!=null && mImplementation.shutdown();
	}
	
	public boolean destroy(){
		return mImplementation!=null && mImplementation.destroy();
	}

	public void setPollSleep(int ms){
		if(mImplementation!=null){
			mImplementation.setPollSleep(ms);
		}
	}

	public void setFilterAlpha(int alpha){
		if(mImplementation!=null){
			mImplementation.setFilterAlpha(alpha);
		}
	}

	public void setNativeOrientation(boolean orientation){
		if(mImplementation!=null){
			mImplementation.setNativeOrientation(orientation);
		}
	}

	public void setListener(MotionDetectorListener listener){
		if(mImplementation!=null){
			mImplementation.setListener(listener);
		}
	}

	public int getState(){
		if(mImplementation!=null){
			return mImplementation.getState();
		}
		else{
			return STATE_DESTROYED;
		}
	}

	public int getError(){
		if(mImplementation!=null){
			return mImplementation.getError();
		}
		else{
			return ERROR_NONE;
		}
	}

	MotionDetector mImplementation;
}
