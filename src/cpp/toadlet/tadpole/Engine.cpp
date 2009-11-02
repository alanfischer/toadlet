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

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/io/InputStream.h>
#include <toadlet/egg/io/OutputStream.h>
#include <toadlet/egg/io/FileInputStream.h>
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/MeshManager.h>
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/handler/BMPHandler.h>
#include <toadlet/tadpole/handler/RGBHandler.h>
#include <toadlet/tadpole/handler/SPRHandler.h>
#include <toadlet/tadpole/handler/MMSHHandler.h>
#include <toadlet/tadpole/handler/AudioBufferHandler.h>

#if defined(TOADLET_HAS_GIF)
	#include <toadlet/tadpole/handler/GIFHandler.h>
#endif
#if defined(TOADLET_HAS_JPEG)
	#include <toadlet/tadpole/handler/JPEGHandler.h>
#endif
#if defined(TOADLET_HAS_PNG)
	#include <toadlet/tadpole/handler/PNGHandler.h>
#endif
#if defined(TOADLET_PLATFORM_OSX)
	#include <toadlet/tadpole/handler/platform/osx/OSXFontHandler.h>
	#include <toadlet/tadpole/handler/platform/osx/OSXTextureHandler.h>
#elif defined(TOADLET_HAS_FREETYPE)
	#include <toadlet/tadpole/handler/FreeTypeHandler.h>
#endif
#if defined(TOADLET_HAS_MXML)
	#include <toadlet/tadpole/handler/XANMHandler.h>
	#include <toadlet/tadpole/handler/XMATHandler.h>
	#include <toadlet/tadpole/handler/XMSHHandler.h>
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::tadpole::entity;
using namespace toadlet::tadpole::handler;

namespace toadlet{
namespace tadpole{

Engine::Engine():
	//mDirectory,
	mInputStreamFactory(NULL),
	//mScene,
	mRenderer(NULL),
	mAudioPlayer(NULL)
{
	Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_DEBUG,
		"Engine::Engine started");

	mTextureManager=new TextureManager(this);

	// Make a guess at what the ideal format is.
	#if defined(TOADLET_FIXED_POINT) && (defined(TOADLET_PLATFORM_WINCE) || defined(TOADLET_PLATFORM_IPHONE) || defined(TOADLET_PLATFORM_ANDROID))
		mIdealFormatBit=VertexElement::Format_BIT_FIXED_32;
	#else
		mIdealFormatBit=VertexElement::Format_BIT_FLOAT_32;
	#endif
	updateVertexFormats();

	mBufferManager=new BufferManager(this);

	mMaterialManager=new MaterialManager(this,mTextureManager);

	mFontManager=new ResourceManager(this);

	mMeshManager=new MeshManager(this);

	mAudioBufferManager=new ResourceManager(this);


	Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_DEBUG,
		"Engine: adding all handlers");

	// Texture handlers
	mTextureManager->addHandler(BMPHandler::ptr(new BMPHandler(mTextureManager)),"bmp");

	mTextureManager->addHandler(RGBHandler::ptr(new RGBHandler(mTextureManager)),"rgb");

	mTextureManager->addHandler(SPRHandler::ptr(new SPRHandler(mTextureManager)),"spr");

	#if defined(TOADLET_HAS_GIF)
		mTextureManager->addHandler(GIFHandler::ptr(new GIFHandler(mTextureManager)),"gif");
	#endif

	#if defined(TOADLET_HAS_JPEG)
		JPEGHandler::ptr jpegHandler(new JPEGHandler(mTextureManager));
		mTextureManager->addHandler(jpegHandler,"jpeg");
		mTextureManager->addHandler(jpegHandler,"jpg");
	#endif

	#if defined(TOADLET_HAS_PNG)
		mTextureManager->addHandler(PNGHandler::ptr(new PNGHandler(mTextureManager)),"png");
	#elif defined(TOADLET_PLATFORM_OSX)
		mTextureManager->addHandler(OSXTextureHandler::ptr(new OSXTextureHandler(mTextureManager)),"png");
	#endif

