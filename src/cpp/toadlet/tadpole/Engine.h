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
#include <toadlet/tadpole/Mesh.h>
#include <toadlet/tadpole/Font.h>
#include <toadlet/tadpole/FontData.h>
#include <toadlet/tadpole/VertexFormats.h>
#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/ContextResourceManager.h>
#include <toadlet/tadpole/entity/Scene.h>
#include <toadlet/tadpole/handler/AudioBufferHandler.h>

namespace toadlet{
namespace tadpole{

class MaterialManager;
class MeshManager;
class TextureManager;

class TOADLET_API Engine:public egg::io::InputStreamFactory{
public:
	Engine();
	virtual ~Engine();

	/// @todo: Remove the Scene in Engine completely.  I believe we only need it currently for some HopEntity stuff.
	///  This would let us have multiple Scenes in an application, pretty neat.
	void setScene(const entity::Scene::ptr &scene);
	const entity::Scene::ptr &getScene() const;

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

	// Entity methods
	entity::Entity *allocEntity(){return NULL;}

	entity::Entity *createEntity(){return NULL;}

	void destroyEntity(entity::Entity *entity){
		if(entity->destroyed()==false){
			entity->destroy();
		}
	}

	void freeEntity(entity::Entity *entity){}

	// Context methods
	void contextActivate(peeper::Renderer *renderer);
	void contextDeactivate(peeper::Renderer *renderer);
	void contextUpdate(peeper::Renderer *renderer);


	// Resource methods
	//  Texture
	peeper::Texture::ptr loadTexture(const egg::String &name);
	peeper::Texture::ptr loadTexture(const egg::String &name,const egg::String &file);
	peeper::Texture::ptr loadTexture(const peeper::Texture::ptr &resource);
	peeper::Texture::ptr loadTexture(const egg::String &name,const peeper::Texture::ptr &resource);

	peeper::Texture::ptr cacheTexture(const egg::String &name);
	peeper::Texture::ptr cacheTexture(const egg::String &name,const egg::String &file);
	peeper::Texture::ptr cacheTexture(const peeper::Texture::ptr &resource);
	peeper::Texture::ptr cacheTexture(const egg::String &name,const peeper::Texture::ptr &resource);

	bool uncacheTexture(const egg::String &name);
	bool uncacheTexture(const peeper::Texture::ptr &resource);

	peeper::Texture::ptr getTexture(const egg::String &name) const;

	TextureManager *getTextureManager() const{return mTextureManager;}

	//  Buffer
	peeper::IndexBuffer::ptr loadIndexBuffer(const peeper::IndexBuffer::ptr &resource);
	peeper::IndexBuffer::ptr loadIndexBuffer(const egg::String &name,const peeper::IndexBuffer::ptr &resource);
	peeper::VertexBuffer::ptr loadVertexBuffer(const peeper::VertexBuffer::ptr &resource);
	peeper::VertexBuffer::ptr loadVertexBuffer(const egg::String &name,const peeper::VertexBuffer::ptr &resource);

	peeper::IndexBuffer::ptr cacheIndexBuffer(const peeper::IndexBuffer::ptr &resource);
	peeper::IndexBuffer::ptr cacheIndexBuffer(const egg::String &name,const peeper::IndexBuffer::ptr &resource);
	peeper::VertexBuffer::ptr cacheVertexBuffer(const peeper::VertexBuffer::ptr &resource);
	peeper::VertexBuffer::ptr cacheVertexBuffer(const egg::String &name,const peeper::VertexBuffer::ptr &resource);

	bool uncacheIndexBuffer(const egg::String &name);
	bool uncacheIndexBuffer(const peeper::IndexBuffer::ptr &resource);
	bool uncacheVertexBuffer(const egg::String &name);
	bool uncacheVertexBuffer(const peeper::VertexBuffer::ptr &resource);

	peeper::IndexBuffer::ptr getIndexBuffer(const egg::String &name) const;
	peeper::VertexBuffer::ptr getVertexBuffer(const egg::String &name) const;

