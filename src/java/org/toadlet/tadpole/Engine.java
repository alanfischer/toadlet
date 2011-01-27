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

package org.toadlet.tadpole;

#include <org/toadlet/tadpole/Types.h>

import org.toadlet.egg.*;
import org.toadlet.egg.Error;
import org.toadlet.egg.io.*;
import org.toadlet.peeper.*;
import org.toadlet.tadpole.entity.*;
import org.toadlet.tadpole.handler.*;
import java.io.*;
import java.util.*;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class Engine implements InputStreamFactory{
	public Engine(){
		Logger.debug(Categories.TOADLET_TADPOLE,
			"creating Engine");

		mTextureManager=new ContextResourceManager<Texture.Peer>(new TextureSemantics(),this);

		// Make a guess at what the ideal format is.
		#if defined(TOADLET_FIXED_POINT) && (defined(TOADLET_PLATFORM_WINCE) || defined(TOADLET_PLATFORM_IPHONE) || defined(TOADLET_PLATFORM_ANDROID))
			mIdealFormatBit=VertexElement.Format.BIT_FIXED_32;
		#else
			mIdealFormatBit=VertexElement.Format.BIT_FLOAT_32;
		#endif
		updateVertexFormats();

		mBufferManager=new ContextResourceManager<Buffer.Peer>(new BufferSemantics(),this);

		mMaterialManager=new ResourceManager(this);

		mFontManager=new ResourceManager(this);

		mMeshManager=new ResourceManager(this);

//		mAudioBufferManager=new ResourceManager(this);


		Logger.debug(Categories.TOADLET_TADPOLE,
			"Engine: adding all handlers");

		// Texture handlers
		mTextureManager.setDefaultHandler(new TextureHandler());
			
		// AudioBuffer handlers
//		mAudioBufferHandler=new AudioBufferHandler(mAudioPlayer);
//		mAudioBufferManager.setDefaultHandler(mAudioBufferHandler);
	}

	public void setScene(Scene scene){
		if(scene!=null && scene.destroyed()){
			Error.unknown("using destroyed scene");
			return;
		}
		
		mScene=scene;
	}

	public Scene getScene(){return mScene;}

	public void setInputStreamFactory(InputStreamFactory inputStreamFactory){
		mInputStreamFactory=inputStreamFactory;
	}

	public InputStreamFactory getInputStreamFactory(){return mInputStreamFactory;}

	public void setRenderer(Renderer renderer){
		if(renderer!=null){
			CapabilitySet capabilitySet=renderer.getCapabilitySet();
			Logger.alert(Categories.TOADLET_TADPOLE,
				"Renderer Capabilities:");
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"maxLights:"+capabilitySet.maxLights);
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"tmaxTextureStages:"+capabilitySet.maxTextureStages);
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"maxTextureSize:"+capabilitySet.maxTextureSize);
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"hardwareVertexBuffers:"+capabilitySet.hardwareVertexBuffers);
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"hardwareIndexBuffers:"+capabilitySet.hardwareIndexBuffers);
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"vertexShaders:"+capabilitySet.vertexShaders);
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"maxVertexShaderLocalParameters:"+capabilitySet.maxVertexShaderLocalParameters);
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"fragmentShaders:"+capabilitySet.fragmentShaders);
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"maxFragmentShaderLocalParameters:"+capabilitySet.maxFragmentShaderLocalParameters);
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"renderToTexture:"+capabilitySet.renderToTexture);
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"renderToDepthTexture:"+capabilitySet.renderToDepthTexture);
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"renderToTextureNonPowerOf2Restricted:"+capabilitySet.renderToTextureNonPowerOf2Restricted);
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"textureDot3:"+capabilitySet.textureDot3);
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"textureNonPowerOf2Restricted:"+capabilitySet.textureNonPowerOf2Restricted);
			Logger.alert(Categories.TOADLET_TADPOLE,
				(char)9+"textureNonPowerOf2:"+capabilitySet.textureNonPowerOf2);
		}

		if(renderer!=mRenderer && mRenderer!=null){
			contextDeactivate(mRenderer);
		}
		if(renderer!=mRenderer && renderer!=null){
			contextActivate(renderer);

			mIdealFormatBit=renderer.getCapabilitySet().idealFormatBit;
			updateVertexFormats();
		}

		mRenderer=renderer;
	}
	
	public Renderer getRenderer(){return mRenderer;}

	public void updateVertexFormats(){
		int formatBit=mIdealFormatBit;

		VertexElement position=new VertexElement(VertexElement.Type.POSITION,formatBit|VertexElement.Format.BIT_COUNT_3);
		VertexElement normal=new VertexElement(VertexElement.Type.NORMAL,formatBit|VertexElement.Format.BIT_COUNT_3);
		VertexElement color=new VertexElement(VertexElement.Type.COLOR,VertexElement.Format.COLOR_RGBA);
		VertexElement texCoord=new VertexElement(VertexElement.Type.TEX_COORD,formatBit|VertexElement.Format.BIT_COUNT_2);

		VertexFormat format;

		format=new VertexFormat(1);
		format.addVertexElement(position);
		mVertexFormats.POSITION=format;

		format=new VertexFormat(2);
		format.addVertexElement(position);
		format.addVertexElement(normal);
		mVertexFormats.POSITION_NORMAL=format;

		format=new VertexFormat(2);
		format.addVertexElement(position);
		format.addVertexElement(color);
		mVertexFormats.POSITION_COLOR=format;

		format=new VertexFormat(2);
		format.addVertexElement(position);
		format.addVertexElement(texCoord);
		mVertexFormats.POSITION_TEX_COORD=format;

		format=new VertexFormat(3);
		format.addVertexElement(position);
		format.addVertexElement(normal);
		format.addVertexElement(color);
		mVertexFormats.POSITION_NORMAL_COLOR=format;

		format=new VertexFormat(3);
		format.addVertexElement(position);
		format.addVertexElement(normal);
		format.addVertexElement(texCoord);
		mVertexFormats.POSITION_NORMAL_TEX_COORD=format;

		format=new VertexFormat(3);
		format.addVertexElement(position);
		format.addVertexElement(color);
		format.addVertexElement(texCoord);
		mVertexFormats.POSITION_COLOR_TEX_COORD=format;

		format=new VertexFormat(4);
		format.addVertexElement(position);
		format.addVertexElement(normal);
		format.addVertexElement(color);
		format.addVertexElement(texCoord);
		mVertexFormats.POSITION_NORMAL_COLOR_TEX_COORD=format;
	}

