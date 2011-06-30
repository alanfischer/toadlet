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
#include <toadlet/tadpole/AudioBufferManager.h>
#include <toadlet/tadpole/FontManager.h>
#include <toadlet/tadpole/VertexFormats.h>
#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/MeshManager.h>
#include <toadlet/tadpole/NodeManager.h>
#include <toadlet/tadpole/ShaderManager.h>
#include <toadlet/tadpole/ContextListener.h>
#include <toadlet/tadpole/node/Node.h>
#include <toadlet/tadpole/handler/AudioBufferHandler.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Engine{
public:
	Engine(bool backable=false);
	virtual ~Engine();

	void destroy();

	bool setRenderDevice(RenderDevice *renderDevice);
	RenderDevice *getRenderDevice() const;
	void updateVertexFormats();

	bool setAudioDevice(AudioDevice *audioDevice);
	AudioDevice *getAudioDevice() const;

	void setDirectory(const String &directory){mArchiveManager->setDirectory(directory);}
	const String &getDirectory() const{return mArchiveManager->getDirectory();}
	Stream::ptr openStream(const String &name){return mArchiveManager->openStream(name);}

	// Node methods
	void registerNodeType(BaseType<Node> *type);
	Node *allocNode(BaseType<Node> *type);
	Node *allocNode(const String &fullName);
	Node *createNode(BaseType<Node> *type,Scene *scene);
	Node *createNode(const String &fullName,Scene *scene);
	int internal_registerNode(Node *node);
	void internal_deregisterNode(Node *node);
	void destroyNode(Node *node);
	void freeNode(Node *node);

	template<typename Type> Type *allocNodeType(egg::Type<Type,Node> *type){return (Type*)allocNode(type);}
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

	inline ArchiveManager *getArchiveManager() const{return mArchiveManager;}
	inline TextureManager *getTextureManager() const{return mTextureManager;}
	inline BufferManager *getBufferManager() const{return mBufferManager;}
	inline ShaderManager *getShaderManager() const{return mShaderManager;}
	inline MaterialManager *getMaterialManager() const{return  mMaterialManager;}
	inline FontManager *getFontManager() const{return mFontManager;}
	inline MeshManager *getMeshManager() const{return mMeshManager;}
	inline NodeManager *getNodeManager() const{return mNodeManager;}
	inline AudioBufferManager *getAudioBufferManager() const{return mAudioBufferManager;}

protected:
	bool mBackable;
	String mDirectory;
	RenderDevice *mRenderDevice;
	RenderDevice *mLastRenderDevice;
	AudioDevice *mAudioDevice;
	AudioDevice *mLastAudioDevice;

	Collection<ContextListener*> mContextListeners;

	int mIdealVertexFormatType;
	VertexFormats mVertexFormats;
	RenderCaps mRenderCaps;

	ArchiveManager *mArchiveManager;
	TextureManager *mTextureManager;
	BufferManager *mBufferManager;
	ShaderManager *mShaderManager;
	MaterialManager *mMaterialManager;
	FontManager *mFontManager;
	MeshManager *mMeshManager;
	AudioBufferManager *mAudioBufferManager;
	NodeManager *mNodeManager;
	Collection<int> mFreeHandles;
	Collection<Node*> mHandles;

	TypeFactory<Node> mNodeFactory;
};

}
}

#endif
