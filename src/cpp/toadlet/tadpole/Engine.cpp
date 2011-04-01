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

/// @todo: OVERALL
// - Sprites and animated meshes need to if(no event frames){deactivate if not being looked at}

// I REALLY NEED TO PLAY WITH THIS, IT WILL LET ME USE A THREADPOOL AND HAUL ASS THROUGH NODE UPDATES
// In theory, each child of a parent node should be updated simultaniously
// Meaning, a child can get the parent node's translate at any time & be correct.
// But a child can not get a sisters or cousins translate at any time & be correct.
//  So nodes should have 2 translates.  The immediate one and the public one.
//  After a node is completely updated, then all public transforms are made to equal the immediate transforms.

// - The node transform should also be buffered.  As in, the order of the nodes to a parent shouldn't affect what order they get updated.
//		So we could have localTransform, and workingLocalTransform.  worldTransform and workingWorldTransform.
//		When we're actually in an update, we would update the workingLocalTransform, and the... FINISH FIGURING THIS OUT
// - Interpolation will be handled by some Interpolator which will do the actual HopEntity interpolation between frames,
//		so we can still have just a Transform(world & local), and the localTransform will be modified by the Interpolator each renderFrame,
//		until a logicFrame, at which point the interpolator should be at the exact position as the solid (or exactly 1 logic dt behind)
// - There are two types of alignments.
//		= Regular Alignments are in the scene, and would be a model pointing at something else, or a billboard pointing a certain direction.
//			Regular Alignments are applied to the rotation of the node.  THIS IS WHAT THE SQUIREL CAMERA IS

// - Interpolation:
//		Can be used by Nodes as they move through space, or by Skeleton's as they move their bones through space.
//		Biggest difference is Skeletons you know all your keyframes.  General nodes you may only know 2, or a few more if we allow it.
//		Should I allow other things besides transforms to be interpolated?  What about general color interpolation?
//		Maybe this needs to be tied into the keyFrame stuff.  We have a keyframe, which can be position, rotation, color, who knows.
//		Then the interpolator will operate on the KeyFrames & time, and change something based on that.
//		Then nodes can be assigned an interpolator...  But how?  Is it a node property?  Or maybe a NodeListener?  This is like the M3G Sequence or whatever.  They have it part of the node.
//		HopEntity's interpolator will listen to the logicFrame.  

/// @todo  I think a good way to let us use EventKeyFrames with the Controller framework is to:
///   1: Have a way of ensuring that the EventKeyFrames are aligned on LogicDT times.
///   2: Have the AnimationControllerNode not issue a renderUpdate until we have enough logicUpdates to keep up with it.
///  That way, entities without EventKeyFrames should update smoothly if something keeps the logic from flowing, but
///   we will still be able to have any logic dependent stuff happen at the correct times.
///
///  Or perhaps, we could use the above tactic for all the update entities.  There is just 1 function, update
///   But it stops being called if logic updates cease due to networking issues.
///   Down sides are that its harder to do logic based updating, since you'd have to handle the checking to see if its
///    been a logicDT yourself.

/// @todo: Force no-scene-graph updates in renderUpdate, to allow us to thread that easily


#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Error.h>
#include <toadlet/peeper/CapabilityState.h>
#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/MeshManager.h>
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/Version.h>

#include <toadlet/tadpole/node/AudioNode.h>
#include <toadlet/tadpole/node/LabelNode.h>
#include <toadlet/tadpole/node/LightNode.h>
#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/tadpole/node/ParticleNode.h>
#include <toadlet/tadpole/node/SpriteNode.h>

#include <toadlet/tadpole/handler/BMPHandler.h>
#include <toadlet/tadpole/handler/DDSHandler.h>
#include <toadlet/tadpole/handler/SPRHandler.h>
#include <toadlet/tadpole/handler/TGAHandler.h>
#include <toadlet/tadpole/handler/TMSHHandler.h>
#include <toadlet/tadpole/handler/TPKGHandler.h>
#include <toadlet/tadpole/handler/WADHandler.h>
#include <toadlet/tadpole/handler/WaveHandler.h>

