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

package us.toadlet.flick;

import android.content.Context;
import android.hardware.*;
import java.util.List;

public class AndroidMotionDevice implements MotionDevice,SensorEventListener{
	public AndroidMotionDevice(Context context){
		mContext=context;
		mSensorManager=(SensorManager)mContext.getSystemService(Context.SENSOR_SERVICE);
	}

	public boolean create(){
        List<Sensor> sensors=mSensorManager.getSensorList(Sensor.TYPE_ACCELEROMETER);
		if(sensors.size()>0){
			mSensor=sensors.get(0);
		}
		return mSensor!=null;
	}
	
	public void destroy(){
		stop();
		mSensor=null;
	}

	public boolean start(){
		mRunning=false;
		if(mSensor!=null){
			mRunning=mSensorManager.registerListener(this,mSensor,SensorManager.SENSOR_DELAY_GAME);
		}
		return mRunning;
	}
	
	public void update(int dt){}
	
	public void stop(){
		if(mSensor!=null){
			mSensorManager.unregisterListener(this,mSensor);
		}
		mRunning=false;
	}
	
	public boolean isRunning(){return mRunning;}

	public void setSampleTime(int dt){}
	
	public void setListener(MotionDeviceListener listener){mListener=listener;}

	public void onAccuracyChanged(Sensor sensor,int accuracy){}
 
	public void onSensorChanged(SensorEvent event){
		mMotionData.time=event.timestamp/1000;
		mMotionData.acceleration[0]=event.values[0];
		mMotionData.acceleration[1]=event.values[1];
		mMotionData.acceleration[2]=event.values[2];

		if(mListener!=null){
			mListener.motionDetected(mMotionData);
		}
	}

	Context mContext;
	SensorManager mSensorManager;
	Sensor mSensor;
	boolean mRunning;
	MotionDeviceListener mListener;
	MotionData mMotionData=new MotionData();
}
