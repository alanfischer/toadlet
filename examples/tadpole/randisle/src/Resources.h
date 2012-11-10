#ifndef RESOURCES_H
#define RESOURCES_H

#include <toadlet/toadlet.h>

class Resources{
public:
	static bool init(Engine *engine){
		if(instance==NULL){
			instance=new Resources();
		}
		return instance->load(engine);
	}
	
	static void destroy(){
		delete instance;
		instance=NULL;
	}

	bool load(Engine *engine){
		skyColor=Colors::AZURE;
		fadeColor=Vector4(0xB5C1C3FF);

		#if defined(TOADLET_PLATFORM_ANDROID) || defined(TOADLET_PLATFORM_IOS)
			cloudSize=128;
			patchSize=32;
			tolerance=0.0001;

			numProps=20;
			maxPropDist=80;
			minPropDist=60;
		#else
			cloudSize=512;
			patchSize=16;
			tolerance=0.000001;

			numProps=100;
			maxPropDist=80;
			minPropDist=60;
		#endif

		Log::alert("Loading terrain");

		terrainMaterialSource=new DiffuseTerrainMaterialSource(engine);
		terrainMaterialSource->setDiffuseScale(Vector3(16,16,16));
		terrainMaterialSource->setDetailScale(Vector3(96,96,96));
		terrainMaterialSource->setDetailTexture("detail.png");
		#if defined(TOADLET_PLATFORM_ANDROID)
			terrainMaterialSource->setDiffuseTexture(0,"rock.png");
			terrainMaterialSource->setDiffuseTexture(1,NULL);
		#else
			terrainMaterialSource->setDiffuseTexture(0,"seafloor.png");
			terrainMaterialSource->setDiffuseTexture(1,"rock.png");
		#endif
		terrainMaterialSource->setDiffuseTexture(2,"grass.png");

		Log::alert("Loading water");

		Vector4 color=Colors::AZURE*1.5;
		color.w=0.5f;

		TextureFormat::ptr waterFormat(new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_RGB_8,512,512,1,0));

		if(engine->isShaderAllowed()){
			reflectTexture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_RENDERTARGET|Texture::Usage_BIT_AUTOGEN_MIPMAPS,waterFormat);
			reflectTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
			reflectTarget->attach(reflectTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

			refractTexture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_RENDERTARGET|Texture::Usage_BIT_AUTOGEN_MIPMAPS,waterFormat);
			refractTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
			refractTarget->attach(refractTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);
		}

		bumpTexture=engine->getTextureManager()->findTexture("water_bump.png");
		waterMaterial=engine->createWaterMaterial(reflectTexture,refractTexture,bumpTexture,color);

		/// @todo: Change the water shader so the bump time is replaced with a texture matrix offset, then we can animate it properly both fixed and shader
