#ifndef RESOURCES_H
#define RESOURCES_H

#include <toadlet/toadlet.h>

class Resources:public Object,public ResourceRequest{
public:
	TOADLET_IOBJECT(Resources);

	static bool init(Engine *engine,ResourceRequest *request){
		if(instance==NULL){
			instance=new Resources();
			instance->engine=engine;
			instance->request=request;
			instance->resourceCount=0;
		}
		return instance->load(engine);
	}
	
	bool load(Engine *engine){
		resourceCount++;

		loadResource("detail.png",engine->getTextureManager());
		loadResource("sea.png",engine->getTextureManager());
		loadResource("rock.png",engine->getTextureManager());
		loadResource("grass.png",engine->getTextureManager());
		loadResource("water_bump.png",engine->getTextureManager());
		loadResource("frog.tmsh",engine->getMeshManager());
		loadResource("tall_grass.tmsh",engine->getMeshManager());
		loadResource("bark.png",engine->getMaterialManager());
		loadResource("leaf_top1_alpha.png",engine->getMaterialManager());
		loadResource("leaf_bottom1_alpha.png",engine->getMaterialManager());
		loadResource("acorn.png",engine->getTextureManager());
		loadResource("compass.png",engine->getTextureManager());
		loadResource("dog.wav",engine->getAudioManager());
		loadResource("rustle.wav",engine->getAudioManager());
		loadResource("crunch.wav",engine->getAudioManager());
		loadResource("Pinewood.ttf",engine->getFontManager(),new FontData(100));

		resourceReady(NULL); // Dummy ready to finish loading

		return true;
	}

	void loadResource(const String &resource,ResourceManager *manager,ResourceData *data=NULL){
		resourceCount++;
		manager->find(resource,this,data);
	}

	void resourceReady(Resource *resource){
		resources.add(resource);
		resourceCount--;
		if(resourceCount==0){
			build(engine);
			request->resourceReady(NULL);
		}
	}

	void resourceException(const Exception &ex){
		resourceReady(NULL);
	}
	
	void resourceProgress(float progress){}

	bool build(Engine *engine){
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
			maxPropDist=100;
			minPropDist=80;
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

		bumpTexture=engine->getTextureManager()->getTexture("water_bump.png");
		waterMaterial=engine->createWaterMaterial(reflectTexture,refractTexture,bumpTexture,color);

		Log::alert("Loading frog");

		creature=engine->getMeshManager()->getMesh("frog.tmsh");
		if(creature!=NULL){
			Transform::ptr transform=new Transform();
			transform->setTranslate(0,0,-2.0);
			transform->setRotate(Math::Z_UNIT_VECTOR3,Math::PI);
			creature->setTransform(transform);
		}

		Log::alert("Loading grass");

		grass=engine->getMeshManager()->getMesh("tall_grass.tmsh");
		if(grass!=NULL){
			Transform::ptr transform=new Transform();
			transform->setScale(0.1,0.1,0.1);
			grass->setTransform(transform);

			for(int i=0;i<grass->getNumSubMeshes();++i){
				Mesh::SubMesh *subMesh=grass->getSubMesh(i);
				tforeach(Material::PathCollection::iterator,path,subMesh->material->getPaths()){
					RenderPass *pass=path->getPasses()[0];
					RenderState *state=pass->getRenderState();

					state->setBlendState(BlendState(BlendState::Combination_ALPHA));
					state->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
				}
			}
		}

		Log::alert("Loading tree items");

		treeBranch=engine->getMaterialManager()->getMaterial("bark.png");

		treeLeaf=engine->getMaterialManager()->createMaterial();
		if(treeLeaf!=NULL){
			Material::ptr treeLeafTop=engine->getMaterialManager()->getMaterial("leaf_top1_alpha.png");
			Material::ptr treeLeafBottom=engine->getMaterialManager()->getMaterial("leaf_bottom1_alpha.png");

			if(treeLeafTop!=NULL && treeLeafBottom!=NULL){
				for(int i=0;i<treeLeafTop->getPaths().size();++i){
					RenderPath::ptr path=treeLeaf->addPath();
					RenderPath::ptr topPath=treeLeafTop->getPaths()[i];
					RenderPath::ptr bottomPath=treeLeafBottom->getPaths()[i];

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

 		dog=engine->getAudioManager()->getAudioBuffer("dog.wav");
		rustle=engine->getAudioManager()->getAudioBuffer("rustle.wav");
		crunch=engine->getAudioManager()->getAudioBuffer("crunch.wav");

		{
			RenderState::ptr renderState=engine->getMaterialManager()->createRenderState();
			renderState->setMaterialState(MaterialState(false));
			renderState->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
			renderState->setBlendState(BlendState(BlendState::Combination_ALPHA));
			renderState->setGeometryState(GeometryState(GeometryState::MatrixFlag_ASPECT_CORRECT));
			SamplerState samplerState;
			samplerState.uAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
			samplerState.vAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
			samplerState.wAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
			renderState->setSamplerState(Shader::ShaderType_FRAGMENT,0,samplerState);
			acorn=engine->createDiffuseMaterial(engine->getTextureManager()->getTexture("acorn.png"),renderState);
		}

		// HUD
		{
			RenderState::ptr renderState=engine->getMaterialManager()->createRenderState();
			renderState->setSamplerState(Shader::ShaderType_FRAGMENT,0,SamplerState(
				SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,
				SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE));
			renderState->setDepthState(DepthState(DepthState::DepthTest_ALWAYS,false));
			renderState->setBlendState(BlendState::Combination_ALPHA);
			renderState->setMaterialState(MaterialState(Colors::TRANSPARENT_RED));
			renderState->setRasterizerState(RasterizerState());
			renderState->setSamplerState(Shader::ShaderType_FRAGMENT,0,SamplerState(
				SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,
				SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE));

			TextureFormat::ptr pointFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_LA_8,128,128,1,0);
			tbyte *pointData=createPoint(pointFormat);
			Texture::ptr pointTexture=engine->getTextureManager()->createTexture(pointFormat,pointData);
			delete[] pointData;

			fade=engine->createDiffuseMaterial(pointTexture,renderState);
		}
		
		{
			RenderState::ptr renderState=engine->getMaterialManager()->createRenderState();
			renderState->setMaterialState(MaterialState(false));
			renderState->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
			renderState->setBlendState(BlendState(BlendState::Operation_ZERO,BlendState::Operation_SOURCE_COLOR));
			renderState->setGeometryState(GeometryState(GeometryState::MatrixFlag_ASPECT_CORRECT));
			SamplerState samplerState;
			samplerState.uAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
			samplerState.vAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
			samplerState.wAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
			renderState->setSamplerState(Shader::ShaderType_FRAGMENT,0,samplerState);
			compass=engine->createDiffuseMaterial(engine->getTextureManager()->getTexture("compass.png"),renderState);
		}

		wooden=engine->getFontManager()->getFont("Pinewood.ttf",100);

		system=engine->getFontManager()->getDefaultFont();

		return true;
	}

	static tbyte *createNoise(TextureFormat *format,int scale,int seed,scalar brightnessScale,scalar brightnessOffset);

	static tbyte *createPoint(TextureFormat *format);

	static Resources::ptr instance;

	Engine *engine;
	ResourceRequest *request;
	Collection<Resource::ptr> resources;
	int resourceCount;

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
	AudioBuffer::ptr rustle;
	AudioBuffer::ptr crunch;

	// HUD
	Material::ptr fade;
	Material::ptr compass;
	Font::ptr wooden;
	Font::ptr system;
};

#endif