#if defined(TOADLET_HAS_GDIPLUS)
	#include <toadlet/tadpole/handler/platform/win32/Win32TextureHandler.h>
#endif
#if defined(TOADLET_PLATFORM_OSX)
	#include <toadlet/tadpole/handler/platform/osx/OSXTextureHandler.h>
#endif
#if defined(TOADLET_HAS_GIF)
	#include <toadlet/tadpole/handler/GIFHandler.h>
#endif
#if defined(TOADLET_HAS_JPEG)
	#include <toadlet/tadpole/handler/JPEGHandler.h>
#endif
#if defined(TOADLET_HAS_PNG)
	#include <toadlet/tadpole/handler/PNGHandler.h>
#endif

#if defined(TOADLET_HAS_GDIPLUS)
	#include <toadlet/tadpole/handler/platform/win32/Win32FontHandler.h>
#endif
#if defined(TOADLET_PLATFORM_OSX)
	#include <toadlet/tadpole/handler/platform/osx/OSXFontHandler.h>
#endif
#if defined(TOADLET_HAS_FREETYPE)
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

#if defined(TOADLET_HAS_OGGVORBIS)
	#include <toadlet/tadpole/handler/OggVorbisHandler.h>
#endif
#if defined(TOADLET_HAS_SIDPLAY)
	#include <toadlet/tadpole/handler/SIDHandler.h>
#endif
#if defined(TOADLET_PLATFORM_OSX)
//	#include <toadlet/tadpole/handler/CoreAudioHandler.h>
#endif

#if !defined(TOADLET_FIXED_POINT)
	#include <toadlet/tadpole/bsp/BSP30Node.h>
	#include <toadlet/tadpole/studio/StudioModelNode.h>
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::tadpole::node;
using namespace toadlet::tadpole::handler;

namespace toadlet{
namespace tadpole{

Engine::Engine(bool backable):
	mBackable(false),
	//mDirectory,
	mRenderer(NULL),
	mLastRenderer(NULL),
	mAudioPlayer(NULL),
	mLastAudioPlayer(NULL),

