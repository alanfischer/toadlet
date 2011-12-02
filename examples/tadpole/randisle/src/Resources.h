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

		#if defined(TOADLET_PLATFORM_ANDROID)
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
		waterMaterial=engine->getMaterialManager()->createMaterial();
		if(waterMaterial!=NULL){
			Vector4 color=Colors::AZURE*1.5;
			color.w=0.5f;

			TextureFormat::ptr noiseFormat(new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_RGB_8,128,128,1,0));
			tbyte *noise1Data=createNoise(noiseFormat,16,5,0.5,0.5);
			tbyte *noise2Data=createNoise(noiseFormat,16,12,0.5,0.5);
			Texture::ptr noise1=engine->getTextureManager()->createTexture(noiseFormat,noise1Data);
			Texture::ptr noise2=engine->getTextureManager()->createTexture(noiseFormat,noise2Data);
			delete[] noise1Data;
			delete[] noise2Data;

			RenderPath::ptr shaderPath=waterMaterial->addPath();
			{
				RenderPass::ptr pass=shaderPath->addPass();

				pass->setMaterialState(MaterialState(color));
				pass->setBlendState(BlendState::Combination_ALPHA);
				pass->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
				pass->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));

				TextureState textureState;
				textureState.calculation=TextureState::CalculationType_NORMAL;

				pass->setTexture(0,noise1);
				Math::setMatrix4x4FromScale(textureState.matrix,16,16,16);
				pass->setTextureState(0,textureState);

				pass->setTexture(1,noise2);
				Math::setMatrix4x4FromScale(textureState.matrix,16,16,16);
				pass->setTextureState(1,textureState);

				String profiles[]={
					"glsl",
					"hlsl"
				};

				String vertexCodes[]={
					"attribute vec4 POSITION;\n"
					"attribute vec3 NORMAL;\n"
					"attribute vec2 TEXCOORD0;\n"
					"varying vec4 color\n;"
					"varying vec2 texCoord0\n;"
					"varying float fog;\n"
					"varying vec2 texCoord1\n;"

					"uniform mat4 modelViewProjectionMatrix;\n"
					"uniform mat4 normalMatrix;\n"
					"uniform vec4 materialDiffuseColor;\n"
					"uniform vec4 materialAmbientColor;\n"
					"uniform vec4 lightViewPosition;\n"
					"uniform vec4 lightColor;\n"
					"uniform vec4 ambientColor;\n"
					"uniform mat4 textureMatrix0,textureMatrix1;\n"
					"uniform vec2 fogDistance;\n"

					"void main(){\n"
						"gl_Position=modelViewProjectionMatrix * POSITION;\n"
						"vec3 viewNormal=normalize(normalMatrix * vec4(NORMAL,0.0)).xyz;\n"
						"float lightIntensity=clamp(-dot(lightViewPosition.xyz,viewNormal),0.0,1.0);\n"
						"vec4 localLightColor=lightIntensity*lightColor;\n"
						"color=localLightColor*materialDiffuseColor + ambientColor*materialAmbientColor;\n"
						"texCoord0=(textureMatrix0 * vec4(TEXCOORD0,0.0,1.0)).xy;\n "
						"texCoord1=(textureMatrix1 * vec4(TEXCOORD0,0.0,1.0)).xy;\n "
						"fog=clamp(1.0-(gl_Position.z-fogDistance.x)/(fogDistance.y-fogDistance.x),0.0,1.0);\n"
					"}",



					"struct VIN{\n"
						"float4 position : POSITION;\n"
						"float3 normal : NORMAL;\n"
						"float2 texCoord: TEXCOORD0;\n"
					"};\n"
					"struct VOUT{\n"
						"float4 position : SV_POSITION;\n"
						"float4 color : COLOR;\n"
						"float fog: FOG;\n"
						"float2 texCoord0: TEXCOORD0;\n"
						"float2 texCoord1: TEXCOORD1;\n"
					"};\n"

					"float4x4 modelViewProjectionMatrix;\n"
					"float4x4 normalMatrix;\n"
					"float4 materialDiffuseColor;\n"
					"float4 materialAmbientColor;\n"
					"float4 lightViewPosition;\n"
					"float4 lightColor;\n"
					"float4 ambientColor;\n"
					"float4x4 textureMatrix0,textureMatrix1;\n"
					"float2 fogDistance;\n"

					"VOUT main(VIN vin){\n"
						"VOUT vout;\n"
						"vout.position=mul(modelViewProjectionMatrix,vin.position);\n"
						"float3 viewNormal=normalize(mul(normalMatrix,float4(vin.normal,0.0)));\n"
						"float lightIntensity=clamp(-dot(lightViewPosition,viewNormal),0,1);\n"
						"float4 localLightColor=lightIntensity*lightColor;\n"
						"vout.color=localLightColor*materialDiffuseColor + ambientColor*materialAmbientColor;\n"
						"vout.texCoord0=mul(textureMatrix0,float4(vin.texCoord,0.0,1.0));\n "
						"vout.texCoord1=mul(textureMatrix1,float4(vin.texCoord,0.0,1.0));\n "
						"vout.fog=clamp(1.0-(vout.position.z-fogDistance.x)/(fogDistance.y-fogDistance.x),0.0,1.0);\n"
						"return vout;\n"
					"}"
				};

				String fragmentCodes[]={
					"varying vec4 color;\n"
					"varying vec2 texCoord0;\n"
					"varying vec2 texCoord1;\n"
					"varying float fog;\n"

					"uniform vec4 fogColor;\n"
					"uniform sampler2D tex0,tex1;\n"
					
					"void main(){\n"
						"vec4 fragColor=color*texture2D(tex0,texCoord0)*texture2D(tex1,texCoord1);\n"
						"gl_FragColor=mix(fogColor,fragColor,fog);\n"
					"}",



					"struct PIN{\n"
						"float4 position: SV_POSITION;\n"
						"float4 color: COLOR;\n"
						"float fog: FOG;\n"
						"float2 texCoord0: TEXCOORD0;\n"
						"float2 texCoord1: TEXCOORD1;\n"
					"};\n"

					"float4 fogColor;\n"
					"Texture2D tex0,tex1;\n"
					"SamplerState samp0,samp1;\n"

					"float4 main(PIN pin): SV_TARGET{\n"
						"float4 fragColor=pin.color*tex0.Sample(samp0,pin.texCoord0)*tex1.Sample(samp1,pin.texCoord1);\n"
						"return lerp(fogColor,fragColor,pin.fog);\n"
					"}"
				};

				Shader::ptr vertexShader=engine->getShaderManager()->createShader(Shader::ShaderType_VERTEX,profiles,vertexCodes,2);
				pass->setShader(Shader::ShaderType_VERTEX,vertexShader);
				Shader::ptr fragmentShader=engine->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,fragmentCodes,2);
				pass->setShader(Shader::ShaderType_FRAGMENT,fragmentShader);

				pass->getVariables()->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
				pass->getVariables()->addVariable("normalMatrix",RenderVariable::ptr(new NormalMatrixVariable()),Material::Scope_RENDERABLE);
				pass->getVariables()->addVariable("lightViewPosition",RenderVariable::ptr(new LightViewPositionVariable()),Material::Scope_MATERIAL);
				pass->getVariables()->addVariable("lightColor",RenderVariable::ptr(new LightDiffuseVariable()),Material::Scope_MATERIAL);
				pass->getVariables()->addVariable("ambientColor",RenderVariable::ptr(new AmbientVariable()),Material::Scope_RENDERABLE);
				pass->getVariables()->addVariable("materialDiffuseColor",RenderVariable::ptr(new MaterialDiffuseVariable()),Material::Scope_MATERIAL);
				pass->getVariables()->addVariable("materialAmbientColor",RenderVariable::ptr(new MaterialAmbientVariable()),Material::Scope_MATERIAL);
				pass->getVariables()->addVariable("fogDistance",RenderVariable::ptr(new FogDistanceVariable()),Material::Scope_MATERIAL);
				pass->getVariables()->addVariable("fogColor",RenderVariable::ptr(new FogColorVariable()),Material::Scope_MATERIAL);
				pass->getVariables()->addVariable("textureMatrix0",RenderVariable::ptr(new TextureMatrixVariable(0)),Material::Scope_MATERIAL);
				pass->getVariables()->addVariable("textureMatrix1",RenderVariable::ptr(new TextureMatrixVariable(1)),Material::Scope_MATERIAL);
			}

			RenderPath::ptr fixedPath=waterMaterial->addPath();
			{
				RenderPass::ptr pass=fixedPath->addPass();

				pass->setMaterialState(MaterialState(color));
				pass->setBlendState(BlendState::Combination_ALPHA);
				pass->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
				pass->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));

				TextureState textureState;
				textureState.calculation=TextureState::CalculationType_NORMAL;

				pass->setTexture(0,noise1);
				Math::setMatrix4x4FromScale(textureState.matrix,16,16,16);
				pass->setTextureState(0,textureState);

				pass->setTexture(1,noise2);
				Math::setMatrix4x4FromScale(textureState.matrix,16,16,16);
				pass->setTextureState(1,textureState);
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

		if(treeBranch!=NULL){
			treeBranchHighlighted=engine->getMaterialManager()->cloneMaterial(treeBranch);
		}
		if(treeBranchHighlighted!=NULL){
			treeBranchHighlighted->getPass()->setMaterialState(MaterialState(Vector4(2,2,2,2)));
			treeBranchHighlighted->retain();
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
		hudFade->getPass()->setSamplerState(0,SamplerState(
			SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,
			SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE));
		hudFade->getPass()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
		hudFade->getPass()->setBlendState(BlendState::Combination_ALPHA);
		hudFade->getPass()->setMaterialState(MaterialState(Colors::TRANSPARENT_RED));
		hudFade->retain();
		
		hudCompass=engine->getMaterialManager()->findMaterial("compass.png");
		if(hudCompass!=NULL){
			hudCompass->getPass()->setMaterialState(MaterialState(false));
			hudCompass->getPass()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
			hudCompass->getPass()->setBlendState(BlendState(BlendState::Operation_ZERO,BlendState::Operation_SOURCE_COLOR));
			hudCompass->retain();
		}

		if(acorn!=NULL){
			hudAcorn=engine->getMaterialManager()->cloneMaterial(acorn);
		}
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
	Material::ptr waterMaterial;
	Mesh::ptr creature;
	Mesh::ptr shadow;
	Material::ptr treeBranch;
	Material::ptr treeLeaf;
	Material::ptr treeBranchHighlighted;
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

