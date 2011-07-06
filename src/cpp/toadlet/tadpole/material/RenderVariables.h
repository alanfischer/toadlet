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

namespace toadlet{
namespace tadpole{
namespace material{

class MVPMatrixVariable:public RenderVariable{
public:
	TOADLET_SHARED_POINTERS(MVPMatrixVariable);

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

class LightModelPositionVariable:public RenderVariable{
public:
	TOADLET_SHARED_POINTERS(LightModelPositionVariable);

	void update(tbyte *data,SceneParameters *parameters){
		if(parameters->getRenderable()!=NULL){
			/// @todo: Modify the LightState so it only has 1 4d vector position/direction
			Vector4 position(parameters->getLightState().direction,0.0);
			parameters->getRenderable()->getRenderTransform().getMatrix(mModelMatrix);
			Math::invert(mInverseModelMatrix,mModelMatrix);
			Math::mul(position,mInverseModelMatrix);
			memcpy(data,position.getData(),sizeof(Vector4));
		}
	}

protected:
	Matrix4x4 mModelMatrix,mInverseModelMatrix;
};

class LightDiffuseVariable:public RenderVariable{
public:
	TOADLET_SHARED_POINTERS(LightDiffuseVariable);
	
	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getLightState().diffuseColor.getData(),sizeof(Vector4));
	}
};

class AmbientVariable:public RenderVariable{
public:
	TOADLET_SHARED_POINTERS(AmbientVariable);
	
	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getAmbient().getData(),sizeof(Vector4));
	}
};

class MaterialDiffuseVariable:public RenderVariable{
public:
	TOADLET_SHARED_POINTERS(MaterialDiffuseVariable);
	
	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getMaterialState().diffuse.getData(),sizeof(Vector4));
	}
};

class MaterialAmbientVariable:public RenderVariable{
public:
	TOADLET_SHARED_POINTERS(MaterialAmbientVariable);
	
	void update(tbyte *data,SceneParameters *parameters){
		memcpy(data,parameters->getMaterialState().ambient.getData(),sizeof(Vector4));
	}
};

class TimeVariable:public RenderVariable{
public:
	TOADLET_SHARED_POINTERS(TimeVariable);

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
