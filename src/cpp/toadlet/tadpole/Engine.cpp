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
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/MeshManager.h>
#include <toadlet/tadpole/TextureManager.h>

#include <toadlet/tadpole/node/AnimationControllerNode.h>
#include <toadlet/tadpole/node/AudioNode.h>
#include <toadlet/tadpole/node/LabelNode.h>
#include <toadlet/tadpole/node/LightNode.h>
#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/tadpole/node/ParticleNode.h>
#include <toadlet/tadpole/node/SkeletonParentNode.h>
#include <toadlet/tadpole/node/SpriteNode.h>

#include <toadlet/tadpole/handler/AudioBufferHandler.h>
#include <toadlet/tadpole/handler/MMSHHandler.h>
#include <toadlet/tadpole/handler/SPRHandler.h>
#include <toadlet/tadpole/handler/TPKGHandler.h>
#include <toadlet/tadpole/handler/WADHandler.h>

#if defined(TOADLET_PLATFORM_WIN32)
	#include <toadlet/tadpole/handler/platform/win32/Win32TextureHandler.h>
#elif defined(TOADLET_PLATFORM_OSX)
	#include <toadlet/tadpole/handler/platform/osx/OSXTextureHandler.h>
#else
	#include <toadlet/tadpole/handler/BMPHandler.h>
	#if defined(TOADLET_HAS_GIF)
		#include <toadlet/tadpole/handler/GIFHandler.h>
	#endif
	#if defined(TOADLET_HAS_JPEG)
		#include <toadlet/tadpole/handler/JPEGHandler.h>
	#endif
	#if defined(TOADLET_HAS_PNG)
		#include <toadlet/tadpole/handler/PNGHandler.h>
	#endif
#endif

#if defined(TOADLET_PLATFORM_OSX)
	#include <toadlet/tadpole/handler/platform/osx/OSXFontHandler.h>
#elif defined(TOADLET_HAS_FREETYPE)
	#include <toadlet/tadpole/handler/FreeTypeHandler.h>
#endif

#if defined(TOADLET_HAS_ZZIP)
	#include <toadlet/tadpole/handler/ZIPHandler.h>
#endif

#if defined(TOADLET_HAS_MXML)
	#include <toadlet/tadpole/handler/XANMHandler.h>
	#include <toadlet/tadpole/handler/XMATHandler.h>
	#include <toadlet/tadpole/handler/XMSHHandler.h>
#endif

#if !defined(TOADLET_FIXED_POINT)
	#include <toadlet/tadpole/bsp/BSP30SceneNode.h>
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::tadpole::node;
using namespace toadlet::tadpole::handler;
using namespace toadlet::tadpole::mesh;