	mContextListener(NULL)
{
	Logger::debug(Categories::TOADLET_TADPOLE,
		String("creating ")+Categories::TOADLET_TADPOLE+".Engine:"+Version::STRING);

	mBackable=backable;

	mArchiveManager=new ArchiveManager();
	mTextureManager=new TextureManager(this,mBackable);
	mBufferManager=new BufferManager(this,mBackable);
	mMaterialManager=new MaterialManager(this,mBackable);
	mFontManager=new FontManager(this->getArchiveManager());
	mMeshManager=new MeshManager(this);
	mAudioBufferManager=new AudioBufferManager(this);
	mNodeManager=new NodeManager(this);

	mHandles.resize(1); // Handle 0 is always NULL

	// Make a guess at what the ideal format is.
	#if defined(TOADLET_FIXED_POINT) && (defined(TOADLET_PLATFORM_WINCE) || defined(TOADLET_PLATFORM_IPHONE) || defined(TOADLET_PLATFORM_ANDROID))
		mIdealVertexFormatBit=VertexFormat::Format_BIT_FIXED_32;
	#else
		mIdealVertexFormatBit=VertexFormat::Format_BIT_FLOAT_32;
	#endif
	// Create initial BackableVertexFormats.  This doesn't need to be done, but without it, starting an application without a Renderer will crash.
	updateVertexFormats();

	registerNodeType(AudioNode::type());
	registerNodeType(CameraNode::type());
	registerNodeType(LabelNode::type());
	registerNodeType(LightNode::type());
	registerNodeType(MeshNode::type());
	registerNodeType(Node::type());
	registerNodeType(ParentNode::type());
	registerNodeType(ParticleNode::type());
	registerNodeType(SpriteNode::type());

	Logger::debug(Categories::TOADLET_TADPOLE,
		"Engine: adding all handlers");

	// Archive handlers
	mArchiveManager->setHandler(TPKGHandler::ptr(new TPKGHandler()),"tpkg");
	mArchiveManager->setHandler(WADHandler::ptr(new WADHandler(mTextureManager)),"wad");
	#if defined(TOADLET_HAS_ZZIP)
		mArchiveManager->setHandler(ZIPHandler::ptr(new ZIPHandler()),"zip");
	#endif

	// Texture handlers
	mTextureManager->setHandler(DDSHandler::ptr(new DDSHandler(mTextureManager)),"dds");
	#if defined(TOADLET_HAS_GDIPLUS)
		mTextureManager->setDefaultHandler(Win32TextureHandler::ptr(new Win32TextureHandler(mTextureManager)));
	#elif defined(TOADLET_PLATFORM_OSX)
		mTextureManager->setHandler(BMPHandler::ptr(new BMPHandler(mTextureManager)),"bmp"); // OSXTextureHandler only handles jpgs & pngs currently, so add our own bmp handler
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
	mTextureManager->setHandler(TGAHandler::ptr(new TGAHandler(mTextureManager)),"tga");

	// Font handlers, try for freetype first, since it currently looks best.  This can be changed back once the others look as nice
	#if defined(TOADLET_HAS_FREETYPE)
		mFontManager->setDefaultHandler(FreeTypeHandler::ptr(new FreeTypeHandler(mTextureManager)));
	#elif defined(TOADLET_HAS_GDIPLUS)
		mFontManager->setDefaultHandler(Win32FontHandler::ptr(new Win32FontHandler(mTextureManager)));
	#elif defined(TOADLET_PLATFORM_OSX)
		mFontManager->setDefaultHandler(OSXFontHandler::ptr(new OSXFontHandler(mTextureManager)));
	#endif

	// Material handlers
	#if defined(TOADLET_HAS_MXML)
		mMaterialManager->setHandler(XMATHandler::ptr(new XMATHandler(this)),"xmat");
	#endif

	// Mesh handlers
	#if defined(TOADLET_HAS_MXML)
		mMeshManager->setHandler(XMSHHandler::ptr(new XMSHHandler(this)),"xmsh");
	#endif
	mMeshManager->setHandler(TMSHHandler::ptr(new TMSHHandler(this)),"tmsh");

	// AudioBuffer handlers
	mAudioBufferManager->setHandler(WaveHandler::ptr(new WaveHandler(mAudioBufferManager)),"wav");
	#if defined(TOADLET_HAS_OGGVORBIS)
		mAudioBufferManager->setHandler(OggVorbisHandler::ptr(new OggVorbisHandler(mAudioBufferManager)),"ogg");
	#endif
	#if defined(TOADLET_HAS_SIDPLAY)
		mAudioBufferManager->setHandler(SIDHandler::ptr(new SIDHandler(mAudioBufferManager)),"sid");
	#endif
	#if defined(TOADLET_PLATFORM_OSX)
		/// @todo: We need to fix the createStream function of AudioBufferManager so it will try the default handler
//		mAudioBufferManager->setDefaultHandler(CoreAudioHandler::ptr(new CoreAudioHandler(mAudioBufferManager)));
	#endif

	// Plugin types, should be removed from here somehow
	#if !defined(TOADLET_FIXED_POINT)
		registerNodeType(bsp::BSP30Node::type());
		registerNodeType(bsp::BSP30ModelNode::type());
		registerNodeType(studio::StudioModelNode::type());
	#endif
}

Engine::~Engine(){
	destroy();

	if(mNodeManager!=NULL){
		delete mNodeManager;
		mNodeManager=NULL;
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

	if(mArchiveManager!=NULL){
		delete mArchiveManager;
		mArchiveManager=NULL;
	}
}

void Engine::destroy(){
	mNodeManager->destroy();
	mAudioBufferManager->destroy();
	mMeshManager->destroy();
	mMaterialManager->destroy();
	mFontManager->destroy();
	mBufferManager->destroy();
	mTextureManager->destroy();
	mArchiveManager->destroy();
}

bool Engine::setRenderer(Renderer *renderer){
	if(renderer!=NULL){
		if(mLastRenderer==NULL){
			mLastRenderer=renderer;
		}
		else if(mBackable==false && mLastRenderer!=renderer){
			Error::unknown(Categories::TOADLET_TADPOLE,"can not change Renderer in an unbacked engine");
			return false;
		}

		const toadlet::peeper::CapabilityState &caps=renderer->getCapabilityState();
		Logger::alert(Categories::TOADLET_TADPOLE,
			"Renderer Capabilities:");
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"maxLights:"+caps.maxLights);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"maxTextureStages:"+caps.maxTextureStages);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"maxTextureSize:"+caps.maxTextureSize);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"hardwareVertexBuffers:"+caps.hardwareVertexBuffers);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"hardwareIndexBuffers:"+caps.hardwareIndexBuffers);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"vertexShaders:"+caps.vertexShaders);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"maxVertexShaderLocalParameters:"+caps.maxVertexShaderLocalParameters);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"fragmentShaders:"+caps.fragmentShaders);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"maxFragmentShaderLocalParameters:"+caps.maxFragmentShaderLocalParameters);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"renderToTexture:"+caps.renderToTexture);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"renderToDepthTexture:"+caps.renderToDepthTexture);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"renderToTextureNonPowerOf2Restricted:"+caps.renderToTextureNonPowerOf2Restricted);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"textureDot3:"+caps.textureDot3);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"textureNonPowerOf2Restricted:"+caps.textureNonPowerOf2Restricted);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"textureNonPowerOf2:"+caps.textureNonPowerOf2);
		Logger::alert(Categories::TOADLET_TADPOLE,
			String()+(char)9+"textureAutogenMipMaps:"+caps.textureAutogenMipMaps);

		mIdealVertexFormatBit=caps.idealVertexFormatBit;
	}

	if(renderer!=mRenderer && mRenderer!=NULL){
		contextDeactivate(mRenderer);
	}
	if(renderer!=mRenderer && renderer!=NULL){
		contextActivate(renderer);
	}

	mRenderer=renderer;

	if(mRenderer!=NULL){
		updateVertexFormats();
	}

	return true;
}