	int getIdealFormatBit() const{return mIdealFormatBit;}
	const VertexFormats &getVertexFormats() const{return mVertexFormats;}

	ResourceManager *getBufferManager() const{return mBufferManager;}

	//  Shader
	peeper::Shader::ptr loadShader(const egg::String &name);
	peeper::Shader::ptr loadShader(const egg::String &name,const egg::String &file);
	peeper::Shader::ptr loadShader(const peeper::Shader::ptr &resource);
	peeper::Shader::ptr loadShader(const egg::String &name,const peeper::Shader::ptr &resource);

	peeper::Shader::ptr cacheShader(const egg::String &name);
	peeper::Shader::ptr cacheShader(const egg::String &name,const egg::String &file);
	peeper::Shader::ptr cacheShader(const peeper::Shader::ptr &resource);
	peeper::Shader::ptr cacheShader(const egg::String &name,const peeper::Shader::ptr &resource);

	bool uncacheShader(const egg::String &name);
	bool uncacheShader(const peeper::Shader::ptr &resource);

	peeper::Shader::ptr getShader(const egg::String &name) const;

	ResourceManager *getShaderManager() const{return mShaderManager;}

	//  Program
	peeper::Program::ptr loadProgram(const egg::String &name);
	peeper::Program::ptr loadProgram(const egg::String &name,const egg::String &file);
	peeper::Program::ptr loadProgram(const peeper::Program::ptr &resource);
	peeper::Program::ptr loadProgram(const egg::String &name,const peeper::Program::ptr &resource);

	peeper::Program::ptr cacheProgram(const egg::String &name);
	peeper::Program::ptr cacheProgram(const egg::String &name,const egg::String &file);
	peeper::Program::ptr cacheProgram(const peeper::Program::ptr &resource);
	peeper::Program::ptr cacheProgram(const egg::String &name,const peeper::Program::ptr &resource);

	bool uncacheProgram(const egg::String &name);
	bool uncacheProgram(const peeper::Program::ptr &resource);

	peeper::Program::ptr getProgram(const egg::String &name) const;

	ResourceManager *getProgramManager() const{return mProgramManager;}

	//  Material
	Material::ptr loadMaterial(const egg::String &name);
	Material::ptr loadMaterial(const egg::String &name,const egg::String &file);
	Material::ptr loadMaterial(const Material::ptr &resource);

	Material::ptr cacheMaterial(const egg::String &name);
	Material::ptr cacheMaterial(const egg::String &name,const egg::String &file);
	Material::ptr cacheMaterial(const Material::ptr &resource);

	bool uncacheMaterial(const egg::String &name);
	bool uncacheMaterial(const Material::ptr &resource);

	Material::ptr getMaterial(const egg::String &name) const;

	MaterialManager *getMaterialManager() const{return  mMaterialManager;}

	//  Font
	Font::ptr loadFont(const egg::String &name,const FontData::ptr &data);
	Font::ptr loadFont(const egg::String &name,const egg::String &file,const FontData::ptr &data);
	Font::ptr loadFont(const Font::ptr &resource);

	Font::ptr cacheFont(const egg::String &name,const FontData::ptr &data);
	Font::ptr cacheFont(const egg::String &name,const egg::String &file,const FontData::ptr &data);
	Font::ptr cacheFont(const Font::ptr &resource);

	bool uncacheFont(const egg::String &name);
	bool uncacheFont(const Font::ptr &resource);

	Font::ptr getFont(const egg::String &name) const;

	ResourceManager *getFontManager() const{return mFontManager;}

	//  Mesh
	Mesh::ptr loadMesh(const egg::String &name);
	Mesh::ptr loadMesh(const egg::String &name,const egg::String &file);
	Mesh::ptr loadMesh(const Mesh::ptr &resource);
	Mesh::ptr loadMesh(const egg::String &name,const Mesh::ptr &resource);

