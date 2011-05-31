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

	TextureStateMatrix4x4Accessor(Material::ptr material,int state):
		mMaterial(material),mState(state){}

	void setMatrix4x4(const Matrix4x4 &matrix){
		mMaterial->getTextureState(mState,mTextureState);
		mTextureState.matrix.set(matrix);
		mMaterial->setTextureState(mState,mTextureState);
	}

	const Matrix4x4 &getMatrix4x4(){
		mMaterial->getTextureState(mState,mTextureState);
		return mTextureState.matrix;
	}

protected:
	Material::ptr mMaterial;
	int mState;
	TextureState mTextureState;
};

class Sky:public ParentNode{
public:
	TOADLET_NODE(Sky,ParentNode);

	Sky():super(){}

	Node *create(Scene *scene,const Vector4 &skyColor,const Vector4 &fadeColor);
	void destroy();

	void updateLightDirection(const Vector3 &lightDir);

	void frameUpdate(int dt,int scope);

	inline LightNode ::ptr getLight(){return mLight;}
	inline MeshNode::ptr getSkyDome(){return mSkyDome;}
	inline Material::ptr getSkyMaterial(){return mSkyMaterial;}
	inline SpriteNode::ptr getSun(){return mSun;}

protected:
	static Image::ptr createCloud(int width,int height,int scale,int seed,float cover,float sharpness,float brightness);
	static Image::ptr createBump(Image *image,float xscale,float yscale,float zscale,int spread);
	static Image::ptr createComposite(Image *cloud,Image *bump,const Vector3 &lightDir,const Vector4 &skyColor);
	static Image::ptr createFade(int width,int height,const Vector4 &start,const Vector4 &end,float falloff,float sharpness);
	static Image::ptr createGlow(int width,int height);
	static Image::ptr createNorm(int size);

	LightNode::ptr mLight;
	MeshNode::ptr mSkyDome;
	SpriteNode::ptr mSun;
	
	Matrix4x4Accessor::ptr mBumpAccessor,mCloudAccessor,mColorAccessor,mFadeAccessor;
	Matrix4x4Accessor::ptr mCompositeAccessor;
	Material::ptr mSkyMaterial;
};

#endif
