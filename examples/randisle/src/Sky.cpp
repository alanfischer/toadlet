#include "Sky.h"
#include <toadlet/tadpole/plugins/SkyDomeMeshCreator.h>

Sky::Sky(Scene *scene,int cloudSize,const Vector4 &skyColor,const Vector4 &fadeColor):Node(scene)
{
	Vector3 lightDir(1,-1,0.5);

	mClouds=new Clouds(scene,cloudSize,skyColor,fadeColor);
	attach(mClouds);

	mSkyDome=new MeshComponent(mEngine);
	mSkyDome->setMesh(mClouds->getMesh());
	attach(mSkyDome);

	TextureFormat::ptr glowFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_L_8,128,128,1,0);
	tbyte *glowData=createGlow(glowFormat);
	Texture::ptr glowTexture=mEngine->getTextureManager()->createTexture(glowFormat,glowData);
	delete[] glowData;

	RenderState::ptr renderState=mEngine->getMaterialManager()->createRenderState();
	renderState->setGeometryState(GeometryState(GeometryState::MatrixFlag_CAMERA_ALIGNED));

	Material::ptr sunMaterial=mEngine->createDiffuseMaterial(glowTexture,renderState);
	sunMaterial->setLayer(-1);
	sunMaterial->setSort(Material::SortType_MATERIAL);
	sunMaterial->compile();

	mSunNode=new Node(mScene);
	{
		mSun=new SpriteComponent(mEngine);
		mSun->setMaterial(sunMaterial);
		mSun->setSharedRenderState(NULL);
		mSun->getSharedRenderState()->setBlendState(BlendState::Combination_COLOR_ADDITIVE);
		mSun->getSharedRenderState()->setMaterialState(MaterialState(false));
		mSun->getSharedRenderState()->setDepthState(DepthState(DepthState::DepthTest_ALWAYS,false));

		mSunNode->attach(mSun);
	}
	mSunNode->setScale(128,128,128);
	attach(mSunNode);

	Log::alert("Done allocating Sky resources");

	mLightNode=new Node(mScene);
	{
		mLight=new LightComponent();
		LightState lightState;
		lightState.diffuseColor.set(Colors::WHITE);
		lightState.specularColor.set(Colors::WHITE/4);
		mLight->setLightState(lightState);
		mLightNode->attach(mLight);
	}
	attach(mLightNode);

	Math::normalize(lightDir);
	updateLightDirection(lightDir);
}

void Sky::updateLightDirection(const Vector3 &lightDir){
	LightState state;
	mLight->getLightState(state);
	state.direction=-lightDir;
	mLight->setLightState(state);

	mSunNode->setTranslate(lightDir*256);

	mClouds->setLightDirection(lightDir);
}

tbyte *Sky::createGlow(TextureFormat *format){
	int width=format->getWidth(),height=format->getHeight();
	uint8 *data=new uint8[format->getDataSize()];

	int x=0,y=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			float v=1.0;
			v=v*(1.0-Math::length(Vector2(x-width/2,y-height/2))/(width/2));
			v=pow(v*2,3);
			v=Math::clamp(0,1,v);

			data[y*width+x]=255*v;
		}
	}

	return data;
}
