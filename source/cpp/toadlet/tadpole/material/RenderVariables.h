/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_TADPOLE_MATERIAL_RENDERVARIABLES_H
#define TOADLET_TADPOLE_MATERIAL_RENDERVARIABLES_H

#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/material/RenderVariable.h>
#include <toadlet/tadpole/material/SceneParameters.h>

namespace toadlet{
namespace tadpole{
namespace material{

class DataVariable:public RenderVariable{
public:
	TOADLET_ALIGNED_NEW;

	DataVariable(int format,int size){
		mFormat=format;
		mSize=size;
		mData=new tbyte[size];
	}

	virtual ~DataVariable(){
		delete[] mData;
	}

	void setData(tbyte *data){
		memcpy(mData,data,mSize);
	}

	int getFormat(){return mFormat;}

	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,mData,mSize);
	}

	int mFormat;
	int mSize;
	tbyte *mData;
};

class MVPMatrixVariable:public RenderVariable{
public:
	TOADLET_ALIGNED_NEW;

	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X4|VariableBufferFormat::Format_BIT_PROJECTION;}

	void update(tbyte *data,SceneParameters *parameters){
		Math::mul(mViewProjectionMatrix,parameters->getMatrix(RenderDevice::MatrixType_PROJECTION),parameters->getMatrix(RenderDevice::MatrixType_VIEW));
		Math::mul(mMVPMatrix,mViewProjectionMatrix,parameters->getMatrix(RenderDevice::MatrixType_MODEL));
		memcpy(data,mMVPMatrix.getData(),sizeof(Matrix4x4));
	}

protected:
	Matrix4x4 mMVPMatrix,mViewProjectionMatrix;
};

class MVMatrixVariable:public RenderVariable{
public:
	TOADLET_ALIGNED_NEW;

	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X4;}

	void update(tbyte *data,SceneParameters *parameters){
		Math::mul(mMVMatrix,parameters->getMatrix(RenderDevice::MatrixType_VIEW),parameters->getMatrix(RenderDevice::MatrixType_MODEL));
		memcpy(data,mMVMatrix.getData(),sizeof(Matrix4x4));
	}

protected:
	Matrix4x4 mMVMatrix,mModelMatrix;
};

class ModelMatrixVariable:public RenderVariable{
public:
	TOADLET_ALIGNED_NEW;

	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X4;}

	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getMatrix(RenderDevice::MatrixType_MODEL).getData(),sizeof(Matrix4x4));
	}

protected:
	Matrix4x4 mModelMatrix;
};

class ViewMatrixVariable:public RenderVariable{
public:
	TOADLET_ALIGNED_NEW;

	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X4;}

	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getMatrix(RenderDevice::MatrixType_VIEW).getData(),sizeof(Matrix4x4));
	}
};

class ProjectionMatrixVariable:public RenderVariable{
public:
	TOADLET_ALIGNED_NEW;

	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X4|VariableBufferFormat::Format_BIT_PROJECTION;}

	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getMatrix(RenderDevice::MatrixType_PROJECTION).getData(),sizeof(Matrix4x4));
	}
};

class NormalMatrixVariable:public RenderVariable{
public:
	TOADLET_ALIGNED_NEW;

	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X4;}

	void update(tbyte *data,SceneParameters *parameters){
		Math::mul(mNormalMatrix,parameters->getMatrix(RenderDevice::MatrixType_VIEW),parameters->getMatrix(RenderDevice::MatrixType_MODEL));
		Math::zeroTranslateSheer(mNormalMatrix);
		Math::invert(mInverseMatrix,mNormalMatrix);
		Math::transpose(mNormalMatrix,mInverseMatrix);
		memcpy(data,mNormalMatrix.getData(),sizeof(Matrix4x4));
	}

protected:
	Matrix4x4 mModelMatrix,mModelViewMatrix,mInverseMatrix,mNormalMatrix;
};

class CameraPositionVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}

	void update(tbyte *data,SceneParameters *parameters){
		Vector4 position(parameters->getCamera()->getPosition(),1);
		memcpy(data,&position,sizeof(Vector4));
	}
};

class LightPositionVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}

	void update(tbyte *data,SceneParameters *parameters){
		Vector4 direction(parameters->getLightState().direction,0);
		memcpy(data,&direction,sizeof(Vector4));
	}
};

class LightViewPositionVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}

	void update(tbyte *data,SceneParameters *parameters){
		// This Vector4 isn't aligned, so we don't multiply directly into it.
		Vector4 &finalLightPosition=*(Vector4*)data;
		Vector4 lightPosition(parameters->getLightState().direction,0);
		Math::mul(lightPosition,parameters->getMatrix(RenderDevice::MatrixType_VIEW));
		finalLightPosition=lightPosition;
	}
};

class LightDiffuseVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}

	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getLightState().diffuseColor.getData(),sizeof(Vector4));
	}
};

class LightSpecularVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}

	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getLightState().specularColor.getData(),sizeof(Vector4));
	}
};

class AmbientVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}

	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getAmbientColor().getData(),sizeof(Vector4));
	}
};

class MaterialLightVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}

	void update(tbyte *data,SceneParameters *parameters){
		float &light=*(float*)data;
		light=parameters->getMaterialState().light?1.0:0.0;
	}
};

class MaterialDiffuseVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}
	
	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getMaterialState().diffuse.getData(),sizeof(Vector4));
	}
};

class MaterialSpecularVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}
	
	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getMaterialState().specular.getData(),sizeof(Vector4));
	}
};

class MaterialShininessVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}
	
	void update(tbyte *data,SceneParameters *parameters){
		float &s=*(float*)data;
		s=parameters->getMaterialState().shininess;
	}
};

class MaterialAmbientVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}
	
	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getMaterialState().ambient.getData(),sizeof(Vector4));
	}
};

class MaterialTrackColorVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}
	
	void update(tbyte *data,SceneParameters *parameters){
		float &trackColor=*(float*)data;
		trackColor=parameters->getMaterialState().trackColor?1.0:0.0;
	}
};

class PointSizeVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}

	void update(tbyte *data,SceneParameters *parameters){
		float &size=*(float*)data;
		size=parameters->getGeometryState().size;
	}
};

class PointAttenuatedVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}

	void update(tbyte *data,SceneParameters *parameters){
		float &attenuated=*(float*)data;
		attenuated=parameters->getGeometryState().attenuated?Math::ONE:0;
	}
};

class FogTypeVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}

	void update(tbyte *data,SceneParameters *parameters){
		float &fog=*(float*)data;
		fog=parameters->getFogState().fog;
	}
};

class FogDensityVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}

	void update(tbyte *data,SceneParameters *parameters){
		float &density=*(float*)data;
		density=parameters->getFogState().density;
	}
};

class FogDistanceVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_2;}

	void update(tbyte *data,SceneParameters *parameters){
		Vector2 &distances=*(Vector2*)data;
		distances.x=parameters->getFogState().nearDistance;
		distances.y=parameters->getFogState().farDistance;
	}
};

class FogColorVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}

	void update(tbyte *data,SceneParameters *parameters){
		Vector4 &color=*(Vector4*)data;
		color=parameters->getFogState().color;
	}
};

class TextureMatrixVariable:public RenderVariable{
public:
	TextureMatrixVariable(const String &name){
		mName=name;
		mIndex=-1;
	}

	TextureMatrixVariable(Shader::ShaderType type,int index){
		mType=type;
		mIndex=index;
	}

	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X4|((mIndex+1)<<VariableBufferFormat::Format_SHIFT_SAMPLER_MATRIX);}

	void linked(RenderPass *pass){
		pass->findTexture(mType,mIndex,mName);
	}

	void update(tbyte *data,SceneParameters *parameters){
		if(mIndex>=0){
			TextureState state;
			parameters->getRenderPass()->getRenderState()->getTextureState(mType,mIndex,state);
			memcpy(data,state.matrix.getData(),sizeof(Matrix4x4));
		}
	}

protected:
	String mName;
	Shader::ShaderType mType;
	int mIndex;
};

class TextureSetVariable:public RenderVariable{
public:
	TextureSetVariable(const String &name){
		mName=name;
		mIndex=-1;
	}

	TextureSetVariable(Shader::ShaderType type,int index){
		mType=type;
		mIndex=index;
	}

	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}

	void linked(RenderPass *pass){
		pass->findTexture(mType,mIndex,mName);
	}

	void update(tbyte *data,SceneParameters *parameters){
		if(mIndex>=0){
			float &textureSet=*(float*)data;
			textureSet=parameters->getRenderPass()->getTexture(mType,mIndex)!=NULL?1.0:0.0;
		}
	}

protected:
	String mName;
	Shader::ShaderType mType;
	int mIndex;
};

class ViewportVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}

	void update(tbyte *data,SceneParameters *parameters){
		Vector4 &viewport=*(Vector4*)data;
		viewport=Vector4(parameters->getViewport().x,parameters->getViewport().y,parameters->getViewport().width,parameters->getViewport().height);
	}
};

class TimeVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}

	void update(tbyte *data,SceneParameters *parameters){
		float &time=*(float*)data;
		time=Math::fromMilli(parameters->getScene()->getTime());
	}
};

class ScalarVariable:public RenderVariable{
public:
	TOADLET_ALIGNED_NEW;

	ScalarVariable(scalar s){mScalar=s;}

	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}

	void update(tbyte *data,SceneParameters *parameters){
		float &s=*(float*)data;
		s=mScalar;
	}

protected:
	scalar mScalar;
};

class Vector4Variable:public RenderVariable{
public:
	TOADLET_ALIGNED_NEW;

	Vector4Variable(const Vector4 &vector){mVector=vector;}

	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}

	void update(tbyte *data,SceneParameters *parameters){
		Vector4 &vector=*(Vector4*)data;
		vector=mVector;
	}

protected:
	Vector4 mVector;
};

}
}
}

#endif
