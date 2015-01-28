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

#include <toadlet/peeper/RenderCaps.h>
#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/Version.h>

#include <toadlet/tadpole/plugins/AABoxMeshCreator.h>
#include <toadlet/tadpole/plugins/SphereMeshCreator.h>
#include <toadlet/tadpole/plugins/TorusMeshCreator.h>
#include <toadlet/tadpole/plugins/GridMeshCreator.h>
#include <toadlet/tadpole/plugins/SkyBoxMeshCreator.h>
#include <toadlet/tadpole/plugins/SkyDomeMeshCreator.h>
#include <toadlet/tadpole/plugins/NormalizationTextureCreator.h>
#include <toadlet/tadpole/plugins/PointTextureCreator.h>
#include <toadlet/tadpole/plugins/DiffuseMaterialCreator.h>
#include <toadlet/tadpole/plugins/SkyBoxMaterialCreator.h>
#include <toadlet/tadpole/plugins/WaterMaterialCreator.h>
#include <toadlet/tadpole/plugins/TextureMaterialStreamer.h>
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
#if defined(TOADLET_HAS_MODPLUG)
	#include <toadlet/tadpole/plugins/MODStreamer.h>
#endif

#if defined(TOADLET_PLATFORM_EMSCRIPTEN)
	extern toadlet::egg::io::Archive::ptr new_DOMArchive(toadlet::tadpole::Engine *engine);
#endif

#if !defined(TOADLET_FIXED_POINT)
	#include <toadlet/tadpole/plugins/SPRStreamer.h>
	#include <toadlet/tadpole/studio/SpriteStreamer.h>
	#include <toadlet/tadpole/studio/StudioStreamer.h>
#endif