Renderer *Engine::getRenderer() const{
	return mRenderer;
}

void Engine::updateVertexFormats(){
	int formatBit=mIdealVertexFormatBit;

	VertexFormat::ptr format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatBit|VertexFormat::Format_BIT_COUNT_3);
	mVertexFormats.POSITION=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatBit|VertexFormat::Format_BIT_COUNT_3);
	format->addElement(VertexFormat::Semantic_NORMAL,0,formatBit|VertexFormat::Format_BIT_COUNT_3);
	mVertexFormats.POSITION_NORMAL=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatBit|VertexFormat::Format_BIT_COUNT_3);
	format->addElement(VertexFormat::Semantic_COLOR,0,VertexFormat::Format_COLOR_RGBA);
	mVertexFormats.POSITION_COLOR=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatBit|VertexFormat::Format_BIT_COUNT_3);
	format->addElement(VertexFormat::Semantic_TEX_COORD,0,formatBit|VertexFormat::Format_BIT_COUNT_2);
	mVertexFormats.POSITION_TEX_COORD=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatBit|VertexFormat::Format_BIT_COUNT_3);
	format->addElement(VertexFormat::Semantic_NORMAL,0,formatBit|VertexFormat::Format_BIT_COUNT_3);
	format->addElement(VertexFormat::Semantic_COLOR,0,VertexFormat::Format_COLOR_RGBA);
	mVertexFormats.POSITION_NORMAL_COLOR=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatBit|VertexFormat::Format_BIT_COUNT_3);
	format->addElement(VertexFormat::Semantic_NORMAL,0,formatBit|VertexFormat::Format_BIT_COUNT_3);
	format->addElement(VertexFormat::Semantic_TEX_COORD,0,formatBit|VertexFormat::Format_BIT_COUNT_2);
	mVertexFormats.POSITION_NORMAL_TEX_COORD=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatBit|VertexFormat::Format_BIT_COUNT_3);
	format->addElement(VertexFormat::Semantic_COLOR,0,VertexFormat::Format_COLOR_RGBA);
	format->addElement(VertexFormat::Semantic_TEX_COORD,0,formatBit|VertexFormat::Format_BIT_COUNT_2);
	mVertexFormats.POSITION_COLOR_TEX_COORD=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatBit|VertexFormat::Format_BIT_COUNT_3);
	format->addElement(VertexFormat::Semantic_NORMAL,0,formatBit|VertexFormat::Format_BIT_COUNT_3);
	format->addElement(VertexFormat::Semantic_COLOR,0,VertexFormat::Format_COLOR_RGBA);
	format->addElement(VertexFormat::Semantic_TEX_COORD,0,formatBit|VertexFormat::Format_BIT_COUNT_2);
	mVertexFormats.POSITION_NORMAL_COLOR_TEX_COORD=format;
}

