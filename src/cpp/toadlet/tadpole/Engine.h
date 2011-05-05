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
#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/peeper/Shader.h>
#include <toadlet/peeper/Program.h>
#include <toadlet/peeper/RendererCaps.h>
#include <toadlet/ribbit/AudioPlayer.h>
#include <toadlet/ribbit/AudioBuffer.h>
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
#include <toadlet/tadpole/ResourceManager.h>
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

	bool setRenderer(peeper::Renderer *renderer);
	peeper::Renderer *getRenderer() const;
	void updateVertexFormats();

	bool setAudioPlayer(ribbit::AudioPlayer *audioPlayer);
	ribbit::AudioPlayer *getAudioPlayer() const;

	void setDirectory(const egg::String &directory){mArchiveManager->setDirectory(directory);}
	const egg::String &getDirectory() const{return mArchiveManager->getDirectory();}
	egg::io::Stream::ptr openStream(const egg::String &name){return mArchiveManager->openStream(name);}

	// Node methods
	void registerNodeType(egg::BaseType<node::Node> *type);
	node::Node *allocNode(egg::BaseType<node::Node> *type);
	node::Node *allocNode(const egg::String &fullName);
	node::Node *createNode(egg::BaseType<node::Node> *type,Scene *scene);
	node::Node *createNode(const egg::String &fullName,Scene *scene);
	int internal_registerNode(node::Node *node);
	void internal_deregisterNode(node::Node *node);
	void destroyNode(node::Node *node);
	void freeNode(node::Node *node);

	template<typename Type> Type *allocNodeType(egg::Type<Type,node::Node> *type){return (Type*)allocNode(type);}
	template<typename Type> Type *createNodeType(egg::Type<Type,node::Node> *type,Scene *scene){return (Type*)createNode(type,scene);}

	inline node::Node *getNodeByHandle(int handle){return (handle>=0 && handle<mHandles.size())?mHandles[handle]:NULL;}

	// Context methods
	void contextReset(peeper::Renderer *renderer);
	void contextActivate(peeper::Renderer *renderer);
	void contextDeactivate(peeper::Renderer *renderer);

	void addContextListener(ContextListener *listener){mContextListeners.add(listener);}
	void removeContextListener(ContextListener *listener){mContextListeners.remove(listener);}

	int getIdealVertexFormatBit() const{return mIdealVertexFormatBit;}
	const VertexFormats &getVertexFormats() const{return mVertexFormats;}
	const peeper::RendererCaps &getRendererCaps() const{return mRendererCaps;}

	inline ArchiveManager *getArchiveManager() const{return mArchiveManager;}
	inline TextureManager *getTextureManager() const{return mTextureManager;}
	inline BufferManager *getBufferManager() const{return mBufferManager;}
	inline ResourceManager *getShaderManager() const{return mShaderManager;}
	inline ResourceManager *getProgramManager() const{return mProgramManager;}
	inline MaterialManager *getMaterialManager() const{return  mMaterialManager;}
	inline FontManager *getFontManager() const{return mFontManager;}
	inline MeshManager *getMeshManager() const{return mMeshManager;}
	inline NodeManager *getNodeManager() const{return mNodeManager;}
	inline AudioBufferManager *getAudioBufferManager() const{return mAudioBufferManager;}

protected:
	bool mBackable;
	egg::String mDirectory;
	peeper::Renderer *mRenderer;
	peeper::Renderer *mLastRenderer;
	ribbit::AudioPlayer *mAudioPlayer;
	ribbit::AudioPlayer *mLastAudioPlayer;

	egg::Collection<ContextListener*> mContextListeners;

	int mIdealVertexFormatBit;
	VertexFormats mVertexFormats;
	peeper::RendererCaps mRendererCaps;

	ArchiveManager *mArchiveManager;
	TextureManager *mTextureManager;
	BufferManager *mBufferManager;
	ResourceManager *mShaderManager;
	ResourceManager *mProgramManager;
	MaterialManager *mMaterialManager;
	FontManager *mFontManager;
	MeshManager *mMeshManager;
	AudioBufferManager *mAudioBufferManager;
	NodeManager *mNodeManager;
	egg::Collection<int> mFreeHandles;
	egg::Collection<node::Node*> mHandles;

	egg::TypeFactory<node::Node> mNodeFactory;
};

}
}

#endif