namespace toadlet{
namespace tadpole{

Engine::Engine(void *env,void *ctx):
	mEnv(NULL),
	mCtx(NULL),
	mRenderDevice(NULL),
	mRenderDeviceChanged(false),
	mAudioDevice(NULL),
	mAudioDeviceChanged(false)
{
	Log::debug(Categories::TOADLET_TADPOLE,
		String("allocating ")+Categories::TOADLET_TADPOLE+".Engine:"+Version::STRING);

	#if defined(TOADLET_PLATFORM_ANDROID)
		mEnv=env;
		mCtx=((JNIEnv*)mEnv)->NewGlobalRef((jobject)ctx);
	#endif

	mArchiveManager=new ArchiveManager(this);
	mTextureManager=new TextureManager(this);
	mBufferManager=new BufferManager(this);
	mShaderManager=new ShaderManager(this);
	mMaterialManager=new MaterialManager(this);
	mFontManager=new FontManager(this);
	mMeshManager=new MeshManager(this);
	mAudioManager=new AudioManager(this);

	mStudioModelManager=new ResourceManager(this);
	mSpriteModelManager=new ResourceManager(this);

	Math::optimize();

	// Make a guess at what the ideal format is.
	#if defined(TOADLET_FIXED_POINT)
		mRenderCaps.idealVertexFormatType=VertexFormat::Format_TYPE_FIXED_32;
	#else
		mRenderCaps.idealVertexFormatType=VertexFormat::Format_TYPE_FLOAT_32;
	#endif

	mMaximumRenderCaps.maxTextureSize=4096*16;
	mMaximumRenderCaps.maxTextureStages=512;
	mMaximumRenderCaps.maxLights=512;
	mMaximumRenderCaps.triangleFan=true;
	int i;
	for(i=0;i<Shader::ShaderType_MAX;++i){
		mMaximumRenderCaps.hasShader[i]=true;
		mMaximumRenderCaps.hasFixed[i]=true;
	}
	
	mBackableRenderCaps.maxTextureSize=4096*16;
	mBackableRenderCaps.maxTextureStages=512;
	mBackableRenderCaps.maxLights=512;

	// Android currently must be backed, since the context can not be created when the application is
	#if defined(TOADLET_PLATFORM_ANDROID)
		mBackableRenderCaps.textureAutogenMipMaps=true;
		setHasBackableShader(true);
		setHasBackableFixed(true);
		setHasBackableTriangleFan(true);
		setHasBackableFixed(true);
	#endif

	// Create initial BackableVertexFormats.  This doesn't need to be done, but without it, starting an application without a RenderDevice will crash.
	updateVertexFormats();
}

Engine::~Engine(){
	#if defined(TOADLET_PLATFORM_ANDROID)
		((JNIEnv*)mEnv)->DeleteGlobalRef((jobject)mCtx);
		mEnv=NULL;
	#endif

	Log::debug(Categories::TOADLET_TADPOLE,
		"Engine::~Engine");
}

void Engine::destroy(){
	Log::debug(Categories::TOADLET_TADPOLE,
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
	Log::debug(Categories::TOADLET_TADPOLE,
		"Engine: installing handlers");

	// Archive streamers
	#if !defined(TOADLET_PLATFORM_EMSCRIPTEN)
		mArchiveManager->setStreamer(new TPKGStreamer(),"tpkg");
		mArchiveManager->setStreamer(new WADStreamer(mTextureManager),"wad");
	#endif
	#if defined(TOADLET_HAS_ZZIP)
		mArchiveManager->setStreamer(new ZIPStreamer(),"zip");
	#endif
	#if defined(TOADLET_PLATFORM_ANDROID)
		jobject assetManagerObj=NULL;
		jclass oclass=((JNIEnv*)mEnv)->GetObjectClass((jobject)mCtx);
		{
			jmethodID getAssetsID=((JNIEnv*)mEnv)->GetMethodID(oclass,"getAssets","()Landroid/content/res/AssetManager;");
			assetManagerObj=((JNIEnv*)mEnv)->CallObjectMethod((jobject)mCtx,getAssetsID);
		}
		((JNIEnv*)mEnv)->DeleteLocalRef(oclass);
		Archive::ptr assetArchive=new AndroidAssetArchive((JNIEnv*)mEnv,assetManagerObj);
		mArchiveManager->manage(assetArchive);
	#endif

	// Texture streamers
	#if !defined(TOADLET_PLATFORM_EMSCRIPTEN)
		mTextureManager->setStreamer(new BMPStreamer(mTextureManager),"bmp");
		mTextureManager->setStreamer(new DDSStreamer(mTextureManager),"dds");
		mTextureManager->setStreamer(new TGAStreamer(mTextureManager),"tga");
	#endif
	#if defined(TOADLET_HAS_GDIPLUS)
		mTextureManager->setDefaultStreamer(new Win32TextureStreamer(mTextureManager));
	#elif defined(TOADLET_PLATFORM_OSX)
		mTextureManager->setStreamer(new BMPStreamer(mTextureManager),"bmp"); // OSXTextureStreamer only handles jpgs & pngs currently, so add our own bmp streamer
		mTextureManager->setDefaultStreamer(new OSXTextureStreamer(mTextureManager));
	#elif defined(TOADLET_PLATFORM_ANDROID)
		mTextureManager->setDefaultStreamer(new AndroidTextureStreamer(mTextureManager,(JNIEnv*)mEnv));
	#else
		#if defined(TOADLET_HAS_JPEG)
			JPEGStreamer::ptr jpegStreamer(new JPEGStreamer(mTextureManager));
			mTextureManager->setStreamer(jpegStreamer,"jpeg");
			mTextureManager->setStreamer(jpegStreamer,"jpg");
		#endif
		#if defined(TOADLET_HAS_PNG)
			mTextureManager->setStreamer(new PNGStreamer(mTextureManager),"png");
		#endif
	#endif
	#if defined(TOADLET_HAS_GIF)
		mTextureManager->setStreamer(new GIFStreamer(mTextureManager),"gif");
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
	mMaterialManager->setDefaultStreamer(new TextureMaterialStreamer(this));

	// Mesh streamers
	#if defined(TOADLET_HAS_MXML)
		mMeshManager->setStreamer(new XMSHStreamer(this),"xmsh");
	#endif
	mMeshManager->setStreamer(new TMSHStreamer(this),"tmsh");

	// AudioBuffer streamers
	#if !defined(TOADLET_PLATFORM_EMSCRIPTEN)
		mAudioManager->setStreamer(new WaveStreamer(mAudioManager),"wav");
	#endif
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
	#if defined(TOADLET_HAS_MODPLUG)
		MODStreamer::ptr modStreamer=new MODStreamer(mAudioManager);
		mAudioManager->setStreamer(modStreamer,"mod");
		mAudioManager->setStreamer(modStreamer,"s3m");
		mAudioManager->setStreamer(modStreamer,"xm");
		mAudioManager->setStreamer(modStreamer,"it");
		mAudioManager->setStreamer(modStreamer,"669");
		mAudioManager->setStreamer(modStreamer,"amf");
		mAudioManager->setStreamer(modStreamer,"ams");
		mAudioManager->setStreamer(modStreamer,"dbm");
		mAudioManager->setStreamer(modStreamer,"dmf");
		mAudioManager->setStreamer(modStreamer,"dsm");
		mAudioManager->setStreamer(modStreamer,"far");
		mAudioManager->setStreamer(modStreamer,"mdl");
		mAudioManager->setStreamer(modStreamer,"med");
		mAudioManager->setStreamer(modStreamer,"mtm");
		mAudioManager->setStreamer(modStreamer,"okt");
		mAudioManager->setStreamer(modStreamer,"ptm");
		mAudioManager->setStreamer(modStreamer,"stm");
		mAudioManager->setStreamer(modStreamer,"ult");
		mAudioManager->setStreamer(modStreamer,"umx");
		mAudioManager->setStreamer(modStreamer,"mt2");
		mAudioManager->setStreamer(modStreamer,"psm");
		mAudioManager->setStreamer(modStreamer,"abc");
		mAudioManager->setStreamer(modStreamer,"mid");
		mAudioManager->setStreamer(modStreamer,"midi");
	#endif

	// Plugin types, should be removed from here somehow
	#if !defined(TOADLET_FIXED_POINT) && !defined(TOADLET_PLATFORM_EMSCRIPTEN)
		mTextureManager->setStreamer(new SPRStreamer(this),"spr");

		mSpriteModelManager->setStreamer(new studio::SpriteStreamer(this),"spr");
		mStudioModelManager->setStreamer(new studio::StudioStreamer(this),"mdl");
	#endif

	#if defined(TOADLET_PLATFORM_EMSCRIPTEN)
		Archive::ptr domArchive=new_DOMArchive(this);
		mArchiveManager->addArchive(domArchive);
		mTextureManager->addResourceArchive(domArchive);
	#endif
	
	mNormalizationCreator=new NormalizationTextureCreator(this);
	mPointCreator=new PointTextureCreator(this);

	mDiffuseCreator=new DiffuseMaterialCreator(this);
	mSkyBoxMaterialCreator=new SkyBoxMaterialCreator(this);
	mWaterMaterialCreator=new WaterMaterialCreator(this);

	mAABoxCreator=new AABoxMeshCreator(this);
	mSphereCreator=new SphereMeshCreator(this);
	mTorusCreator=new TorusMeshCreator(this);
	mGridCreator=new GridMeshCreator(this);
	mSkyBoxCreator=new SkyBoxMeshCreator(this);
	mSkyDomeCreator=new SkyDomeMeshCreator(this);

	mFontManager->findDefaultFont();
}

void Engine::setMaximumRenderCaps(const RenderCaps &caps){
	mMaximumRenderCaps.set(caps);
	updateRenderCaps();
}

void Engine::setBackableRenderCaps(const RenderCaps &caps){
	mBackableRenderCaps.set(caps);
	updateRenderCaps();
}

int Engine::mergeCap(int render,int maximum,int backable){
	render=Math::intMaxVal(render,backable);
	render=Math::intMinVal(render,maximum);
	return render;
}

void Engine::updateRenderCaps(){
	// engineRenderCaps = min(max(renderCaps,backableCaps),maximumCaps)

	mEngineRenderCaps.set(mRenderCaps);

	mEngineRenderCaps.maxTextureStages=	mergeCap(mRenderCaps.maxTextureStages,	mMaximumRenderCaps.maxTextureStages,mBackableRenderCaps.maxTextureStages);
	mEngineRenderCaps.maxTextureSize=	mergeCap(mRenderCaps.maxTextureSize,	mMaximumRenderCaps.maxTextureSize,	mBackableRenderCaps.maxTextureSize);

	int i;
	for(i=0;i<Shader::ShaderType_MAX;++i){
		mEngineRenderCaps.hasShader[i]=	mergeCap(mRenderCaps.hasShader[i],		mMaximumRenderCaps.hasShader[i],	mBackableRenderCaps.hasShader[i])> 0;
		mEngineRenderCaps.hasFixed[i]=	mergeCap(mRenderCaps.hasFixed[i],		mMaximumRenderCaps.hasFixed[i],		mBackableRenderCaps.hasFixed[i])> 0;
	}

	mEngineRenderCaps.maxLights=		mergeCap(mRenderCaps.maxLights,			mMaximumRenderCaps.maxLights,		mBackableRenderCaps.maxLights);
	mEngineRenderCaps.triangleFan=		mergeCap(mRenderCaps.triangleFan,		mMaximumRenderCaps.triangleFan,		mBackableRenderCaps.triangleFan)> 0;
	mEngineRenderCaps.textureAutogenMipMaps=mergeCap(mRenderCaps.textureAutogenMipMaps,mMaximumRenderCaps.textureAutogenMipMaps,mBackableRenderCaps.textureAutogenMipMaps)> 0;
}

void Engine::updateVertexFormats(){
	int formatType=mRenderCaps.idealVertexFormatType;

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
	format->addElement(VertexFormat::Semantic_ROTATE,0,formatType|VertexFormat::Format_COUNT_4);
	mVertexFormats.ROTATE=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_SCALE,0,formatType|VertexFormat::Format_COUNT_3);
	mVertexFormats.SCALE=format;

	format=mBufferManager->createVertexFormat();
	format->addElement(VertexFormat::Semantic_COLOR,0,formatType|VertexFormat::Format_COUNT_4);
	mVertexFormats.COLOR=format;
}

void Engine::setHasBackableShader(bool has){
	for(int i=0;i<Shader::ShaderType_MAX;++i){
		mBackableRenderCaps.hasShader[i]=has;
	}
	updateRenderCaps();
}

void Engine::setHasBackableFixed(bool has){
	for(int i=0;i<Shader::ShaderType_MAX;++i){
		mBackableRenderCaps.hasFixed[i]=has;
	}
	updateRenderCaps();
}

void Engine::setHasBackableTriangleFan(bool has){
	mBackableRenderCaps.triangleFan=has;
	updateRenderCaps();
}

void Engine::setHasMaximumShader(bool has){
	if(!has){
		Log::warning(Categories::TOADLET_TADPOLE,"Disabling shaders");
	}

	for(int i=0;i<Shader::ShaderType_MAX;++i){
		mMaximumRenderCaps.hasShader[i]=has;
	}
	updateRenderCaps();
}

void Engine::setHasMaximumFixed(bool has){
	if(!has){
		Log::warning(Categories::TOADLET_TADPOLE,"Disabling fixed function");
	}

	for(int i=0;i<Shader::ShaderType_MAX;++i){
		mMaximumRenderCaps.hasFixed[i]=has;
	}
	updateRenderCaps();
}

bool Engine::setRenderDevice(RenderDevice *renderDevice){
	bool reset=false;

	if(renderDevice!=NULL){
		if(isBackable()==false && mRenderDeviceChanged){
			Error::unknown(Categories::TOADLET_TADPOLE,"can not change RenderDevice in an unbacked engine");
			return false;
		}
		else if(mRenderDeviceChanged==false){
			mRenderDeviceChanged=true;
		}
		else{
			// Reset on subsequent RenderDevice changes
			reset=true;
		}

		renderDevice->getRenderCaps(mRenderCaps);
		const RenderCaps &caps=mRenderCaps;
		{
			Log::alert(Categories::TOADLET_TADPOLE,
				"RenderDevice Capabilities:");
			Log::alert(Categories::TOADLET_TADPOLE,
				String()+(char)9+"maxTextureStages:"+caps.maxTextureStages);
			Log::alert(Categories::TOADLET_TADPOLE,
				String()+(char)9+"maxTextureSize:"+caps.maxTextureSize);
			Log::alert(Categories::TOADLET_TADPOLE,
				String()+(char)9+"renderToTexture:"+caps.renderToTexture);
			Log::alert(Categories::TOADLET_TADPOLE,
				String()+(char)9+"renderToDepthTexture:"+caps.renderToDepthTexture);
			Log::alert(Categories::TOADLET_TADPOLE,
				String()+(char)9+"renderToTextureNonPowerOf2Restricted:"+caps.renderToTextureNonPowerOf2Restricted);
			Log::alert(Categories::TOADLET_TADPOLE,
				String()+(char)9+"textureDot3:"+caps.textureDot3);
			Log::alert(Categories::TOADLET_TADPOLE,
				String()+(char)9+"textureNonPowerOf2Restricted:"+caps.textureNonPowerOf2Restricted);
			Log::alert(Categories::TOADLET_TADPOLE,
				String()+(char)9+"textureNonPowerOf2:"+caps.textureNonPowerOf2);
			Log::alert(Categories::TOADLET_TADPOLE,
				String()+(char)9+"textureAutogenMipMaps:"+caps.textureAutogenMipMaps);
		}
	}

	updateRenderCaps();

	RenderDevice *lastRenderDevice=mRenderDevice;

	if(renderDevice!=lastRenderDevice && lastRenderDevice!=NULL){
		contextDeactivate(lastRenderDevice);
	}

	mRenderDevice=renderDevice;

	if(renderDevice!=lastRenderDevice && renderDevice!=NULL){
		contextActivate(renderDevice);

		if(reset){
			contextReset(renderDevice);
		}
	}

	return true;
}

RenderDevice *Engine::getRenderDevice() const{
	return mRenderDevice;
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

// Context methods
void Engine::contextReset(peeper::RenderDevice *renderDevice){
	Log::debug("Engine::contextReset");

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
	Log::debug("Engine::contextActivate");

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
	Log::debug("Engine::contextDeactivate");

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

void Engine::addContextListener(ContextListener *listener){
	mContextListeners.push_back(listener);
}

void Engine::removeContextListener(ContextListener *listener){
	mContextListeners.erase(stlit::remove(mContextListeners.begin(),mContextListeners.end(),listener),mContextListeners.end());
}

Texture::ptr Engine::createNormalizationTexture(int size){
	Texture::ptr texture=static_pointer_cast<NormalizationTextureCreator>(mNormalizationCreator)->createNormalizationTexture(size);
	mTextureManager->manage(texture);
	return texture;
}

Texture::ptr Engine::createPointTexture(TextureFormat *format,float colorOffset,float colorFactor,float alphaOffset,float alphaFactor,float falloff){
	Texture::ptr texture=static_pointer_cast<PointTextureCreator>(mPointCreator)->createPointTexture(format,colorOffset,colorFactor,alphaOffset,alphaFactor,falloff);
	mTextureManager->manage(texture);
	return texture;
}

Material::ptr Engine::createDiffuseMaterial(Texture *texture,RenderState *renderState){
	Material::ptr material=static_pointer_cast<DiffuseMaterialCreator>(mDiffuseCreator)->createDiffuseMaterial(texture,renderState);
	mMaterialManager->manage(material);
	return material;
}

Material::ptr Engine::createPointSpriteMaterial(Texture *texture,scalar size,bool attenuated){
	Material::ptr material=static_pointer_cast<DiffuseMaterialCreator>(mDiffuseCreator)->createPointSpriteMaterial(texture,size,attenuated);
	mMaterialManager->manage(material);
	return material;
}

Material::ptr Engine::createFontMaterial(Font *font){
	Material::ptr material=static_pointer_cast<DiffuseMaterialCreator>(mDiffuseCreator)->createFontMaterial(font);
	mMaterialManager->manage(material);
	return material;
}

Material::ptr Engine::createSkyBoxMaterial(Texture *texture,bool clamp,RenderState *renderState){
	Material::ptr material=static_pointer_cast<SkyBoxMaterialCreator>(mSkyBoxMaterialCreator)->createSkyBoxMaterial(texture,clamp,renderState);
	mMaterialManager->manage(material);
	return material;
}

Material::ptr Engine::createWaterMaterial(Texture *reflectTexture,Texture *refractTexture,Texture *bumpTexture,const Vector4 &color){
	Material::ptr material=static_pointer_cast<WaterMaterialCreator>(mWaterMaterialCreator)->createWaterMaterial(reflectTexture,refractTexture,bumpTexture,color);
	mMaterialManager->manage(material);
	return material;
}

Mesh::ptr Engine::createAABoxMesh(const AABox &box,Material *material){
	Mesh::ptr mesh=static_pointer_cast<AABoxMeshCreator>(mAABoxCreator)->createAABoxMesh(box,material);
	mMeshManager->manage(mesh);
	return mesh;
}

Mesh::ptr Engine::createSphereMesh(const Sphere &sphere,Material *material){
	Mesh::ptr mesh=static_pointer_cast<SphereMeshCreator>(mSphereCreator)->createSphereMesh(sphere,16,16,material);
	mMeshManager->manage(mesh);
	return mesh;
}

Mesh::ptr Engine::createTorusMesh(scalar majorRadius,scalar minorRadius,int numMajor,int numMinor,Material *material){
	Mesh::ptr mesh=static_pointer_cast<TorusMeshCreator>(mTorusCreator)->createTorusMesh(majorRadius,minorRadius,numMajor,numMinor,material);
	mMeshManager->manage(mesh);
	return mesh;
}

Mesh::ptr Engine::createGridMesh(scalar width,scalar height,int numWidth,int numHeight,Material *material){
	Mesh::ptr mesh=static_pointer_cast<GridMeshCreator>(mGridCreator)->createGridMesh(width,height,numWidth,numHeight,material);
	mMeshManager->manage(mesh);
	return mesh;
}

Mesh::ptr Engine::createSkyBoxMesh(scalar size,bool unfolded,bool invert,Material *bottom,Material *top,Material *left,Material *right,Material *back,Material *front){
	Mesh::ptr mesh=static_pointer_cast<SkyBoxMeshCreator>(mSkyBoxCreator)->createSkyBoxMesh(size,unfolded,invert,bottom,top,left,right,back,front);
	mMeshManager->manage(mesh);
	return mesh;
}

Mesh::ptr Engine::createSkyDomeMesh(const Sphere &sphere,int numSegments,int numRings,scalar fade,Material *material){
	Mesh::ptr mesh=static_pointer_cast<SkyDomeMeshCreator>(mSkyDomeCreator)->createSkyDomeMesh(sphere,numSegments,numRings,fade,material);
	mMeshManager->manage(mesh);
	return mesh;
}

}
}
