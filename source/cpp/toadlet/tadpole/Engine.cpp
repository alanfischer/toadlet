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
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/Version.h>

#include <toadlet/tadpole/plugins/AABoxMeshCreator.h>
#include <toadlet/tadpole/plugins/SkyBoxMeshCreator.h>
#include <toadlet/tadpole/plugins/SkyDomeMeshCreator.h>
#include <toadlet/tadpole/plugins/SphereMeshCreator.h>
#include <toadlet/tadpole/plugins/GridMeshCreator.h>
#include <toadlet/tadpole/plugins/NormalizationTextureCreator.h>
#include <toadlet/tadpole/plugins/DiffuseMaterialCreator.h>
#include <toadlet/tadpole/plugins/SkyBoxMaterialCreator.h>
#include <toadlet/tadpole/plugins/WaterMaterialCreator.h>
#include <toadlet/tadpole/plugins/BMPStreamer.h>
#include <toadlet/tadpole/plugins/DDSStreamer.h>
#include <toadlet/tadpole/plugins/TGAStreamer.h>
#include <toadlet/tadpole/plugins/TMSHStreamer.h>
#include <toadlet/tadpole/plugins/TPKGStreamer.h>
#include <toadlet/tadpole/plugins/WADStreamer.h>
#include <toadlet/tadpole/plugins/WaveStreamer.h>

#if defined(TOADLET_HAS_GDIPLUS)
	#include <toadlet/tadpole/platform/win32/Win32TextureStreamer.h>
#endif
#if defined(TOADLET_PLATFORM_OSX)
	#include <toadlet/tadpole/platform/osx/OSXTextureStreamer.h>
#endif
#if defined(TOADLET_PLATFORM_ANDROID)
	#include <toadlet/egg/platform/android/AndroidAssetArchive.h>
	#include <toadlet/tadpole/platform/android/AndroidTextureStreamer.h>
#endif
#if defined(TOADLET_HAS_GIF)
	#include <toadlet/tadpole/plugins/GIFStreamer.h>
#endif
#if defined(TOADLET_HAS_JPEG)
	#include <toadlet/tadpole/plugins/JPEGStreamer.h>
#endif
#if defined(TOADLET_HAS_PNG)
	#include <toadlet/tadpole/plugins/PNGStreamer.h>
#endif
#if defined(TOADLET_HAS_FFMPEG)
	#include <toadlet/tadpole/plugins/FFmpegVideoHandler.h>
#endif

#if defined(TOADLET_HAS_GDIPLUS)
	#include <toadlet/tadpole/platform/win32/Win32FontStreamer.h>
#endif
#if defined(TOADLET_PLATFORM_OSX)
	#include <toadlet/tadpole/platform/osx/OSXFontStreamer.h>
#endif
#if defined(TOADLET_HAS_FREETYPE)
	#include <toadlet/tadpole/plugins/FreeTypeStreamer.h>
#endif

#if defined(TOADLET_HAS_ZZIP)
	#include <toadlet/tadpole/plugins/ZIPStreamer.h>
#endif

#if defined(TOADLET_HAS_MXML)
	#include <toadlet/tadpole/plugins/XANMStreamer.h>
	#include <toadlet/tadpole/plugins/XMATStreamer.h>
	#include <toadlet/tadpole/plugins/XMSHStreamer.h>
#endif

#if defined(TOADLET_HAS_OGGVORBIS)
	#include <toadlet/tadpole/plugins/OggVorbisStreamer.h>
#endif
#if defined(TOADLET_HAS_SIDPLAY)
	#include <toadlet/tadpole/plugins/SIDStreamer.h>
#endif
#if defined(TOADLET_HAS_GME)
	#include <toadlet/tadpole/plugins/GMEStreamer.h>
#endif

#if !defined(TOADLET_FIXED_POINT)
	#include <toadlet/tadpole/plugins/SPRStreamer.h>
	#include <toadlet/tadpole/studio/SpriteStreamer.h>
	#include <toadlet/tadpole/studio/StudioStreamer.h>
#endif

