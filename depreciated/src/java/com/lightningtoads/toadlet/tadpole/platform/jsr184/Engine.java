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
import javax.microedition.m3g.*;
import javax.microedition.lcdui.*;
import com.lightningtoads.toadlet.tadpole.sg.*;
//#ifdef TOADLET_DEBUG
	import com.lightningtoads.toadlet.egg.*;
//#endif

public final class Engine{
	//#include src/java/com/lightningtoads/toadlet/Versions.java.inc
	//#endinclude

	public Engine() throws Exception{
		// Here we do a check to see if this Engine is useable
		Class.forName("javax.microedition.m3g.Graphics3D");

		mGraphics3D=Graphics3D.getInstance();
	}

	public String getVersion(){
		return TOADLET_TADPOLE_VERSION_STRING;
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

		mMeshNameMap.clear();
		mMeshPtrMap.clear();
		mTextureNameMap.clear();
		mTexturePtrMap.clear();
		mSpriteNameMap.clear();
		mSpritePtrMap.clear();

		mFileImageMap.clear();
		mSprite3DCache.clear();
	}

	public Mesh cacheMesh(String name) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching mesh:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mMeshNameMap.get(name);
		if(resCache==null){
			Object3D[] sg=Loader.load(name+".m3g");
			Mesh mesh=(Mesh)sg[0];
			resCache=new ResourceCache(mesh,name);
			mMeshNameMap.put(name,resCache);
			mMeshPtrMap.put(mesh,resCache);
		}
		else{
			resCache.count++;
		}

