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
import java.nio.*;
import java.util.*;
import javax.microedition.khronos.opengles.GL10;
import android.graphics.*;
import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.sg.*;
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
		Class.forName("javax.microedition.khronos.opengles.GL10");
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

	public void setInputStreamFactory(InputStreamFactory factory){
		mInputStreamFactory=factory;
	}

	public InputStream makeInputStream(String filename){
		if(mInputStreamFactory!=null){
			return mInputStreamFactory.makeInputStream(filename);
		}
		else{
			return getClass().getResourceAsStream(filename);
		}
	}

	public void contextActivate(GL10 gl){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Activating context");
		//#endif

		Enumeration e=null;
		for(e=mTexturePtrMap.elements();e.hasMoreElements();){
			Texture texture=(Texture)((ResourceCache)e.nextElement()).resource;
			texture.load(gl);
		}

		mTexturesToLoad.removeAllElements();

		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Activating context finished");
		//#endif
	}

	public void contextDeactivate(GL10 gl){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Deactivating context");
		//#endif

		Enumeration e=null;
		for(e=mTexturePtrMap.elements();e.hasMoreElements();){
			Texture texture=(Texture)((ResourceCache)e.nextElement()).resource;
			texture.unload(gl);
		}

		mTexturesToUnload.removeAllElements();

		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Deactivating context finished");
		//#endif
	}

	public void contextUpdate(GL10 gl){
		Enumeration e=null;
		for(e=mTexturesToUnload.elements();e.hasMoreElements();){
			Texture texture=(Texture)e.nextElement();
			texture.unload(gl);
		}

		mTexturesToUnload.removeAllElements();

		for(e=mTexturesToLoad.elements();e.hasMoreElements();){
			Texture texture=(Texture)e.nextElement();
			texture.load(gl);
		}

		mTexturesToLoad.removeAllElements();
	}

	public void destroy(){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Releasing cached resources");
		//#endif

		Enumeration e=null;
		for(e=mTextureNameMap.elements();e.hasMoreElements();){
			ResourceCache resCache=(ResourceCache)e.nextElement();
			mTexturesToUnload.addElement((Texture)resCache.resource);
		}

		mMeshNameMap.clear();
		mMeshPtrMap.clear();
		mTextureNameMap.clear();
		mTexturePtrMap.clear();
		mSpriteNameMap.clear();
		mSpritePtrMap.clear();
	}

	public Mesh cacheMesh(String name) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching mesh:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mMeshNameMap.get(name);
		if(resCache==null){
			Mesh mesh=loadMicroMeshFromFile(name+".mmsh");
			resCache=new ResourceCache(mesh,name);
			mMeshNameMap.put(name,resCache);
			mMeshPtrMap.put(mesh,resCache);
		}
		else{
			resCache.count++;
		}

		return (Mesh)resCache.resource;
	}

	public Mesh cacheMesh(String name,Mesh mesh) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching named mesh:"+name);
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

		return mesh;
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

	public Texture cacheTexture(String name) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching texture:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mTextureNameMap.get(name);
		if(resCache==null){
			Texture texture=loadTextureFromFile(name+".png");
			resCache=new ResourceCache(texture,name);
			mTextureNameMap.put(name,resCache);
			mTexturePtrMap.put(texture,resCache);

			mTexturesToLoad.addElement(texture);
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

			mTexturesToLoad.addElement(texture);
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

			mTexturesToLoad.addElement(texture);
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

				mTexturesToUnload.addElement((Texture)resCache.resource);
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

				mTexturesToUnload.addElement(texture);
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

	public Sprite cacheSprite(String name,int widthFrames,int heightFrames,int startFrame,int numFrames,String file) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching sprite:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mSpriteNameMap.get(name);
		if(resCache==null){
			Texture texture=cacheTexture(file);
			Sprite sprite=new Sprite(widthFrames,heightFrames,startFrame,numFrames,texture);
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

	// We have to have code paths to work with both bitmap.getConfig and bitmap.hasAlpha,
	//  since Google apparently isn't interested in implementing a lot of the functions of Android
	public static int getFormatFromBitmap(Bitmap bitmap){
		Bitmap.Config config=bitmap.getConfig();
		if(config==Bitmap.Config.ALPHA_8)
			return Texture.FORMAT_A_8;
		else if(config==Bitmap.Config.RGB_565)
			return Texture.FORMAT_RGB_5_6_5;
		else if(config==Bitmap.Config.ARGB_4444)
			return Texture.FORMAT_RGBA_4_4_4_4;
		else if(config==Bitmap.Config.ARGB_8888)
			return Texture.FORMAT_RGBA_8;
		else{
			if(bitmap.hasAlpha())
				return Texture.FORMAT_RGBA_8;
			else
				return Texture.FORMAT_RGB_8;
		}
	}

	public static java.nio.Buffer getBufferFromBitmap(Bitmap bitmap){
		java.nio.Buffer buffer=null;

		int format=getFormatFromBitmap(bitmap);
		int width=bitmap.getWidth();
		int height=bitmap.getHeight();
		int size=width*height;

		if((format&Texture.FORMAT_BIT_UINT_5_6_5)>0 || (format&Texture.FORMAT_BIT_UINT_5_5_5_1)>0 || (format&Texture.FORMAT_BIT_UINT_4_4_4_4)>0){
			buffer=ShortBuffer.allocate(size);
		}
		else{
			int count=0;
			if((format&Texture.FORMAT_BIT_L)>0 || (format&Texture.FORMAT_BIT_A)>0 || (format&Texture.FORMAT_BIT_DEPTH)>0){
				count=1;
			}
			else if((format&Texture.FORMAT_BIT_LA)>0){
				count=2;
			}
			else if((format&Texture.FORMAT_BIT_RGB)>0){
				count=3;
			}
			else if((format&Texture.FORMAT_BIT_RGBA)>0){
				count=4;
			}
			
			if((format&Texture.FORMAT_BIT_UINT_8)>0){
				buffer=ByteBuffer.allocate(size*count);
			}
			else if((format&Texture.FORMAT_BIT_FLOAT_32)>0){
				buffer=FloatBuffer.allocate(size*count);
			}
		}

		if(bitmap.getConfig()!=null){
			bitmap.copyPixelsToBuffer(buffer);

			if(format==Texture.FORMAT_RGBA_4_4_4_4){
				ShortBuffer shortBuffer=((ShortBuffer)buffer);
				short[] array=new short[width*height];
				shortBuffer.get(array);
				shortBuffer.rewind();
				int i;
				for(i=0;i<array.length;++i){
					array[i]=(short)(((array[i]&0xF000)>>12) | ((array[i]&0x000F)<<12) | ((array[i]&0x00F0)<<4) | ((array[i]&0x0F00)>>4));
				}
				shortBuffer.put(array);
				shortBuffer.rewind();
			}
			else if(format==Texture.FORMAT_RGBA_8){
				ByteBuffer byteBuffer=((ByteBuffer)buffer);
				IntBuffer intBuffer=byteBuffer.asIntBuffer();
				int[] array=new int[width*height];
				intBuffer.get(array);
				intBuffer.rewind();
				int i;
				for(i=0;i<array.length;++i){
					array[i]=((array[i]&0xFF000000)>>24) | ((array[i]&0x000000FF)<<24) | ((array[i]&0x0000FF00)<<8) | ((array[i]&0x00FF0000)>>8);
				}
				intBuffer.put(array);
				intBuffer.rewind();
			}
		}
		else{
			if(bitmap.hasAlpha()){
				ByteBuffer byteBuffer=((ByteBuffer)buffer);
				int[] pixels=new int[width*height];
				bitmap.getPixels(pixels,0,width,0,0,width,height);
				int i;
				for(i=0;i<width*height;++i){
					byteBuffer.put((byte)((pixels[i]&0x00FF0000)>>16));
					byteBuffer.put((byte)((pixels[i]&0x0000FF00)>>8));
					byteBuffer.put((byte)(pixels[i]&0x000000FF));
					byteBuffer.put((byte)((pixels[i]&0xFF000000)>>24));
				}
				byteBuffer.rewind();
			}
			else{
				ByteBuffer byteBuffer=((ByteBuffer)buffer);
				int[] pixels=new int[width*height];
				bitmap.getPixels(pixels,0,width,0,0,width,height);
				int i;
				for(i=0;i<width*height;++i){
					byteBuffer.put((byte)((pixels[i]&0x00FF0000)>>16));
					byteBuffer.put((byte)((pixels[i]&0x0000FF00)>>8));
					byteBuffer.put((byte)(pixels[i]&0x000000FF));
				}
				byteBuffer.rewind();
			}
		}

		return buffer;
	}

	protected Texture loadTextureFromFile(String file) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Loading texture:"+file);
		//#endif

		InputStream in=makeInputStream(file);
		Bitmap bitmap=BitmapFactory.decodeStream(in);
		
		if(bitmap==null){
			throw new IOException("Error creating bitmap");
		}
		
		Texture texture=new Texture(Texture.DIMENSION_2D,getFormatFromBitmap(bitmap),bitmap.getWidth(),bitmap.getHeight(),1);
		texture.setBuffer(getBufferFromBitmap(bitmap));
		return texture;
	}

	// Blocks for the mmsh format
	final static byte MESH_BLOCK=1;
	final static byte SKELETON_BLOCK=2;
	final static byte ANIMATION_BLOCK=3;

	// VertexTypes for the mmsh format
	final static int VT_POSITION=1<<0;
	final static int VT_NORMAL=1<<1;
	final static int VT_COLOR=1<<2;
	final static int VT_BONE=1<<3;
	final static int VT_TEXCOORD1=1<<8;

	final static String STRING_UNKNOWN="unknown";

	protected Mesh loadMicroMeshFromFile(String file) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Loading mesh:"+file);
		//#endif

		DataInputStream in=new DataInputStream(makeInputStream(file));

		byte[] signature=new byte[4];
		in.read(signature,0,4);
		if(signature[0]!='T' || signature[1]!='M' || signature[2]!='M' || signature[3]!='H'){
			throw new RuntimeException("Not of TMMH format");
		}

		byte version=0;
		version=in.readByte();
		if(version!=3){
			throw new RuntimeException("Not TMMH version 3");
		}

		Mesh mesh=new Mesh();

		int i,j;

		byte i8=0;
		byte ui8=0;
		short i16=0;
		short ui16=0;

		while(in.available()>0){
			byte block=in.readByte();
			if(block==MESH_BLOCK){
				mesh.worldScale=in.readInt();

				int numVertexes=in.readShort();
				int vertexType=in.readShort();

				VertexFormat vertexFormat=new VertexFormat();
				int positionFormatOffset=0;
				if((vertexType&VT_POSITION)>0){
					positionFormatOffset=vertexFormat.addVertexElement(new VertexElement(VertexElement.TYPE_POSITION,VertexElement.FORMAT_BIT_FIXED_32|VertexElement.FORMAT_BIT_COUNT_3)).formatOffset;
				}
				int normalFormatOffset=0;
				if((vertexType&VT_NORMAL)>0){
					normalFormatOffset=vertexFormat.addVertexElement(new VertexElement(VertexElement.TYPE_NORMAL,VertexElement.FORMAT_BIT_FIXED_32|VertexElement.FORMAT_BIT_COUNT_3)).formatOffset;
				}
				int colorFormatOffset=0;
				if((vertexType&VT_COLOR)>0){
					colorFormatOffset=vertexFormat.addVertexElement(new VertexElement(VertexElement.TYPE_COLOR,VertexElement.FORMAT_COLOR_RGBA)).formatOffset;
				}
				int texCoordFormatOffset=0;
				if((vertexType&VT_TEXCOORD1)>0){
					texCoordFormatOffset=vertexFormat.addVertexElement(new VertexElement(VertexElement.TYPE_TEX_COORD,VertexElement.FORMAT_BIT_FIXED_32|VertexElement.FORMAT_BIT_COUNT_2)).formatOffset;
				}
				int formatSize=vertexFormat.getVertexFormatSize();

				if((vertexType&VT_BONE)>0){
					mesh.boneControllingVertexes=new byte[numVertexes];
				}

				Buffer vertexBuffer=new Buffer((vertexType&VT_BONE)>0?Buffer.UT_STATIC:Buffer.UT_DYNAMIC,Buffer.AT_WRITE_ONLY,vertexFormat,numVertexes);
				java.nio.IntBuffer data=((java.nio.IntBuffer)vertexBuffer.lock(Buffer.LT_WRITE_ONLY));

				if((vertexType&VT_POSITION)>0){
					byte bytes=0;
					bytes=in.readByte();

					int[] bias=new int[3];
					bias[0]=in.readInt();
					bias[1]=in.readInt();
					bias[2]=in.readInt();

					int scale=in.readInt();

					if(bytes==1){
						for(i=0;i<numVertexes;++i){
							i8=in.readByte();
							data.put(formatSize*i+positionFormatOffset+0,Math.mul(Math.intToFixed(i8),scale)+bias[0]);
							i8=in.readByte();
							data.put(formatSize*i+positionFormatOffset+1,Math.mul(Math.intToFixed(i8),scale)+bias[1]);
							i8=in.readByte();
							data.put(formatSize*i+positionFormatOffset+2,Math.mul(Math.intToFixed(i8),scale)+bias[2]);
						}
					}
					else if(bytes==2){
						for(i=0;i<numVertexes;++i){
							i16=in.readShort();
							data.put(formatSize*i+positionFormatOffset+0,Math.mul(Math.intToFixed(i16),scale)+bias[0]);
							i16=in.readShort();
							data.put(formatSize*i+positionFormatOffset+1,Math.mul(Math.intToFixed(i16),scale)+bias[1]);
							i16=in.readShort();
							data.put(formatSize*i+positionFormatOffset+2,Math.mul(Math.intToFixed(i16),scale)+bias[2]);
						}
					}
					else{
						throw new RuntimeException("Invalid byte count");
					}
				}

				if((vertexType&VT_NORMAL)>0){
					Vector3 normal=new Vector3();

					for(i=0;i<numVertexes;i++){
						i8=in.readByte();
						normal.x=((int)i8)<<9;
						i8=in.readByte();
						normal.y=((int)i8)<<9;
						i8=in.readByte();
						normal.z=((int)i8)<<9;

						Math.normalizeCarefully(normal,0);

						data.put(formatSize*i+normalFormatOffset+0,normal.x);
						data.put(formatSize*i+normalFormatOffset+1,normal.y);
						data.put(formatSize*i+normalFormatOffset+2,normal.z);
					}
				}

				if((vertexType&VT_COLOR)>0){
					for(i=0;i<numVertexes;i++){
						int r=(int)in.readByte() & 0xFF;
						int g=(int)in.readByte() & 0xFF;
						int b=(int)in.readByte() & 0xFF;

						data.put(formatSize*i+colorFormatOffset+0,0xFF000000 | (((int)b)<<16) | (((int)g)<<8) | ((int)r)<<0);
					}
				}

				if((vertexType&VT_BONE)>0){
					for(i=0;i<numVertexes;i++){
						mesh.boneControllingVertexes[i]=in.readByte();
					}
				}

				if((vertexType&VT_TEXCOORD1)>0){
					byte bytes=in.readByte();

					int[] bias=new int[2];
					bias[0]=in.readInt();
					bias[1]=in.readInt();

					int scale=in.readInt();

					if(bytes==1){
						for(i=0;i<numVertexes;++i){
							i8=in.readByte();
							data.put(formatSize*i+texCoordFormatOffset+0,Math.mul(Math.intToFixed(i8),scale)+bias[0]);
							i8=in.readByte();
							data.put(formatSize*i+texCoordFormatOffset+1,Math.mul(Math.intToFixed(i8),scale)+bias[1]);
						}
					}
					else if(bytes==2){
						for(i=0;i<numVertexes;++i){
							i16=in.readByte();
							data.put(formatSize*i+texCoordFormatOffset+0,Math.mul(Math.intToFixed(i16),scale)+bias[0]);
							i16=in.readByte();
							data.put(formatSize*i+texCoordFormatOffset+1,Math.mul(Math.intToFixed(i16),scale)+bias[1]);
						}
					}
					else{
						throw new RuntimeException("Invalid byte count");
					}
				}

				data.rewind();
				vertexBuffer.unlock();
				
				mesh.staticVertexData=new VertexData(vertexBuffer);

				byte numSubMeshes=in.readByte();
				mesh.subMeshes=new Mesh.SubMesh[numSubMeshes];

				for(i=0;i<numSubMeshes;++i){
					Mesh.SubMesh sub=new Mesh.SubMesh();
					mesh.subMeshes[i]=sub;

					// Material
					{
						Material material=new Material();
						sub.material=material;

						material.setFaceCulling(in.readByte());

						boolean hasTexture=in.readByte()>0;
						if(hasTexture){
							material.setTexture(null,STRING_UNKNOWN);
						}

						boolean lighting=in.readByte()>0;
						material.setLighting(lighting);
						if(lighting){
							LightEffect lightEffect=new LightEffect();

							int color=in.readInt();
							Entity.uint32ColorToFixedColor(color,lightEffect.ambient);
							color=in.readInt();
							Entity.uint32ColorToFixedColor(color,lightEffect.diffuse);
							color=in.readInt();
							Entity.uint32ColorToFixedColor(color,lightEffect.specular);
							lightEffect.shininess=in.readInt();
							color=in.readInt();
							Entity.uint32ColorToFixedColor(color,lightEffect.emissive);

							material.setLightEffect(lightEffect);
						}
					}

					short numIndexes=in.readShort();
					short[] indexes=new short[numIndexes];

					for(j=0;j<numIndexes;++j){
						indexes[j]=in.readShort();
					}
					
					short numStrips=in.readShort();

					short[] stripLengths=new short[numStrips];

					int newNumIndexes=0;
					for(j=0;j<numStrips;++j){
						stripLengths[j]=in.readShort();
						newNumIndexes+=(stripLengths[j]-2)*3;
					}

					int indexBufferSize=newNumIndexes;
					Buffer indexBuffer=new Buffer(Buffer.UT_STATIC,Buffer.AT_WRITE_ONLY,Buffer.IDT_UINT_16,newNumIndexes);
					{
						java.nio.ShortBuffer indexData=((java.nio.ShortBuffer)indexBuffer.lock(Buffer.LT_WRITE_ONLY));
						int ipo=0;
						int ipn=0;
						for(j=0;j<numStrips;++j){
							int k;
							for(k=0;k<stripLengths[j]-2;++k){
								if(k%2==0){
									indexData.put(ipn++,indexes[ipo+k+0]);
									indexData.put(ipn++,indexes[ipo+k+1]);
									indexData.put(ipn++,indexes[ipo+k+2]);
								}
								else{
									indexData.put(ipn++,indexes[ipo+k+2]);
									indexData.put(ipn++,indexes[ipo+k+1]);
									indexData.put(ipn++,indexes[ipo+k+0]);
								}
							}
							ipo+=stripLengths[j];
						}
						indexData.rewind();
						indexBuffer.unlock();
					}
					sub.indexData=new IndexData(IndexData.PRIMITIVE_TRIS,indexBuffer,0,newNumIndexes);
				}
			}
			else if(block==SKELETON_BLOCK){
				byte numBones=in.readByte();

				mesh.bones=new Mesh.Bone[numBones];

				byte translateBytes=in.readByte();

				int[] translateBias=new int[3];
				translateBias[0]=in.readInt();
				translateBias[1]=in.readInt();
				translateBias[2]=in.readInt();

				int translateScale=in.readInt();

				for(i=0;i<numBones;++i){
					Mesh.Bone b=new Mesh.Bone();
					mesh.bones[i]=b;
					b.index=(byte)i;

					if(translateBytes==1){
						i8=in.readByte();
						b.translate.x=Math.mul(Math.intToFixed(i8),translateScale)+translateBias[0];
						i8=in.readByte();
						b.translate.y=Math.mul(Math.intToFixed(i8),translateScale)+translateBias[1];
						i8=in.readByte();
						b.translate.z=Math.mul(Math.intToFixed(i8),translateScale)+translateBias[2];
					}
					else{
						i16=in.readShort();
						b.translate.x=Math.mul(Math.intToFixed(i16),translateScale)+translateBias[0];
						i16=in.readShort();
						b.translate.y=Math.mul(Math.intToFixed(i16),translateScale)+translateBias[1];
						i16=in.readShort();
						b.translate.z=Math.mul(Math.intToFixed(i16),translateScale)+translateBias[2];
					}

					i8=in.readByte();
					b.rotate.x=((int)i8)<<9;
					i8=in.readByte();
					b.rotate.y=((int)i8)<<9;
					i8=in.readByte();
					b.rotate.z=((int)i8)<<9;
					i8=in.readByte();
					b.rotate.w=((int)i8)<<9;
					Math.normalizeCarefully(b.rotate,0);

					if(translateBytes==1){
						i8=in.readByte();
						b.worldToBoneTranslate.x=Math.mul(Math.intToFixed(i8),translateScale)+translateBias[0];
						i8=in.readByte();
						b.worldToBoneTranslate.y=Math.mul(Math.intToFixed(i8),translateScale)+translateBias[1];
						i8=in.readByte();
						b.worldToBoneTranslate.z=Math.mul(Math.intToFixed(i8),translateScale)+translateBias[2];
					}
					else{
						i16=in.readShort();
						b.worldToBoneTranslate.x=Math.mul(Math.intToFixed(i16),translateScale)+translateBias[0];
						i16=in.readShort();
						b.worldToBoneTranslate.y=Math.mul(Math.intToFixed(i16),translateScale)+translateBias[1];
						i16=in.readShort();
						b.worldToBoneTranslate.z=Math.mul(Math.intToFixed(i16),translateScale)+translateBias[2];
					}

					i8=in.readByte();
					b.worldToBoneRotate.x=((int)i8)<<9;
					i8=in.readByte();
					b.worldToBoneRotate.y=((int)i8)<<9;
					i8=in.readByte();
					b.worldToBoneRotate.z=((int)i8)<<9;
					i8=in.readByte();
					b.worldToBoneRotate.w=((int)i8)<<9;
					Math.normalizeCarefully(b.worldToBoneRotate,0);

					b.parent=in.readByte();

					if(b.parent!=((byte)-1)){ // Force this to -1
						byte[] bones=null;
						if(mesh.bones[b.parent].children!=null){
							bones=new byte[mesh.bones[b.parent].children.length+1];
							System.arraycopy(mesh.bones[b.parent].children,0,bones,0,mesh.bones[b.parent].children.length);
						}
						else{
							bones=new byte[1];
						}
						mesh.bones[b.parent].children=bones;
						mesh.bones[b.parent].children[mesh.bones[b.parent].children.length-1]=(byte)i;
					}
				}
			}
			else if(block==ANIMATION_BLOCK){
				Animation animation=new Animation();

				animation.length=in.readInt();

				byte translateBytes=in.readByte();

				int[] translateBias=new int[3];
				translateBias[0]=in.readInt();
				translateBias[1]=in.readInt();
				translateBias[2]=in.readInt();

				int translateScale=in.readInt();

				byte numTracks=in.readByte();
				animation.tracks=new Animation.Track[numTracks];

				for(i=0;i<numTracks;++i){
					Animation.Track track=new Animation.Track(animation);
					animation.tracks[i]=track;

					track.bone=in.readByte();

					short numKeyFrames=in.readShort();
					track.keyFrames=new Animation.KeyFrame[numKeyFrames];

					for(j=0;j<numKeyFrames;++j){
						Animation.KeyFrame keyFrame=new Animation.KeyFrame();
						track.keyFrames[j]=keyFrame;

						keyFrame.time=in.readInt();

						if(translateBytes==1){
							i8=in.readByte();
							keyFrame.translate.x=Math.mul(Math.intToFixed(i8),translateScale)+translateBias[0];
							i8=in.readByte();
							keyFrame.translate.y=Math.mul(Math.intToFixed(i8),translateScale)+translateBias[1];
							i8=in.readByte();
							keyFrame.translate.z=Math.mul(Math.intToFixed(i8),translateScale)+translateBias[2];
						}
						else{
							i16=in.readShort();
							keyFrame.translate.x=Math.mul(Math.intToFixed(i16),translateScale)+translateBias[0];
							i16=in.readShort();
							keyFrame.translate.y=Math.mul(Math.intToFixed(i16),translateScale)+translateBias[1];
							i16=in.readShort();
							keyFrame.translate.z=Math.mul(Math.intToFixed(i16),translateScale)+translateBias[2];
						}

						i8=in.readByte();
						keyFrame.rotate.x=((int)i8)<<9;
						i8=in.readByte();
						keyFrame.rotate.y=((int)i8)<<9;
						i8=in.readByte();
						keyFrame.rotate.z=((int)i8)<<9;
						i8=in.readByte();
						keyFrame.rotate.w=((int)i8)<<9;
						Math.normalizeCarefully(keyFrame.rotate,0);
					}
				}

				animation.compile();
				Animation[] animations=new Animation[mesh.animations.length+1];
				System.arraycopy(mesh.animations,0,animations,0,mesh.animations.length);
				mesh.animations=animations;
				mesh.animations[mesh.animations.length-1]=animation;
			}
			else{
				throw new RuntimeException("Unknown block type in mesh file");
			}
		}

		return mesh;
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
	protected InputStreamFactory mInputStreamFactory=null;
	public int mWidth=0;
	public int mHeight=0;

	protected Hashtable mMeshNameMap=new Hashtable();
	protected Hashtable mMeshPtrMap=new Hashtable();
	protected Hashtable mTextureNameMap=new Hashtable();
	protected Hashtable mTexturePtrMap=new Hashtable();
	protected Hashtable mSpriteNameMap=new Hashtable();
	protected Hashtable mSpritePtrMap=new Hashtable();

	protected Vector mTexturesToLoad=new Vector();
	protected Vector mTexturesToUnload=new Vector();
}

