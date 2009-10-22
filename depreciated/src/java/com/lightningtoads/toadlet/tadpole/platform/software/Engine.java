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
import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.sg.*;
//#ifdef TOADLET_PLATFORM_BLACKBERRY
	import net.rim.device.api.system.Bitmap;
//#else
	import javax.microedition.lcdui.Image;
//#endif
//#ifdef TOADLET_DEBUG
	import com.lightningtoads.toadlet.egg.*;
//#endif

public final class Engine{
	//#include src/java/com/lightningtoads/toadlet/Versions.java.inc
	//#endinclude

	public Engine(int width,int height){
		mWidth=width;
		mHeight=height;

		mRenderer=new Renderer(mWidth,mHeight);
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

	public void setRenderShiftBits(int b){
		mRenderShiftBits=b;
	}

	public int getRenderShiftBits(){
		return mRenderShiftBits;
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
	}

	public Mesh cacheMesh(String name) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching mesh:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mMeshNameMap.get(name);
		if(resCache==null){
			Mesh mesh=loadMicroMeshFromFile(name);
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

	// A Texture is represented by a int[2][], [0][0] = width, [0][1] = height, [1][] = data
	public int[][] cacheTexture(String name) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching texture:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mTextureNameMap.get(name);
		if(resCache==null){
			int[][] texture=new int[2][];
			texture[0]=new int[2];
			// All bitmaps are stuck into a 256x256 block of memory, for faster indexing
			//#ifdef TOADLET_PLATFORM_BLACKBERRY
				String cleanName;
				if(name.startsWith("/")){
					cleanName=name.substring(1);
				}
				else{
					cleanName=name;
				}
				Bitmap bitmap=Bitmap.getBitmapResource(cleanName+".png");
				texture[0][0]=bitmap.getWidth();
				texture[0][1]=bitmap.getHeight();
				if(texture[0][0]>256 || texture[0][1]>256){
					throw new RuntimeException("Image size > 256");
				}
				texture[1]=new int[256*256];
				bitmap.getARGB(texture[1],0,256,0,0,texture[0][0],texture[0][1]);
			//#else
				Image image=Image.createImage(name+".png");
				texture[0][0]=image.getWidth();
				texture[0][1]=image.getHeight();
				if(texture[0][0]>256 || texture[0][1]>256){
					throw new RuntimeException("Image size > 256");
				}
				texture[1]=new int[256*256];
				image.getRGB(texture[1],0,256,0,0,texture[0][0],texture[0][1]);
			//#endif
			resCache=new ResourceCache(texture,name);
			mTextureNameMap.put(name,resCache);
			mTexturePtrMap.put(texture,resCache);
		}
		else{
			resCache.count++;
		}

		return (int[][])resCache.resource;
	}

	public int[][] cacheTexture(String name,int[][] texture){
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

	public int[][] cacheTexture(int[][] texture){
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
				mTexturePtrMap.remove((int[][])resCache.resource);
			}
		}
	}

