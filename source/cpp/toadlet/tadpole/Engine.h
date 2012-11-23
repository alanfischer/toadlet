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

#ifndef TOADLET_TADPOLE_ENGINE_H
#define TOADLET_TADPOLE_ENGINE_H

#include <toadlet/egg/Log.h>
#include <toadlet/egg/Error.h>
#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/peeper/RenderCaps.h>
#include <toadlet/ribbit/AudioDevice.h>
#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/ArchiveManager.h>
#include <toadlet/tadpole/AudioManager.h>
#include <toadlet/tadpole/FontManager.h>
#include <toadlet/tadpole/VertexFormats.h>
#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/ShaderManager.h>
#include <toadlet/tadpole/Mesh.h>
#include <toadlet/tadpole/ContextListener.h>
#include <toadlet/tadpole/Node.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Engine:public Object{
public:
	TOADLET_OBJECT(Engine);

	Engine(void *env=NULL,void *ctx=NULL);
	virtual ~Engine();

	void destroy();

	void installHandlers();
	
	void setMaximumRenderCaps(const RenderCaps &caps);
	void setBackableRenderCaps(const RenderCaps &caps);
	int mergeCap(int render,int maximum,int backable);
	void updateRenderCaps();
	const RenderCaps &getRenderCaps() const{return mEngineRenderCaps;}
	void updateVertexFormats();
	const VertexFormats &getVertexFormats() const{return mVertexFormats;}
	bool isBackable() const{return mBackableRenderCaps.hasShader[Shader::ShaderType_VERTEX] | mBackableRenderCaps.hasFixed[Shader::ShaderType_VERTEX];}
	bool isShaderBackable() const{return mBackableRenderCaps.hasShader[Shader::ShaderType_VERTEX];}
	bool isFixedBackable() const{return mBackableRenderCaps.hasFixed[Shader::ShaderType_VERTEX];}
	bool hasShader(Shader::ShaderType type) const{return mEngineRenderCaps.hasShader[type];}
	bool hasFixed(Shader::ShaderType type) const{return mEngineRenderCaps.hasFixed[type];}

	bool setRenderDevice(RenderDevice *renderDevice);
	RenderDevice *getRenderDevice() const;

	bool setAudioDevice(AudioDevice *audioDevice);
	AudioDevice *getAudioDevice() const;

	Stream::ptr openStream(const String &name){return mArchiveManager->openStream(name);}

	// Node methods
	void nodeCreated(Node *node);
	void nodeDestroyed(Node *node);

	inline Node *getNodeByHandle(int handle){return (handle>=0 && handle<mHandles.size())?mHandles[handle]:NULL;}

	// Context methods
	void contextReset(RenderDevice *renderDevice);
	void contextActivate(RenderDevice *renderDevice);
	void contextDeactivate(RenderDevice *renderDevice);

	void addContextListener(ContextListener *listener){mContextListeners.add(listener);}
	void removeContextListener(ContextListener *listener){mContextListeners.remove(listener);}

	inline ArchiveManager *getArchiveManager() const{return mArchiveManager;}
	inline TextureManager *getTextureManager() const{return mTextureManager;}
	inline BufferManager *getBufferManager() const{return mBufferManager;}
	inline ShaderManager *getShaderManager() const{return mShaderManager;}
	inline MaterialManager *getMaterialManager() const{return  mMaterialManager;}
	inline FontManager *getFontManager() const{return mFontManager;}
	inline ResourceManager *getMeshManager() const{return mMeshManager;}
	inline AudioManager *getAudioManager() const{return mAudioManager;}

	/// @todo: These managers need to be optionally created upon BSP plugin loading
	inline ResourceManager *getStudioModelManager() const{return mStudioModelManager;}
	inline ResourceManager *getSpriteModelManager() const{return mSpriteModelManager;}

	Texture::ptr createNormalizationTexture(int size);

	Material::ptr createDiffuseMaterial(Texture *texture);
	Material::ptr createPointSpriteMaterial(Texture *texture,scalar size,bool attenuated);
	Material::ptr createFontMaterial(Font *font);
	Material::ptr createSkyBoxMaterial(Texture *texture,bool clamp=true);
	Material::ptr createWaterMaterial(Texture *reflectTexture,Texture *refractTexture,Texture *bumpTexture,const Vector4 &color);

	Mesh::ptr createAABoxMesh(const AABox &box,Material *material=NULL);
	Mesh::ptr createSkyBoxMesh(scalar size,bool unfolded,bool invert,Material *bottom,Material *top,Material *left,Material *right,Material *back,Material *front);
	Mesh::ptr createSkyDomeMesh(const Sphere &sphere,int numSegments,int numRings,scalar fade,Material *material=NULL);
	Mesh::ptr createSphereMesh(const Sphere &sphere,Material *material=NULL);
	Mesh::ptr createGridMesh(scalar width,scalar height,int numWidth,int numHeight,Material *material=NULL);

protected:
	void *mEnv,*mCtx;
	RenderDevice *mRenderDevice;
	bool mRenderDeviceChanged;
	AudioDevice *mAudioDevice;
	bool mAudioDeviceChanged;

	Collection<ContextListener*> mContextListeners;

	RenderCaps mMaximumRenderCaps;
	RenderCaps mBackableRenderCaps;
	RenderCaps mRenderCaps;
	RenderCaps mEngineRenderCaps;
	VertexFormats mVertexFormats;

	ArchiveManager::ptr mArchiveManager;
	TextureManager::ptr mTextureManager;
	BufferManager::ptr mBufferManager;
	ShaderManager::ptr mShaderManager;
	MaterialManager::ptr mMaterialManager;
	FontManager::ptr mFontManager;
	ResourceManager::ptr mMeshManager;
	AudioManager::ptr mAudioManager;

	ResourceManager::ptr mStudioModelManager;
	ResourceManager::ptr mSpriteModelManager;

	ResourceCreator::ptr mNormalizationCreator;
	ResourceCreator::ptr mDiffuseCreator,mSkyBoxMaterialCreator,mWaterMaterialCreator;
	ResourceCreator::ptr mAABoxCreator,mSkyBoxCreator,mSkyDomeCreator,mSphereCreator,mGridCreator;

	Collection<int> mFreeHandles;
	Collection<Node*> mHandles;
};

}
}

#endif
