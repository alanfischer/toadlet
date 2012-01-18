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

package us.toadlet.pad;

import android.app.Activity;
import android.content.Context;
import android.hardware.*;
import android.view.*;
import android.os.*;
import us.toadlet.peeper.*;
import us.toadlet.ribbit.*;
import us.toadlet.flick.*;

class ApplicationView extends SurfaceView implements SurfaceHolder.Callback{
	public ApplicationView(AndroidApplication application){
		super(application);
		mApplication=application;
		requestFocus();
		setFocusableInTouchMode(true);
		getHolder().addCallback(this);
		getHolder().setType(SurfaceHolder.SURFACE_TYPE_GPU);
	}

	public void onSizeChanged(int w,int h,int oldw,int oldh){
		mApplication.notifySizeChanged(w,h);
	}

	public boolean onKeyDown(int keyCode,KeyEvent event){
		return mApplication.notifyKeyPressed(mApplication.keyEventToKey(event));
	}
	
	public boolean onKeyUp(int keyCode,KeyEvent event){
		return mApplication.notifyKeyReleased(mApplication.keyEventToKey(event));
	}

	public boolean onTouchEvent(MotionEvent event){
		int x=(int)event.getX(),y=(int)event.getY();
		
		boolean result=false;
		switch(event.getAction()){
			case MotionEvent.ACTION_DOWN:
				result=mApplication.notifyMousePressed(x,y);
			break;
			case MotionEvent.ACTION_MOVE:
				result=mApplication.notifyMouseMoved(x,y);
			break;
			case MotionEvent.ACTION_UP:
				result=mApplication.notifyMouseReleased(x,y);
			break;
		}

		return true;
	}

	public void surfaceCreated(SurfaceHolder holder){
		mApplication.notifySurfaceCreated(holder,true);
		mApplication.notifySizeChanged(getWidth(),getHeight());
	}

	public void surfaceDestroyed(SurfaceHolder holder){
		mApplication.notifySurfaceDestroyed(holder,true);
	}

	public void surfaceChanged(SurfaceHolder holder,int format,int width,int height){}

	AndroidApplication mApplication;
}

public abstract class AndroidApplication extends Activity implements RenderTarget,Runnable{
	// Keys
	final static int Key_ENTER=	10;
	final static int Key_TAB=	8;
	final static int Key_SPACE=	32;

	final static int Key_LEFT=	1024;
	final static int Key_RIGHT=	Key_LEFT+1;
	final static int Key_UP=	Key_LEFT+2;
	final static int Key_DOWN=	Key_LEFT+3;

	final static int Key_ESC=	2048;
	final static int Key_PAUSE=	Key_ESC+1;
	final static int Key_SHIFT=	Key_ESC+2;
	final static int Key_CTRL=	Key_ESC+3;
	final static int Key_ALT=	Key_ESC+4;
	final static int Key_SPECIAL=	Key_ESC+5;
	final static int Key_BACKSPACE=	Key_ESC+6;
	final static int Key_DELETE=	Key_ESC+7;

	final static int Key_ACTION=	4096;
	final static int Key_BACK=		Key_ACTION+1;
	final static int Key_MENU=		Key_ACTION+2;
	final static int Key_SEARCH=	Key_ACTION+3;
	final static int Key_SOFTLEFT=	Key_ACTION+4;
	final static int Key_SOFTRIGHT=	Key_ACTION+5;

	public AndroidApplication(){
		super();
	}

	protected void onCreate(Bundle bundle){
		super.onCreate(bundle);

		System.out.println("Free memory on create:"+Runtime.getRuntime().freeMemory());

		create();
	}

	protected void onStart(){
		System.out.println("AndroidApplication.onStart");

		super.onStart();
		
		System.gc();
		System.out.println("Free memory on start:"+Runtime.getRuntime().freeMemory());

		mLastTime=System.currentTimeMillis();
		mRun=true;
		mThread=new Thread(this);
		mThread.start();
	}

	protected void onStop(){
		System.out.println("AndroidApplication.onStop");

		super.onStop();

		mRun=false;
		if(mThread!=null){
			try{
				mThread.join();
			}catch(InterruptedException ex){}
			mThread=null;
		}
	}

	protected void onDestroy(){
		System.out.println("AndroidApplication.onDestroy");

		super.onDestroy();
		destroy();
	}

	protected void onPause(){
		super.onPause();
		if(mActive==true){
			notifySurfaceDestroyed(mView.getHolder(),false);
		}
	}

	protected void onResume(){
		super.onResume();
		if(mActive==false){
			notifySurfaceCreated(mView.getHolder(),false);
			notifySizeChanged(mView.getWidth(),mView.getHeight());
		}
	}
	
