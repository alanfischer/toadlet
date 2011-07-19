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
	
	bool load(Engine *engine){
		skyColor=Colors::AZURE;
		fadeColor=Vector4(0xB5C1C3FF);

		grass=engine->getMaterialManager()->findMaterial("grass.png");
		if(grass!=NULL){
			TextureState textureState;
			if(grass->getPass()->getTextureState(0,textureState)){
				textureState.calculation=TextureState::CalculationType_NORMAL;
				Math::setMatrix4x4FromScale(textureState.matrix,16,16,16);
				grass->getPass()->setTextureState(0,textureState);
			}
			grass->setLayer(-1);
			grass->retain();
		}

		water=engine->getMaterialManager()->createMaterial();
		if(water!=NULL){
			Vector4 color=Colors::AZURE*1.5;
			color.w=0.5f;

			RenderPath::ptr shaderPath=water->addPath();
			RenderPass::ptr shaderPass=shaderPath->addPass();
			{
				shaderPass->setMaterialState(MaterialState(color));
				shaderPass->setBlendState(BlendState::Combination_ALPHA);
				shaderPass->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
				shaderPass->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));

				TextureState textureState;
				textureState.calculation=TextureState::CalculationType_NORMAL;

				shaderPass->setTexture(0,engine->getTextureManager()->createTexture(createNoise(512,512,16,6,0.5,0.5)));
				Math::setMatrix4x4FromScale(textureState.matrix,16,16,16);
				shaderPass->setTextureState(0,textureState);

				shaderPass->setTexture(1,engine->getTextureManager()->createTexture(createNoise(512,512,16,5,0.5,0.5)));
				Math::setMatrix4x4FromScale(textureState.matrix,16,16,16);
				shaderPass->setTextureState(1,textureState);

				String profiles[]={
					"glsl",
					"hlsl"
				};

				String vertexCodes[]={
					(char*)NULL,

					"struct VIN{\n"
						"float4 position : POSITION;\n"
						"float3 normal : NORMAL;\n"
						"float2 texCoord: TEXCOORD0;\n"
					"};\n"
					"struct VOUT{\n"
						"float4 position : SV_POSITION;\n"
						"float4 color : COLOR;\n"
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

					"VOUT main(VIN vin){\n"
						"VOUT vout;\n"
						"vout.position=mul(modelViewProjectionMatrix,vin.position);\n"
						"float3 viewNormal=normalize(mul(normalMatrix,float4(vin.normal,0.0)));\n"
						"float lightIntensity=clamp(-dot(lightViewPosition,viewNormal),0,1);\n"
						"float4 localLightColor=lightIntensity*lightColor;\n"
						"vout.color=localLightColor*materialDiffuseColor + ambientColor*materialAmbientColor;\n"
						"vout.texCoord0=mul(textureMatrix0,float4(vin.texCoord,0.0,1.0));\n "
						"vout.texCoord1=mul(textureMatrix1,float4(vin.texCoord,0.0,1.0));\n "
						"return vout;\n"
					"}"
				};

				String fragmentCodes[]={
					(char*)NULL,

					"struct PIN{\n"
						"float4 position: SV_POSITION;\n"
						"float4 color: COLOR;\n"
						"float2 texCoord0: TEXCOORD0;\n"
						"float2 texCoord1: TEXCOORD1;\n"
					"};\n"

					"Texture2D tex0,tex1;\n"
					"SamplerState samp0,samp1;\n"

					"float4 main(PIN pin): SV_TARGET{\n"
						"return pin.color*tex0.Sample(samp0,pin.texCoord0)*tex1.Sample(samp1,pin.texCoord1);\n"
					"}"
				};

				Shader::ptr vertexShader=engine->getShaderManager()->createShader(Shader::ShaderType_VERTEX,profiles,vertexCodes,2);
				shaderPass->setShader(Shader::ShaderType_VERTEX,vertexShader);
				Shader::ptr fragmentShader=engine->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,fragmentCodes,2);
				shaderPass->setShader(Shader::ShaderType_FRAGMENT,fragmentShader);

				shaderPass->getVariables()->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
				shaderPass->getVariables()->addVariable("normalMatrix",RenderVariable::ptr(new NormalMatrixVariable()),Material::Scope_RENDERABLE);
				shaderPass->getVariables()->addVariable("lightViewPosition",RenderVariable::ptr(new LightViewPositionVariable()),Material::Scope_MATERIAL);
				shaderPass->getVariables()->addVariable("lightColor",RenderVariable::ptr(new LightDiffuseVariable()),Material::Scope_MATERIAL);
				shaderPass->getVariables()->addVariable("ambientColor",RenderVariable::ptr(new AmbientVariable()),Material::Scope_RENDERABLE);
				shaderPass->getVariables()->addVariable("materialDiffuseColor",RenderVariable::ptr(new MaterialDiffuseVariable()),Material::Scope_MATERIAL);
				shaderPass->getVariables()->addVariable("materialAmbientColor",RenderVariable::ptr(new MaterialAmbientVariable()),Material::Scope_MATERIAL);
				shaderPass->getVariables()->addVariable("textureMatrix0",RenderVariable::ptr(new TextureMatrixVariable(0)),Material::Scope_MATERIAL);
				shaderPass->getVariables()->addVariable("textureMatrix1",RenderVariable::ptr(new TextureMatrixVariable(1)),Material::Scope_MATERIAL);
			}

			RenderPath::ptr fixedPath=water->addPath();
			RenderPass::ptr fixedPass=fixedPath->addPass();
			{
				fixedPass->setMaterialState(MaterialState(color));
				fixedPass->setBlendState(BlendState::Combination_ALPHA);
				fixedPass->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
				fixedPass->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));

				TextureState textureState;
				textureState.calculation=TextureState::CalculationType_NORMAL;

				fixedPass->setTexture(0,engine->getTextureManager()->createTexture(createNoise(512,512,16,6,0.5,0.5)));
				Math::setMatrix4x4FromScale(textureState.matrix,16,16,16);
				fixedPass->setTextureState(0,textureState);

				fixedPass->setTexture(1,engine->getTextureManager()->createTexture(createNoise(512,512,16,5,0.5,0.5)));
				Math::setMatrix4x4FromScale(textureState.matrix,16,16,16);
				fixedPass->setTextureState(1,textureState);
			}

			water->setLayer(-1);
			water->compile();
			water->retain();
		}

		creature=engine->getMeshManager()->findMesh("frog.xmsh");
		if(creature!=NULL){
			Transform transform;
			transform.setTranslate(0,0,-3.5);
			transform.setRotate(Math::Z_UNIT_VECTOR3,Math::PI);
			creature->setTransform(transform);
			creature->retain();
		}
		
		shadow=engine->getMeshManager()->createBox(AABox(-4,-4,0,4,4,0));
		{
			Material::ptr material=engine->getMaterialManager()->createDiffuseMaterial(engine->getTextureManager()->createTexture(createPoint(128,128)));
			material->getPass()->setBlendState(BlendState(BlendState::Operation_ONE_MINUS_SOURCE_ALPHA,BlendState::Operation_SOURCE_ALPHA));
			material->getPass()->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
			material->getPass()->setMaterialState(MaterialState(Colors::BLACK));
			material->retain();
			shadow->getSubMesh(0)->material=material;
			shadow->retain();
		}

		treeBranch=engine->getMaterialManager()->findMaterial("bark.png");
		if(treeBranch!=NULL){
			treeBranch->retain();
		}

		treeLeaf=engine->getMaterialManager()->findMaterial("leaf_top1_alpha.png");
		if(treeLeaf!=NULL){
			treeLeaf->getPass()->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
			treeLeaf->getPass()->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
			treeLeaf->getPass()->setBlendState(BlendState::Combination_ALPHA);
			treeLeaf->retain();
		}

		if(treeBranch!=NULL){
			treeBranchHighlighted=engine->getMaterialManager()->cloneMaterial(treeBranch);
		}
		if(treeBranchHighlighted!=NULL){
			treeBranchHighlighted->getPass()->setMaterialState(MaterialState(Vector4(2,2,2,2)));
			treeBranchHighlighted->retain();
		}

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
		hudFade=engine->getMaterialManager()->createDiffuseMaterial(engine->getTextureManager()->createTexture(createPoint(128,128)));
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

	static Image::ptr createPoint(int width,int height);
	static Image::ptr createNoise(int width,int height,int scale,int seed,scalar brightnessScale,scalar brightnessOffset);

	static Resources *instance;

	Vector4 skyColor,fadeColor;
	Material::ptr grass;
	Material::ptr water;
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