		return (Mesh)resCache.resource;
	}

	public Mesh cacheMesh(String name,Mesh mesh){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching created mesh:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mMeshNameMap.get(name);
		if(resCache==null){
			resCache=new ResourceCache(mesh,name);
			mMeshNameMap.put(name,resCache);
			mMeshPtrMap.put(mesh,resCache);
		}
		else{
			resCache.count++;
		}

		return (Mesh)resCache.resource;
	}

	public Mesh cacheMesh(Mesh mesh){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching unnamed mesh");
		//#endif

		ResourceCache resCache=(ResourceCache)mMeshPtrMap.get(mesh);
		if(resCache==null){
			resCache=new ResourceCache(mesh);
			mMeshPtrMap.put(mesh,resCache);
		}
		else{
			resCache.count++;
		}

		return mesh;
	}

	public void uncacheMesh(String name){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Uncaching mesh:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mMeshNameMap.get(name);
		if(resCache!=null){
			resCache.count--;
			if(resCache.count<=0){
				mMeshNameMap.remove(name);
				mMeshPtrMap.remove((Mesh)resCache.resource);
			}
		}
	}

	public void uncacheMesh(Mesh mesh){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Uncaching mesh");
		//#endif

		ResourceCache resCache=(ResourceCache)mMeshPtrMap.get(mesh);
		if(resCache!=null){
			resCache.count--;
			if(resCache.count<=0){
				if(resCache.name!=null){
					mMeshNameMap.remove(resCache.name);
				}
				mMeshPtrMap.remove(mesh);
			}
		}
	}

	public Mesh getMesh(String name){
		if(name==null){
			return null;
		}

		ResourceCache resCache=(ResourceCache)mMeshNameMap.get(name);
		if(resCache==null){
			return null;
		}
		else{
			return (Mesh)resCache.resource;
		}
	}

	public Texture2D cacheTexture(String name) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching texture:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mTextureNameMap.get(name);
		if(resCache==null){
			Texture2D texture=new Texture2D(loadImage2DFromFile(name));
			resCache=new ResourceCache(texture,name);
			mTextureNameMap.put(name,resCache);
			mTexturePtrMap.put(texture,resCache);
		}
		else{
			resCache.count++;
		}

		return (Texture2D)resCache.resource;
	}

	public Texture2D cacheTexture(String name,Texture2D texture){
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

	public Texture2D cacheTexture(Texture2D texture){
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
				mTexturePtrMap.remove((Texture2D)resCache.resource);

				Enumeration e=mSpritePtrMap.elements();
				while(e.hasMoreElements()){
					Sprite sprite=(Sprite)(((ResourceCache)e.nextElement()).resource);
					if(sprite.image==((Texture2D)resCache.resource).getImage()){
						break;
					}
				}
				if(e.hasMoreElements()==false){
					mFileImageMap.remove(name);
				}
			}
		}
	}

	public void uncacheTexture(Texture2D texture){
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

				// Texture was cached by reference, so no need to remove it from the FileImageMap
			}
		}
	}

	public Texture2D getTexture(String name){
		if(name==null){
			return null;
		}

		ResourceCache resCache=(ResourceCache)mTextureNameMap.get(name);
		if(resCache==null){
			return null;
		}
		else{
			return (Texture2D)resCache.resource;
		}
	}

	public Sprite cacheSprite(String name,int widthFrames,int heightFrames,int startFrame,int numFrames,String file,int cacheHint) throws IOException{
		Sprite sprite=cacheSprite(name,widthFrames,heightFrames,startFrame,numFrames,file);

		Vector cache=(Vector)mSprite3DCache.get(name);
		if(cache==null){
			cache=new Vector();
			mSprite3DCache.put(name,cache);

			int i;
			for(i=0;i<cacheHint;++i){
				Sprite3D sprite3D=new Sprite3D(true,sprite.image,null);
				cache.addElement(sprite3D);
			}
		}

		return sprite;
	}

	public Sprite cacheSprite(String name,int widthFrames,int heightFrames,int startFrame,int numFrames,String file) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching sprite:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mSpriteNameMap.get(name);
		if(resCache==null){
			Image2D image=loadImage2DFromFile(file);
			Sprite sprite=new Sprite(widthFrames,heightFrames,startFrame,numFrames,image);
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

				Enumeration e=mTexturePtrMap.elements();
				while(e.hasMoreElements()){
					Texture2D texture=(Texture2D)((ResourceCache)e.nextElement()).resource;
					if(texture.getImage()==((Sprite)resCache.resource).image){
						break;
					}
				}
				if(e.hasMoreElements()==false){
					mFileImageMap.remove(name);
				}
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

				// Sprite was cached by reference, so no need to remove its image from the FileImageMap
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

	public Sprite3D checkoutSprite3D(String name){
		Vector cache=(Vector)mSprite3DCache.get(name);
		if(cache!=null && cache.size()>0){
			Sprite3D sprite=(Sprite3D)cache.elementAt(cache.size()-1);
			cache.removeElementAt(cache.size()-1);
			return sprite;
		}

		return null;
	}

	public void checkinSprite3D(String name,Sprite3D sprite){
		Vector cache=(Vector)mSprite3DCache.get(name);
		if(cache!=null){
			cache.addElement(sprite);
		}
	}

	protected Image2D loadImage2DFromFile(String file) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Loading image:"+file);
		//#endif

		Image2D image=(Image2D)mFileImageMap.get(file);
		if(image==null){
			//image=(Image2D)Loader.load(file+".png")[0]; // Nokia's jsr184 has problems with this
			image=new Image2D(Image2D.RGBA,Image.createImage(file+".png"));
			mFileImageMap.put(file,image);
		}
		return image;
	}

	public Graphics3D getGraphics3D(){
		return mGraphics3D;
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

	protected SceneManager mSceneManager;
	protected Graphics3D mGraphics3D;

	protected Hashtable mMeshNameMap=new Hashtable();
	protected Hashtable mMeshPtrMap=new Hashtable();
	protected Hashtable mTextureNameMap=new Hashtable();
	protected Hashtable mTexturePtrMap=new Hashtable();
	protected Hashtable mSpriteNameMap=new Hashtable();
	protected Hashtable mSpritePtrMap=new Hashtable();

	protected Hashtable mFileImageMap=new Hashtable();
	protected Hashtable mSprite3DCache=new Hashtable();
}