/*
	Material::ptr waterMaterial=Resources::instance->waterMaterial;
	if(waterMaterial!=NULL){
		TextureState textureState;
		waterMaterial->getPass()->getTextureState(Shader::ShaderType_FRAGMENT,0,textureState);
		Math::setMatrix4x4FromTranslate(textureState.matrix,Math::sin(Math::fromMilli(mScene->getTime())/4)/4,0,0);
		waterMaterial->getPass()->setTextureState(Shader::ShaderType_FRAGMENT,0,textureState);

		waterMaterial->getPass()->getTextureState(Shader::ShaderType_FRAGMENT,1,textureState);
		Math::setMatrix4x4FromTranslate(textureState.matrix,0,Math::sin(Math::fromMilli(mScene->getTime())/4)/4,0);
		waterMaterial->getPass()->setTextureState(Shader::ShaderType_FRAGMENT,1,textureState);
	}
*/

		Log::alert("Loading frog");

		creature=shared_static_cast<Mesh>(engine->getMeshManager()->find("frog.tmsh"));
		if(creature!=NULL){
			Transform transform;
			transform.setTranslate(0,0,-2.0);
			transform.setRotate(Math::Z_UNIT_VECTOR3,Math::PI);
			creature->setTransform(transform);
		}

		Log::alert("Loading shadow");

		TextureFormat::ptr pointFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_A_8,128,128,1,0);
		tbyte *pointData=createPoint(pointFormat);
		Texture::ptr pointTexture=engine->getTextureManager()->createTexture(pointFormat,pointData);
		delete[] pointData;

		shadow=engine->createAABoxMesh(AABox(-4,-4,0,4,4,0));
		{
			Material::ptr material=engine->createDiffuseMaterial(pointTexture);
			material->getPass()->setBlendState(BlendState(BlendState::Operation_ONE_MINUS_SOURCE_ALPHA,BlendState::Operation_SOURCE_ALPHA));
			material->getPass()->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
			material->getPass()->setMaterialState(MaterialState(Colors::BLACK));
			// We want it rendered before the water, but on the water layer
			material->setSort(Material::SortType_MATERIAL);
			material->setLayer(-1);
			shadow->getSubMesh(0)->material=material;
		}

		Log::alert("Loading grass");

		grass=shared_static_cast<Mesh>(engine->getMeshManager()->find("tall_grass.tmsh"));
		if(creature!=NULL){
			Transform transform;
			transform.setScale(0.1,0.1,0.1);
			grass->setTransform(transform);

			for(int i=0;i<grass->getNumSubMeshes();++i){
				Mesh::SubMesh *subMesh=grass->getSubMesh(i);
				for(int j=0;j<subMesh->material->getNumPaths();++j){
					RenderPath *path=subMesh->material->getPath(j);
					RenderPass *pass=path->getPass(0);

					pass->setBlendState(BlendState(BlendState::Combination_ALPHA));
					pass->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
				}
			}
		}

		Log::alert("Loading tree items");

		treeBranch=engine->getMaterialManager()->findMaterial("bark.png");

		treeLeaf=engine->getMaterialManager()->createMaterial();
		if(treeLeaf!=NULL){
			Material::ptr treeLeafTop=engine->getMaterialManager()->findMaterial("leaf_top1_alpha.png");
			Material::ptr treeLeafBottom=engine->getMaterialManager()->findMaterial("leaf_bottom1_alpha.png");

			if(treeLeafTop!=NULL && treeLeafBottom!=NULL){
				for(int i=0;i<treeLeafTop->getNumPaths();++i){
					RenderPath::ptr path=treeLeaf->addPath();
					RenderPath::ptr topPath=treeLeafTop->getPath(i);
					RenderPath::ptr bottomPath=treeLeafBottom->getPath(i);

					RenderPass::ptr topPass=path->addPass(topPath->takePass(0));
					topPass->setRasterizerState(RasterizerState(RasterizerState::CullType_BACK));
					topPass->setBlendState(BlendState::Combination_ALPHA);
					topPass->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));

					RenderPass::ptr bottomPass=path->addPass(bottomPath->takePass(0));
					bottomPass->setRasterizerState(RasterizerState(RasterizerState::CullType_FRONT));
					bottomPass->setBlendState(BlendState::Combination_ALPHA);
					bottomPass->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
				}
			}

			treeLeaf->compile();
		}

		Log::alert("Loading sounds");

 		dog=engine->getAudioManager()->findAudioBuffer("dog.wav");
		shark=engine->getAudioManager()->findAudioBuffer("shark.wav");
		rustle=engine->getAudioManager()->findAudioBuffer("rustle.wav");
		crunch=engine->getAudioManager()->findAudioBuffer("crunch.wav");

		acorn=engine->getMaterialManager()->findMaterial("acorn.png");
		if(acorn!=NULL){
			acorn->getPass()->setMaterialState(MaterialState(false));
			acorn->getPass()->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
			acorn->getPass()->setBlendState(BlendState(BlendState::Combination_ALPHA));
		}

		// HUD
		hudFade=engine->createDiffuseMaterial(pointTexture);
		/// TODO
/*		hudFade->getPass()->setSamplerState(0,SamplerState(
			SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,
			SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE));
		hudFade->getPass()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
		hudFade->getPass()->setBlendState(BlendState::Combination_ALPHA);
		hudFade->getPass()->setMaterialState(MaterialState(Colors::TRANSPARENT_RED));
*/
		
		hudCompass=engine->getMaterialManager()->findMaterial("compass.png");
		if(hudCompass!=NULL){
			hudCompass->getPass()->setMaterialState(MaterialState(false));
			hudCompass->getPass()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
			hudCompass->getPass()->setBlendState(BlendState(BlendState::Operation_ZERO,BlendState::Operation_SOURCE_COLOR));
		}

		hudAcorn=engine->getMaterialManager()->findMaterial("acorn.png");
		if(hudAcorn!=NULL){
			hudAcorn->getPass()->setMaterialState(MaterialState(false));
			hudAcorn->getPass()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
			hudAcorn->getPass()->setBlendState(BlendState(BlendState::Combination_ALPHA));
		}

		hudWooden=engine->getFontManager()->findFont("Pinewood.ttf",100);

		hudSystem=engine->getFontManager()->getDefaultFont();

		return true;
	}

	static tbyte *createPoint(TextureFormat *format);
	static tbyte *createNoise(TextureFormat *format,int scale,int seed,scalar brightnessScale,scalar brightnessOffset);

	static Resources *instance;

	int cloudSize;
	int patchSize;
	scalar tolerance;
	Vector4 skyColor,fadeColor;
	int numProps;
	scalar maxPropDist,minPropDist;
	DiffuseTerrainMaterialSource::ptr terrainMaterialSource;
	Texture::ptr reflectTexture,refractTexture,refractDepthTexture,bumpTexture;
	PixelBufferRenderTarget::ptr reflectTarget,refractTarget;
	Material::ptr waterMaterial;
	Mesh::ptr creature;
	Mesh::ptr shadow;
	Mesh::ptr grass;
	Material::ptr treeBranch;
	Material::ptr treeLeaf;
	Material::ptr acorn;
	AudioBuffer::ptr dog;
	AudioBuffer::ptr shark;
	AudioBuffer::ptr rustle;
	AudioBuffer::ptr crunch;

	// HUD
	Material::ptr hudFade;
	Material::ptr hudCompass;
	Material::ptr hudAcorn;
	Font::ptr hudWooden;
	Font::ptr hudSystem;
};

#endif

