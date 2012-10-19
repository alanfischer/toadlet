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

package com.lightningtoads.toadlet.tadpole.sg;

import com.lightningtoads.toadlet.tadpole.*;
import com.motorola.iden.micro3d.*;

public class RenderableEntity extends Entity{
	public static final int MAX_LAYERS=4;
	public static final int MAX_RENDERABLES_IN_LAYER=32;

	public RenderableEntity(Engine engine){
		super(engine);
		mType|=TYPE_RENDERABLE;
		mLayer=1;
		mVisible=true;

		mEngine.getSceneManager().registerRenderableEntity(this);
	}

	public void destroy(){
		if(mEngine.getSceneManager()!=null){
			mEngine.getSceneManager().unregisterRenderableEntity(this);
		}

		super.destroy();
	}
	
	public final Layout3D getLayout(){
		return mLayout;
	}

	public final void setLayer(int layer){
		mLayer=layer;
	}

	public final int getLayer(){
		return mLayer;
	}

	public final void setVisible(boolean visible){
		mVisible=visible;
	}

	public final boolean getVisible(){
		return mVisible;
	}

	public final void setObject3D(Object3D object){
		mObject3D=object;
	}

	public final Object3D getObject3D(){
		return mObject3D;
	}

	public final void setTexture(String name){
		if(name!=null){
			mTexture=mEngine.getTexture(name);
		}
		else{
			mTexture=null;
		}
		mTextures=null;
	}

	public final Texture getTexture(){
		return mTexture;
	}

	public final void setTextures(String[] names){
		mTexture=null;
		if(names!=null){
			mTextures=new MultiTexture(names.length);
			int i;
			for(i=0;i<names.length;++i){
				mTextures.addTexture(mEngine.getTexture(names[i]));
			}
		}
		else{
			mTextures=null;
		}
	}
	
	public final MultiTexture getTextures(){
		return mTextures;
	}

	public final void setLit(boolean lit){
		mLit=lit;

		if(mLit){
			LightEntity light=mEngine.getSceneManager().mLight;
			if(light!=null){
				mLayout.setLight(light.mLight);
			}
			else{
				mLayout.setLight(null);
			}
		}
		else{
			mLayout.setLight(null);
		}
	}

	public final boolean getLit(){
		return mLit;
	}

	Object3D mObject3D;
	Texture mTexture;
	MultiTexture mTextures;
	Layout3D mLayout=new Layout3D();
	int mLayer;
	boolean mVisible;
	boolean mLit;
	boolean mWorldSpace;
}
