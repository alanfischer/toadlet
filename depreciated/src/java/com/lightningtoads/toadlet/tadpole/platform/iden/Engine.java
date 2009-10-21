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

package com.lightningtoads.toadlet.tadpole;

import java.io.*;
import java.util.*;
import com.lightningtoads.toadlet.tadpole.sg.*;
import com.motorola.iden.micro3d.*;
//#ifdef TOADLET_DEBUG
	import com.lightningtoads.toadlet.egg.*;
//#endif

public final class Engine{
	//#include src/java/com/lightningtoads/toadlet/Versions.java.inc
	//#endinclude

	public Engine(int width,int height) throws Exception{
		mWidth=width;
		mHeight=height;

		// Here we do a check to see if this Engine is useable
		Class.forName("com.motorola.iden.micro3d.Renderer");

		mRenderer=new Renderer();
	}

	public String getVersion(){
		return TOADLET_TADPOLE_VERSION_STRING;
	}

	public void setScreenSize(int width,int height){
		mWidth=width;
		mHeight=height;
	}

	public int getScreenWidth(){
		return mWidth;
	}

	public int getScreenHeight(){
		return mHeight;
	}

	public void setSceneManager(SceneManager manager){
		mSceneManager=manager;
	}

	public SceneManager getSceneManager(){
		return mSceneManager;
	}

	public void destroy(){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Releasing cached resources");
		//#endif

		mFigureNameMap.clear();
		mFigurePtrMap.clear();
		mActionTableNameMap.clear();
		mActionTablePtrMap.clear();
		mTextureNameMap.clear();
		mTexturePtrMap.clear();
		mSpriteNameMap.clear();
		mSpritePtrMap.clear();
	}

	public Figure cacheMesh(String name,boolean animation) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching mesh:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mFigureNameMap.get(name);
		if(resCache==null){
			Figure figure=Figure.createFigure(name+".mbac");
			resCache=new ResourceCache(figure,name);
			mFigureNameMap.put(name,resCache);
			mFigurePtrMap.put(figure,resCache);
		}
		else{
			resCache.count++;
		}

		Figure figure=(Figure)resCache.resource;

		if(animation){
			resCache=(ResourceCache)mActionTableNameMap.get(name);
			if(resCache==null){
				ActionTable actionTable=ActionTable.createActionTable(name+".mtra");
				resCache=new ResourceCache(actionTable,name);
				mActionTableNameMap.put(name,resCache);
				mActionTablePtrMap.put(actionTable,resCache);
			}
			else{
				resCache.count++;
			}
		}

