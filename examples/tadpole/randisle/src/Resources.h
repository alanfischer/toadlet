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
			tolerance=0.00005;
		#else
			cloudSize=256;
			patchSize=128;
			tolerance=0.000001;
		#endif

		Logger::alert("Loading terrain");

		terrainMaterialSource=DiffuseTerrainMaterialSource::ptr(new DiffuseTerrainMaterialSource(engine));
		#if defined(TOADLET_ANDROID)
			terrainMaterialSource->setDiffuseTexture(0,engine->getTextureManager()->findTexture("rock.png"));
			terrainMaterialSource->setDiffuseTexture(1,NULL);
		#else
			terrainMaterialSource->setDiffuseTexture(0,engine->getTextureManager()->findTexture("seafloor.png"));
			terrainMaterialSource->setDiffuseTexture(1,engine->getTextureManager()->findTexture("rock.png"));
		#endif
		terrainMaterialSource->setDiffuseTexture(2,engine->getTextureManager()->findTexture("grass.png"));

		Logger::alert("Loading water");
		{
			TextureFormat::ptr waterFormat(new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_RGB_8,1024,1024,1,0));

			reflectTexture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_RENDERTARGET|Texture::Usage_BIT_AUTOGEN_MIPMAPS,waterFormat);
			reflectTexture->retain();
			reflectTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
			reflectTarget->attach(reflectTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

			refractTexture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_RENDERTARGET|Texture::Usage_BIT_AUTOGEN_MIPMAPS,waterFormat);
			refractTexture->retain();
			refractTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
			refractTarget->attach(refractTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

			bumpTexture=engine->getTextureManager()->findTexture("water_bump.png");
			bumpTexture->retain();
		}

		waterMaterial=engine->getMaterialManager()->createMaterial();
		if(waterMaterial!=NULL){
			Vector4 color=Colors::AZURE*1.5;
			color.w=0.5f;

			TextureFormat::ptr noiseFormat(new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_RGBA_8,256,256,1,0));
			tbyte *noise1Data=createNoise(noiseFormat,16,5,0.5,0.5);
			tbyte *noise2Data=createNoise(noiseFormat,16,12,0.5,0.5);
			Texture::ptr noise1=engine->getTextureManager()->createTexture(noiseFormat,noise1Data);
			Texture::ptr noise2=engine->getTextureManager()->createTexture(noiseFormat,noise2Data);
			delete[] noise1Data;
			delete[] noise2Data;

			if(engine->hasShader(Shader::ShaderType_VERTEX) && engine->hasShader(Shader::ShaderType_FRAGMENT)){
				RenderPath::ptr shaderPath=waterMaterial->addPath();
				RenderPass::ptr pass=shaderPath->addPass();

				pass->setMaterialState(MaterialState(color));
				pass->setBlendState(BlendState::Combination_ALPHA);
				pass->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
				pass->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
				pass->setMaterialState(MaterialState(Math::ZERO_VECTOR4,Math::ONE_VECTOR4,Math::ONE_VECTOR4,128));

				String profiles[]={
					"glsl",
					"hlsl"
				};

				String vertexCodes[]={
					"attribute vec4 POSITION;\n"
					"attribute vec3 NORMAL;\n"
					"attribute vec2 TEXCOORD0;\n"

					"varying float fog;\n"
					"varying vec4 reflectPosition;\n"
					"varying vec4 refractPosition;\n"
					"varying vec2 bumpPosition;\n"
					"varying vec4 positionFull;\n"

					"uniform mat4 modelViewProjectionMatrix;\n"
					"uniform mat4 modelMatrix;\n"
					"uniform vec2 fogDistance;\n"
					"uniform float time;\n"

					"void main(){\n"
						"gl_Position=modelViewProjectionMatrix * POSITION;\n"
						"positionFull=modelMatrix * POSITION;\n"

						"reflectPosition.x = 0.5 * (gl_Position.w - gl_Position.x);\n"
						"reflectPosition.y = 0.5 * (gl_Position.w - gl_Position.y);\n"
						"reflectPosition.z = gl_Position.w;\n"
						"reflectPosition.w = 1.0;\n"

						"refractPosition.x = 0.5 * (gl_Position.w + gl_Position.x);\n"
						"refractPosition.y = 0.5 * (gl_Position.w - gl_Position.y);\n"
						"refractPosition.z = gl_Position.w;\n"
						"refractPosition.w = 1.0;\n"

						"vec2 windVector=vec2(0,0.1);"
						"float waveLength=0.1;\n"
						"bumpPosition=TEXCOORD0/waveLength + time*windVector;\n"

						"fog=clamp(1.0-(gl_Position.z-fogDistance.x)/(fogDistance.y-fogDistance.x),0.0,1.0);\n"
						// We calcaulate a separate refractFog, stored in positionFull.w, since the refractTex shows artifacts closer than the far plane
						//  due to not being able to use fog in an oblique frustum
						"positionFull.w=clamp(1.0-(gl_Position.z-fogDistance.x/4)/(fogDistance.y/4-fogDistance.x/4),0.0,1.0);\n"
					"}",


                                                                                  
					"struct VIN{\n"
						"float4 position : POSITION;\n"
						"float3 normal : NORMAL;\n"
						"float2 texCoord: TEXCOORD0;\n"
					"};\n"
					"struct VOUT{\n"
						"float4 position : SV_POSITION;\n"
						"float fog: FOG;\n"
						"float4 reflectPosition: TEXCOORD0;\n"
						"float4 refractPosition: TEXCOORD1;\n"
						"float2 bumpPosition: TEXCOORD2;\n"
						"float4 positionFull: TEXCOORD3;\n"
					"};\n"

					"float4x4 modelViewProjectionMatrix;\n"
					"float4x4 modelMatrix;\n"
					"float2 fogDistance;\n"
					"float time;\n"

					"VOUT main(VIN vin){\n"
						"VOUT vout;\n"
						"vout.position=mul(modelViewProjectionMatrix,vin.position);\n"
						"vout.positionFull=mul(modelMatrix,vin.position);\n"

						"vout.reflectPosition.x = 0.5 * (vout.position.w - vout.position.x);\n"
						"vout.reflectPosition.y = 0.5 * (vout.position.w - vout.position.y);\n"
						"vout.reflectPosition.z = vout.position.w;\n"
						"vout.reflectPosition.w=1.0;\n"

						"vout.refractPosition.x = 0.5 * (vout.position.w + vout.position.x);\n"
						"vout.refractPosition.y = 0.5 * (vout.position.w - vout.position.y);\n"
						"vout.refractPosition.z = vout.position.w;\n"
						"vout.refractPosition.w=1.0;\n"

						"float2 windVector=float2(0,0.1f);"
						"float waveLength=0.1f;\n"
						"vout.bumpPosition=vin.texCoord/waveLength + time*windVector;\n"

						"vout.fog=clamp(1.0-(vout.position.z-fogDistance.x)/(fogDistance.y-fogDistance.x),0.0,1.0);\n"
						// We calcaulate a separate refractFog, stored in positionFull.w, since the refractTex shows artifacts closer than the far plane
						//  due to not being able to use fog in an oblique frustum
						"vout.positionFull.w=clamp(1.0-(vout.position.z-fogDistance.x/4)/(fogDistance.y/4-fogDistance.x/4),0.0,1.0);\n"

						"return vout;\n"
					"}"
				};

				String fragmentCodes[]={
					"varying float fog;\n"
					"varying vec4 reflectPosition;\n"
					"varying vec4 refractPosition;\n"
					"varying vec2 bumpPosition;\n"
					"varying vec4 positionFull;\n"

					"uniform vec4 fogColor;\n"
					"uniform vec4 lightPosition;\n"
					"uniform vec4 lightSpecular;\n"
					"uniform vec4 cameraPosition;\n"
					"uniform float waveHeight;\n"
					"uniform vec4 materialSpecular;\n"
					"uniform float materialShininess;\n"
					"uniform sampler2D reflectTex;\n"
					"uniform sampler2D refractTex;\n"
					"uniform sampler2D bumpTex;\n"
					"uniform mat4 reflectMatrix;\n"
					"uniform mat4 refractMatrix;\n"

					"void main(){\n"
						"vec3 normalVector=vec3(0,0,1.0);\n"
						"vec4 bumpColor=texture2D(bumpTex,bumpPosition);\n"
						"vec3 bumpVector=(bumpColor.xyz-0.5)*2.0;\n"

						"vec2 perturbation=waveHeight*bumpVector.xy;\n"

						"vec3 eyeVector = normalize(cameraPosition.xyz - positionFull.xyz);\n"
						"float fresnelTerm = 0.02+0.67*pow((1.0-dot(eyeVector, bumpVector)),5.0);\n"
						"fresnelTerm=clamp(fresnelTerm,0.0,1.0);\n"

						"vec4 reflectColor = texture2D(reflectTex,mod((reflectMatrix * vec4((reflectPosition.xy / reflectPosition.z) + perturbation,0,0)).xy,1.0));\n"
						"vec4 refractColor = texture2D(refractTex,mod((refractMatrix * vec4((refractPosition.xy / refractPosition.z) + perturbation,0,0)).xy,1.0));\n"
						"refractColor=mix(reflectColor,refractColor,positionFull.w);\n"

						"vec4 fragColor=mix(reflectColor,refractColor,fresnelTerm);\n"

						"vec4 dullColor=vec4(0.1,0.1,0.2,1.0);\n"
						"float dullBlendFactor=0.2;\n"
						"fragColor=mix(fragColor,dullColor,dullBlendFactor);\n"

						"float specPerturb=4.0;\n"
						"vec3 halfvec=normalize(eyeVector-lightPosition.xyz+vec3(perturbation.x*specPerturb,perturbation.y*specPerturb,0));\n"
						"float spec=pow(dot(halfvec,normalVector),materialShininess);\n"
						"fragColor=fragColor+mix(vec4(0),lightSpecular*materialSpecular,spec);\n"

						"fragColor.w=1.0;\n"
						"gl_FragColor=mix(fogColor,fragColor,fog);\n"
					"}",



					"struct PIN{\n"
						"float4 position: SV_POSITION;\n"
						"float fog: FOG;\n"
						"float4 reflectPosition: TEXCOORD0;\n"
						"float4 refractPosition: TEXCOORD1;\n"
						"float2 bumpPosition: TEXCOORD2;\n"
						"float4 positionFull: TEXCOORD3;\n"
					"};\n"

					"float4 fogColor;\n"
					"float4 lightPosition;\n"
					"float4 lightSpecular;\n"
					"float4 cameraPosition;\n"
					"float waveHeight;\n"
					"float4 materialSpecular;\n"
					"float materialShininess;\n"
					"Texture2D reflectTex;\n"
					"Texture2D refractTex;\n"
					"Texture2D bumpTex;\n"
					"SamplerState reflectSamp;\n"
					"SamplerState refractSamp;\n"
					"SamplerState bumpSamp;\n"

					"float4 main(PIN pin): SV_TARGET{\n"
						"float3 normalVector=float3(0,0,1.0);\n"
						"float4 bumpColor=bumpTex.Sample(bumpSamp,pin.bumpPosition);\n"
						"float3 bumpVector=(bumpColor.xyz-0.5)*2.0;\n"

						"float2 perturbation=waveHeight*bumpVector.xy;\n"

						"float3 eyeVector = normalize(cameraPosition.xyz - pin.positionFull.xyz);\n"
						"float fresnelTerm = 0.02+0.67*pow((1.0-dot(eyeVector, bumpVector)),5.0);\n"
						"fresnelTerm=clamp(fresnelTerm,0.0,1.0);\n"

						"float4 reflectColor = reflectTex.Sample(reflectSamp,(pin.reflectPosition.xy / pin.reflectPosition.z) + perturbation);\n"
						"float4 refractColor = refractTex.Sample(refractSamp,(pin.refractPosition.xy / pin.refractPosition.z) + perturbation);\n"
						"refractColor=lerp(reflectColor,refractColor,pin.positionFull.w);\n"

						"float4 fragColor=lerp(reflectColor,refractColor,fresnelTerm);\n"

						"float4 dullColor=float4(0.1,0.1,0.2,1.0);\n"
						"float dullBlendFactor=0.2;\n"
						"fragColor=lerp(fragColor,dullColor,dullBlendFactor);\n"

						"float specPerturb=4.0;\n"
						"float3 halfvec=normalize(eyeVector-lightPosition.xyz+float3(perturbation.x*specPerturb,perturbation.y*specPerturb,0));\n"
						"float spec=pow(dot(halfvec,normalVector),materialShininess);\n"
						"fragColor=fragColor+lerp(0.0,lightSpecular*materialSpecular,spec);\n"

						"fragColor.w=1.0;\n"
						"return lerp(fogColor,fragColor,pin.fog);\n"
					"}"
				};

				Shader::ptr vertexShader=engine->getShaderManager()->createShader(Shader::ShaderType_VERTEX,profiles,vertexCodes,2);
				pass->setShader(Shader::ShaderType_VERTEX,vertexShader);
				Shader::ptr fragmentShader=engine->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,fragmentCodes,2);
				pass->setShader(Shader::ShaderType_FRAGMENT,fragmentShader);

				RenderVariableSet::ptr variables=pass->makeVariables();
				variables->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
				variables->addVariable("modelMatrix",RenderVariable::ptr(new ModelMatrixVariable()),Material::Scope_RENDERABLE);
				variables->addVariable("fogDistance",RenderVariable::ptr(new FogDistanceVariable()),Material::Scope_MATERIAL);
				variables->addVariable("fogColor",RenderVariable::ptr(new FogColorVariable()),Material::Scope_MATERIAL);
				variables->addVariable("cameraPosition",RenderVariable::ptr(new CameraPositionVariable()),Material::Scope_MATERIAL);
				variables->addVariable("lightPosition",RenderVariable::ptr(new LightPositionVariable()),Material::Scope_RENDERABLE);
				variables->addVariable("lightSpecular",RenderVariable::ptr(new LightSpecularVariable()),Material::Scope_RENDERABLE);
				variables->addVariable("time",RenderVariable::ptr(new TimeVariable()),Material::Scope_MATERIAL);
				variables->addVariable("waveHeight",RenderVariable::ptr(new ScalarVariable(0.25)),Material::Scope_MATERIAL);
				variables->addVariable("materialSpecular",RenderVariable::ptr(new MaterialSpecularVariable()),Material::Scope_RENDERABLE);
				variables->addVariable("materialShininess",RenderVariable::ptr(new MaterialShininessVariable()),Material::Scope_RENDERABLE);

				variables->addTexture("reflectTex",reflectTexture,"reflectSamp",SamplerState(SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE),TextureState());
				variables->addTexture("refractTex",refractTexture,"refractSamp",SamplerState(SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE),TextureState());
				variables->addTexture("bumpTex",bumpTexture,"bumpSamp",SamplerState(),TextureState());
				variables->addVariable("reflectMatrix",RenderVariable::ptr(new TextureMatrixVariable(variables,"reflectTex")),Material::Scope_MATERIAL);
				variables->addVariable("refractMatrix",RenderVariable::ptr(new TextureMatrixVariable(variables,"refractTex")),Material::Scope_MATERIAL);
			}

			if(engine->hasFixed(Shader::ShaderType_VERTEX) && engine->hasFixed(Shader::ShaderType_FRAGMENT)){
				RenderPath::ptr fixedPath=waterMaterial->addPath();
				RenderPass::ptr pass=fixedPath->addPass();

				pass->setMaterialState(MaterialState(color));
//				pass->setBlendState(BlendState::Combination_ALPHA);
				pass->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
				pass->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));

				TextureState textureState;
				textureState.calculation=TextureState::CalculationType_NORMAL;

				Math::setMatrix4x4FromScale(textureState.matrix,16,16,16);
				pass->setTexture(Shader::ShaderType_FRAGMENT,0,reflectTexture,SamplerState(),textureState);

