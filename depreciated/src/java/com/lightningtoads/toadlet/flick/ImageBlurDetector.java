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

public interface ImageBlurDetector{
	// State
	public final static int STATE_DESTROYED=0;
	public final static int STATE_STOPPED=1;
	public final static int STATE_RUNNING=2;

	// LightingResults
	public final static int LR_GOOD=0;
	public final static int LR_TOO_BRIGHT=1;
	public final static int LR_TOO_DIM=2;

	// Errors
	public final static int ERROR_NONE=0;
	public final static int ERROR_SETUP=1;
	public final static int ERROR_SEQUENCE=2;
	public final static int ERROR_SECURITY=3;
	public final static int ERROR_IMAGE_ACQUISITION=4;
	public final static int ERROR_IMAGE_CREATION=5;
	public final static int ERROR_IMAGE_ANALYSIS=6;

	public final static int INVALID_ANGLE_DETECTED=-360;

	public abstract boolean available();

	public abstract boolean create();

	public abstract boolean startup();

	public abstract boolean shutdown();

	public abstract boolean destroy();

	public abstract void setSleepTimes(int prestart,int precapture);

	public abstract void setListener(ImageBlurDetectorListener listener);

	public abstract boolean startDetection();

	public abstract int getDetectedAngle();

	public abstract int getDetectedAngleClusterSize();

	public abstract int getDetectedAngleClusterWidth();

	public abstract boolean startLightingCheck();

	public abstract int getLightingResult();

	public abstract int getState();

	public abstract int getError();
}
