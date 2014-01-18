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
	const RenderCaps &getMaximumRenderCaps() const{return mMaximumRenderCaps;}
	void setBackableRenderCaps(const RenderCaps &caps);
	const RenderCaps &getBackableRenderCaps() const{return mBackableRenderCaps;}
	int mergeCap(int render,int maximum,int backable);
	void updateRenderCaps();
	const RenderCaps &getRenderCaps() const{return mEngineRenderCaps;}
	void updateVertexFormats();
	const VertexFormats &getVertexFormats() const{return mVertexFormats;}

	bool isBackable() const{return mBackableRenderCaps.hasShader[Shader::ShaderType_VERTEX] | mBackableRenderCaps.hasFixed[Shader::ShaderType_VERTEX];}
	// Until we have Shader exposed to swig, we will use int here instead of Shader::Type
	bool hasShader(int type) const{return mEngineRenderCaps.hasShader[type];}
	bool hasFixed(int type) const{return mEngineRenderCaps.hasFixed[type];}
	/// @todo: Once the RenderCaps has been better Objectized and exposed to swig, these can go away
	bool hasBackableShader(int type) const{return mBackableRenderCaps.hasShader[type];}
	bool hasBackableFixed(int type) const{return mBackableRenderCaps.hasFixed[type];}
	void setHasBackableShader(bool has);
	void setHasBackableFixed(bool has);
	void setHasBackableTriangleFan(bool has);
	bool hasMaximumShader(int type) const{return mMaximumRenderCaps.hasShader[type];}
	bool hasMaximumFixed(int type) const{return mMaximumRenderCaps.hasFixed[type];}
	void setHasMaximumShader(bool has);
	void setHasMaximumFixed(bool has);

	bool setRenderDevice(RenderDevice *renderDevice);
	RenderDevice *getRenderDevice() const;

	bool setAudioDevice(AudioDevice *audioDevice);
	AudioDevice *getAudioDevice() const;

	Stream::ptr openStream(const String &name){return mArchiveManager->openStream(name);}

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

	Material::ptr createDiffuseMaterial(Texture *texture,RenderState *renderState=NULL);
	Material::ptr createPointSpriteMaterial(Texture *texture,scalar size,bool attenuated);
	Material::ptr createFontMaterial(Font *font);
	Material::ptr createSkyBoxMaterial(Texture *texture,bool clamp=true,RenderState *renderState=NULL);
	Material::ptr createWaterMaterial(Texture *reflectTexture,Texture *refractTexture,Texture *bumpTexture,const Vector4 &color);

	Mesh::ptr createAABoxMesh(const AABox &box,Material *material=NULL);
	Mesh::ptr createSphereMesh(const Sphere &sphere,Material *material=NULL);
	Mesh::ptr createTorusMesh(scalar majorRadius,scalar minorRadius,int numMajor,int numMinor,Material *material=NULL);
	Mesh::ptr createGridMesh(scalar width,scalar height,int numWidth,int numHeight,Material *material=NULL);
	Mesh::ptr createSkyBoxMesh(scalar size,bool unfolded,bool invert,Material *bottom,Material *top,Material *left,Material *right,Material *back,Material *front);
	Mesh::ptr createSkyDomeMesh(const Sphere &sphere,int numSegments,int numRings,scalar fade,Material *material=NULL);

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
	ResourceCreator::ptr mAABoxCreator,mSphereCreator,mTorusCreator,mGridCreator,mSkyBoxCreator,mSkyDomeCreator;
};

}
}

#endif