	// Font handlers
	#if defined(TOADLET_PLATFORM_OSX)
		OSXFontHandler::ptr osxFontHandler(new OSXFontHandler(mTextureManager));
		mFontManager->addHandler(osxFontHandler,"ttf");
		mFontManager->addHandler(osxFontHandler,"dfont");
	#elif defined(TOADLET_HAS_FREETYPE)
		FreeTypeHandler::ptr freeTypeHandler(new FreeTypeHandler(mTextureManager));
		mFontManager->addHandler(freeTypeHandler,"ttf");
		mFontManager->addHandler(freeTypeHandler,"dfont");
	#endif

	// Material handlers
	#if defined(TOADLET_HAS_MXML)
		mMaterialManager->addHandler(XMATHandler::ptr(new XMATHandler(mTextureManager)),"xmat");
	#endif

	// Mesh handlers
	#if defined(TOADLET_HAS_MXML)
		mMeshManager->addHandler(XMSHHandler::ptr(new XMSHHandler(mBufferManager,mMaterialManager,mTextureManager)),"xmsh");
	#endif

	mMeshManager->addHandler(MMSHHandler::ptr(new MMSHHandler(mBufferManager,this)),"mmsh");

	// AudioBuffer handlers
	mAudioBufferHandler=AudioBufferHandler::ptr(new AudioBufferHandler(mAudioPlayer));
	mAudioBufferManager->setDefaultHandler(mAudioBufferHandler);
}

Engine::~Engine(){
	if(mScene!=NULL){
		mScene->destroy();
		mScene=NULL;
	}

	if(mAudioBufferManager!=NULL){
		delete mAudioBufferManager;
		mAudioBufferManager=NULL;
	}

	if(mMeshManager!=NULL){
		delete mMeshManager;
		mMeshManager=NULL;
	}

	if(mMaterialManager!=NULL){
		delete mMaterialManager;
		mMaterialManager=NULL;
	}

	if(mFontManager!=NULL){
		delete mFontManager;
		mFontManager=NULL;
	}

	if(mBufferManager!=NULL){
		delete mBufferManager;
		mBufferManager=NULL;
	}

	if(mTextureManager!=NULL){
		delete mTextureManager;
		mTextureManager=NULL;
	}
}

void Engine::setScene(const Scene::ptr &scene){
	mScene=scene;
}

const Scene::ptr &Engine::getScene() const{
	return mScene;
}

void Engine::setInputStreamFactory(InputStreamFactory *inputStreamFactory){
	mInputStreamFactory=inputStreamFactory;
}

InputStreamFactory *Engine::getInputStreamFactory() const{
	return mInputStreamFactory;
}

void Engine::setRenderer(Renderer *renderer){
	if(renderer!=NULL){
		const toadlet::peeper::CapabilitySet &capabilitySet=renderer->getCapabilitySet();
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			"Renderer Capabilities:");
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"maxLights:"+capabilitySet.maxLights);
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"maxTextureStages:"+capabilitySet.maxTextureStages);
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"maxTextureSize:"+capabilitySet.maxTextureSize);
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"hardwareVertexBuffers:"+capabilitySet.hardwareVertexBuffers);
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"hardwareIndexBuffers:"+capabilitySet.hardwareIndexBuffers);
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"vertexShaders:"+capabilitySet.vertexShaders);
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"maxVertexShaderLocalParameters:"+capabilitySet.maxVertexShaderLocalParameters);
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"fragmentShaders:"+capabilitySet.fragmentShaders);
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"maxFragmentShaderLocalParameters:"+capabilitySet.maxFragmentShaderLocalParameters);
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"renderToTexture:"+capabilitySet.renderToTexture);
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"renderToDepthTexture:"+capabilitySet.renderToDepthTexture);
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"renderToTextureNonPowerOf2Restricted:"+capabilitySet.renderToTextureNonPowerOf2Restricted);
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"textureDot3:"+capabilitySet.textureDot3);
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"textureNonPowerOf2Restricted:"+capabilitySet.textureNonPowerOf2Restricted);
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,
			String()+(char)9+"textureNonPowerOf2:"+capabilitySet.textureNonPowerOf2);
	}

	if(renderer!=mRenderer && mRenderer!=NULL){
		contextDeactivate(mRenderer);
	}
	if(renderer!=mRenderer && renderer!=NULL){
		contextActivate(renderer);

		mIdealFormatBit=renderer->getCapabilitySet().idealFormatBit;
		updateVertexFormats();
	}

	mRenderer=renderer;
}

