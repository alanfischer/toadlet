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

package com.lightningtoads.toadlet.pad;

#include <com/lightningtoads/toadlet/Types.h>

import com.lightningtoads.toadlet.egg.*;
import com.lightningtoads.toadlet.egg.Error;
import com.lightningtoads.toadlet.peeper.*;
import com.lightningtoads.toadlet.peeper.plugins.glrenderer.*;
import com.lightningtoads.toadlet.tadpole.*;
import android.app.Activity;
import android.content.Context;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

class ApplicationView extends SurfaceView implements SurfaceHolder.Callback{
	public ApplicationView(Application application){
		super(application);
		mApplication=application;
		getHolder().addCallback(this);
		getHolder().setType(SurfaceHolder.SURFACE_TYPE_GPU);
	}

	public void onSizeChanged(int w,int h,int oldw,int oldh){
		mApplication.notifySizeChanged(w,h);
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
	}

	public void surfaceDestroyed(SurfaceHolder holder){
		mApplication.notifySurfaceDestroyed(holder,true);
	}

	public void surfaceChanged(SurfaceHolder holder,int format,int width,int height){}

	Application mApplication;
}

public class Application extends Activity implements RenderContext,Runnable{
    public Application(){
		super();
	}

	protected void onCreate(Bundle bundle){
		super.onCreate(bundle);
		create();
	}

	protected void onStart(){
		super.onStart();
		start(true);
	}

	protected void onStop(){
		super.onStop();
		stop();
	}

	protected void onDestroy(){
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
		}
	}
	
	public void create(){
		mEngine=new Engine();
	}
	
	public void destroy(){
	}
	
	public boolean start(boolean runEventLoop){
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
		
		return true;
	}
	
	public void stop(){
		finish();
		mRun=false;
	}

	public void run(){
		while(mRun){
			long currentTime=System.currentTimeMillis();
			if(mActive){
				update((int)(currentTime-mLastTime));
				if(mRenderer!=null){
					render(mRenderer);
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

					if(mActive && mRenderer!=null){
						if(mRenderer.getCapabilitySet().resetOnResize){
							mEngine.setRenderer(null);

							mRenderer.reset();

							mEngine.setRenderer(mRenderer);
						}
						render(mRenderer);
					}
				}
			}
			synchronized(this){
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
	
	public void setVisual(Visual visual){}
	public Visual getVisual(){return null;}

	public void setApplicationListener(ApplicationListener listener){mListener=listener;}
	public ApplicationListener getApplicationListener(){return mListener;}

	public Engine getEngine(){return mEngine;}
	public Renderer getRenderer(){return mRenderer;}
	//public AudioPlayer getAudioPlayer(){return mAudioPlayer;}

	public void resized(int width,int height)		{if(mListener!=null)mListener.resized(width,height);}
	public void focusGained()						{if(mListener!=null)mListener.focusGained();}
	public void focusLost()							{if(mListener!=null)mListener.focusLost();}
	public void keyPressed(int key)					{if(mListener!=null)mListener.keyPressed(key);}
	public void keyReleased(int key)				{if(mListener!=null)mListener.keyReleased(key);}
	public void mouseMoved(int x,int y)				{if(mListener!=null)mListener.mouseMoved(x,y);}
	public void mousePressed(int x,int y,int button){if(mListener!=null)mListener.mousePressed(x,y,button);}
	public void mouseReleased(int x,int y,int button){if(mListener!=null)mListener.mouseReleased(x,y,button);}
	public void mouseScrolled(int x,int y,int scroll){if(mListener!=null)mListener.mouseScrolled(x,y,scroll);}
	public void update(int dt)						{if(mListener!=null)mListener.update(dt);}
	public void render(Renderer renderer)			{if(mListener!=null)mListener.render(renderer);}

	public void setRendererOptions(int[] options){
		mRendererOptions=new int[options.length];
		System.arraycopy(options,0,mRendererOptions,0,options.length);
	}

	public void surfaceCreated(SurfaceHolder holder){
		Logger.alert(Categories.TOADLET_PAD,
			"surfaceCreated");

		RenderTarget.Peer renderTargetPeer=new EGLRenderContextPeer(holder,new Visual());
		if(renderTargetPeer.isValid()){
			internal_setRenderTargetPeer(renderTargetPeer);
			
			mRenderer=new GLRenderer();
			if(mRenderer.startup(this,mRendererOptions)==false){
				mRenderer=null;

				Error.unknown(Categories.TOADLET_PAD,
					"Error starting Renderer");
			}

			if(mRenderer==null){
				internal_setRenderTargetPeer(null);
			}
		}
		else{
			Error.unknown(Categories.TOADLET_PAD,
				"Error creating RenderTargetPeer");
		}

		if(mRenderer!=null){
			mRenderer.setRenderTarget(this);
			mEngine.setRenderer(mRenderer);
		}
	}

	public void surfaceDestroyed(SurfaceHolder holder){
		Logger.alert(Categories.TOADLET_PAD,
			"surfaceDestroyed");

		if(mRenderer!=null){
			mEngine.setRenderer(null);

			mRenderer.shutdown();
			mRenderer=null;
		}

		internal_setRenderTargetPeer(null);
	}

	public Texture castToTexture(){return null;}
	public void internal_setRenderTargetPeer(RenderTarget.Peer peer){
		if(mRenderTargetPeer!=null){
			mRenderTargetPeer.destroy();
		}
		mRenderTargetPeer=peer;
	}
	public RenderTarget.Peer internal_getRenderTargetPeer(){return mRenderTargetPeer;}
	
	Thread mThread=null;
	protected ApplicationView mView;
	protected ApplicationListener mListener;
	protected Engine mEngine;
	protected boolean mFullscreen;
	protected boolean mActive;
	protected boolean mRun;
	protected Object mSurfaceMutex=new Object();
	protected long mLastTime=0;
	protected RenderTarget.Peer mRenderTargetPeer;
	protected Renderer mRenderer;
	protected int[] mRendererOptions;
		
	protected SurfaceHolder mNotifySurfaceCreated;
	protected SurfaceHolder mNotifySurfaceDestroyed;
	protected boolean mSurfaceCreated;
	protected boolean mNotifySizeChanged;
	protected int mWidth,mHeight;
	protected boolean mNotifyMousePressed;
	protected int mMousePressedX,mMousePressedY;
	protected boolean mNotifyMouseMoved;
	protected int mMouseMovedX,mMouseMovedY;
	protected boolean mNotifyMouseReleased;
	protected int mMouseReleasedX,mMouseReleasedY;
}
