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
import android.view.*;
import android.os.*;
import us.toadlet.peeper.*;

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
		mApplication.notifyKeyPressed(event.getUnicodeChar());
		return true;
	}
	
	public boolean onKeyUp(int keyCode,KeyEvent event){
		mApplication.notifyKeyReleased(event.getUnicodeChar());
		return true;
	}

	public boolean onTouchEvent(MotionEvent event){
		int x=(int)event.getX(),y=(int)event.getY();
		
		switch(event.getAction()){
			case MotionEvent.ACTION_DOWN:
				mApplication.notifyMousePressed(x,y);
			break;
			case MotionEvent.ACTION_MOVE:
				mApplication.notifyMouseMoved(x,y);
			break;
			case MotionEvent.ACTION_UP:
				mApplication.notifyMouseReleased(x,y);
			break;
		}

		try{Thread.sleep(100);}catch(Exception ex){}
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
    public AndroidApplication(){
		super();
	}

	protected void onCreate(Bundle bundle){
		super.onCreate(bundle);

		create();
	}

	protected void onStart(){
		System.out.println("AndroidApplication.onStart");
	
		super.onStart();
		start();
	}

	protected void onStop(){
		System.out.println("AndroidApplication.onStop");

		super.onStop();
		stop();
	}

	protected void onDestroy(){
		System.out.println("AndroidApplication.onDestroy");

		super.onStop();
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
	
	public void create(){
//		mEngine=makeEngine();

//		mEngine.installHandlers();
		
//		if(mApplet!=null){
//			mApplet.create();
//		}
	}
	
	public void destroy(){
//		if(mApplet!=null){
//			mApplet.destroy();
//		}

//		deleteEngine(mEngine);
	}
	
	public void start(){
		if(mFullscreen){
			requestWindowFeature(Window.FEATURE_NO_TITLE);   
			getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN);
		}
	
		mView=new ApplicationView(this);
		setContentView(mView);

		mLastTime=System.currentTimeMillis();

		mRun=true;
		mThread=new Thread(this);
		mThread.start();
	}
	
	public void stop(){
		finish();
		
		mRun=false;
		
		try{
			mThread.join();
		}catch(InterruptedException ex){}
	}

	protected abstract Applet createApplet(AndroidApplication app);
	protected abstract void destroyApplet(Applet applet);

	public void run(){
		while(mRun){
			long currentTime=System.currentTimeMillis();
			if(mActive){
				update((int)(currentTime-mLastTime));
				if(mRenderDevice!=null){
					render(mRenderDevice);
				}
			}
			mLastTime=currentTime;

			synchronized(mSurfaceMutex){
				if(mNotifySurfaceCreated!=null){
if(mEngine==null){
	mEngine=makeEngine();
}

					surfaceCreated(mNotifySurfaceCreated);

if(mEngine!=null){
	mEngine.installHandlers();
}

if(mApplet==null){
	setApplet(createApplet(this));
	mApplet.create();
}

					mNotifySurfaceCreated=null;
					mActive=true;
					mSurfaceMutex.notify();
				}
				if(mNotifySurfaceDestroyed!=null){
if(mApplet!=null){
	mApplet.destroy();
	destroyApplet(mApplet);
	setApplet(null);
}

if(mEngine!=null){
	mEngine.destroy();
}
					surfaceDestroyed(mNotifySurfaceDestroyed);

if(mEngine!=null){
	deleteEngine(mEngine);
	mEngine=null;
}

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
				}
				if(mNotifyMouseMoved){
					mNotifyMouseMoved=false;
					mouseMoved(mMouseMovedX,mMouseMovedY);
				}
				if(mNotifyMouseReleased){
					mNotifyMouseReleased=false;
					if(mMouseMovedX!=mMouseReleasedX || mMouseMovedY!=mMouseReleasedY){
						mouseMoved(mMouseReleasedX,mMouseReleasedY);
					}
					mouseReleased(mMouseReleasedX,mMouseReleasedY,0);
				}
			}
		}
	}

	synchronized void notifyKeyPressed(int key){
		if(mNotifyKeyPressed==false){
			mNotifyKeyPressed=true;
			mKeyPressed=key;
		}
	}

	synchronized void notifyKeyReleased(int key){
		if(mNotifyKeyReleased==false){
			mNotifyKeyReleased=true;
			mKeyReleased=key;
		}
	}

	synchronized void notifyMousePressed(int x,int y){
		if(mNotifyMousePressed==false){
			mNotifyMousePressed=true;
			mMousePressedX=x;
			mMousePressedY=y;
		}
	}

	synchronized void notifyMouseMoved(int x,int y){
		if(mNotifyMouseMoved==false){
			mNotifyMouseMoved=true;
			mMouseMovedX=x;
			mMouseMovedY=y;
		}
	}

	synchronized void notifyMouseReleased(int x,int y){
		if(mNotifyMouseReleased==false){
			mNotifyMouseReleased=true;
			mMouseReleasedX=x;
			mMouseReleasedY=y;
		}
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

	public void setAutoActivate(boolean autoActivate){}
	public boolean getAutoActivate(){return false;}
	
	public boolean isActive(){return mActive;}

	public void setPosition(int x,int y){}
	public int getPositionX(){return 0;}
	public int getPositionY(){return 0;}

	public void setSize(int width,int height){}
	public int getWidth(){return mView==null?0:mView.getWidth();}
	public int getHeight(){return mView==null?0:mView.getHeight();}

	public void setFullscreen(boolean fullscreen){mFullscreen=fullscreen;}
	public boolean getFullscreen(){return mFullscreen;}
	
	public void setApplet(Applet applet){mApplet=applet;}
	public Applet getApplet(){return mApplet;}

	public RenderTarget getRootRenderTarget(){return mRenderTarget;}
	public boolean isPrimary(){return mRenderTarget.isPrimary();}
	public boolean isValid(){return mRenderTarget.isValid();}

	public Engine getEngine(){return mEngine;}
	public RenderDevice getRenderDevice(){return mRenderDevice;}

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
			mRenderDevice=makeRenderDevice();

			result=mRenderDevice.create(this,null);
			if(result==false){
				mRenderDevice=null;
			}
		}
		if(result==false){
			mRenderTarget=null;

			System.out.println(
				"Error creating RenderTargetPeer");
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
			deleteRenderDevice(mRenderDevice);
			mRenderDevice=null;
		}

		if(mRenderTarget!=null){
			mRenderTarget=null;
		}
	}

	protected RenderTarget makeRenderTarget(SurfaceHolder holder){
		WindowRenderTargetFormat format=new WindowRenderTargetFormat();
		format.pixelFormat=TextureFormat.Format_RGB_5_6_5;
		format.depthBits=16;
		format.multisamples=0;
		format.threads=0;
		RenderTarget target=new EGLWindowRenderTarget(holder,format);
		if(target!=null && target.isValid()==false){
			target=null;
		}
		return target;
	}

	protected native Engine makeEngine();
	protected native void deleteEngine(Engine engine);
	protected native RenderDevice makeRenderDevice();
	protected native void deleteRenderDevice(RenderDevice device);
	
	Thread mThread=null;
	protected ApplicationView mView;
	protected Applet mApplet;
	protected Engine mEngine;
	protected boolean mFullscreen;
	protected boolean mActive;
	protected boolean mRun;
	protected Object mSurfaceMutex=new Object();
	protected long mLastTime=0;
	protected RenderTarget mRenderTarget;
	protected RenderDevice mRenderDevice;
//	protected AudioDevice mAudioDevice;
//	protected MotionDevice mMotionDevice;
		
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