	public boolean onCreateOptionsMenu(Menu menu){
		menu.add("Exit");
		return true;
	}
	
	public boolean onOptionsItemSelected(MenuItem item){
		stop();
		return true;
	}
	
	public boolean create(){
		if(mEngine==null){
			mEngine=new NEngine(this);
			mEngine.installHandlers();
		}

		if(mAudioDevice==null){
			mAudioDevice=new ATAudioDevice();
			if(mAudioDevice!=null && mAudioDevice.create()==false){
				mAudioDevice=null;
			}
			mEngine.setAudioDevice(mAudioDevice);
		}

		us.toadlet.flick.InputDevice motionDevice=new AndroidSensorDevice(this,Sensor.TYPE_ACCELEROMETER);
		if(motionDevice.create()){
			mInputDevices[motionDevice.getType()]=motionDevice;
		}

		us.toadlet.flick.InputDevice angularDevice=new AndroidSensorDevice(this,Sensor.TYPE_GYROSCOPE);
		if(angularDevice.create()){
			mInputDevices[angularDevice.getType()]=angularDevice;
		}

		us.toadlet.flick.InputDevice lightDevice=new AndroidSensorDevice(this,Sensor.TYPE_LIGHT);
		if(lightDevice.create()){
			mInputDevices[lightDevice.getType()]=lightDevice;
		}

		us.toadlet.flick.InputDevice proximityDevice=new AndroidSensorDevice(this,Sensor.TYPE_PROXIMITY);
		if(proximityDevice.create()){
			mInputDevices[proximityDevice.getType()]=proximityDevice;
		}

		us.toadlet.flick.InputDevice magneticDevice=new AndroidSensorDevice(this,Sensor.TYPE_MAGNETIC_FIELD);
		if(magneticDevice.create()){
			mInputDevices[magneticDevice.getType()]=magneticDevice;
		}
		
		if(mApplet==null){
			mApplet=createApplet(this);
			mApplet.create();
		}

		if(mFullscreen){
			requestWindowFeature(Window.FEATURE_NO_TITLE);   
			getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN);
		}

		mView=new ApplicationView(this);
		setContentView(mView);
		