//	public void setAudioPlayer(AudioPlayer audioPlayer){
//		mAudioPlayer=audioPlayer;
//		shared_static_cast<AudioBufferHandler>(mAudioBufferHandler).setAudioPlayer(mAudioPlayer);
//	}

//	public AudioPlayer getAudioPlayer(){
//		return mAudioPlayer;
//	}

	public void setDirectory(String directory){
		if(directory!=null){
			mDirectory=directory+"/";
		}
		else{
			mDirectory=directory;
		}
	}

	public String getDirectory(){return mDirectory;}

	public InputStream makeInputStream(String name){
		InputStream in=null;
		if(mInputStreamFactory!=null){
			in=mInputStreamFactory.makeInputStream(name);
		}
		else{
			try{
				FileInputStream fin=new FileInputStream(mDirectory+name);
				in=fin;
			}
			catch(Exception ex){}
		}
		return in;
	}

	// Entity methods
	public Entity allocEntity(Class type){
		Entity entity=(Entity)mEntityManager.alloc(type);
		entity.internal_setManaged(true);
		return entity;
	}

	public Entity createEntity(Class type){
		Entity entity=allocEntity(type);
		entity.create(this);
		return entity;
	}

	public void destroyEntity(Entity entity){
		if(entity.destroyed()==false){
			entity.destroy();
		}
	}

	public void freeEntity(Entity entity){
		if(entity.destroyed()==false){
			Error.unknown("freeing undestroyed entity");
			return;
		}
		if(entity.internal_getManaged()){
			entity.internal_setManaged(false);
			mEntityManager.free(entity);
		}
	}
	
	// Context methods
	public void contextActivate(Renderer renderer){
		assert(renderer!=null);

		Logger.debug(Categories.TOADLET_TADPOLE,
			"Engine::contextActivate");

		Map<Resource,ResourceManager.ResourceCache> textureResourceMap=mTextureManager.getResourcePtrMap();
		Iterator<Map.Entry<Resource,ResourceManager.ResourceCache>> textureIt;

		int numTexturesLoaded=0;
		for(textureIt=textureResourceMap.entrySet().iterator();textureIt.hasNext();){
			Texture resource=(Texture)(textureIt.next().getKey());
			if(resource!=null){
				assert(resource.internal_getTexturePeer()==null);
				Texture.Peer peer=renderer.createTexturePeer(resource);
				if(peer!=null){
					resource.internal_setTexturePeer(peer);
					numTexturesLoaded++;
				}
			}
		}

		mTextureManager.getContextResourcesToLoad().clear();

		
		Map<Resource,ResourceManager.ResourceCache> bufferResourceMap=mBufferManager.getResourcePtrMap();
		Iterator<Map.Entry<Resource,ResourceManager.ResourceCache>> bufferIt;

		int numBuffersLoaded=0;
		for(bufferIt=bufferResourceMap.entrySet().iterator();bufferIt.hasNext();){
			Buffer resource=(Buffer)(bufferIt.next().getKey());
			if(resource!=null){
				assert(resource.internal_getBufferPeer()==null);
				Buffer.Peer peer=null;
				if(resource.getBufferSize()>0){
					peer=renderer.createBufferPeer(resource);
				}
				if(peer!=null){
					resource.internal_setBufferPeer(peer);
					numBuffersLoaded++;
				}
			}
		}

		mBufferManager.getContextResourcesToLoad().clear();

		if((numTexturesLoaded>0 || numBuffersLoaded>0) && Logger.getInstance().getMasterCategoryReportingLevel(Categories.TOADLET_TADPOLE)>=Logger.Level.EXCESS){
			Logger.excess(Categories.TOADLET_TADPOLE,
				("contextActivate: Loaded ")+numTexturesLoaded+" textures, loaded "+numBuffersLoaded+" buffers");
		}
	}

	public void contextDeactivate(Renderer renderer){
		assert(renderer!=null);

		Logger.debug(Categories.TOADLET_TADPOLE,
			"Engine::contextDeactivate");

		int i;
		List<Texture.Peer> textureResourcePeersToUnload=mTextureManager.getContextResourcePeersToUnload();
		Map<Resource,ResourceManager.ResourceCache> textureResourceMap=mTextureManager.getResourcePtrMap();
		Iterator<Map.Entry<Resource,ResourceManager.ResourceCache>> textureIt;

		int numTexturePeersUnloaded=0;
		for(i=0;i<textureResourcePeersToUnload.size();++i){
			textureResourcePeersToUnload.get(i).destroy();
			numTexturePeersUnloaded++;
		}
		textureResourcePeersToUnload.clear();

		for(textureIt=textureResourceMap.entrySet().iterator();textureIt.hasNext();){
			Texture resource=(Texture)(textureIt.next().getKey());
			if(resource!=null){
				Texture.Peer peer=resource.internal_getTexturePeer();
				if(peer!=null){
					/// @todo: Modify Texture so it reads back the texture data and stores it internally?
					//  This would let us deactivate a context and reactivate it.
					//  The Buffer already does this
					resource.internal_setTexturePeer(null);
					peer.destroy();
					numTexturePeersUnloaded++;
				}
			}
		}


		List<Buffer.Peer> bufferResourcePeersToUnload=mBufferManager.getContextResourcePeersToUnload();
		Map<Resource,ResourceManager.ResourceCache> bufferResourceMap=mBufferManager.getResourcePtrMap();
		Iterator<Map.Entry<Resource,ResourceManager.ResourceCache>> bufferIt;

		int numBufferPeersUnloaded=0;
		for(i=0;i<bufferResourcePeersToUnload.size();++i){
			bufferResourcePeersToUnload.get(i).destroy();
			numBufferPeersUnloaded++;
		}
		bufferResourcePeersToUnload.clear();

		for(bufferIt=bufferResourceMap.entrySet().iterator();bufferIt.hasNext();){
			Buffer resource=(Buffer)(bufferIt.next().getKey());
			if(resource!=null){
				Buffer.Peer peer=resource.internal_getBufferPeer();
				if(peer!=null){
					resource.internal_setBufferPeer(null);
					peer.destroy();
					numBufferPeersUnloaded++;
				}
			}
		}

		if((numTexturePeersUnloaded>0 || numBufferPeersUnloaded>0) && Logger.getInstance().getMasterCategoryReportingLevel(Categories.TOADLET_TADPOLE)>=Logger.Level.EXCESS){
			Logger.excess(Categories.TOADLET_TADPOLE,
				("contextDeactivate: Unloaded ")+numTexturePeersUnloaded+" texture peers, unloaded "+numBufferPeersUnloaded+" buffer peers");
		}
	}

	public void contextUpdate(Renderer renderer){
		assert(renderer!=null);

		int i;
		List<Texture.Peer> textureResourcePeersToUnload=mTextureManager.getContextResourcePeersToUnload();
		List<Resource> textureResourcesToLoad=mTextureManager.getContextResourcesToLoad();

		int numTexturePeersUnloaded=0;
		for(i=0;i<textureResourcePeersToUnload.size();++i){
			textureResourcePeersToUnload.get(i).destroy();
			numTexturePeersUnloaded++;
		}
		textureResourcePeersToUnload.clear();

		int numTexturesLoaded=0;
		for(i=0;i<textureResourcesToLoad.size();++i){
			Texture resource=(Texture)textureResourcesToLoad.get(i);
			if(resource!=null){
				assert(resource.internal_getTexturePeer()==null);
				Texture.Peer peer=renderer.createTexturePeer(resource);
				if(peer!=null){
					resource.internal_setTexturePeer(peer);
					numTexturesLoaded++;
				}
			}
		}
		textureResourcesToLoad.clear();

		if((numTexturePeersUnloaded>0 || numTexturesLoaded>0) && Logger.getInstance().getMasterCategoryReportingLevel(Categories.TOADLET_TADPOLE)>=Logger.Level.EXCESS){
			Logger.excess(Categories.TOADLET_TADPOLE,
				("contextUpdate: Unloaded ")+numTexturePeersUnloaded+" texture peers, loaded "+numTexturesLoaded+" textures");
		}

		List<Buffer.Peer> bufferResourcePeersToUnload=mBufferManager.getContextResourcePeersToUnload();
		List<Resource> bufferResourcesToLoad=mBufferManager.getContextResourcesToLoad();

		int numBufferPeersUnloaded=0;
		for(i=0;i<bufferResourcePeersToUnload.size();++i){
			bufferResourcePeersToUnload.get(i).destroy();
			numBufferPeersUnloaded++;
		}
		bufferResourcePeersToUnload.clear();

		int numBuffersLoaded=0;
		for(i=0;i<bufferResourcesToLoad.size();++i){
			Buffer resource=(Buffer)bufferResourcesToLoad.get(i);
			if(resource!=null){
				assert(resource.internal_getBufferPeer()==null);
				Buffer.Peer peer=null;
				if(resource.getBufferSize()>0){
					peer=renderer.createBufferPeer(resource);
				}
				if(peer!=null){
					resource.internal_setBufferPeer(peer);
					numBuffersLoaded++;
				}
			}
		}
		bufferResourcesToLoad.clear();

		if((numBufferPeersUnloaded>0 || numBuffersLoaded>0) && Logger.getInstance().getMasterCategoryReportingLevel(Categories.TOADLET_TADPOLE)>=Logger.Level.EXCESS){
			Logger.excess(Categories.TOADLET_TADPOLE,
				("contextUpdate: Unloaded ")+numBufferPeersUnloaded+" buffer peers, loaded "+numBuffersLoaded+" buffers");
		}
	}


	// Resource methods
	//  Texture
	public Texture loadTexture(String name){return (Texture)(mTextureManager.load(name));}
	public Texture loadTexture(String name,String file){return (Texture)(mTextureManager.load(name,file));}
	public Texture loadTexture(Texture resource){return (Texture)(mTextureManager.load(resource));}
	public Texture loadTexture(String name,Texture resource){return (Texture)(mTextureManager.load(name,resource));}

	public Texture cacheTexture(String name){return (Texture)(mTextureManager.cache(name));}
	public Texture cacheTexture(String name,String file){return (Texture)(mTextureManager.cache(name,file));}
	public Texture cacheTexture(Texture resource){return (Texture)(mTextureManager.cache(resource));}
	public Texture cacheTexture(String name,Texture resource){return (Texture)(mTextureManager.cache(name,resource));}

	public boolean uncacheTexture(String name){return mTextureManager.uncache(name);}
	public boolean uncacheTexture(Texture resource){return mTextureManager.uncache(resource);}

	public Texture getTexture(String name){return (Texture)(mTextureManager.get(name));}

	public ResourceManager getTextureManager(){return mTextureManager;}

	//  Buffer
	public IndexBuffer loadIndexBuffer(IndexBuffer resource){return (IndexBuffer)(mBufferManager.load(resource));}
	public IndexBuffer loadIndexBuffer(String name,IndexBuffer resource){return (IndexBuffer)(mBufferManager.load(name,resource));}
	public VertexBuffer loadVertexBuffer(VertexBuffer resource){return (VertexBuffer)(mBufferManager.load(resource));}
	public VertexBuffer loadVertexBuffer(String name,VertexBuffer resource){return (VertexBuffer)(mBufferManager.load(name,resource));}

	public IndexBuffer cacheIndexBuffer(IndexBuffer resource){return (IndexBuffer)(mBufferManager.cache(resource));}
	public IndexBuffer cacheIndexBuffer(String name,IndexBuffer resource){return (IndexBuffer)(mBufferManager.cache(name,resource));}
	public VertexBuffer cacheVertexBuffer(VertexBuffer resource){return (VertexBuffer)(mBufferManager.cache(resource));}
	public VertexBuffer cacheVertexBuffer(String name,VertexBuffer resource){return (VertexBuffer)(mBufferManager.cache(name,resource));}

	public boolean uncacheIndexBuffer(String name){return mBufferManager.uncache(name);}
	public boolean uncacheIndexBuffer(IndexBuffer resource){return mBufferManager.uncache(resource);}
	public boolean uncacheVertexBuffer(String name){return mBufferManager.uncache(name);}
	public boolean uncacheVertexBuffer(VertexBuffer resource){return mBufferManager.uncache(resource);}

	public IndexBuffer getIndexBuffer(String name){return (IndexBuffer)(mBufferManager.get(name));}
	public VertexBuffer getVertexBuffer(String name){return (VertexBuffer)(mBufferManager.get(name));}

	public int getIdealFormatBit(){return mIdealFormatBit;}
	public VertexFormats getVertexFormats(){return mVertexFormats;}

	/// @todo: Change these buffer lookups to use maps of some sort
	public VertexBuffer checkoutVertexBuffer(VertexFormat format,int numVertexes){
		int i;
		for(i=0;i<mVertexBufferCheckins.size();++i){
			VertexBuffer buffer=mVertexBufferCheckins.get(i);
			if(buffer.getSize()==numVertexes && buffer.getVertexFormat().equals(format)){
				mVertexBufferCheckins.remove(i);
				return buffer;
			}
		}
		return loadVertexBuffer(new VertexBuffer(Buffer.UsageType.DYNAMIC,Buffer.AccessType.WRITE_ONLY,format,numVertexes));
	}

	public void checkinVertexBuffer(VertexBuffer buffer){
		mVertexBufferCheckins.add(buffer);
	}

	public IndexBuffer checkoutIndexBuffer(IndexBuffer.IndexFormat format,int numIndexes){
		int i;
		for(i=0;i<mIndexBufferCheckins.size();++i){
			IndexBuffer buffer=mIndexBufferCheckins.get(i);
			if(buffer.getSize()==numIndexes && buffer.getIndexFormat()==format){
				mIndexBufferCheckins.remove(i);
				return buffer;
			}
		}
		return loadIndexBuffer(new IndexBuffer(Buffer.UsageType.DYNAMIC,Buffer.AccessType.WRITE_ONLY,format,numIndexes));
	}

	public void checkinIndexBuffer(IndexBuffer buffer){
		mIndexBufferCheckins.add(buffer);
	}

	public ResourceManager getBufferManager(){return mBufferManager;}
