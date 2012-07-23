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

#include <toadlet/egg/Categories.h>
#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/VertexBuffer.h>
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
#include <toadlet/tadpole/MeshManager.h>
#include <toadlet/tadpole/ShaderManager.h>
#include <toadlet/tadpole/ContextListener.h>
#include <toadlet/tadpole/Node.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Engine:public Object{
public:
	TOADLET_OBJECT(Engine);

	Engine(void *env=NULL,void *ctx=NULL);//bool fixedBackable=true,bool shaderBackable=true);
	virtual ~Engine();

	void destroy();

	void installHandlers();
	
	bool setRenderDevice(RenderDevice *renderDevice);
	RenderDevice *getRenderDevice() const;
	void updateVertexFormats();

	bool setAudioDevice(AudioDevice *audioDevice);
	AudioDevice *getAudioDevice() const;

	void setDirectory(const String &directory){mArchiveManager->setDirectory(directory);}
	const String &getDirectory() const{return mArchiveManager->getDirectory();}
	Stream::ptr openStream(const String &name){return mArchiveManager->openStream(name);}

	// Node methods
	void nodeCreated(Node *node);
	void nodeDestroyed(Node *node);
	int internal_registerNode(Node *node);

	template<typename Type> Type *createNodeType(egg::Type<Type,Node> *type,Scene *scene){return (Type*)createNode(type,scene);}

	inline Node *getNodeByHandle(int handle){return (handle>=0 && handle<mHandles.size())?mHandles[handle]:NULL;}

	// Context methods
	void contextReset(RenderDevice *renderDevice);
	void contextActivate(RenderDevice *renderDevice);
	void contextDeactivate(RenderDevice *renderDevice);

	void addContextListener(ContextListener *listener){mContextListeners.add(listener);}
	void removeContextListener(ContextListener *listener){mContextListeners.remove(listener);}

	int getIdealVertexFormatType() const{return mIdealVertexFormatType;}
	const VertexFormats &getVertexFormats() const{return mVertexFormats;}
	const RenderCaps &getRenderCaps() const{return mRenderCaps;}
	bool isBackable() const{return mFixedBackable || mShaderBackable;}
	bool isShaderBackable() const{return mShaderBackable;}
	bool isFixedBackable() const{return mFixedBackable;}
	bool hasShader(Shader::ShaderType type){return mShaderBackable || mRenderCaps.hasShader[type];}
	bool hasFixed(Shader::ShaderType type){return mFixedBackable || mRenderCaps.hasFixed[type];}

	inline ArchiveManager *getArchiveManager() const{return mArchiveManager;}
	inline TextureManager *getTextureManager() const{return mTextureManager;}
	inline BufferManager *getBufferManager() const{return mBufferManager;}
	inline ShaderManager *getShaderManager() const{return mShaderManager;}
	inline MaterialManager *getMaterialManager() const{return  mMaterialManager;}
	inline FontManager *getFontManager() const{return mFontManager;}
	inline MeshManager *getMeshManager() const{return mMeshManager;}
	inline AudioManager *getAudioManager() const{return mAudioManager;}

	Texture::ptr createNormalizationTexture(int size);

	Material::ptr createDiffuseMaterial(Texture *texture);
	Material::ptr createPointSpriteMaterial(Texture *texture,scalar size,bool attenuated);
	Material::ptr createFontMaterial(Font *font);
	Material::ptr createSkyBoxMaterial(Texture *texture,bool clamp=true);

	Mesh::ptr createAABoxMesh(const AABox &box,Material::ptr material=Material::ptr());
	Mesh::ptr createSkyBoxMesh(scalar size,bool unfolded,bool invert,Material::ptr bottom,Material::ptr top,Material::ptr left,Material::ptr right,Material::ptr back,Material::ptr front);
	Mesh::ptr createSkyDomeMesh(const Sphere &sphere,int numSegments,int numRings,scalar fade,Material::ptr material=Material::ptr());
	Mesh::ptr createSphereMesh(const Sphere &sphere,Material::ptr material=Material::ptr());
	Mesh::ptr createGridMesh(scalar width,scalar height,int numWidth,int numHeight,Material::ptr material=Material::ptr());

protected:
	void *mEnv,*mCtx;
	bool mFixedBackable,mShaderBackable;
	String mDirectory;
	RenderDevice *mRenderDevice;
	bool mRenderDeviceChanged;
	AudioDevice *mAudioDevice;
	bool mAudioDeviceChanged;

	Collection<ContextListener*> mContextListeners;

	int mIdealVertexFormatType;
	VertexFormats mVertexFormats;
	RenderCaps mRenderCaps;

	ArchiveManager::ptr mArchiveManager;
	TextureManager::ptr mTextureManager;
	BufferManager::ptr mBufferManager;
	ShaderManager::ptr mShaderManager;
	MaterialManager::ptr mMaterialManager;
	FontManager::ptr mFontManager;
	MeshManager::ptr mMeshManager;
	AudioManager::ptr mAudioManager;

	ResourceCreator::ptr mNormalizationCreator;
	ResourceCreator::ptr mDiffuseCreator,mSkyBoxMaterialCreator;
	ResourceCreator::ptr mAABoxCreator,mSkyBoxCreator,mSkyDomeCreator,mSphereCreator,mGridCreator;

	Collection<int> mFreeHandles;
	Collection<Node*> mHandles;
};

}
}

#endif
