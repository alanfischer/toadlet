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
#include <toadlet/peeper/RenderCaps.h>
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

#include <toadlet/tadpole/creator/AABoxMeshCreator.h>
#include <toadlet/tadpole/creator/SkyBoxMeshCreator.h>
#include <toadlet/tadpole/creator/SkyDomeMeshCreator.h>
#include <toadlet/tadpole/creator/SphereMeshCreator.h>
#include <toadlet/tadpole/creator/GridMeshCreator.h>
#include <toadlet/tadpole/creator/NormalizationTextureCreator.h>
#include <toadlet/tadpole/creator/DiffuseMaterialCreator.h>
#include <toadlet/tadpole/creator/SkyBoxMaterialCreator.h>

#include <toadlet/tadpole/handler/BMPHandler.h>
#include <toadlet/tadpole/handler/DDSHandler.h>
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
#if defined(TOADLET_HAS_FFMPEG)
	#include <toadlet/tadpole/handler/FFmpegVideoHandler.h>
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

#if !defined(TOADLET_FIXED_POINT)
	#include <toadlet/tadpole/handler/SPRHandler.h>
	#include <toadlet/tadpole/bsp/BSP30Node.h>
	#include <toadlet/tadpole/studio/StudioModelNode.h>
	#include <toadlet/tadpole/studio/SpriteModelNode.h>
#endif

namespace toadlet{
namespace tadpole{

Engine::Engine(bool fixedBackable,bool shaderBackable):
	mFixedBackable(false),
	mShaderBackable(false),
	//mDirectory,
	mRenderDevice(NULL),
	mRenderDeviceChanged(false),
	mAudioDevice(NULL),
	mAudioDeviceChanged(false)