Renderer *Engine::getRenderer() const{
	return mRenderer;
}

void Engine::updateVertexFormats(){
	int formatBit=mIdealFormatBit;

	VertexElement position(VertexElement::Type_POSITION,formatBit|VertexElement::Format_BIT_COUNT_3);
	VertexElement normal(VertexElement::Type_NORMAL,formatBit|VertexElement::Format_BIT_COUNT_3);
	VertexElement color(VertexElement::Type_COLOR,VertexElement::Format_COLOR_RGBA);
	VertexElement texCoord(VertexElement::Type_TEX_COORD,formatBit|VertexElement::Format_BIT_COUNT_2);

	VertexFormat::ptr format;

	format=VertexFormat::ptr(new VertexFormat(1));
	format->addVertexElement(position);
	mVertexFormats.POSITION=format;

	format=VertexFormat::ptr(new VertexFormat(2));
	format->addVertexElement(position);
	format->addVertexElement(normal);
	mVertexFormats.POSITION_NORMAL=format;

	format=VertexFormat::ptr(new VertexFormat(2));
	format->addVertexElement(position);
	format->addVertexElement(color);
	mVertexFormats.POSITION_COLOR=format;

	format=VertexFormat::ptr(new VertexFormat(2));
	format->addVertexElement(position);
	format->addVertexElement(texCoord);
	mVertexFormats.POSITION_TEX_COORD=format;

	format=VertexFormat::ptr(new VertexFormat(3));
	format->addVertexElement(position);
	format->addVertexElement(normal);
	format->addVertexElement(color);
	mVertexFormats.POSITION_NORMAL_COLOR=format;

	format=VertexFormat::ptr(new VertexFormat(3));
	format->addVertexElement(position);
	format->addVertexElement(normal);
	format->addVertexElement(texCoord);
	mVertexFormats.POSITION_NORMAL_TEX_COORD=format;

	format=VertexFormat::ptr(new VertexFormat(3));
	format->addVertexElement(position);
	format->addVertexElement(color);
	format->addVertexElement(texCoord);
	mVertexFormats.POSITION_COLOR_TEX_COORD=format;

	format=VertexFormat::ptr(new VertexFormat(4));
	format->addVertexElement(position);
	format->addVertexElement(normal);
	format->addVertexElement(color);
	format->addVertexElement(texCoord);
	mVertexFormats.POSITION_NORMAL_COLOR_TEX_COORD=format;
}

void Engine::setAudioPlayer(AudioPlayer *audioPlayer){
	mAudioPlayer=audioPlayer;
	shared_static_cast<AudioBufferHandler>(mAudioBufferHandler)->setAudioPlayer(mAudioPlayer);
}

AudioPlayer *Engine::getAudioPlayer() const{
	return mAudioPlayer;
}

void Engine::setDirectory(const String &directory){
	if(directory.length()>0){
		mDirectory=directory+"/";
	}
	else{
		mDirectory=directory;
	}
}

const String &Engine::getDirectory() const{
	return mDirectory;
}

InputStream::ptr Engine::makeInputStream(const String &name){
	InputStream::ptr in;
	if(mInputStreamFactory!=NULL){
		in=mInputStreamFactory->makeInputStream(name);
	}
	else{
		FileInputStream::ptr fin(new FileInputStream(mDirectory+name));
		if(fin->isOpen()){
			in=fin;
		}
	}
	return in;
}


