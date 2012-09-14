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

#ifndef TOADLET_FLICK_ANDROIDSENSORDEVICE_H
#define TOADLET_FLICK_ANDROIDSENSORDEVICE_H

#include <toadlet/flick/BaseInputDevice.h>
#include <toadlet/flick/InputData.h>


struct ALooper;
typedef struct ALooper ALooper;

enum {
    ALOOPER_PREPARE_ALLOW_NON_CALLBACKS = 1<<0
};

typedef int (*ALooper_callbackFunc)(int fd, int events, void* data);
typedef ALooper*(*TALooper_forThread)();
typedef ALooper*(*TALooper_prepare)(int);


struct ASensorManager;
typedef struct ASensorManager ASensorManager;
struct ASensorEventQueue;
typedef struct ASensorEventQueue ASensorEventQueue;
struct ASensor;
typedef struct ASensor ASensor;

enum {
    ASENSOR_TYPE_ACCELEROMETER      = 1,
    ASENSOR_TYPE_MAGNETIC_FIELD     = 2,
    ASENSOR_TYPE_GYROSCOPE          = 4,
    ASENSOR_TYPE_LIGHT              = 5,
    ASENSOR_TYPE_PROXIMITY          = 8
};

typedef struct ASensorVector {
    union {
        float v[3];
        struct {
            float x;
            float y;
            float z;
        };
        struct {
            float azimuth;
            float pitch;
            float roll;
        };
    };
    int8_t status;
    uint8_t reserved[3];
} ASensorVector;

/* NOTE: Must match hardware/sensors.h */
typedef struct ASensorEvent {
    int32_t version; /* sizeof(struct ASensorEvent) */
    int32_t sensor;
    int32_t type;
    int32_t reserved0;
    int64_t timestamp;
    union {
        float           data[16];
        ASensorVector   vector;
        ASensorVector   acceleration;
        ASensorVector   magnetic;
        float           temperature;
        float           distance;
        float           light;
        float           pressure;
    };
    int32_t reserved1[4];
} ASensorEvent;

typedef ASensorManager*(*TASensorManager_getInstance)();
typedef ASensorEventQueue*(*TASensorManager_createEventQueue)(ASensorManager*,ALooper*,int,ALooper_callbackFunc,void*);
typedef ASensor const*(*TASensorManager_getDefaultSensor)(ASensorManager*,int);
typedef int(*TASensorManager_destroyEventQueue)(ASensorManager*,ASensorEventQueue*);
typedef int(*TASensorEventQueue_enableSensor)(ASensorEventQueue*,ASensor const*);
typedef int(*TASensorEventQueue_disableSensor)(ASensorEventQueue*,ASensor const*);
typedef int(*TASensorEventQueue_setEventRate)(ASensorEventQueue*,ASensor const*,int32_t);
typedef ssize_t(*TASensorEventQueue_getEvents)(ASensorEventQueue*,ASensorEvent*,size_t);


namespace toadlet{
namespace flick{

class TOADLET_API AndroidSensorDevice:public BaseInputDevice{
public:
	TOADLET_OBJECT(AndroidSensorDevice);

	AndroidSensorDevice(int type);

	virtual bool create();
	virtual void destroy();

	virtual InputType getType(){return mInputType;}
	virtual bool start();
	virtual void update(int dt){}
	virtual void stop();

	virtual void setListener(InputDeviceListener *listener){mListener=listener;}
	virtual void setSampleTime(int dt);
	virtual void setAlpha(scalar alpha){}
	
protected:
	void onSensorChanged();

	static InputType getInputTypeFromSensorType(int sensorType);
	static int sensorChanged(int fd,int events,void  *data){((AndroidSensorDevice*)data)->onSensorChanged();return 1;}

	TALooper_forThread mALooper_forThread;
	TALooper_prepare mALooper_prepare;

	TASensorManager_getInstance mASensorManager_getInstance;
	TASensorManager_createEventQueue mASensorManager_createEventQueue;
	TASensorManager_getDefaultSensor mASensorManager_getDefaultSensor;
	TASensorManager_destroyEventQueue mASensorManager_destroyEventQueue;
	TASensorEventQueue_enableSensor mASensorEventQueue_enableSensor;
	TASensorEventQueue_disableSensor mASensorEventQueue_disableSensor;
	TASensorEventQueue_setEventRate mASensorEventQueue_setEventRate;
	TASensorEventQueue_getEvents mASensorEventQueue_getEvents;
	
	ASensorManager *mSensorManager;
	ASensorEventQueue *mEventQueue;
	int mSensorType;
	const ASensor *mSensor;
	InputType mInputType;
	bool mRunning;
	InputDeviceListener *mListener;
	InputData mData;
};

}
}

#endif