	public void uncacheTexture(int[][] texture){
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

	public int[][] getTexture(String name){
		if(name==null){
			return null;
		}

		ResourceCache resCache=(ResourceCache)mTextureNameMap.get(name);
		if(resCache==null){
			return null;
		}
		else{
			return (int[][])resCache.resource;
		}
	}

	public Sprite cacheSprite(String name,int widthFrames,int heightFrames,int startFrame,int numFrames,String file) throws IOException{
		return cacheSprite(name,widthFrames,heightFrames,startFrame,numFrames,file,0);
	}

	public Sprite cacheSprite(String name,int widthFrames,int heightFrames,int startFrame,int numFrames,String file,int distanceCount) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Caching sprite:"+name);
		//#endif

		ResourceCache resCache=(ResourceCache)mSpriteNameMap.get(name);
		if(resCache==null){
			//#ifdef TOADLET_PLATFORM_BLACKBERRY
				String cleanName;
				if(file.startsWith("/")){
					cleanName=file.substring(1);
				}
				else{
					cleanName=file;
				}
				Bitmap[] images=null;
			//#else
				Image[] images=null;
			//#endif

			if(distanceCount>0){
				//#ifdef TOADLET_PLATFORM_BLACKBERRY
					images=new Bitmap[distanceCount];
				//#else
					images=new Image[distanceCount];
				//#endif

				int i;
				for(i=0;i<distanceCount;++i){
					//#ifdef TOADLET_DEBUG
						Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"\tCaching sprite frame:"+(i+1));
					//#endif

					//#ifdef TOADLET_PLATFORM_BLACKBERRY
						images[i]=Bitmap.getBitmapResource(cleanName+"_"+i+".png");
					//#else
						images[i]=Image.createImage(file+"_"+i+".png");
					//#endif
				}
			}
			else{
				//#ifdef TOADLET_PLATFORM_BLACKBERRY
					images=new Bitmap[1];
				//#else
					images=new Image[1];
				//#endif

				//#ifdef TOADLET_PLATFORM_BLACKBERRY
					images[0]=Bitmap.getBitmapResource(cleanName+".png");
				//#else
					images[0]=Image.createImage(file+".png");
				//#endif
			}
			
			Sprite sprite=new Sprite(widthFrames,heightFrames,startFrame,numFrames,images);
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

	protected Mesh loadMicroMeshFromFile(String file) throws IOException{
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Loading mesh:"+file);
		//#endif

		DataInputStream in=new DataInputStream(getClass().getResourceAsStream(file+".mmsh"));

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

		int bits=mRenderShiftBits;

		Mesh mesh=new Mesh();

		int i,j;

		byte i8=0;
		short i16=0;

		while(in.available()>0){
			byte block=in.readByte();
			if(block==MESH_BLOCK){
				mesh.worldScale=in.readInt();

				int numVertexes=in.readShort();
				int vertexType=in.readShort();

				mesh.vertexData=new VertexData();
				if((vertexType&VT_POSITION)>0){
					mesh.vertexData.positions=new int[numVertexes*3];
				}
				if((vertexType&VT_NORMAL)>0){
					mesh.vertexData.normals=new int[numVertexes*3];
				}
				if((vertexType&VT_COLOR)>0){
					mesh.vertexData.colors=new int[numVertexes];
				}
				if((vertexType&VT_TEXCOORD1)>0){
					mesh.vertexData.texCoords=new int[numVertexes*2];
				}

				if((vertexType&VT_BONE)>0){
					mesh.boneControllingVertexes=new byte[numVertexes];
				}

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
							mesh.vertexData.positions[i*3+0]=(Math.mul(Math.intToFixed(i8),scale)+bias[0])>>bits;
							i8=in.readByte();
							mesh.vertexData.positions[i*3+1]=(Math.mul(Math.intToFixed(i8),scale)+bias[1])>>bits;
							i8=in.readByte();
							mesh.vertexData.positions[i*3+2]=(Math.mul(Math.intToFixed(i8),scale)+bias[2])>>bits;
						}
					}
					else if(bytes==2){
						for(i=0;i<numVertexes;++i){
							i16=in.readShort();
							mesh.vertexData.positions[i*3+0]=(Math.mul(Math.intToFixed(i16),scale)+bias[0])>>bits;
							i16=in.readShort();
							mesh.vertexData.positions[i*3+1]=(Math.mul(Math.intToFixed(i16),scale)+bias[1])>>bits;
							i16=in.readShort();
							mesh.vertexData.positions[i*3+2]=(Math.mul(Math.intToFixed(i16),scale)+bias[2])>>bits;
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

						mesh.vertexData.normals[i*3+0]=normal.x;
						mesh.vertexData.normals[i*3+1]=normal.y;
						mesh.vertexData.normals[i*3+2]=normal.z;
					}
				}

				if((vertexType&VT_COLOR)>0){
					for(i=0;i<numVertexes;i++){
						byte r=in.readByte();
						byte g=in.readByte();
						byte b=in.readByte();

						mesh.vertexData.colors[i]=0xFF000000 | (((int)b)<<16) | (((int)g)<<8) | (((int)r)<<0);
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
							mesh.vertexData.texCoords[i*2+0]=(Math.mul(Math.intToFixed(i8),scale)+bias[0]);
							i8=in.readByte();
							mesh.vertexData.texCoords[i*2+1]=(Math.mul(Math.intToFixed(i8),scale)+bias[1]);
						}
					}
					else if(bytes==2){
						for(i=0;i<numVertexes;++i){
							i16=in.readShort();
							mesh.vertexData.texCoords[i*2+0]=(Math.mul(Math.intToFixed(i16),scale)+bias[0]);
							i16=in.readShort();
							mesh.vertexData.texCoords[i*2+1]=(Math.mul(Math.intToFixed(i16),scale)+bias[1]);
						}
					}
					else{
						throw new RuntimeException("Invalid byte count");
					}
				}

				byte numSubMeshes=in.readByte();
				mesh.subMeshes=new Mesh.SubMesh[numSubMeshes];

				short[][] triIndexData=new short[numSubMeshes][];
				int totalIndexes=0;

				int textureCount=0;
				for(i=0;i<numSubMeshes;++i){
					Mesh.SubMesh sub=new Mesh.SubMesh();
					mesh.subMeshes[i]=sub;

					// Material
					{
						sub.faceCulling=in.readByte();
						sub.hasTexture=in.readByte()>0;
						if(sub.hasTexture){
							sub.textureIndex=textureCount++;
						}
						sub.lightingEnabled=in.readByte()>0;
						if(sub.lightingEnabled){
							int ambientColor=in.readInt();
							int diffuseColor=in.readInt();
							sub.color=diffuseColor;
							int specularColor=in.readInt();
							int shininess=in.readInt();
							int emissiveColor=in.readInt();
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
					triIndexData[i]=new short[indexBufferSize];
					totalIndexes+=indexBufferSize;
					{
						int ipo=0;
						int ipn=0;
						for(j=0;j<numStrips;++j){
							int k;
							for(k=0;k<stripLengths[j]-2;++k){
								if(k%2==0){
									triIndexData[i][ipn++]=indexes[ipo+k+0];
									triIndexData[i][ipn++]=indexes[ipo+k+1];
									triIndexData[i][ipn++]=indexes[ipo+k+2];
								}
								else{
									triIndexData[i][ipn++]=indexes[ipo+k+2];
									triIndexData[i][ipn++]=indexes[ipo+k+1];
									triIndexData[i][ipn++]=indexes[ipo+k+0];
								}
							}
							ipo+=stripLengths[j];
						}
					}
				}

				mesh.triIndexData=new short[totalIndexes];
				mesh.triSubMesh=new byte[totalIndexes/3];

				totalIndexes=0;
				int totalTris=0;
				for(i=0;i<numSubMeshes;++i){
					for(j=0;j<triIndexData[i].length/3;++j){
						mesh.triIndexData[totalIndexes++]=triIndexData[i][j*3];
						mesh.triIndexData[totalIndexes++]=triIndexData[i][j*3+1];
						mesh.triIndexData[totalIndexes++]=triIndexData[i][j*3+2];
						mesh.triSubMesh[totalTris++]=(byte)i;
					}
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
						b.translate.x=(Math.mul(Math.intToFixed(i8),translateScale)+translateBias[0])>>bits;
						i8=in.readByte();
						b.translate.y=(Math.mul(Math.intToFixed(i8),translateScale)+translateBias[1])>>bits;
						i8=in.readByte();
						b.translate.z=(Math.mul(Math.intToFixed(i8),translateScale)+translateBias[2])>>bits;
					}
					else{
						i16=in.readShort();
						b.translate.x=(Math.mul(Math.intToFixed(i16),translateScale)+translateBias[0])>>bits;
						i16=in.readShort();
						b.translate.y=(Math.mul(Math.intToFixed(i16),translateScale)+translateBias[1])>>bits;
						i16=in.readShort();
						b.translate.z=(Math.mul(Math.intToFixed(i16),translateScale)+translateBias[2])>>bits;
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
						b.worldToBoneTranslate.x=(Math.mul(Math.intToFixed(i8),translateScale)+translateBias[0])>>bits;
						i8=in.readByte();
						b.worldToBoneTranslate.y=(Math.mul(Math.intToFixed(i8),translateScale)+translateBias[1])>>bits;
						i8=in.readByte();
						b.worldToBoneTranslate.z=(Math.mul(Math.intToFixed(i8),translateScale)+translateBias[2])>>bits;
					}
					else{
						i16=in.readShort();
						b.worldToBoneTranslate.x=(Math.mul(Math.intToFixed(i16),translateScale)+translateBias[0])>>bits;
						i16=in.readShort();
						b.worldToBoneTranslate.y=(Math.mul(Math.intToFixed(i16),translateScale)+translateBias[1])>>bits;
						i16=in.readShort();
						b.worldToBoneTranslate.z=(Math.mul(Math.intToFixed(i16),translateScale)+translateBias[2])>>bits;
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
							keyFrame.translate.x=(Math.mul(Math.intToFixed(i8),translateScale)+translateBias[0])>>bits;
							i8=in.readByte();
							keyFrame.translate.y=(Math.mul(Math.intToFixed(i8),translateScale)+translateBias[1])>>bits;
							i8=in.readByte();
							keyFrame.translate.z=(Math.mul(Math.intToFixed(i8),translateScale)+translateBias[2])>>bits;
						}
						else{
							i16=in.readShort();
							keyFrame.translate.x=(Math.mul(Math.intToFixed(i16),translateScale)+translateBias[0])>>bits;
							i16=in.readShort();
							keyFrame.translate.y=(Math.mul(Math.intToFixed(i16),translateScale)+translateBias[1])>>bits;
							i16=in.readShort();
							keyFrame.translate.z=(Math.mul(Math.intToFixed(i16),translateScale)+translateBias[2])>>bits;
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
	public int mRenderShiftBits=0;
	protected Renderer mRenderer;

	protected Hashtable mMeshNameMap=new Hashtable();
	protected Hashtable mMeshPtrMap=new Hashtable();
	protected Hashtable mTextureNameMap=new Hashtable();
	protected Hashtable mTexturePtrMap=new Hashtable();
	protected Hashtable mSpriteNameMap=new Hashtable();
	protected Hashtable mSpritePtrMap=new Hashtable();

	protected Vector mTexturesToLoad=new Vector();
	protected Vector mTexturesToUnload=new Vector();
}