// Context methods
void Engine::contextActivate(Renderer *renderer){
	TOADLET_ASSERT(renderer!=NULL);

	Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_DEBUG,
		"Engine::contextActivate");
}

void Engine::contextDeactivate(Renderer *renderer){
	TOADLET_ASSERT(renderer!=NULL);

	Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_DEBUG,
		"Engine::contextDeactivate");
}

/// @todo   I have to implement the resource freeing after context loading, and then have a method of reloading the resource data if the context is destroyed.  Possibily getting it from its file handle, or letting the user supply some other factory method
void Engine::contextUpdate(Renderer *renderer){
	TOADLET_ASSERT(renderer!=NULL);
}

// Resource methods
// Texture
Texture::ptr Engine::loadTexture(const String &name){
	return shared_static_cast<Texture>(mTextureManager->load(name));
}

Texture::ptr Engine::loadTexture(const String &name,const String &file){
	return shared_static_cast<Texture>(mTextureManager->load(name,file));
}

Texture::ptr Engine::loadTexture(const Texture::ptr &resource){
	return shared_static_cast<Texture>(mTextureManager->load(resource));
}

Texture::ptr Engine::loadTexture(const String &name,const Texture::ptr &resource){
	return shared_static_cast<Texture>(mTextureManager->load(name,resource));
}

Texture::ptr Engine::cacheTexture(const String &name){
	return shared_static_cast<Texture>(mTextureManager->cache(name));
}

Texture::ptr Engine::cacheTexture(const String &name,const String &file){
	return shared_static_cast<Texture>(mTextureManager->cache(name,file));
}

Texture::ptr Engine::cacheTexture(const Texture::ptr &resource){
	return shared_static_cast<Texture>(mTextureManager->cache(resource));
}

Texture::ptr Engine::cacheTexture(const String &name,const Texture::ptr &resource){
	return shared_static_cast<Texture>(mTextureManager->cache(name,resource));
}

bool Engine::uncacheTexture(const String &name){
	return mTextureManager->uncache(name);
}

bool Engine::uncacheTexture(const Texture::ptr &resource){
	return mTextureManager->uncache(resource);
}

Texture::ptr Engine::getTexture(const String &name) const{
	return shared_static_cast<Texture>(mTextureManager->get(name));
}

// Buffer

// Shader
Shader::ptr Engine::loadShader(const String &name){
	return shared_static_cast<Shader>(mShaderManager->load(name));
}

Shader::ptr Engine::loadShader(const String &name,const String &file){
	return shared_static_cast<Shader>(mShaderManager->load(name,file));
}

Shader::ptr Engine::loadShader(const Shader::ptr &resource){
	return shared_static_cast<Shader>(mShaderManager->load(resource));
}

Shader::ptr Engine::loadShader(const String &name,const Shader::ptr &resource){
	return shared_static_cast<Shader>(mShaderManager->load(name,resource));
}

Shader::ptr Engine::cacheShader(const String &name){
	return shared_static_cast<Shader>(mShaderManager->cache(name));
}

Shader::ptr Engine::cacheShader(const String &name,const String &file){
	return shared_static_cast<Shader>(mShaderManager->cache(name,file));
}

Shader::ptr Engine::cacheShader(const Shader::ptr &resource){
	return shared_static_cast<Shader>(mShaderManager->cache(resource));
}

Shader::ptr Engine::cacheShader(const String &name,const Shader::ptr &resource){
	return shared_static_cast<Shader>(mShaderManager->cache(name,resource));
}

bool Engine::uncacheShader(const String &name){
	return mShaderManager->uncache(name);
}

bool Engine::uncacheShader(const Shader::ptr &resource){
	return mShaderManager->uncache(resource);
}

Shader::ptr Engine::getShader(const String &name) const{
	return shared_static_cast<Shader>(mShaderManager->get(name));
}