bool Engine::setAudioPlayer(AudioPlayer *audioPlayer){
	if(audioPlayer!=NULL){
		if(mLastAudioPlayer==NULL){
			mLastAudioPlayer=audioPlayer;
		}
		else if(mBackable==false && mLastAudioPlayer!=audioPlayer){
			Error::unknown(Categories::TOADLET_TADPOLE,"can not change AudioPlayer in an unbacked engine");
			return false;
		}
	}

	mAudioPlayer=audioPlayer;

	return true;
}

AudioPlayer *Engine::getAudioPlayer() const{
	return mAudioPlayer;
}

void Engine::registerNodeType(BaseType<Node> *type){
	mNodeFactory.registerType(type);
}

/// @todo: Use a pool for these entities
Node *Engine::allocNode(BaseType<Node> *type){
	Logger::excess(Categories::TOADLET_TADPOLE,String("Allocating: ")+type->getFullName());

	Node *node=NULL;
	TOADLET_TRY
		node=type->newInstance();
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

Node *Engine::createNode(BaseType<Node> *type,Scene *scene){
	Node *node=allocNode(type);
	if(node!=NULL){
		if(scene!=NULL){
			node->create(scene);
		}
		else{
			node->create(this);
		}
	}
	return node;
}

Node *Engine::createNode(const String &fullName,Scene *scene){
	Node *node=allocNode(fullName);
	if(node!=NULL){
		node->create(scene);
	}
	return node;
}

int Engine::internal_registerNode(Node *node){
	int handle=node->getUniqueHandle();

	if(handle<=0){
		int size=mFreeHandles.size();
		if(size>0){
			handle=mFreeHandles.at(size-1);
			mFreeHandles.removeAt(size-1);
		}
		else{
			handle=mHandles.size();
			mHandles.resize(handle+1);
		}
		mHandles[handle]=node;
	}

	return handle;
}

void Engine::internal_deregisterNode(Node *node){
	int handle=node->getUniqueHandle();

	if(handle>0){
		mHandles[handle]=NULL;
		mFreeHandles.add(handle);
	}
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

	if(mContextListener!=NULL){
		mContextListener->preContextReset(renderer);
	}

	mBufferManager->preContextReset(renderer);
	mTextureManager->preContextReset(renderer);

	renderer->reset();

	mBufferManager->postContextReset(renderer);
	mTextureManager->postContextReset(renderer);

	if(mContextListener!=NULL){
		mContextListener->postContextReset(renderer);
	}
}

void Engine::contextActivate(Renderer *renderer){
	Logger::debug("Engine::contextActivate");

	if(mContextListener!=NULL){
		mContextListener->preContextActivate(renderer);
	}

	mBufferManager->contextActivate(renderer);
	mTextureManager->contextActivate(renderer);
	mMaterialManager->contextActivate(renderer);

	if(mContextListener!=NULL){
		mContextListener->postContextActivate(renderer);
	}
}

void Engine::contextDeactivate(Renderer *renderer){
	Logger::debug("Engine::contextDeactivate");

	if(mContextListener!=NULL){
		mContextListener->preContextDeactivate(renderer);
	}

	mBufferManager->contextDeactivate(renderer);
	mTextureManager->contextDeactivate(renderer);
	mMaterialManager->contextDeactivate(renderer);

	if(mContextListener!=NULL){
		mContextListener->postContextDeactivate(renderer);
	}
}

}
}
