#ifndef SKY_H
#define SKY_H

#include <toadlet/egg.h>
#include <toadlet/peeper.h>
#include <toadlet/tadpole.h>

/// @todo: Move these into toadlet somewhere, maybe just base of tadpole
class Matrix4x4Accessor{
public:
	TOADLET_SHARED_POINTERS(Matrix4x4Accessor);

	virtual void setMatrix4x4(const Matrix4x4 &matrix)=0;
	virtual const Matrix4x4 &getMatrix4x4()=0;
};

class TextureStateMatrix4x4Accessor:public Matrix4x4Accessor{
public:
	TOADLET_SHARED_POINTERS(TextureStateMatrix4x4Accessor);

	TextureStateMatrix4x4Accessor(RenderPass::ptr pass,int state,TextureState::CalculationType calculation=TextureState::CalculationType_NORMAL):
		mPass(pass),mState(state){
		mPass->getTextureState(Shader::ShaderType_FRAGMENT,mState,mTextureState);
		mTextureState.calculation=calculation;
		mPass->setTextureState(Shader::ShaderType_FRAGMENT,mState,mTextureState);
	}

	void setMatrix4x4(const Matrix4x4 &matrix){
		mPass->getTextureState(Shader::ShaderType_FRAGMENT,mState,mTextureState);
		mTextureState.matrix.set(matrix);
		mPass->setTextureState(Shader::ShaderType_FRAGMENT,mState,mTextureState);
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

class Sky:public ParentNode{
public:
	TOADLET_NODE(Sky,ParentNode);

	Sky():super(){}

	Node *create(Scene *scene,int cloudSize,const Vector4 &skyColor,const Vector4 &fadeColor);
	void destroy();

	void updateLightDirection(const Vector3 &lightDir);

	void frameUpdate(int dt,int scope);

	inline LightNode ::ptr getLight(){return mLight;}
	inline MeshNode::ptr getSkyDome(){return mSkyDome;}
	inline Material::ptr getSkyMaterial(){return mSkyMaterial;}
	inline SpriteNode::ptr getSun(){return mSun;}

protected:
	static tbyte *createCloud(TextureFormat *format,int scale,int seed,float cover,float sharpness,float brightness);
	static tbyte *createBump(TextureFormat *format,tbyte *src,float xscale,float yscale,float zscale,int spread);
	static tbyte *createComposite(TextureFormat *format,tbyte *cloudSrc,tbyte *bumpSrc,const Vector3 &lightDir,const Vector4 &skyColor);
	static tbyte *createFade(TextureFormat *format,const Vector4 &start,const Vector4 &end,float falloff,float sharpness);
	static tbyte *createGlow(TextureFormat *format);

	LightNode::ptr mLight;
	MeshNode::ptr mSkyDome;
	SpriteNode::ptr mSun;
	
	Matrix4x4Accessor::ptr mShaderAccessor,mBumpAccessor,mCloudAccessor,mColorAccessor,mFadeAccessor;
	Matrix4x4Accessor::ptr mCompositeAccessor;
	Material::ptr mSkyMaterial;
};

#endif
