#ifndef CLOUDS_H
#define CLOUDS_H

#include <toadlet/egg.h>
#include <toadlet/peeper.h>
#include <toadlet/tadpole.h>
#include <toadlet/tadpole/plugins/SkyDomeMeshCreator.h>
#include <toadlet/tadpole/plugins/SkyBoxMaterialCreator.h>

using namespace toadlet;

class Clouds:public BaseComponent,public ContextListener{
public:
	TOADLET_COMPONENT(Clouds);

	Clouds(Scene *scene,int cloudSize,const Vector4 &skyColor,const Vector4 &fadeColor);
	void destroy();

	void setLightDirection(const Vector3 &lightDir);

	void frameUpdate(int dt,int scope);

	inline Mesh *getMesh(){return mMesh;}
	inline Material *getSkyMaterial(){return mMaterial;}

	virtual void preContextReset(RenderDevice *renderDevice){}
	virtual void postContextReset(RenderDevice *renderDevice){}
	virtual void preContextActivate(RenderDevice *renderDevice){}
	virtual void postContextActivate(RenderDevice *renderDevice);
	virtual void preContextDeactivate(RenderDevice *renderDevice){}
	virtual void postContextDeactivate(RenderDevice *renderDevice){}

protected:
	static void createCloud(TextureFormat *format,tbyte *data,int scale,int seed,float cover,float sharpness,float brightness);
	static void createBump(TextureFormat *format,tbyte *data,tbyte *src,float xscale,float yscale,float zscale,int spread);
	static void createComposite(TextureFormat *format,tbyte *data,tbyte *cloudSrc,tbyte *bumpSrc,const Vector3 &lightDir,const Vector4 &skyColor);
	static void createFade(TextureFormat *format,tbyte *data,const Vector4 &start,const Vector4 &end,float falloff,float sharpness);

	static void setTextureMatrix(const String &name,Material *material,const Matrix4x4 &matrix);

	Engine *mEngine;
	Scene *mScene;

	Vector4 mSkyColor;
	Vector3 mLightDirection;

	TextureFormat::ptr mCloudFormat;
	tbyte *mCloudData;
	Texture::ptr mCloudTexture;
	TextureFormat::ptr mBumpFormat;
	tbyte *mBumpData;
	Texture::ptr mBumpTexture;
	TextureFormat::ptr mFadeFormat;
	tbyte *mFadeData;
	Texture::ptr mFadeTexture;
	TextureFormat::ptr mCompositeFormat;
	tbyte *mCompositeData;
	Texture::ptr mCompositeTexture;

	SkyDomeMeshCreator::ptr mSkyDomeCreator;
	SkyBoxMaterialCreator::ptr mSkyBoxCreator;

	Mesh::ptr mMesh;
	Material::ptr mMaterial;
};

#endif