namespace toadlet{
namespace tadpole{

Engine::Engine():
	//mDirectory,
	//mScene,
	mRenderer(NULL),
	mAudioPlayer(NULL)
{
	Logger::debug(Categories::TOADLET_TADPOLE,
		"creating Engine");

	// Make a guess at what the ideal format is.
	#if defined(TOADLET_FIXED_POINT) && (defined(TOADLET_PLATFORM_WINCE) || defined(TOADLET_PLATFORM_IPHONE) || defined(TOADLET_PLATFORM_ANDROID))
		mIdealVertexFormatBit=VertexElement::Format_BIT_FIXED_32;
	#else
		mIdealVertexFormatBit=VertexElement::Format_BIT_FLOAT_32;
	#endif
	updateVertexFormats();

	mArchiveManager=new ArchiveManager();
	mTextureManager=new TextureManager(this);
	mBufferManager=new BufferManager(this);
	mMaterialManager=new MaterialManager(this);
	mFontManager=new FontManager(this->getArchiveManager());
	mMeshManager=new MeshManager(this);
	mAudioBufferManager=new ResourceManager(this->getArchiveManager());

	registerNodeType(AnimationControllerNode::type());
	registerNodeType(AudioNode::type());
	registerNodeType(CameraNode::type());
	registerNodeType(LabelNode::type());
	registerNodeType(LightNode::type());
	registerNodeType(MeshNode::type());
	registerNodeType(Node::type());
	registerNodeType(ParentNode::type());
	registerNodeType(ParticleNode::type());
	registerNodeType(SkeletonParentNode::type());
	registerNodeType(SpriteNode::type());

	// Plugin types, should be removed from here somehow
	#if !defined(TOADLET_FIXED_POINT)
		registerNodeType(bsp::BSP30SceneNode::type());
		registerNodeType(bsp::BSP30ModelNode::type());
	#endif

	Logger::debug(Categories::TOADLET_TADPOLE,
		"Engine: adding all handlers");

	// Archive handlers
	mArchiveManager->setHandler(TPKGHandler::ptr(new TPKGHandler()),"tpkg");
	mArchiveManager->setHandler(WADHandler::ptr(new WADHandler(mTextureManager)),"wad");
	#if defined(TOADLET_HAS_ZZIP)
		ZIPHandler::ptr zipHandler(new ZIPHandler());
		mArchiveManager->setHandler(zipHandler,"zip");
	#endif

	// Texture handlers
	#if defined(TOADLET_HAS_GDIPLUS)
		mTextureManager->setDefaultHandler(Win32TextureHandler::ptr(new Win32TextureHandler(mTextureManager)));
	#elif defined(TOADLET_PLATFORM_OSX)
		mTextureManager->setDefaultHandler(OSXTextureHandler::ptr(new OSXTextureHandler(mTextureManager)));
	#else
		mTextureManager->setHandler(BMPHandler::ptr(new BMPHandler(mTextureManager)),"bmp");
		#if defined(TOADLET_HAS_GIF)
			mTextureManager->setHandler(GIFHandler::ptr(new GIFHandler(mTextureManager)),"gif");
		#endif
		#if defined(TOADLET_HAS_JPEG)
			JPEGHandler::ptr jpegHandler(new JPEGHandler(mTextureManager));
			mTextureManager->setHandler(jpegHandler,"jpeg");
			mTextureManager->setHandler(jpegHandler,"jpg");
		#endif
		#if defined(TOADLET_HAS_PNG)
			mTextureManager->setHandler(PNGHandler::ptr(new PNGHandler(mTextureManager)),"png");
		#endif
	#endif
	mTextureManager->setHandler(SPRHandler::ptr(new SPRHandler(mTextureManager)),"spr");

	// Font handlers
	#if defined(TOADLET_PLATFORM_OSX)
		OSXFontHandler::ptr osxFontHandler(new OSXFontHandler(mTextureManager));
		mFontManager->setHandler(osxFontHandler,"ttf");
		mFontManager->setHandler(osxFontHandler,"dfont");
	#elif defined(TOADLET_HAS_FREETYPE)
		FreeTypeHandler::ptr freeTypeHandler(new FreeTypeHandler(mTextureManager));
		mFontManager->setHandler(freeTypeHandler,"ttf");
		mFontManager->setHandler(freeTypeHandler,"dfont");
	#endif

	// Material handlers
	#if defined(TOADLET_HAS_MXML)
		mMaterialManager->setHandler(XMATHandler::ptr(new XMATHandler(mMaterialManager,mTextureManager)),"xmat");
	#endif

	// Mesh handlers
	#if defined(TOADLET_HAS_MXML)
		mMeshManager->setHandler(XMSHHandler::ptr(new XMSHHandler(mBufferManager,mMaterialManager,mTextureManager)),"xmsh");
	#endif

	mMeshManager->setHandler(MMSHHandler::ptr(new MMSHHandler(this)),"mmsh");

	// AudioBuffer handlers
	mAudioBufferHandler=AudioBufferHandler::ptr(new AudioBufferHandler(mAudioPlayer));
	mAudioBufferManager->setDefaultHandler(mAudioBufferHandler);
}

Engine::~Engine(){
	destroy();

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

	if(mArchiveManager!=NULL){
		delete mArchiveManager;
		mArchiveManager=NULL;
	}
}

void Engine::destroy(){
	if(mScene!=NULL){
		mScene->destroy();
		mScene=NULL;
	}

	mAudioBufferManager->destroy();
	mMeshManager->destroy();
	mMaterialManager->destroy();
	mFontManager->destroy();
	mBufferManager->destroy();
	mTextureManager->destroy();
	mArchiveManager->destroy();
}

void Engine::setScene(const Scene::ptr &scene){
	mScene=scene;
}

const Scene::ptr &Engine::getScene() const{
	return mScene;
}

void Engine::setRenderer(Renderer *renderer){
	if(renderer!=NULL){
		const toadlet::peeper::CapabilitySet &capabilitySet=renderer->getCapabilitySet();
		Logger::alert(Categories::TOADLET_TADPOLE,
			"Renderer Capabilities:");
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"maxLights:"+capabilitySet.maxLights);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"maxTextureStages:"+capabilitySet.maxTextureStages);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"maxTextureSize:"+capabilitySet.maxTextureSize);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"hardwareVertexBuffers:"+capabilitySet.hardwareVertexBuffers);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"hardwareIndexBuffers:"+capabilitySet.hardwareIndexBuffers);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"vertexShaders:"+capabilitySet.vertexShaders);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"maxVertexShaderLocalParameters:"+capabilitySet.maxVertexShaderLocalParameters);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"fragmentShaders:"+capabilitySet.fragmentShaders);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"maxFragmentShaderLocalParameters:"+capabilitySet.maxFragmentShaderLocalParameters);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"renderToTexture:"+capabilitySet.renderToTexture);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"renderToDepthTexture:"+capabilitySet.renderToDepthTexture);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"renderToTextureNonPowerOf2Restricted:"+capabilitySet.renderToTextureNonPowerOf2Restricted);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"textureDot3:"+capabilitySet.textureDot3);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"textureNonPowerOf2Restricted:"+capabilitySet.textureNonPowerOf2Restricted);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"textureNonPowerOf2:"+capabilitySet.textureNonPowerOf2);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"textureAutogenMipMaps:"+capabilitySet.textureAutogenMipMaps);
	}

	if(renderer!=mRenderer && mRenderer!=NULL){
		contextDeactivate(mRenderer);
	}
	if(renderer!=mRenderer && renderer!=NULL){
		contextActivate(renderer);

		mIdealVertexFormatBit=renderer->getCapabilitySet().idealVertexFormatBit;
		updateVertexFormats();
	}

	mRenderer=renderer;
}

