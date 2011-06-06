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
		Matrix4x4 matrix;
	
		skyColor=Colors::AZURE;
		fadeColor=Vector4(0xB5C1C3FF);

		grass=engine->getMaterialManager()->findMaterial("grass.png");
		if(grass!=NULL){
			TextureState textureState;
			if(grass->getTextureState(0,textureState)){
				textureState.calculation=TextureState::CalculationType_NORMAL;
				Math::setMatrix4x4FromScale(textureState.matrix,16,16,16);
				grass->setTextureState(0,textureState);
			}
//			grass->setLayer(-1);
			grass->retain();
		}

		water=engine->getMaterialManager()->createMaterial();
		if(water!=NULL){
			Vector4 color=Colors::AZURE*1.5;
			color.w=0.5f;
			water->setMaterialState(MaterialState(color));
			water->setBlendState(BlendState::Combination_ALPHA);
			water->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
			water->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));

			TextureState textureState;
			textureState.calculation=TextureState::CalculationType_NORMAL;

			water->setTexture(0,engine->getTextureManager()->createTexture(createNoise(512,512,16,6,0.5,0.5)));
			Math::setMatrix4x4FromScale(textureState.matrix,16,16,16);
			water->setTextureState(0,textureState);

			water->setTexture(1,engine->getTextureManager()->createTexture(createNoise(512,512,16,5,0.5,0.5)));
			Math::setMatrix4x4FromScale(textureState.matrix,16,16,16);
			water->setTextureState(1,textureState);

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
			Material::ptr material=engine->getMaterialManager()->createMaterial(engine->getTextureManager()->createTexture(createPoint(128,128)));
			material->setBlendState(BlendState(BlendState::Operation_ONE_MINUS_SOURCE_ALPHA,BlendState::Operation_SOURCE_ALPHA));
			material->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
			material->setMaterialState(MaterialState(Colors::BLACK));
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
			treeLeaf->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
			treeLeaf->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
			treeLeaf->setBlendState(BlendState::Combination_ALPHA);
			treeLeaf->retain();
		}

		if(treeBranch!=NULL){
			treeBranchHighlighted=engine->getMaterialManager()->cloneMaterial(treeBranch,false);
		}
		if(treeBranchHighlighted!=NULL){
			treeBranchHighlighted->setMaterialState(MaterialState(Vector4(2,2,2,2)));
			treeBranchHighlighted->retain();
		}

 		dog=engine->getAudioBufferManager()->findAudioBuffer("dog.wav");
		shark=engine->getAudioBufferManager()->findAudioBuffer("shark.wav");
		rustle=engine->getAudioBufferManager()->findAudioBuffer("rustle.wav");
		crunch=engine->getAudioBufferManager()->findAudioBuffer("crunch.wav");

		acorn=engine->getMaterialManager()->findMaterial("acorn.png");
		if(acorn!=NULL){
			acorn->setMaterialState(MaterialState(false));
			acorn->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
			acorn->setBlendState(BlendState(BlendState::Combination_ALPHA));
			acorn->retain();
		}

		// HUD
		hudFade=engine->getMaterialManager()->createMaterial(engine->getTextureManager()->createTexture(createPoint(128,128)),true);
		hudFade->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
		hudFade->setBlendState(BlendState::Combination_ALPHA);
		hudFade->setMaterialState(MaterialState(Colors::TRANSPARENT_RED));
		hudFade->retain();
		
		hudCompass=engine->getMaterialManager()->findMaterial("compass.png");
		if(hudCompass!=NULL){
			hudCompass->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
			hudCompass->setBlendState(BlendState(BlendState::Operation_ZERO,BlendState::Operation_SOURCE_COLOR));
			hudCompass->setMaterialState(MaterialState(false));
			hudCompass->retain();
		}

		if(acorn!=NULL){
			hudAcorn=engine->getMaterialManager()->cloneMaterial(acorn,true);
		}
		if(hudAcorn!=NULL){
			hudAcorn->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
			hudAcorn->setBlendState(BlendState(BlendState::Combination_ALPHA));
			hudAcorn->setMaterialState(MaterialState(false));
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