namespace toadlet{
namespace tadpole{

Engine::Engine(void *env,void *ctx,int options):
	mOptions(0),
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

	mEnv=env,mCtx=ctx;
	mOptions=options;

	mArchiveManager=new ArchiveManager(this);
	mTextureManager=new TextureManager(this);
	mBufferManager=new BufferManager(this);
	mShaderManager=new ShaderManager(this);
	mMaterialManager=new MaterialManager(this);
	mFontManager=new FontManager(this);
	mMeshManager=new ResourceManager(this);
	mAudioManager=new AudioManager(this);

	mStudioModelManager=new ResourceManager(this);
	mSpriteModelManager=new ResourceManager(this);

	mHandles.resize(1,NULL); // Handle 0 is always NULL

	Math::optimize();

	// Make a guess at what the ideal format is.
	#if defined(TOADLET_FIXED_POINT) && (defined(TOADLET_PLATFORM_WINCE) || defined(TOADLET_PLATFORM_IOS) || defined(TOADLET_PLATFORM_ANDROID))
		mIdealVertexFormatType=VertexFormat::Format_TYPE_FIXED_32;
	#else
		mIdealVertexFormatType=VertexFormat::Format_TYPE_FLOAT_32;
	#endif

	// Create initial BackableVertexFormats.  This doesn't need to be done, but without it, starting an application without a RenderDevice will crash.
	updateVertexFormats();
}

Engine::~Engine(){
	Logger::debug(Categories::TOADLET_TADPOLE,
		"Engine::~Engine");
}

void Engine::destroy(){
	Logger::debug(Categories::TOADLET_TADPOLE,
		"Engine::destroy");

	if(mRenderDevice!=NULL){
		mRenderDevice->activate();
	}

	mStudioModelManager->destroy();
	mSpriteModelManager->destroy();

	mAudioManager->destroy();
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

	// Archive streamers
	mArchiveManager->setStreamer(new TPKGStreamer(),"tpkg");
	mArchiveManager->setStreamer(new WADStreamer(mTextureManager),"wad");
	#if defined(TOADLET_HAS_ZZIP)
		mArchiveManager->setStreamer(new ZIPStreamer(),"zip");
	#endif
	#if defined(TOADLET_PLATFORM_ANDROID)
		jobject assetManagerObj=NULL;
		jclass contextClass=((JNIEnv*)mEnv)->GetObjectClass((jobject)mCtx);
		{
			jmethodID getAssetsID=((JNIEnv*)mEnv)->GetMethodID(contextClass,"getAssets","()Landroid/content/res/AssetManager;");
			assetManagerObj=((JNIEnv*)mEnv)->CallObjectMethod((jobject)mCtx,getAssetsID);
		}
		((JNIEnv*)mEnv)->DeleteLocalRef(contextClass);
		Archive::ptr assetArchive=new AndroidAssetArchive((JNIEnv*)mEnv,assetManagerObj);
		mArchiveManager->manage(assetArchive);
	#endif

	// Texture streamers
	mTextureManager->setStreamer(new DDSStreamer(mTextureManager),"dds");
	#if defined(TOADLET_HAS_GDIPLUS)
		mTextureManager->setDefaultStreamer(new Win32TextureStreamer(mTextureManager));
	#elif defined(TOADLET_PLATFORM_OSX)
		mTextureManager->setStreamer(new BMPStreamer(mTextureManager),"bmp"); // OSXTextureStreamer only handles jpgs & pngs currently, so add our own bmp streamer
		mTextureManager->setDefaultStreamer(new OSXTextureStreamer(mTextureManager));
	#elif defined(TOADLET_PLATFORM_ANDROID)
		mTextureManager->setDefaultStreamer(new AndroidTextureStreamer(mTextureManager,(JNIEnv*)mEnv));
	#else
		mTextureManager->setStreamer(new BMPStreamer(mTextureManager),"bmp");
		#if defined(TOADLET_HAS_GIF)
			mTextureManager->setStreamer(new GIFStreamer(mTextureManager),"gif");
		#endif
		#if defined(TOADLET_HAS_JPEG)
			JPEGStreamer::ptr jpegStreamer(new JPEGStreamer(mTextureManager));
			mTextureManager->setStreamer(jpegStreamer,"jpeg");
			mTextureManager->setStreamer(jpegStreamer,"jpg");
		#endif
		#if defined(TOADLET_HAS_PNG)
			mTextureManager->setStreamer(new PNGStreamer(mTextureManager),"png");
		#endif
	#endif
	mTextureManager->setStreamer(new TGAStreamer(mTextureManager),"tga");
	#if defined(TOADLET_HAS_FFMPEG)
		mTextureManager->setVideoHandler(FFmpegVideoHandler::ptr(new FFmpegVideoHandler(this)));
	#endif

	// Font streamers, try for freetype first, since it currently looks best.  This can be changed back once the others look as nice
	#if defined(TOADLET_HAS_FREETYPE)
		mFontManager->setDefaultStreamer(new FreeTypeStreamer(mTextureManager));
	#elif defined(TOADLET_HAS_GDIPLUS)
		mFontManager->setDefaultStreamer(new Win32FontStreamer(mTextureManager));
	#elif defined(TOADLET_PLATFORM_OSX)
		mFontManager->setDefaultStreamer(new OSXFontStreamer(mTextureManager));
	#endif

	// Material streamers
	#if defined(TOADLET_HAS_MXML)
		mMaterialManager->setStreamer(new XMATStreamer(this),"xmat");
	#endif

	// Mesh streamers
	#if defined(TOADLET_HAS_MXML)
		mMeshManager->setStreamer(new XMSHStreamer(this),"xmsh");
	#endif
	mMeshManager->setStreamer(new TMSHStreamer(this),"tmsh");

	// AudioBuffer streamers
	mAudioManager->setStreamer(new WaveStreamer(mAudioManager),"wav");
	#if defined(TOADLET_HAS_OGGVORBIS)
		mAudioManager->setStreamer(new OggVorbisStreamer(mAudioManager),"ogg");
	#endif
	#if defined(TOADLET_HAS_SIDPLAY)
		mAudioManager->setStreamer(new SIDStreamer(mAudioManager),"sid");
	#endif
	#if defined(TOADLET_HAS_GME)
		GMEStreamer::ptr gmeStreamer=new GMEStreamer(mAudioManager);
		mAudioManager->setStreamer(gmeStreamer,"ay");
		mAudioManager->setStreamer(gmeStreamer,"gbs");
		mAudioManager->setStreamer(gmeStreamer,"gym");
		mAudioManager->setStreamer(gmeStreamer,"hes");
		mAudioManager->setStreamer(gmeStreamer,"kss");
		mAudioManager->setStreamer(gmeStreamer,"nsf");
		mAudioManager->setStreamer(gmeStreamer,"nsfe");
		mAudioManager->setStreamer(gmeStreamer,"sap");
		mAudioManager->setStreamer(gmeStreamer,"spc");
		mAudioManager->setStreamer(gmeStreamer,"vgm");
		mAudioManager->setStreamer(gmeStreamer,"vgz");
	#endif

	// Plugin types, should be removed from here somehow
	#if !defined(TOADLET_FIXED_POINT)
		mTextureManager->setStreamer(new SPRStreamer(this),"spr");

		mSpriteModelManager->setStreamer(new studio::SpriteStreamer(this),"spr");
		mStudioModelManager->setStreamer(new studio::StudioStreamer(this),"mdl");
	#endif

	mNormalizationCreator=new NormalizationTextureCreator(this);

	mDiffuseCreator=new DiffuseMaterialCreator(this);
	mSkyBoxMaterialCreator=new SkyBoxMaterialCreator(this);
	mWaterMaterialCreator=new WaterMaterialCreator(this);

	mAABoxCreator=new AABoxMeshCreator(this);
	mSkyBoxCreator=new SkyBoxMeshCreator(this);
	mSkyDomeCreator=new SkyDomeMeshCreator(this);
	mSphereCreator=new SphereMeshCreator(this);
	mGridCreator=new GridMeshCreator(this);
}

bool Engine::setRenderDevice(RenderDevice *renderDevice){
	if(renderDevice!=NULL){
		if(isFixedBackable()==false && isShaderBackable()==false && mRenderDeviceChanged){
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

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_POSITION,0,formatType|VertexFormat::Format_COUNT_3);
	format->addElement(VertexFormat::Semantic_ROTATE,0,formatType|VertexFormat::Format_COUNT_4);
	format->addElement(VertexFormat::Semantic_SCALE,0,formatType|VertexFormat::Format_COUNT_3);
	mVertexFormats.POSITION_ROTATE_SCALE=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_COLOR,0,formatType|VertexFormat::Format_COUNT_4);
	mVertexFormats.COLOR=format;
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

void Engine::nodeCreated(Node *node){
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
		node->internal_setUniqueHandle(handle);
	}
}

void Engine::nodeDestroyed(Node *node){
	/// @todo: Network: I dont want handles to be reused at this time
/*
	int handle=node->getUniqueHandle();
	if(handle>0){
		mHandles[handle]=NULL;
		mFreeHandles.add(handle);
		node->internal_setUniqueHandle(0);
	}
*/
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

Texture::ptr Engine::createNormalizationTexture(int size){
	return shared_static_cast<NormalizationTextureCreator>(mNormalizationCreator)->createNormalizationTexture(size);
}

Material::ptr Engine::createDiffuseMaterial(Texture *texture){
	return shared_static_cast<DiffuseMaterialCreator>(mDiffuseCreator)->createDiffuseMaterial(texture);
}

Material::ptr Engine::createPointSpriteMaterial(Texture *texture,scalar size,bool attenuated){
	return shared_static_cast<DiffuseMaterialCreator>(mDiffuseCreator)->createPointSpriteMaterial(texture,size,attenuated);
}

Material::ptr Engine::createFontMaterial(Font *font){
	return shared_static_cast<DiffuseMaterialCreator>(mDiffuseCreator)->createFontMaterial(font);
}

Material::ptr Engine::createSkyBoxMaterial(Texture *texture,bool clamp){
	return shared_static_cast<SkyBoxMaterialCreator>(mSkyBoxMaterialCreator)->createSkyBoxMaterial(texture,clamp);
}

Material::ptr Engine::createWaterMaterial(Texture *reflectTexture,Texture *refractTexture,Texture *bumpTexture,const Vector4 &color){
	return shared_static_cast<WaterMaterialCreator>(mWaterMaterialCreator)->createWaterMaterial(reflectTexture,refractTexture,bumpTexture,color);
}

Mesh::ptr Engine::createAABoxMesh(const AABox &box,Material *material){
	return shared_static_cast<AABoxMeshCreator>(mAABoxCreator)->createAABoxMesh(box,material);
}

Mesh::ptr Engine::createSkyBoxMesh(scalar size,bool unfolded,bool invert,Material *bottom,Material *top,Material *left,Material *right,Material *back,Material *front){
	return shared_static_cast<SkyBoxMeshCreator>(mSkyBoxCreator)->createSkyBoxMesh(size,unfolded,invert,bottom,top,left,right,back,front);
}

Mesh::ptr Engine::createSkyDomeMesh(const Sphere &sphere,int numSegments,int numRings,scalar fade,Material *material){
	return shared_static_cast<SkyDomeMeshCreator>(mSkyDomeCreator)->createSkyDomeMesh(sphere,numSegments,numRings,fade,material);
}

Mesh::ptr Engine::createSphereMesh(const Sphere &sphere,Material *material){
	return shared_static_cast<SphereMeshCreator>(mSphereCreator)->createSphereMesh(sphere,16,16,material);
}

Mesh::ptr Engine::createGridMesh(scalar width,scalar height,int numWidth,int numHeight,Material *material){
	return shared_static_cast<GridMeshCreator>(mGridCreator)->createGridMesh(width,height,numWidth,numHeight,material);
}

}
}