	Mesh::ptr cacheMesh(const egg::String &name);
	Mesh::ptr cacheMesh(const egg::String &name,const egg::String &file);
	Mesh::ptr cacheMesh(const Mesh::ptr &resource);
	Mesh::ptr cacheMesh(const egg::String &name,const Mesh::ptr &resource);

	bool uncacheMesh(const egg::String &name);
	bool uncacheMesh(const Mesh::ptr &resource);

	Mesh::ptr getMesh(const egg::String &name) const;

	MeshManager *getMeshManager() const{return mMeshManager;}

	// AudioBuffer
	ribbit::AudioBuffer::ptr loadAudioBuffer(const egg::String &name,const AudioBufferData::ptr &data);
	ribbit::AudioBuffer::ptr loadAudioBuffer(const egg::String &name,const egg::String &file,const AudioBufferData::ptr &data);
	ribbit::AudioBuffer::ptr loadAudioBuffer(const ribbit::AudioBuffer::ptr &resource);
	ribbit::AudioBuffer::ptr loadAudioBuffer(const egg::String &name,const ribbit::AudioBuffer::ptr &resource);

	ribbit::AudioBuffer::ptr cacheAudioBuffer(const egg::String &name,const AudioBufferData::ptr &data);
	ribbit::AudioBuffer::ptr cacheAudioBuffer(const egg::String &name,const egg::String &file,const AudioBufferData::ptr &data);
	ribbit::AudioBuffer::ptr cacheAudioBuffer(const ribbit::AudioBuffer::ptr &resource);
	ribbit::AudioBuffer::ptr cacheAudioBuffer(const egg::String &name,const ribbit::AudioBuffer::ptr &resource);

	bool uncacheAudioBuffer(const egg::String &name);
	bool uncacheAudioBuffer(const ribbit::AudioBuffer::ptr &resource);

	ribbit::AudioBuffer::ptr getAudioBuffer(const egg::String &name) const;

	ResourceManager *getAudioBufferManager() const{return mAudioBufferManager;}

protected:
	egg::String mDirectory;
	egg::io::InputStreamFactory *mInputStreamFactory;
	entity::Scene::ptr mScene;
	peeper::Renderer *mRenderer;
	ribbit::AudioPlayer *mAudioPlayer;

	// Resource members
	// Texture
	TextureManager *mTextureManager;

	// Buffer
	class BufferSemantics{
	public:
		static inline peeper::BufferPeer *getResourcePeer(egg::Resource *resource){return ((peeper::Buffer*)resource)->internal_getBufferPeer();}
	};

	typedef ContextResourceManager<peeper::BufferPeer,BufferSemantics> BufferManager;
	BufferManager *mBufferManager;
	int mIdealFormatBit;
	VertexFormats mVertexFormats;

	// Shader
	class ShaderSemantics{
	public:
		static inline peeper::ShaderPeer *getResourcePeer(egg::Resource *resource){return ((peeper::Shader*)resource)->internal_getShaderPeer();}
	};

	typedef ContextResourceManager<peeper::ShaderPeer,ShaderSemantics> ShaderManager;
	ShaderManager *mShaderManager;

	// Program
	class ProgramSemantics{
	public:
		static inline peeper::ProgramPeer *getResourcePeer(egg::Resource *resource){return ((peeper::Program*)resource)->internal_getProgramPeer();}
	};

	typedef ContextResourceManager<peeper::ProgramPeer,ProgramSemantics> ProgramManager;
	ProgramManager *mProgramManager;

	// Material
	MaterialManager *mMaterialManager;

	// Font
	typedef ResourceManager FontManager;
	FontManager *mFontManager;

	// Mesh
	MeshManager *mMeshManager;


	// AudioBuffer
	typedef ResourceManager AudioBufferManager;
	AudioBufferManager *mAudioBufferManager;
	handler::AudioBufferHandler::ptr mAudioBufferHandler;
};

}
}

#endif