//				Math::setMatrix4x4FromScale(textureState.matrix,16,16,16);
//				pass->setTexture(Shader::ShaderType_FRAGMENT,0,reflectTexture,SamplerState(),textureState);
			}

			waterMaterial->setLayer(-1);
			waterMaterial->compile();
			waterMaterial->retain();
		}

		Logger::alert("Loading frog");

		creature=engine->getMeshManager()->findMesh("frog.tmsh");
		if(creature!=NULL){
			Transform transform;
			transform.setTranslate(0,0,-3.5);
			transform.setRotate(Math::Z_UNIT_VECTOR3,Math::PI);
			creature->setTransform(transform);
			creature->retain();
		}

		Logger::alert("Loading shadow");

		TextureFormat::ptr pointFormat(new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_A_8,128,128,1,0));
		tbyte *pointData=createPoint(pointFormat);
		Texture::ptr pointTexture=engine->getTextureManager()->createTexture(pointFormat,pointData);
		delete[] pointData;

		shadow=engine->getMeshManager()->createAABoxMesh(AABox(-4,-4,0,4,4,0));
		{
			Material::ptr material=engine->getMaterialManager()->createDiffuseMaterial(pointTexture);
			material->getPass()->setBlendState(BlendState(BlendState::Operation_ONE_MINUS_SOURCE_ALPHA,BlendState::Operation_SOURCE_ALPHA));
			material->getPass()->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
			material->getPass()->setMaterialState(MaterialState(Colors::BLACK));
			// We want it rendered before the water, but on the water layer
			material->setSort(Material::SortType_MATERIAL);
			material->setLayer(-1);
			material->retain();
			shadow->getSubMesh(0)->material=material;
			shadow->retain();
		}

		Logger::alert("Loading tree items");

		treeBranch=engine->getMaterialManager()->findMaterial("bark.png");
		if(treeBranch!=NULL){
			treeBranch->retain();
		}

		treeLeaf=engine->getMaterialManager()->createMaterial();
		if(treeLeaf!=NULL){
			Material::ptr treeLeafTop=engine->getMaterialManager()->findMaterial("leaf_top1_alpha.png");
			Material::ptr treeLeafBottom=engine->getMaterialManager()->findMaterial("leaf_bottom1_alpha.png");

			if(treeLeafTop!=NULL && treeLeafBottom!=NULL){
				int i;
				for(i=0;i<treeLeafTop->getNumPaths();++i){
					RenderPath::ptr path=treeLeaf->addPath();
					RenderPath::ptr topPath=treeLeafTop->getPath(i);
					RenderPath::ptr bottomPath=treeLeafBottom->getPath(i);

					RenderPass::ptr topPass=path->addPass(topPath->takePass(0));
					topPass->setRasterizerState(RasterizerState(RasterizerState::CullType_BACK));
					topPass->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
					topPass->setBlendState(BlendState::Combination_ALPHA);

					RenderPass::ptr bottomPass=path->addPass(bottomPath->takePass(0));
					bottomPass->setRasterizerState(RasterizerState(RasterizerState::CullType_FRONT));
					bottomPass->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
					bottomPass->setBlendState(BlendState::Combination_ALPHA);
				}
			}

			treeLeaf->compile();
			treeLeaf->retain();
		}

		Logger::alert("Loading sounds");

 		dog=engine->getAudioBufferManager()->findAudioBuffer("dog.wav");
		shark=engine->getAudioBufferManager()->findAudioBuffer("shark.wav");
		rustle=engine->getAudioBufferManager()->findAudioBuffer("rustle.wav");
		crunch=engine->getAudioBufferManager()->findAudioBuffer("crunch.wav");

		acorn=engine->getMaterialManager()->findMaterial("acorn.png");
		if(acorn!=NULL){
			acorn->getPass()->setMaterialState(MaterialState(false));
			acorn->getPass()->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
			acorn->getPass()->setBlendState(BlendState(BlendState::Combination_ALPHA));
			acorn->retain();
		}

		// HUD
		hudFade=engine->getMaterialManager()->createDiffuseMaterial(pointTexture);
		/// TODO
