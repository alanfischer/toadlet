#ifndef CLOUDS_H
#define CLOUDS_H

#include <toadlet/egg.h>
#include <toadlet/peeper.h>
#include <toadlet/tadpole.h>

/// @todo: Move these into toadlet somewhere, maybe just base of tadpole
class Matrix4x4Accessor{
public:
	TOADLET_SPTR(Matrix4x4Accessor);
	virtual ~Matrix4x4Accessor(){}

	virtual void setMatrix4x4(const Matrix4x4 &matrix)=0;
	virtual const Matrix4x4 &getMatrix4x4()=0;
};

class TextureStateMatrix4x4Accessor:public Matrix4x4Accessor{
public:
	TOADLET_SPTR(TextureStateMatrix4x4Accessor);

	TextureStateMatrix4x4Accessor(RenderPass::ptr pass,int state,TextureState::CalculationType calculation=TextureState::CalculationType_NORMAL):
		mPass(pass),mState(state){
		mPass->getTextureState(Shader::ShaderType_VERTEX,mState,mTextureState);
		mTextureState.calculation=calculation;
		mPass->setTextureState(Shader::ShaderType_VERTEX,mState,mTextureState);
	}

	void setMatrix4x4(const Matrix4x4 &matrix){
		mPass->getTextureState(Shader::ShaderType_VERTEX,mState,mTextureState);
		mTextureState.matrix.set(matrix);
		mPass->setTextureState(Shader::ShaderType_VERTEX,mState,mTextureState);
	}

	const Matrix4x4 &getMatrix4x4(){
		mPass->getTextureState(Shader::ShaderType_FRAGMENT,mState,mTextureState);
		return mTextureState.matrix;
	}

protected:
	RenderPass::ptr mPass;
	int mState;
	TextureState mTextureState;
};

class Clouds:public BaseComponent{
public:
	TOADLET_OBJECT(Clouds);

	Clouds(Scene *scene,int cloudSize,const Vector4 &skyColor,const Vector4 &fadeColor);
	void destroy();

	void setLightDirection(const Vector3 &lightDir);

	void frameUpdate(int dt,int scope);

	inline Mesh *getMesh(){return mMesh;}
	inline Material *getSkyMaterial(){return mMaterial;}

protected:
	static void createCloud(TextureFormat *format,tbyte *data,int scale,int seed,float cover,float sharpness,float brightness);
	static void createBump(TextureFormat *format,tbyte *data,tbyte *src,float xscale,float yscale,float zscale,int spread);
	static void createComposite(TextureFormat *format,tbyte *data,tbyte *cloudSrc,tbyte *bumpSrc,const Vector3 &lightDir,const Vector4 &skyColor);
	static void createFade(TextureFormat *format,tbyte *data,const Vector4 &start,const Vector4 &end,float falloff,float sharpness);

	Engine *mEngine;
	Scene *mScene;

	Vector4 mSkyColor;

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

	Matrix4x4Accessor::ptr mShaderAccessor,mBumpAccessor,mCloudAccessor,mColorAccessor,mFadeAccessor;
	Matrix4x4Accessor::ptr mCompositeAccessor;

	Mesh::ptr mMesh;
	Material::ptr mMaterial;
};

#endif