		return true;
	}
	
	public void destroy(){
		if(mApplet!=null){
			mApplet.destroy();
			destroyApplet(mApplet);
			mApplet=null;
		}
		
		if(mEngine!=null){
			mEngine.destroy();
			mEngine=null;
		}

		if(mAudioDevice!=null){
			mAudioDevice.destroy();
			mAudioDevice=null;
		}

		int i;
		for(i=0;i<us.toadlet.flick.InputDevice.InputType_MAX;++i){
			if(mInputDevices[i]!=null){
				mInputDevices[i].destroy();
				mInputDevices[i]=null;
			}
		}
  	}
	
	public void start(){
	}
	
	public void stop(){
		finish();
	}

	protected abstract Applet createApplet(AndroidApplication app);
	protected abstract void destroyApplet(Applet applet);

	public void run(){
		while(mRun){
			long currentTime=System.currentTimeMillis();
			if(mActive){
				update((int)(currentTime-mLastTime));
				if(mAudioDevice!=null){
					mAudioDevice.update((int)(currentTime-mLastTime));
				}
				if(mRenderDevice!=null){
					render(mRenderDevice);
				}
			}
			mLastTime=currentTime;

			synchronized(mSurfaceMutex){
				if(mNotifySurfaceCreated!=null){
					surfaceCreated(mNotifySurfaceCreated);

					mNotifySurfaceCreated=null;
					mActive=true;
					mSurfaceMutex.notify();
				}
				if(mNotifySurfaceDestroyed!=null){
					surfaceDestroyed(mNotifySurfaceDestroyed);

					mNotifySurfaceDestroyed=null;
					mActive=false;
					mSurfaceMutex.notify();
				}
				if(mNotifySizeChanged){
					mNotifySizeChanged=false;
					resized(mWidth,mHeight);

					if(mActive && mRenderDevice!=null){
						render(mRenderDevice);
					}
				}
			}

			synchronized(this){
				if(mNotifyKeyPressed){
					mNotifyKeyPressed=false;
					keyPressed(mKeyPressed);
				}
				if(mNotifyKeyReleased){
					mNotifyKeyReleased=false;
					keyReleased(mKeyReleased);
				}
				if(mNotifyMousePressed){
					mNotifyMousePressed=false;
					mousePressed(mMousePressedX,mMousePressedY,0);
					mLastMouseX=mMousePressedX;
					mLastMouseY=mMousePressedY;
				}
				if(mNotifyMouseMoved){
					mNotifyMouseMoved=false;
					if(mDifferenceMouse){
						mouseMoved(mLastMouseX-mMouseMovedX,mLastMouseY-mMouseMovedY);
					}
					else{
						mouseMoved(mMouseMovedX,mMouseMovedY);
					}
					mLastMouseX=mMouseMovedX;
					mLastMouseY=mMouseMovedY;
				}
				if(mNotifyMouseReleased){
					mNotifyMouseReleased=false;
					if(mMouseMovedX!=mMouseReleasedX || mMouseMovedY!=mMouseReleasedY){
						if(mDifferenceMouse){
							mouseMoved(mLastMouseX-mMouseReleasedX,mLastMouseY-mMouseReleasedY);
						}
						else{
							mouseMoved(mMouseReleasedX,mMouseReleasedY);
						}
					}
					mouseReleased(mMouseReleasedX,mMouseReleasedY,0);
					mLastMouseX=mMouseReleasedX;
					mLastMouseY=mMouseReleasedY;
				}
			}
		}
	}

	synchronized boolean notifyKeyPressed(int key){
		if(mNotifyKeyPressed==false){
			mNotifyKeyPressed=true;
			mKeyPressed=key;
		}
		return false;
	}

	synchronized boolean notifyKeyReleased(int key){
		if(mNotifyKeyReleased==false){
			mNotifyKeyReleased=true;
			mKeyReleased=key;
		}
		return false;
	}

	synchronized boolean notifyMousePressed(int x,int y){
		if(mNotifyMousePressed==false){
			mNotifyMousePressed=true;
			mMousePressedX=x;
			mMousePressedY=y;
		}
		return false;
	}

	synchronized boolean notifyMouseMoved(int x,int y){
		if(mNotifyMouseMoved==false){
			mNotifyMouseMoved=true;
			mMouseMovedX=x;
			mMouseMovedY=y;
		}
		return false;
	}

	synchronized boolean notifyMouseReleased(int x,int y){
		if(mNotifyMouseReleased==false){
			mNotifyMouseReleased=true;
			mMouseReleasedX=x;
			mMouseReleasedY=y;
		}
		return false;
	}

	void notifySizeChanged(int width,int height){
		synchronized(mSurfaceMutex){
			mNotifySizeChanged=true;
			mWidth=width;
			mHeight=height;
		}
	}
	
	void notifySurfaceCreated(SurfaceHolder holder,boolean fromSurface){
		synchronized(mSurfaceMutex){
			if(mRun){
				if(fromSurface==true || mSurfaceCreated){
					mNotifySurfaceCreated=holder;
					try{ mSurfaceMutex.wait(); }
					catch(Exception ex){}
				}
			}
			else{
				surfaceDestroyed(holder);
			}

			if(fromSurface){
				mSurfaceCreated=true;
			}
		}
	}

	void notifySurfaceDestroyed(SurfaceHolder holder,boolean fromSurface){
		synchronized(mSurfaceMutex){
			if(mRun){
				if(fromSurface==true || mSurfaceCreated){
					mNotifySurfaceDestroyed=holder;
					try{ mSurfaceMutex.wait(); }
					catch(Exception ex){}
				}
			}
			else{
				surfaceDestroyed(holder);
			}

			if(fromSurface){
				mSurfaceCreated=false;
			}
		}
	}

	public boolean isRunning(){return mRun;}

	public boolean isActive(){return mActive;}

	public void setPosition(int x,int y){}
	public int getPositionX(){return 0;}
	public int getPositionY(){return 0;}

	public void setSize(int width,int height){}
	public int getWidth(){return mView==null?0:mView.getWidth();}
	public int getHeight(){return mView==null?0:mView.getHeight();}

	public void setDifferenceMouse(boolean difference){mDifferenceMouse=difference;}
	public boolean getDifferenceMouse(){return mDifferenceMouse;}

	public void setFullscreen(boolean fullscreen){mFullscreen=fullscreen;}
	public boolean getFullscreen(){return mFullscreen;}
	
	public void setApplet(Applet applet){mApplet=applet;}
	public Applet getApplet(){return mApplet;}

	public RenderTarget getRootRenderTarget(){return mRenderTarget;}
	public boolean isPrimary(){return mRenderTarget.isPrimary();}
	public boolean isValid(){return mRenderTarget.isValid();}

	public Engine getEngine(){return mEngine;}
	public RenderDevice getRenderDevice(){return mRenderDevice;}
	public AudioDevice getAudioDevice(){return mAudioDevice;}
	public us.toadlet.flick.InputDevice getInputDevice(int i){return mInputDevices[i];}
	
	public void resized(int width,int height)		{if(mApplet!=null)mApplet.resized(width,height);}
	public void focusGained()						{if(mApplet!=null)mApplet.focusGained();}
	public void focusLost()							{if(mApplet!=null)mApplet.focusLost();}
	public void keyPressed(int key)					{if(mApplet!=null)mApplet.keyPressed(key);}
	public void keyReleased(int key)				{if(mApplet!=null)mApplet.keyReleased(key);}
	public void mouseMoved(int x,int y)				{if(mApplet!=null)mApplet.mouseMoved(x,y);}
	public void mousePressed(int x,int y,int button){if(mApplet!=null)mApplet.mousePressed(x,y,button);}
	public void mouseReleased(int x,int y,int button){if(mApplet!=null)mApplet.mouseReleased(x,y,button);}
	public void mouseScrolled(int x,int y,int scroll){if(mApplet!=null)mApplet.mouseScrolled(x,y,scroll);}
	public void update(int dt)						{if(mApplet!=null)mApplet.update(dt);}
	public void render(RenderDevice renderDevice)	{if(mApplet!=null)mApplet.render(renderDevice);}

	public void surfaceCreated(SurfaceHolder holder){
		System.out.println(
			"AndroidApplication.surfaceCreated");

		boolean result=false;
		mRenderTarget=makeRenderTarget(holder);
		if(mRenderTarget!=null){
			mRenderDevice=new NRenderDevice();
			result=mRenderDevice.create(this,null);
			if(result==false){
				mRenderDevice=null;
			}
		}
		if(result==false){
			mRenderTarget=null;

			System.out.println(
				"Error creating RenderTarget");
		}
		else if(mRenderDevice==null){
			System.out.println(
				"error creating RenderDevice");
		}

		if(mRenderDevice!=null){
			mRenderDevice.setRenderTarget(this);
			mEngine.setRenderDevice(mRenderDevice);
		}
	}

	public void surfaceDestroyed(SurfaceHolder holder){
		System.out.println(
			"AndroidApplication.surfaceDestroyed");

		if(mRenderDevice!=null){
			mEngine.setRenderDevice(null);
			mRenderDevice.destroy();
			mRenderDevice=null;
		}

		if(mRenderTarget!=null){
			mRenderTarget.destroy();
			mRenderTarget=null;
		}
	}

	protected RenderTarget makeRenderTarget(SurfaceHolder holder){
		System.out.println("AndroidApplication.makeRenderTarget");

		WindowRenderTargetFormat format=new WindowRenderTargetFormat();
		format.pixelFormat=TextureFormat.Format_RGB_5_6_5;
		format.depthBits=16;
		format.multisamples=0;
		format.threads=0;
		RenderTarget target=new EGLWindowRenderTarget(null,holder,format);
		if(target!=null && target.isValid()==false){
			target=null;
		}

		return target;
	}

	public int keyEventToKey(KeyEvent event){
		switch(event.getKeyCode()){
			case KeyEvent.KEYCODE_BACK:
				return Key_BACK;
			case KeyEvent.KEYCODE_MENU:
				return Key_MENU;
			case KeyEvent.KEYCODE_SEARCH:
				return Key_SEARCH;
			default:
				return event.getUnicodeChar();
		}
	}
		
	Thread mThread=null;
	protected ApplicationView mView;
	protected Applet mApplet;
	protected Engine mEngine;
	protected boolean mFullscreen;
	protected boolean mActive;
	protected boolean mRun;
	protected Object mSurfaceMutex=new Object();
	protected long mLastTime=0;
	protected String mTitle;
	protected boolean mDifferenceMouse;
	protected int mLastMouseX,mLastMouseY;
	protected RenderTarget mRenderTarget;
	protected RenderDevice mRenderDevice;
	protected AudioDevice mAudioDevice;
	protected us.toadlet.flick.InputDevice[] mInputDevices=new us.toadlet.flick.InputDevice[us.toadlet.flick.InputDevice.InputType_MAX];
	
	protected SurfaceHolder mNotifySurfaceCreated;
	protected SurfaceHolder mNotifySurfaceDestroyed;
	protected boolean mSurfaceCreated;
	protected boolean mNotifySizeChanged;
	protected int mWidth,mHeight;
	protected boolean mNotifyKeyPressed;
	protected int mKeyPressed;
	protected boolean mNotifyKeyReleased;
	protected int mKeyReleased;
	protected boolean mNotifyMousePressed;
	protected int mMousePressedX,mMousePressedY;
	protected boolean mNotifyMouseMoved;
	protected int mMouseMovedX,mMouseMovedY;
	protected boolean mNotifyMouseReleased;
	protected int mMouseReleasedX,mMouseReleasedY;
}