/*		hudFade->getPass()->setSamplerState(0,SamplerState(
			SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,
			SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE));
		hudFade->getPass()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
		hudFade->getPass()->setBlendState(BlendState::Combination_ALPHA);
		hudFade->getPass()->setMaterialState(MaterialState(Colors::TRANSPARENT_RED));
*/		hudFade->retain();
		
		hudCompass=engine->getMaterialManager()->findMaterial("compass.png");
		if(hudCompass!=NULL){
			hudCompass->getPass()->setMaterialState(MaterialState(false));
			hudCompass->getPass()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
			hudCompass->getPass()->setBlendState(BlendState(BlendState::Operation_ZERO,BlendState::Operation_SOURCE_COLOR));
			hudCompass->retain();
		}

		hudAcorn=engine->getMaterialManager()->findMaterial("acorn.png");
		if(hudAcorn!=NULL){
			hudAcorn->getPass()->setMaterialState(MaterialState(false));
			hudAcorn->getPass()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
			hudAcorn->getPass()->setBlendState(BlendState(BlendState::Combination_ALPHA));
			hudAcorn->retain();
		}

		hudWooden=engine->getFontManager()->findFont("Pinewood.ttf",100);
		if(hudWooden!=NULL){
			hudWooden->retain();
		}

		hudSystem=engine->getFontManager()->getDefaultFont();
		if(hudSystem!=NULL){
			hudSystem->retain();
		}

		return true;
	}

	static tbyte *createPoint(TextureFormat *format);
	static tbyte *createNoise(TextureFormat *format,int scale,int seed,scalar brightnessScale,scalar brightnessOffset);

	static Resources *instance;

	int cloudSize;
	int patchSize;
	scalar tolerance;
	Vector4 skyColor,fadeColor;
	DiffuseTerrainMaterialSource::ptr terrainMaterialSource;
	Texture::ptr reflectTexture,refractTexture,bumpTexture;
	PixelBufferRenderTarget::ptr reflectTarget,refractTarget;
	Material::ptr waterMaterial;
	Mesh::ptr creature;
	Mesh::ptr shadow;
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

