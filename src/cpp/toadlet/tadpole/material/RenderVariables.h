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
#include <toadlet/tadpole/node/CameraNode.h>
#include <string.h>

namespace toadlet{
namespace tadpole{
namespace material{

class MVPMatrixVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X4;}

	void update(tbyte *data,SceneParameters *parameters){
		if(parameters->getRenderable()!=NULL){
			parameters->getRenderable()->getRenderTransform().getMatrix(mModelMatrix);
			Math::mul(mMVPMatrix,parameters->getCamera()->getViewProjectionMatrix(),mModelMatrix);
			memcpy(data,mMVPMatrix.getData(),sizeof(Matrix4x4));
		}
	}

protected:
	Matrix4x4 mMVPMatrix,mModelMatrix;
};

class MVMatrixVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X4;}

	void update(tbyte *data,SceneParameters *parameters){
		if(parameters->getRenderable()!=NULL){
			parameters->getRenderable()->getRenderTransform().getMatrix(mModelMatrix);
			Math::mul(mMVMatrix,parameters->getCamera()->getViewMatrix(),mModelMatrix);
			memcpy(data,mMVMatrix.getData(),sizeof(Matrix4x4));
		}
	}

protected:
	Matrix4x4 mMVMatrix,mModelMatrix;
};

class NormalMatrixVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X4;}

	void update(tbyte *data,SceneParameters *parameters){
		if(parameters->getRenderable()!=NULL){
			parameters->getRenderable()->getRenderTransform().getMatrix(mModelMatrix);
			Math::mul(mNormalMatrix,parameters->getCamera()->getViewMatrix(),mModelMatrix);
			Math::zeroTranslateSheer(mNormalMatrix);
			Math::invert(mInverseMatrix,mNormalMatrix);
			Math::transpose(mNormalMatrix,mInverseMatrix);
			memcpy(data,mNormalMatrix.getData(),sizeof(Matrix4x4));
		}
	}

protected:
	Matrix4x4 mModelMatrix,mModelViewMatrix,mInverseMatrix,mNormalMatrix;
};

class LightViewPositionVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}

	void update(tbyte *data,SceneParameters *parameters){
		// This Vector4 isn't aligned, so we don't multiply directly into it.
		Vector4 &finalLightPosition=*(Vector4*)data;
		if(parameters->getRenderable()!=NULL){
			Vector4 lightPosition(parameters->getLightState().direction,0);
			Math::mul(lightPosition,parameters->getCamera()->getViewMatrix());
			finalLightPosition=lightPosition;
		}
	}
};

class LightDiffuseVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}

	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getLightState().diffuseColor.getData(),sizeof(Vector4));
	}
};

class AmbientVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}

	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getAmbient().getData(),sizeof(Vector4));
	}
};

class MaterialLightingVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}

	void update(tbyte *data,SceneParameters *parameters){
		float &lighting=*(float*)data;
		lighting=parameters->getMaterialState().lighting?1.0:0.0;
	}
};

class MaterialDiffuseVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}
	
	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getMaterialState().diffuse.getData(),sizeof(Vector4));
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
		size=parameters->getPointState().size;
	}
};

class PointAttenuatedVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}

	void update(tbyte *data,SceneParameters *parameters){
		float &attenuated=*(float*)data;
		attenuated=parameters->getPointState().attenuated?Math::ONE:0;
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
	TextureMatrixVariable(int index):mIndex(index){}

	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X4|((mIndex+1)<<VariableBufferFormat::Format_SHIFT_SAMPLER_MATRIX);}
	
	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getTextureState(mIndex).matrix.getData(),sizeof(Matrix4x4));
	}

protected:
	int mIndex;
};

class TextureSetVariable:public RenderVariable{
public:
	TextureSetVariable(int index):mIndex(index){}

	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}

	void update(tbyte *data,SceneParameters *parameters){
		float &textureSet=*(float*)data;
		textureSet=parameters->getRenderPass()->getTexture(mIndex)!=NULL?1.0:0.0;
	}

protected:
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

class ConstantVariable:public RenderVariable{
public:
	ConstantVariable(const Vector4 &constant){mConstant=constant;}

	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}

	void update(tbyte *data,SceneParameters *parameters){
		Vector4 &constant=*(Vector4*)data;
		constant=mConstant;
	}

protected:
	Vector4 mConstant;
};

}
}
}

#endif