	//mContextListeners
{
	Logger *logger=Logger::getInstance();
	logger->addCategory(Categories::TOADLET_EGG_LOGGER);
	logger->addCategory(Categories::TOADLET_EGG_NET);
	logger->setCategoryReportingLevel(Categories::TOADLET_EGG_NET,Logger::Level_DISABLED); // Don't log socket errors
	logger->addCategory(Categories::TOADLET_EGG);
	logger->addCategory(Categories::TOADLET_FLICK);
	logger->addCategory(Categories::TOADLET_HOP);
	logger->addCategory(Categories::TOADLET_KNOT);
	logger->addCategory(Categories::TOADLET_PEEPER);
	logger->addCategory(Categories::TOADLET_RIBBIT);
	logger->addCategory(Categories::TOADLET_TADPOLE);
	logger->addCategory(Categories::TOADLET_PAD);

	Logger::debug(Categories::TOADLET_TADPOLE,
		String("allocating ")+Categories::TOADLET_TADPOLE+".Engine:"+Version::STRING);

	mFixedBackable=fixedBackable;
	mShaderBackable=shaderBackable;

	mArchiveManager=new ArchiveManager();
	mTextureManager=new TextureManager(this);
	mBufferManager=new BufferManager(this);
	mShaderManager=new ShaderManager(this);
	mMaterialManager=new MaterialManager(this);
	mFontManager=new FontManager(this->getArchiveManager());
	mMeshManager=new MeshManager(this);
	mAudioBufferManager=new AudioBufferManager(this);
	mNodeManager=new NodeManager(this);
	
	mHandles.resize(1); // Handle 0 is always NULL

	Math::optimize();

	// Make a guess at what the ideal format is.
	#if defined(TOADLET_FIXED_POINT) && (defined(TOADLET_PLATFORM_WINCE) || defined(TOADLET_PLATFORM_IOS) || defined(TOADLET_PLATFORM_ANDROID))
		mIdealVertexFormatType=VertexFormat::Format_TYPE_FIXED_32;
	#else
		mIdealVertexFormatType=VertexFormat::Format_TYPE_FLOAT_32;
	#endif
	// Create initial BackableVertexFormats.  This doesn't need to be done, but without it, starting an application without a RenderDevice will crash.
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

	// Plugin types, should be removed from here somehow
	#if !defined(TOADLET_FIXED_POINT)
		registerNodeType(bsp::BSP30Node::type());
		registerNodeType(bsp::BSP30ModelNode::type());
		registerNodeType(studio::StudioModelNode::type());
		registerNodeType(studio::SpriteModelNode::type());
	#endif
}

Engine::~Engine(){
	Logger::debug(Categories::TOADLET_TADPOLE,
		"Engine::~Engine");

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

	if(mShaderManager!=NULL){
		delete mShaderManager;
		mShaderManager=NULL;
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
	Logger::debug(Categories::TOADLET_TADPOLE,
		"Engine::destroy");

	if(mRenderDevice!=NULL){
		mRenderDevice->activate();
	}

	mNodeManager->destroy();
	mAudioBufferManager->destroy();
	mMeshManager->destroy();
	mShaderManager->destroy();
	mMaterialManager->destroy();
	mFontManager->destroy();
	mBufferManager->destroy();
	mTextureManager->destroy();
	mArchiveManager->destroy();
}

void Engine::installHandlers(){
	Logger::debug(Categories::TOADLET_TADPOLE,
		"Engine: installing handlers");

	// Archive handlers
	mArchiveManager->setStreamer(TPKGHandler::ptr(new TPKGHandler()),"tpkg");
	mArchiveManager->setStreamer(WADHandler::ptr(new WADHandler(mTextureManager)),"wad");
	#if defined(TOADLET_HAS_ZZIP)
		mArchiveManager->setStreamer(ZIPHandler::ptr(new ZIPHandler()),"zip");
	#endif

	// Texture handlers
	mTextureManager->setStreamer(DDSHandler::ptr(new DDSHandler(mTextureManager)),"dds");
	#if defined(TOADLET_HAS_GDIPLUS)
		mTextureManager->setDefaultStreamer(Win32TextureHandler::ptr(new Win32TextureHandler(mTextureManager)));
	#elif defined(TOADLET_PLATFORM_OSX)
		mTextureManager->setStreamer(BMPHandler::ptr(new BMPHandler(mTextureManager)),"bmp"); // OSXTextureHandler only handles jpgs & pngs currently, so add our own bmp handler
		mTextureManager->setDefaultStreamer(OSXTextureHandler::ptr(new OSXTextureHandler(mTextureManager)));
	#else
		mTextureManager->setStreamer(BMPHandler::ptr(new BMPHandler(mTextureManager)),"bmp");
		#if defined(TOADLET_HAS_GIF)
			mTextureManager->setStreamer(GIFHandler::ptr(new GIFHandler(mTextureManager)),"gif");
		#endif
		#if defined(TOADLET_HAS_JPEG)
			JPEGHandler::ptr jpegHandler(new JPEGHandler(mTextureManager));
			mTextureManager->setStreamer(jpegHandler,"jpeg");
			mTextureManager->setStreamer(jpegHandler,"jpg");
		#endif
		#if defined(TOADLET_HAS_PNG)
			mTextureManager->setStreamer(PNGHandler::ptr(new PNGHandler(mTextureManager)),"png");
		#endif
	#endif
	mTextureManager->setStreamer(TGAHandler::ptr(new TGAHandler(mTextureManager)),"tga");
	#if defined(TOADLET_HAS_FFMPEG)
		mTextureManager->setVideoHandler(FFmpegVideoHandler::ptr(new FFmpegVideoHandler(this)));
	#endif

	// Font handlers, try for freetype first, since it currently looks best.  This can be changed back once the others look as nice
	#if defined(TOADLET_HAS_FREETYPE)
		mFontManager->setDefaultStreamer(FreeTypeHandler::ptr(new FreeTypeHandler(mTextureManager)));
	#elif defined(TOADLET_HAS_GDIPLUS)
		mFontManager->setDefaultStreamer(Win32FontHandler::ptr(new Win32FontHandler(mTextureManager)));
	#elif defined(TOADLET_PLATFORM_OSX)
		mFontManager->setDefaultStreamer(OSXFontHandler::ptr(new OSXFontHandler(mTextureManager)));
	#endif

	// Material handlers
	#if defined(TOADLET_HAS_MXML)
		mMaterialManager->setStreamer(XMATHandler::ptr(new XMATHandler(this)),"xmat");
	#endif

	// Mesh handlers
	#if defined(TOADLET_HAS_MXML)
		mMeshManager->setStreamer(XMSHHandler::ptr(new XMSHHandler(this)),"xmsh");
	#endif
	mMeshManager->setStreamer(TMSHHandler::ptr(new TMSHHandler(this)),"tmsh");

	// AudioBuffer handlers
	mAudioBufferManager->setStreamer(WaveHandler::ptr(new WaveHandler(mAudioBufferManager)),"wav");
	#if defined(TOADLET_HAS_OGGVORBIS)
		mAudioBufferManager->setStreamer(OggVorbisHandler::ptr(new OggVorbisHandler(mAudioBufferManager)),"ogg");
	#endif
	#if defined(TOADLET_HAS_SIDPLAY)
		mAudioBufferManager->setStreamer(SIDHandler::ptr(new SIDHandler(mAudioBufferManager)),"sid");
	#endif

	// Plugin types, should be removed from here somehow
	#if !defined(TOADLET_FIXED_POINT)
		mTextureManager->setStreamer(SPRHandler::ptr(new SPRHandler(this)),"spr");
	#endif

	mTextureManager->setNormalizationCreator(ResourceCreator::ptr(new NormalizationTextureCreator(this)));

	mMeshManager->setAABoxCreator(ResourceCreator::ptr(new AABoxMeshCreator(this)));
	mMeshManager->setSkyBoxCreator(ResourceCreator::ptr(new SkyBoxMeshCreator(this)));
	mMeshManager->setSkyDomeCreator(ResourceCreator::ptr(new SkyDomeMeshCreator(this)));
	mMeshManager->setSphereCreator(ResourceCreator::ptr(new SphereMeshCreator(this)));
	mMeshManager->setGridCreator(ResourceCreator::ptr(new GridMeshCreator(this)));

	mMaterialManager->setDiffuseCreator(ResourceCreator::ptr(new DiffuseMaterialCreator(this)));
	mMaterialManager->setSkyBoxCreator(ResourceCreator::ptr(new SkyBoxMaterialCreator(this)));
}

bool Engine::setRenderDevice(RenderDevice *renderDevice){
	if(renderDevice!=NULL){
		if(mFixedBackable==false && mShaderBackable==false && mRenderDeviceChanged){
			Error::unknown(Categories::TOADLET_TADPOLE,"can not change RenderDevice in an unbacked engine");
			return false;
		}
		else{
			mRenderDeviceChanged=true;
		}

		renderDevice->getRenderCaps(mRenderCaps);
		const RenderCaps &caps=mRenderCaps;
		{
			Logger::alert(Categories::TOADLET_TADPOLE,
				"RenderDevice Capabilities:");
			Logger::alert(Categories::TOADLET_TADPOLE,
				String()+(char)9+"maxTextureStages:"+caps.maxTextureStages);
			Logger::alert(Categories::TOADLET_TADPOLE,
				String()+(char)9+"maxTextureSize:"+caps.maxTextureSize);
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
		}

		mIdealVertexFormatType=caps.idealVertexFormatType;
	}

	RenderDevice *lastRenderDevice=mRenderDevice;

	if(renderDevice!=lastRenderDevice && lastRenderDevice!=NULL){
		contextDeactivate(lastRenderDevice);
	}

	mRenderDevice=renderDevice;

	if(renderDevice!=lastRenderDevice && renderDevice!=NULL){
		contextActivate(renderDevice);
	}

	if(mRenderDevice!=NULL){
		updateVertexFormats();
	}

	return true;
}

RenderDevice *Engine::getRenderDevice() const{
	return mRenderDevice;
}

void Engine::updateVertexFormats(){
	int formatType=mIdealVertexFormatType;

	VertexFormat::ptr format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatType|VertexFormat::Format_COUNT_3);
	mVertexFormats.POSITION=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatType|VertexFormat::Format_COUNT_3);
	format->addElement(VertexFormat::Semantic_NORMAL,0,formatType|VertexFormat::Format_COUNT_3);
	mVertexFormats.POSITION_NORMAL=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatType|VertexFormat::Format_COUNT_3);
	format->addElement(VertexFormat::Semantic_COLOR,0,VertexFormat::Format_TYPE_COLOR_RGBA);
	mVertexFormats.POSITION_COLOR=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatType|VertexFormat::Format_COUNT_3);
	format->addElement(VertexFormat::Semantic_TEXCOORD,0,formatType|VertexFormat::Format_COUNT_2);
	mVertexFormats.POSITION_TEX_COORD=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatType|VertexFormat::Format_COUNT_3);
	format->addElement(VertexFormat::Semantic_NORMAL,0,formatType|VertexFormat::Format_COUNT_3);
	format->addElement(VertexFormat::Semantic_COLOR,0,VertexFormat::Format_TYPE_COLOR_RGBA);
	mVertexFormats.POSITION_NORMAL_COLOR=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatType|VertexFormat::Format_COUNT_3);
	format->addElement(VertexFormat::Semantic_NORMAL,0,formatType|VertexFormat::Format_COUNT_3);
	format->addElement(VertexFormat::Semantic_TEXCOORD,0,formatType|VertexFormat::Format_COUNT_2);
	mVertexFormats.POSITION_NORMAL_TEX_COORD=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatType|VertexFormat::Format_COUNT_3);
	format->addElement(VertexFormat::Semantic_COLOR,0,VertexFormat::Format_TYPE_COLOR_RGBA);
	format->addElement(VertexFormat::Semantic_TEXCOORD,0,formatType|VertexFormat::Format_COUNT_2);
	mVertexFormats.POSITION_COLOR_TEX_COORD=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatType|VertexFormat::Format_COUNT_3);
	format->addElement(VertexFormat::Semantic_NORMAL,0,formatType|VertexFormat::Format_COUNT_3);
	format->addElement(VertexFormat::Semantic_COLOR,0,VertexFormat::Format_TYPE_COLOR_RGBA);
	format->addElement(VertexFormat::Semantic_TEXCOORD,0,formatType|VertexFormat::Format_COUNT_2);
	mVertexFormats.POSITION_NORMAL_COLOR_TEX_COORD=format;
}