/*
	//  Shader
	peeper.Shader.ptr loadShader(const egg.String &name);
	peeper.Shader.ptr loadShader(const egg.String &name,const egg.String &file);
	peeper.Shader.ptr loadShader(const peeper.Shader.ptr &resource);
	peeper.Shader.ptr loadShader(const egg.String &name,const peeper.Shader.ptr &resource);

	peeper.Shader.ptr cacheShader(const egg.String &name);
	peeper.Shader.ptr cacheShader(const egg.String &name,const egg.String &file);
	peeper.Shader.ptr cacheShader(const peeper.Shader.ptr &resource);
	peeper.Shader.ptr cacheShader(const egg.String &name,const peeper.Shader.ptr &resource);

	bool uncacheShader(const egg.String &name);
	bool uncacheShader(const peeper.Shader.ptr &resource);

	peeper.Shader.ptr getShader(const egg.String &name) const;

	ResourceManager *getShaderManager() const;

	//  Program
	peeper.Program.ptr loadProgram(const egg.String &name);
	peeper.Program.ptr loadProgram(const egg.String &name,const egg.String &file);
	peeper.Program.ptr loadProgram(const peeper.Program.ptr &resource);
	peeper.Program.ptr loadProgram(const egg.String &name,const peeper.Program.ptr &resource);

	peeper.Program.ptr cacheProgram(const egg.String &name);
	peeper.Program.ptr cacheProgram(const egg.String &name,const egg.String &file);
	peeper.Program.ptr cacheProgram(const peeper.Program.ptr &resource);
	peeper.Program.ptr cacheProgram(const egg.String &name,const peeper.Program.ptr &resource);

	bool uncacheProgram(const egg.String &name);
	bool uncacheProgram(const peeper.Program.ptr &resource);

	peeper.Program.ptr getProgram(const egg.String &name) const;

	ResourceManager *getProgramManager() const;
*/
	//  Material
	public Material loadMaterial(String name){return (Material)(mMaterialManager.load(name));}
	public Material loadMaterial(String name,String file){return (Material)(mMaterialManager.load(name,file));}
	public Material loadMaterial(Material resource){return (Material)(mMaterialManager.load(resource));}
	public Material loadMaterial(String name,Material resource){return (Material)(mMaterialManager.load(name,resource));}

	public Material cacheMaterial(String name){return (Material)(mMaterialManager.cache(name));}
	public Material cacheMaterial(String name,String file){return (Material)(mMaterialManager.cache(name,file));}
	public Material cacheMaterial(Material resource){return (Material)(mMaterialManager.cache(resource));}
	public Material cacheMaterial(String name,Material resource){return (Material)(mMaterialManager.cache(name,resource));}

	public boolean uncacheMaterial(String name){return mMaterialManager.uncache(name);}
	public boolean uncacheMaterial(Material resource){return mMaterialManager.uncache(resource);}

	Material getMaterial(String name){return (Material)(mMaterialManager.get(name));}

	ResourceManager getMaterialManager(){return mMaterialManager;}

	//  Font
	public Font loadFont(String name,FontData data){return (Font)(mFontManager.load(name,data));}
	public Font loadFont(String name,String file,FontData data){return (Font)(mFontManager.load(name,file,data));}
	public Font loadFont(Font resource){return (Font)(mFontManager.load(resource));}

	public Font cacheFont(String name,FontData data){return (Font)(mFontManager.cache(name,data));}
	public Font cacheFont(String name,String file,FontData data){return (Font)(mFontManager.cache(name,file,data));}
	public Font cacheFont(Font resource){return (Font)(mFontManager.cache(resource));}

	public boolean uncacheFont(String name){return mFontManager.uncache(name);}
	public boolean uncacheFont(Font resource){return mFontManager.uncache(resource);}

	public Font getFont(String name){return (Font)(mFontManager.get(name));}

	public ResourceManager getFontManager(){return mFontManager;}

	//  Mesh
	public Mesh loadMesh(String name){return (Mesh)(mMeshManager.load(name));}
	public Mesh loadMesh(String name,String file){return (Mesh)(mMeshManager.load(name,file));}
	public Mesh loadMesh(Mesh resource){return (Mesh)(mMeshManager.load(resource));}
	public Mesh loadMesh(String name,Mesh resource){return (Mesh)(mMeshManager.load(name,resource));}

	public Mesh cacheMesh(String name){return (Mesh)(mMeshManager.cache(name));}
	public Mesh cacheMesh(String name,String file){return (Mesh)(mMeshManager.cache(name,file));}
	public Mesh cacheMesh(Mesh resource){return (Mesh)(mMeshManager.cache(resource));}
	public Mesh cacheMesh(String name,Mesh resource){return (Mesh)(mMeshManager.cache(name,resource));}

	public boolean uncacheMesh(String name){return mMeshManager.uncache(name);}
	public boolean uncacheMesh(Mesh resource){return mMeshManager.uncache(resource);}

	public Mesh getMesh(String name){return (Mesh)(mMeshManager.get(name));}

	public ResourceManager getMeshManager(){return mMeshManager;}
