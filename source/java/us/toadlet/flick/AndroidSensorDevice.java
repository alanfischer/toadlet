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

public class AndroidSensorDevice extends BaseInputDevice implements SensorEventListener{
	public AndroidSensorDevice(Context context,int type){
		mContext=context;
		mSensorManager=(SensorManager)mContext.getSystemService(Context.SENSOR_SERVICE);
		mSensorType=type;
		mInputType=getInputTypeFromSensorType(mSensorType);
		mData=new InputData(mInputType,0,2);
	}

	public boolean create(){
		mSensor=mSensorManager.getDefaultSensor(mSensorType);
		return mSensor!=null;
	}
	
	public void destroy(){
		stop();
		mSensor=null;
	}

	public int getType(){return mInputType;}

	public boolean start(){
		mRunning=false;
		if(mSensor!=null){
			mRunning=mSensorManager.registerListener(this,mSensor,mSampleTime);
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
	
	public void setListener(InputDeviceListener listener){mListener=listener;}
	public void setSampleTime(int dt){mSampleTime=dt*1000;}
	public void setAlpha(float alpha){}

	public void onAccuracyChanged(Sensor sensor,int accuracy){}
 
	public void onSensorChanged(SensorEvent event){
		mData.setTime(event.timestamp/1000);

		/// @todo: Change InputData so setValid does not need to be called if setValue is called
		switch(mInputType){
			case InputType_MOTION:
				mData.setValid(1<<InputData.Semantic_MOTION_ACCELERATION);
				mData.setValue(InputData.Semantic_MOTION_ACCELERATION,event.values);
			break;
			case InputType_ANGULAR:
				mData.setValid(1<<InputData.Semantic_ANGULAR);
				mData.setValue(InputData.Semantic_ANGULAR,event.values);
			break;
			case InputType_LIGHT:
				mData.setValid(1<<InputData.Semantic_LIGHT);
				mData.setValue(InputData.Semantic_LIGHT,event.values);
			break;
			case InputType_PROXIMITY:
				mData.setValid(1<<InputData.Semantic_PROXIMITY);
				mData.setValue(InputData.Semantic_PROXIMITY,event.values);
			break;
			case InputType_MAGNETIC:
				mData.setValid(1<<InputData.Semantic_MAGNETIC);
				mData.setValue(InputData.Semantic_MAGNETIC,event.values);
			break;
		}

		if(mListener!=null){
			mListener.inputDetected(mData);
		}
	}
	
	public static int getInputTypeFromSensorType(int sensorType){
		switch(sensorType){
			case Sensor.TYPE_ACCELEROMETER:
				return InputType_MOTION;
			case Sensor.TYPE_GYROSCOPE:
				return InputType_ANGULAR;
			case Sensor.TYPE_LIGHT:
				return InputType_LIGHT;
			case Sensor.TYPE_PROXIMITY:
				return InputType_PROXIMITY;
			case Sensor.TYPE_MAGNETIC_FIELD:
				return InputType_MAGNETIC;
			default:
				return InputType_MAX;
		}
	}

	Context mContext;
	SensorManager mSensorManager;
	int mSampleTime=SensorManager.SENSOR_DELAY_GAME;
	int mSensorType;
	Sensor mSensor;
	int mInputType;
	boolean mRunning;
	InputDeviceListener mListener;
	InputData mData;
}