bool Engine::setAudioDevice(AudioDevice *audioDevice){
	if(audioDevice!=NULL){
		if(mAudioDeviceChanged){
			Error::unknown(Categories::TOADLET_TADPOLE,"can not change AudioDevice");
			return false;
		}
		else{
			mAudioDeviceChanged=true;
		}
	}

	mAudioDevice=audioDevice;

	return true;
}

AudioDevice *Engine::getAudioDevice() const{
	return mAudioDevice;
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
		if(scene!=NULL){
			node->create(scene);
		}
		else{
			node->create(this);
		}
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
void Engine::contextReset(peeper::RenderDevice *renderDevice){
	Logger::debug("Engine::contextReset");

	int i;
	for(i=0;i<mContextListeners.size();++i){
		mContextListeners[i]->preContextReset(renderDevice);
	}

	mBufferManager->preContextReset(renderDevice);
	mTextureManager->preContextReset(renderDevice);

	renderDevice->reset();

	mBufferManager->postContextReset(renderDevice);
	mTextureManager->postContextReset(renderDevice);

	for(i=0;i<mContextListeners.size();++i){
		mContextListeners[i]->postContextReset(renderDevice);
	}
}

void Engine::contextActivate(RenderDevice *renderDevice){
	Logger::debug("Engine::contextActivate");

	renderDevice->activate();

	int i;
	for(i=0;i<mContextListeners.size();++i){
		mContextListeners[i]->preContextActivate(renderDevice);
	}

	mShaderManager->contextActivate(renderDevice);
	mBufferManager->contextActivate(renderDevice);
	mTextureManager->contextActivate(renderDevice);
	mMaterialManager->contextActivate(renderDevice);

	for(i=0;i<mContextListeners.size();++i){
		mContextListeners[i]->postContextActivate(renderDevice);
	}
}

void Engine::contextDeactivate(RenderDevice *renderDevice){
	Logger::debug("Engine::contextDeactivate");

	renderDevice->activate();

	int i;
	for(i=0;i<mContextListeners.size();++i){
		mContextListeners[i]->preContextDeactivate(renderDevice);
	}

	mShaderManager->contextDeactivate(renderDevice);
	mBufferManager->contextDeactivate(renderDevice);
	mTextureManager->contextDeactivate(renderDevice);
	mMaterialManager->contextDeactivate(renderDevice);

	for(i=0;i<mContextListeners.size();++i){
		mContextListeners[i]->postContextDeactivate(renderDevice);
	}
}

}
}