		return figure;
	}

	public Figure cacheMesh(String name,Figure figure){
		return cacheMesh(name,figure,null);
	}

	public Figure cacheMesh(String name,Figure figure,ActionTable actionTable){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching created mesh:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mFigureNameMap.get(name);
		if(resCache==null){
			resCache=new ResourceCache(figure,name);
			mFigureNameMap.put(name,resCache);
			mFigurePtrMap.put(figure,resCache);
		}
		else{
			resCache.count++;
		}

		if(actionTable!=null){
			resCache=(ResourceCache)mActionTableNameMap.get(name);
			if(resCache==null){
				resCache=new ResourceCache(actionTable,name);
				mActionTableNameMap.put(name,resCache);
				mActionTablePtrMap.put(figure,resCache);
			}
			else{
				resCache.count++;
			}
		}

		return figure;
	}

	public Figure cacheMesh(Figure figure){
		return cacheMesh(figure,null);
	}

	public Figure cacheMesh(Figure figure,ActionTable actionTable){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching unnamed mesh");
		//#endif

		ResourceCache resCache=(ResourceCache)mFigurePtrMap.get(figure);
		if(resCache==null){
			resCache=new ResourceCache(figure);
			mFigurePtrMap.put(figure,resCache);
		}
		else{
			resCache.count++;
		}

		if(actionTable!=null){
			resCache=(ResourceCache)mActionTablePtrMap.get(actionTable);
			if(resCache==null){
				resCache=new ResourceCache(actionTable);
				mActionTablePtrMap.put(actionTable,resCache);
			}
			else{
				resCache.count++;
			}
		}
		
		return figure;
	}

	public void uncacheMesh(String name){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Uncaching mesh:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mFigureNameMap.get(name);
		if(resCache!=null){
			resCache.count--;
			if(resCache.count<=0){
				mFigureNameMap.remove(name);
				mFigurePtrMap.remove((Figure)resCache.resource);
			}
		}

		resCache=(ResourceCache)mActionTableNameMap.get(name);
		if(resCache!=null){
			resCache.count--;
			if(resCache.count<=0){
				mActionTableNameMap.remove(name);
				mActionTablePtrMap.remove((ActionTable)resCache.resource);
			}
		}
	}

	public void uncacheMesh(Figure figure){
		uncacheMesh(figure,null);
	}

	public void uncacheMesh(Figure figure,ActionTable actionTable){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Uncaching mesh");
		//#endif

		ResourceCache resCache=(ResourceCache)mFigurePtrMap.get(figure);
		if(resCache!=null){
			resCache.count--;
			if(resCache.count<=0){
				if(resCache.name!=null){
					mFigureNameMap.remove(resCache.name);
				}
				mFigurePtrMap.remove(figure);
			}
		}

		if(actionTable!=null){
			resCache=(ResourceCache)mActionTablePtrMap.get(actionTable);
			if(resCache!=null){
				resCache.count--;
				if(resCache.count<=0){
					if(resCache.name!=null){
						mActionTableNameMap.remove(resCache.name);
					}
					mActionTablePtrMap.remove(actionTable);
				}
			}
		}
	}

	public Figure getFigure(String name){
		if(name==null){
			return null;
		}

		ResourceCache resCache=(ResourceCache)mFigureNameMap.get(name);
		if(resCache==null){
			return null;
		}
		else{
			return (Figure)resCache.resource;
		}
	}

	public ActionTable getActionTable(String name){
		if(name==null){
			return null;
		}

		ResourceCache resCache=(ResourceCache)mActionTableNameMap.get(name);
		if(resCache==null){
			return null;
		}
		else{
			return (ActionTable)resCache.resource;
		}
	}

	public Texture cacheTexture(String name) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching texture:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mTextureNameMap.get(name);
		if(resCache==null){
			Texture texture=Texture.createTexture(name+".bmp",false);
			resCache=new ResourceCache(texture,name);
			mTextureNameMap.put(name,resCache);
			mTexturePtrMap.put(texture,resCache);
		}
		else{
			resCache.count++;
		}

		return (Texture)resCache.resource;
	}

	public Texture cacheTexture(String name,Texture texture){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching named texture:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mTextureNameMap.get(name);
		if(resCache==null){
			resCache=new ResourceCache(texture,name);
			mTextureNameMap.put(name,resCache);
			mTexturePtrMap.put(texture,resCache);
		}
		else{
			resCache.count++;
		}

		return texture;
	}

	public Texture cacheTexture(Texture texture){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching unnamed texture");
		//#endif

		ResourceCache resCache=(ResourceCache)mTexturePtrMap.get(texture);
		if(resCache==null){
			resCache=new ResourceCache(texture);
			mTexturePtrMap.put(texture,resCache);
		}
		else{
			resCache.count++;
		}

		return texture;
	}

	public void uncacheTexture(String name){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Uncaching texture:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mTextureNameMap.get(name);
		if(resCache!=null){
			resCache.count--;
			if(resCache.count<=0){
				mTextureNameMap.remove(name);
				mTexturePtrMap.remove((Texture)resCache.resource);
			}
		}
	}

	public void uncacheTexture(Texture texture){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Uncaching texture");
		//#endif

		ResourceCache resCache=(ResourceCache)mTexturePtrMap.get(texture);
		if(resCache!=null){
			resCache.count--;
			if(resCache.count<=0){
				if(resCache.name!=null){
					mTextureNameMap.remove(resCache.name);
				}
				mTexturePtrMap.remove(texture);
			}
		}
	}

	public Texture getTexture(String name){
		if(name==null){
			return null;
		}

		ResourceCache resCache=(ResourceCache)mTextureNameMap.get(name);
		if(resCache==null){
			return null;
		}
		else{
			return (Texture)resCache.resource;
		}
	}

	public Sprite cacheSprite(String name,int widthFrames,int heightFrames,int startFrame,int numFrames,String file,int totalWidth,int totalHeight) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching sprite:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mSpriteNameMap.get(name);
		if(resCache==null){
			Texture texture=cacheTexture(file);
			Sprite sprite=new Sprite(widthFrames,heightFrames,startFrame,numFrames,texture,totalWidth/widthFrames,totalHeight/heightFrames);
			resCache=new ResourceCache(sprite,name);
			mSpriteNameMap.put(name,resCache);
			mSpritePtrMap.put(sprite,resCache);
		}
		else{
			resCache.count++;
		}

		return (Sprite)resCache.resource;
	}

	public Sprite cacheSprite(Sprite sprite){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching unnamed sprite");
		//#endif

		ResourceCache resCache=(ResourceCache)mSpritePtrMap.get(sprite);
		if(resCache==null){
			resCache=new ResourceCache(sprite);
			mSpritePtrMap.put(sprite,resCache);
		}
		else{
			resCache.count++;
		}

		return sprite;
	}

	public void uncacheSprite(String name){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Uncaching sprite:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mSpriteNameMap.get(name);
		if(resCache!=null){
			resCache.count--;
			if(resCache.count<=0){
				mSpriteNameMap.remove(name);
				mSpritePtrMap.remove((Sprite)resCache.resource);

				uncacheTexture(((Sprite)resCache.resource).texture);
			}
		}
	}

	public void uncacheSprite(Sprite sprite){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Uncaching sprite");
		//#endif

		ResourceCache resCache=(ResourceCache)mSpritePtrMap.get(sprite);
		if(resCache!=null){
			resCache.count--;
			if(resCache.count<=0){
				if(resCache.name!=null){
					mSpriteNameMap.remove(resCache.name);
				}
				mSpritePtrMap.remove(sprite);

				uncacheTexture(sprite.texture);
			}
		}
	}

	public Sprite getSprite(String name){
		if(name==null){
			return null;
		}

		ResourceCache resCache=(ResourceCache)mSpriteNameMap.get(name);
		if(resCache==null){
			return null;
		}
		else{
			return (Sprite)resCache.resource;
		}
	}

	public Renderer getRenderer(){
		return mRenderer;
	}

	protected final static class ResourceCache{
		public ResourceCache(Object resource,String name){
			this.resource=resource;
			this.name=name;
			count=1;
		}

		public ResourceCache(Object resource){
			this.resource=resource;
			count=1;
		}

		public Object resource;
		public String name;
		public int count;
	};

	protected SceneManager mSceneManager=null;
	public int mWidth=0;
	public int mHeight=0;
	Renderer mRenderer;

	protected Hashtable mFigureNameMap=new Hashtable();
	protected Hashtable mFigurePtrMap=new Hashtable();
	protected Hashtable mActionTableNameMap=new Hashtable();
	protected Hashtable mActionTablePtrMap=new Hashtable();
	protected Hashtable mTextureNameMap=new Hashtable();
	protected Hashtable mTexturePtrMap=new Hashtable();
	protected Hashtable mSpriteNameMap=new Hashtable();
	protected Hashtable mSpritePtrMap=new Hashtable();
}