// Program
Program::ptr Engine::loadProgram(const String &name){
	return shared_static_cast<Program>(mProgramManager->load(name));
}

Program::ptr Engine::loadProgram(const String &name,const String &file){
	return shared_static_cast<Program>(mProgramManager->load(name,file));
}

Program::ptr Engine::loadProgram(const Program::ptr &resource){
	return shared_static_cast<Program>(mProgramManager->load(resource));
}

Program::ptr Engine::loadProgram(const String &name,const Program::ptr &resource){
	return shared_static_cast<Program>(mProgramManager->load(name,resource));
}

Program::ptr Engine::cacheProgram(const String &name){
	return shared_static_cast<Program>(mProgramManager->cache(name));
}

Program::ptr Engine::cacheProgram(const String &name,const String &file){
	return shared_static_cast<Program>(mProgramManager->cache(name,file));
}

Program::ptr Engine::cacheProgram(const Program::ptr &resource){
	return shared_static_cast<Program>(mProgramManager->cache(resource));
}

Program::ptr Engine::cacheProgram(const String &name,const Program::ptr &resource){
	return shared_static_cast<Program>(mProgramManager->cache(name,resource));
}

bool Engine::uncacheProgram(const String &name){
	return mProgramManager->uncache(name);
}

bool Engine::uncacheProgram(const Program::ptr &resource){
	return mProgramManager->uncache(resource);
}

Program::ptr Engine::getProgram(const String &name) const{
	return shared_static_cast<Program>(mProgramManager->get(name));
}

// Font
Font::ptr Engine::loadFont(const String &name,const FontData::ptr &data){
	return shared_static_cast<Font>(mFontManager->load(name,data));
}

Font::ptr Engine::loadFont(const String &name,const String &file,const FontData::ptr &data){
	return shared_static_cast<Font>(mFontManager->load(name,file,data));
}

Font::ptr Engine::loadFont(const Font::ptr &resource){
	return shared_static_cast<Font>(mFontManager->load(resource));
}

Font::ptr Engine::cacheFont(const String &name,const FontData::ptr &data){
	return shared_static_cast<Font>(mFontManager->cache(name,data));
}

Font::ptr Engine::cacheFont(const String &name,const String &file,const FontData::ptr &data){
	return shared_static_cast<Font>(mFontManager->cache(name,file,data));
}

Font::ptr Engine::cacheFont(const Font::ptr &resource){
	return shared_static_cast<Font>(mFontManager->cache(resource));
}

bool Engine::uncacheFont(const String &name){
	return mFontManager->uncache(name);
}

bool Engine::uncacheFont(const Font::ptr &resource){
	return mFontManager->uncache(resource);
}

Font::ptr Engine::getFont(const String &name) const{
	return shared_static_cast<Font>(mFontManager->get(name));
}

// Material
Material::ptr Engine::loadMaterial(const String &name){
	return shared_static_cast<Material>(mMaterialManager->load(name));
}

Material::ptr Engine::loadMaterial(const String &name,const String &file){
	return shared_static_cast<Material>(mMaterialManager->load(name,file));
}

Material::ptr Engine::loadMaterial(const Material::ptr &resource){
	return shared_static_cast<Material>(mMaterialManager->load(resource));
}

Material::ptr Engine::cacheMaterial(const String &name){
	return shared_static_cast<Material>(mMaterialManager->cache(name));
}

Material::ptr Engine::cacheMaterial(const String &name,const String &file){
	return shared_static_cast<Material>(mMaterialManager->cache(name,file));
}

Material::ptr Engine::cacheMaterial(const Material::ptr &resource){
	return shared_static_cast<Material>(mMaterialManager->cache(resource));
}

bool Engine::uncacheMaterial(const String &name){
	return mMaterialManager->uncache(name);
}

bool Engine::uncacheMaterial(const Material::ptr &resource){
	return mMaterialManager->uncache(resource);
}

