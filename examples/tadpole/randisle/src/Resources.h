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
			patchSize=128;
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

		Vector4 color=Colors::AZURE;
		color.w=0.5f;

		TextureFormat::ptr waterFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_RGB_8,512,512,1,0);

		if(engine->hasShader(Shader::ShaderType_FRAGMENT)){
			reflectTexture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_RENDERTARGET|Texture::Usage_BIT_AUTOGEN_MIPMAPS,waterFormat);
			reflectTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
			reflectTarget->attach(reflectTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

			refractTexture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_RENDERTARGET|Texture::Usage_BIT_AUTOGEN_MIPMAPS,waterFormat);
			refractTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
			refractTarget->attach(refractTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);
		}

		bumpTexture=engine->getTextureManager()->findTexture("water_bump.png");
		waterMaterial=engine->createWaterMaterial(reflectTexture,refractTexture,bumpTexture,color);

		Log::alert("Loading frog");

		creature=shared_static_cast<Mesh>(engine->getMeshManager()->find("frog.tmsh"));
		if(creature!=NULL){
			Transform::ptr transform=new Transform();
			transform->setTranslate(0,0,-2.0);
			transform->setRotate(Math::Z_UNIT_VECTOR3,Math::PI);
			creature->setTransform(transform);
		}

		Log::alert("Loading grass");

		grass=shared_static_cast<Mesh>(engine->getMeshManager()->find("tall_grass.tmsh"));
		if(grass!=NULL){
			Transform::ptr transform=new Transform();
			transform->setScale(0.1,0.1,0.1);
			grass->setTransform(transform);

			for(int i=0;i<grass->getNumSubMeshes();++i){
				Mesh::SubMesh *subMesh=grass->getSubMesh(i);
				for(int j=0;j<subMesh->material->getNumPaths();++j){
					RenderPath *path=subMesh->material->getPath(j);
					RenderPass *pass=path->getPass(0);
					RenderState *state=pass->getRenderState();

					state->setBlendState(BlendState(BlendState::Combination_ALPHA));
					state->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
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
					RenderState::ptr topState=topPass->getRenderState();
					topState->setRasterizerState(RasterizerState(RasterizerState::CullType_BACK));
					topState->setBlendState(BlendState::Combination_ALPHA);
					topState->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));

					RenderPass::ptr bottomPass=path->addPass(bottomPath->takePass(0));
					RenderState::ptr bottomState=bottomPass->getRenderState();
					bottomState->setRasterizerState(RasterizerState(RasterizerState::CullType_FRONT));
					bottomState->setBlendState(BlendState::Combination_ALPHA);
					bottomState->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
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
//			acorn->getPass()->setMaterialState(MaterialState(false));
//			acorn->getPass()->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
//			acorn->getPass()->setBlendState(BlendState(BlendState::Combination_ALPHA));
		}

		// HUD
		{
			RenderState::ptr hudFadeState=engine->getMaterialManager()->createRenderState();
			hudFadeState->setSamplerState(Shader::ShaderType_FRAGMENT,0,SamplerState(
				SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,
				SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE));
			hudFadeState->setDepthState(DepthState(DepthState::DepthTest_ALWAYS,false));
			hudFadeState->setBlendState(BlendState::Combination_ALPHA);
			hudFadeState->setMaterialState(MaterialState(Colors::TRANSPARENT_RED));
			hudFadeState->setRasterizerState(RasterizerState());

			TextureFormat::ptr pointFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_LA_8,128,128,1,0);
			tbyte *pointData=createPoint(pointFormat);
			Texture::ptr pointTexture=engine->getTextureManager()->createTexture(pointFormat,pointData);
			delete[] pointData;

			hudFade=engine->createDiffuseMaterial(pointTexture,hudFadeState);

			hudFade->getRenderState()->setSamplerState(Shader::ShaderType_FRAGMENT,0,SamplerState(SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,
				SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE));
		}
		
		hudCompass=engine->getMaterialManager()->findMaterial("compass.png");
		if(hudCompass!=NULL){
//			hudCompass->getPass()->setMaterialState(MaterialState(false));
//			hudCompass->getPass()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
//			hudCompass->getPass()->setBlendState(BlendState(BlendState::Operation_ZERO,BlendState::Operation_SOURCE_COLOR));
		}

		hudAcorn=engine->getMaterialManager()->findMaterial("acorn.png");
		if(hudAcorn!=NULL){
//			hudAcorn->getPass()->setMaterialState(MaterialState(false));
//			hudAcorn->getPass()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
//			hudAcorn->getPass()->setBlendState(BlendState(BlendState::Combination_ALPHA));
		}

		hudWooden=engine->getFontManager()->findFont("Pinewood.ttf",100);

		hudSystem=engine->getFontManager()->getDefaultFont();

		return true;
	}

	static tbyte *createNoise(TextureFormat *format,int scale,int seed,scalar brightnessScale,scalar brightnessOffset);

	static tbyte *createPoint(TextureFormat *format);

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

