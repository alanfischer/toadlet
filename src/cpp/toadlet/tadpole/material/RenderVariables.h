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
		if(parameters->getRenderable()!=NULL){
			Vector4 lightViewPosition(parameters->getLightState().direction,0);
			Math::mul(lightViewPosition,parameters->getCamera()->getViewMatrix());
			memcpy(data,lightViewPosition.getData(),sizeof(Vector4));
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
		float lighting=parameters->getMaterialState().lighting?1.0:0.0;
		memcpy(data,&lighting,sizeof(float));
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

class PointSizeVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}

	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,&parameters->getPointState().size,sizeof(float));
	}
};

class PointAttenuatedVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}

	void update(tbyte *data,SceneParameters *parameters){
		float attenuated=parameters->getPointState().attenuated?Math::ONE:0;
		memcpy(data,&attenuated,sizeof(float));
	}
};

class TextureMatrixVariable:public RenderVariable{
public:
	TextureMatrixVariable(int index):mIndex(index){}

	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X4;}
	
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
		float textureSet=parameters->getRenderPass()->getTexture(mIndex)!=NULL?1.0:0.0;
		memcpy(data,&textureSet,sizeof(float));
	}

protected:
	int mIndex;
};

class ViewportVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;}

	void update(tbyte *data,SceneParameters *parameters){
		Vector4 viewport(parameters->getViewport().x,parameters->getViewport().y,parameters->getViewport().width,parameters->getViewport().height);
		memcpy(data,&viewport,sizeof(Vector4));
	}
};

class TimeVariable:public RenderVariable{
public:
	int getFormat(){return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;}

	void update(tbyte *data,SceneParameters *parameters){
		mTime=Math::fromMilli(parameters->getScene()->getTime());
		memcpy(data,&mTime,sizeof(scalar));
	}

protected:
	scalar mTime;
};

}
}
}

#endif