/*
	// AudioBuffer
	ribbit.AudioBuffer.ptr loadAudioBuffer(const egg.String &name,const AudioBufferData.ptr &data);
	ribbit.AudioBuffer.ptr loadAudioBuffer(const egg.String &name,const egg.String &file,const AudioBufferData.ptr &data);
	ribbit.AudioBuffer.ptr loadAudioBuffer(const ribbit.AudioBuffer.ptr &resource);
	ribbit.AudioBuffer.ptr loadAudioBuffer(const egg.String &name,const ribbit.AudioBuffer.ptr &resource);

	ribbit.AudioBuffer.ptr cacheAudioBuffer(const egg.String &name,const AudioBufferData.ptr &data);
	ribbit.AudioBuffer.ptr cacheAudioBuffer(const egg.String &name,const egg.String &file,const AudioBufferData.ptr &data);
	ribbit.AudioBuffer.ptr cacheAudioBuffer(const ribbit.AudioBuffer.ptr &resource);
	ribbit.AudioBuffer.ptr cacheAudioBuffer(const egg.String &name,const ribbit.AudioBuffer.ptr &resource);

	bool uncacheAudioBuffer(const egg.String &name);
	bool uncacheAudioBuffer(const ribbit.AudioBuffer.ptr &resource);

	ribbit.AudioBuffer.ptr getAudioBuffer(const egg.String &name) const;

	ResourceManager *getAudioBufferManager() const;
*/
	/// @todo  Modify the Mesh structure so it can store multiple types of meshes, like triStrip meshes, lineStrip meshes, and just line meshes, which this would be.
	///  Then we could modify the ParticleEntity to be able to take a Mesh on startup, and if its a line mesh, then it would load up the structure from that.
	public boolean loadParticleFile(List<Vector3> points,String file){
		InputStream in=makeInputStream(file);
		if(in==null){
			Error.unknown(Categories.TOADLET_TADPOLE,
				"error opening particle file");
			return false;
		}

		Document document=null;
		try{
			document=javax.xml.parsers.DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(in);
		}
		catch(Exception ex){
			Error.unknown(Categories.TOADLET_TADPOLE,
				"error loading particle file");
			return false;
		}

		Element xmlNode=document.getDocumentElement();
		if(xmlNode!=null){
			Node lineNode=xmlNode.getFirstChild();
			while(lineNode!=null){
				if("line".equals(lineNode.getNodeName())==false){lineNode=lineNode.getNextSibling();continue;}
				Node pointNode=lineNode.getFirstChild();
				while(pointNode!=null){
					if("point".equals(pointNode.getNodeName())==false){pointNode=pointNode.getNextSibling();continue;}
					Vector3 point=new Vector3();
					Node dataNode=pointNode.getFirstChild();
					if(dataNode!=null){
						String data=dataNode.getNodeValue();
						int space1=data.indexOf((char)10,1);
						int space2=data.indexOf((char)10,space1+1);
						if(space1>0 && space2>0){
							float x=Float.parseFloat(data.substring(0,space1));
							float y=Float.parseFloat(data.substring(space1+1,space2));
							float z=Float.parseFloat(data.substring(space2+1,data.length()));
							#if defined(TOADLET_FIXED_POINT)
								point.x=Math.fromFloat(x);
								point.y=Math.fromFloat(y);
								point.z=Math.fromFloat(z);
							#else
								point.x=x;
								point.y=y;
								point.z=z;
							#endif
						}
					}
					points.add(point);
					pointNode=pointNode.getNextSibling();
				}
				lineNode=lineNode.getNextSibling();
			}
		}

		return true;
	}

	protected ObjectManager mEntityManager=new ObjectManager();
	protected String mDirectory;
	protected InputStreamFactory mInputStreamFactory=null;
	protected Scene mScene=null;
	protected Renderer mRenderer=null;
	//protected AudioPlayer mAudioPlayer=null;

	// Resource members
	// Texture
	class TextureSemantics implements ContextResourceManager.ResourceSemantics{
		public Texture.Peer getResourcePeer(Resource resource){return ((Texture)resource).internal_getTexturePeer();}
	}

	ContextResourceManager<Texture.Peer> mTextureManager;

	// Buffer
	class BufferSemantics implements ContextResourceManager.ResourceSemantics{
		public Buffer.Peer getResourcePeer(Resource resource){return ((Buffer)resource).internal_getBufferPeer();}
	};

	ContextResourceManager<Buffer.Peer> mBufferManager;
	int mIdealFormatBit=0;
	VertexFormats mVertexFormats=new VertexFormats();
	Vector<VertexBuffer> mVertexBufferCheckins=new Vector<VertexBuffer>();
	Vector<IndexBuffer> mIndexBufferCheckins=new Vector<IndexBuffer>();

	/*
	// Shader
	class ShaderSemantics{
	public:
		static inline peeper.ShaderPeer *getResourcePeer(egg.Resource *resource){return ((peeper.Shader*)resource).internal_getShaderPeer();}
	};

	typedef ContextResourceManager<peeper.ShaderPeer,ShaderSemantics> ShaderManager;
	ShaderManager *mShaderManager;

	// Program
	class ProgramSemantics{
	public:
		static inline peeper.ProgramPeer *getResourcePeer(egg.Resource *resource){return ((peeper.Program*)resource).internal_getProgramPeer();}
	};

	typedef ContextResourceManager<peeper.ProgramPeer,ProgramSemantics> ProgramManager;
	ProgramManager *mProgramManager;
*/
	// Material
	ResourceManager mMaterialManager;

	// Font
	ResourceManager mFontManager;

	// Mesh
	ResourceManager mMeshManager;

	// AudioBuffer
//	ResourceManager *mAudioBufferManager;
//	AudioBufferHandler mAudioBufferHandler;
}