Material::ptr Engine::getMaterial(const String &name) const{
	return shared_static_cast<Material>(mMaterialManager->get(name));
}

// Mesh
Mesh::ptr Engine::loadMesh(const String &name){
	return shared_static_cast<Mesh>(mMeshManager->load(name));
}

Mesh::ptr Engine::loadMesh(const String &name,const String &file){
	return shared_static_cast<Mesh>(mMeshManager->load(name,file));
}

Mesh::ptr Engine::loadMesh(const Mesh::ptr &resource){
	return shared_static_cast<Mesh>(mMeshManager->load(resource));
}

Mesh::ptr Engine::loadMesh(const String &name,const Mesh::ptr &resource){
	return shared_static_cast<Mesh>(mMeshManager->load(name,resource));
}

Mesh::ptr Engine::cacheMesh(const String &name){
	return shared_static_cast<Mesh>(mMeshManager->cache(name));
}

Mesh::ptr Engine::cacheMesh(const String &name,const String &file){
	return shared_static_cast<Mesh>(mMeshManager->cache(name,file));
}

Mesh::ptr Engine::cacheMesh(const Mesh::ptr &resource){
	return shared_static_cast<Mesh>(mMeshManager->cache(resource));
}

Mesh::ptr Engine::cacheMesh(const String &name,const Mesh::ptr &resource){
	return shared_static_cast<Mesh>(mMeshManager->cache(name,resource));
}

bool Engine::uncacheMesh(const String &name){
	return mMeshManager->uncache(name);
}

bool Engine::uncacheMesh(const Mesh::ptr &resource){
	return mMeshManager->uncache(resource);
}

Mesh::ptr Engine::getMesh(const String &name) const{
	return shared_static_cast<Mesh>(mMeshManager->get(name));
}

// AudioBuffer
AudioBuffer::ptr Engine::loadAudioBuffer(const String &name,const AudioBufferData::ptr &data){
	return shared_static_cast<AudioBuffer>(mAudioBufferManager->load(name,data));
}

AudioBuffer::ptr Engine::loadAudioBuffer(const String &name,const String &file,const AudioBufferData::ptr &data){
	return shared_static_cast<AudioBuffer>(mAudioBufferManager->load(name,file,data));
}

AudioBuffer::ptr Engine::loadAudioBuffer(const AudioBuffer::ptr &resource){
	return shared_static_cast<AudioBuffer>(mAudioBufferManager->load(resource));
}

AudioBuffer::ptr Engine::loadAudioBuffer(const String &name,const AudioBuffer::ptr &resource){
	return shared_static_cast<AudioBuffer>(mAudioBufferManager->load(name,resource));
}

AudioBuffer::ptr Engine::cacheAudioBuffer(const String &name,const AudioBufferData::ptr &data){
	return shared_static_cast<AudioBuffer>(mAudioBufferManager->cache(name,data));
}

AudioBuffer::ptr Engine::cacheAudioBuffer(const String &name,const String &file,const AudioBufferData::ptr &data){
	return shared_static_cast<AudioBuffer>(mAudioBufferManager->cache(name,file,data));
}

AudioBuffer::ptr Engine::cacheAudioBuffer(const AudioBuffer::ptr &resource){
	return shared_static_cast<AudioBuffer>(mAudioBufferManager->cache(resource));
}

AudioBuffer::ptr Engine::cacheAudioBuffer(const String &name,const AudioBuffer::ptr &resource){
	return shared_static_cast<AudioBuffer>(mAudioBufferManager->cache(name,resource));
}

bool Engine::uncacheAudioBuffer(const String &name){
	return mAudioBufferManager->uncache(name);
}

bool Engine::uncacheAudioBuffer(const AudioBuffer::ptr &resource){
	return mAudioBufferManager->uncache(resource);
}

AudioBuffer::ptr Engine::getAudioBuffer(const String &name) const{
	return shared_static_cast<AudioBuffer>(mAudioBufferManager->get(name));
}

}
}