Renderer *Engine::getRenderer() const{
	return mRenderer;
}

void Engine::updateVertexFormats(){
	int formatBit=mIdealVertexFormatBit;

	VertexElement position(VertexElement::Type_POSITION,formatBit|VertexElement::Format_BIT_COUNT_3);
	VertexElement normal(VertexElement::Type_NORMAL,formatBit|VertexElement::Format_BIT_COUNT_3);
	VertexElement color(VertexElement::Type_COLOR_DIFFUSE,VertexElement::Format_COLOR_RGBA);
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

void Engine::registerNodeType(const BaseType<Node> &type){
	mNodeFactory.registerType(type);
}

// TODO: Use a pool for these entities
Node *Engine::allocNode(const BaseType<Node> &type){
	Logger::excess(Categories::TOADLET_TADPOLE,String("Allocating: ")+type.getFullName());

	Node *node=NULL;
	TOADLET_TRY
		node=type.newInstance();
	TOADLET_CATCH(const Exception &){node=NULL;}
	if(node!=NULL){
		node->internal_setManaged(true);
	}
	return node;
}

Node *Engine::allocNode(const String &fullName){
	Logger::excess(Categories::TOADLET_TADPOLE,String("Allocating: ")+fullName);

	Node *node=NULL;
	TOADLET_TRY
		node=mNodeFactory.newInstance(fullName);
	TOADLET_CATCH(const Exception &){node=NULL;}
	if(node!=NULL){
		node->internal_setManaged(true);
	}
	return node;
}

Node *Engine::createNode(const BaseType<Node> &type){
	Node *node=allocNode(type);
	if(node!=NULL){
		node->create(this);
	}
	return node;
}

Node *Engine::createNode(const String &fullName){
	Node *node=allocNode(fullName);
	if(node!=NULL){
		node->create(this);
	}
	return node;
}

void Engine::destroyNode(Node *node){
	if(node->created()){
		node->destroy();
	}
}

void Engine::freeNode(Node *node){
	if(node->created()){
		Error::unknown("freeing undestroyed node");
		return;
	}
	if(node->internal_getManaged()){
		node->internal_setManaged(false);
	}
}

// Context methods
void Engine::contextReset(peeper::Renderer *renderer){
	Logger::debug("Engine::contextReset");

	mBufferManager->preContextReset(renderer);
	mTextureManager->preContextReset(renderer);

	renderer->reset();

	mBufferManager->postContextReset(renderer);
	mTextureManager->postContextReset(renderer);
}

void Engine::contextActivate(Renderer *renderer){
	Logger::debug("Engine::contextActivate");

	mBufferManager->contextActivate(renderer);
	mTextureManager->contextActivate(renderer);
}

void Engine::contextDeactivate(Renderer *renderer){
	Logger::debug("Engine::contextDeactivate");

	mBufferManager->contextDeactivate(renderer);
	mTextureManager->contextDeactivate(renderer);
}

}
}
