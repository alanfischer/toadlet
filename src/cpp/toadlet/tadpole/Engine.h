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

#include <toadlet/tadpole/Types.h>
#include <toadlet/egg/io/InputStreamFactory.h>
#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/peeper/Shader.h>
#include <toadlet/peeper/Program.h>
#include <toadlet/ribbit/AudioPlayer.h>
#include <toadlet/ribbit/AudioBuffer.h>
#include <toadlet/tadpole/AudioBufferData.h>
#include <toadlet/tadpole/Font.h>
#include <toadlet/tadpole/FontData.h>
#include <toadlet/tadpole/FontManager.h>
#include <toadlet/tadpole/VertexFormats.h>
#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/MeshManager.h>
#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/node/Scene.h>
#include <toadlet/tadpole/handler/AudioBufferHandler.h>
#include <toadlet/tadpole/mesh/Mesh.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Engine:public egg::io::InputStreamFactory{
public:
	Engine();
	virtual ~Engine();

	void destroy();

	/// @todo: Remove the Scene in Engine completely.  I believe we only need it currently for some HopNode stuff.
	///  This would let us have multiple Scenes in an application, pretty neat.
	void setScene(const node::Scene::ptr &scene);
	const node::Scene::ptr &getScene() const;

	void setInputStreamFactory(egg::io::InputStreamFactory *inputStreamFactory);
	egg::io::InputStreamFactory *getInputStreamFactory() const;

	void setRenderer(peeper::Renderer *renderer);
	peeper::Renderer *getRenderer() const;
	void updateVertexFormats();

	void setAudioPlayer(ribbit::AudioPlayer *audioPlayer);
	ribbit::AudioPlayer *getAudioPlayer() const;

	void setDirectory(const egg::String &directory);
	const egg::String &getDirectory() const;
	egg::io::InputStream::ptr makeInputStream(const egg::String &name);

	// Node methods
	node::Node *allocNode(const egg::BaseType<node::Node> &type);
	node::Node *createNode(const egg::BaseType<node::Node> &type);
	void destroyNode(node::Node *node);
	void freeNode(node::Node *node);

	template<typename Type> Type *allocNodeType(const egg::Type<Type,node::Node> &type){return (Type*)allocNode(type);}
	template<typename Type> Type *createNodeType(const egg::Type<Type,node::Node> &type){return (Type*)createNode(type);}

	// Context methods
	void contextReset(peeper::Renderer *renderer);
	void contextActivate(peeper::Renderer *renderer);
	void contextDeactivate(peeper::Renderer *renderer);
	void contextUpdate(peeper::Renderer *renderer);

	int getIdealVertexFormatBit() const{return mIdealVertexFormatBit;}
	const VertexFormats &getVertexFormats() const{return mVertexFormats;}

	inline TextureManager *getTextureManager() const{return mTextureManager;}
	inline BufferManager *getBufferManager() const{return mBufferManager;}
	inline ResourceManager *getShaderManager() const{return mShaderManager;}
	inline ResourceManager *getProgramManager() const{return mProgramManager;}
	inline MaterialManager *getMaterialManager() const{return  mMaterialManager;}
	inline FontManager *getFontManager() const{return mFontManager;}
	inline MeshManager *getMeshManager() const{return mMeshManager;}
	inline ResourceManager *getAudioBufferManager() const{return mAudioBufferManager;}

protected:
	egg::String mDirectory;
	egg::io::InputStreamFactory *mInputStreamFactory;
	node::Scene::ptr mScene;
	peeper::Renderer *mRenderer;
	ribbit::AudioPlayer *mAudioPlayer;

	int mIdealVertexFormatBit;
	VertexFormats mVertexFormats;

	TextureManager *mTextureManager;
	BufferManager *mBufferManager;
	ResourceManager *mShaderManager;
	ResourceManager *mProgramManager;
	MaterialManager *mMaterialManager;
	FontManager *mFontManager;
	MeshManager *mMeshManager;
	ResourceManager *mAudioBufferManager;
	handler::AudioBufferHandler::ptr mAudioBufferHandler;
};

}
}

#endif
