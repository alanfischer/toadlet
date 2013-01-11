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

#include <toadlet/egg/Log.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include "DecalShadowRenderManager.h"

namespace toadlet{
namespace tadpole{

DecalShadowRenderManager::DecalShadowRenderManager(Scene *scene):SimpleRenderManager(scene),
	mShadowScope(-1)
{
	mRenderableSet->setGatherNodes(true);

	Engine *engine=mScene->getEngine();

	TextureFormat::ptr pointFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_A_8,128,128,1,0);
	tbyte *pointData=createPoint(pointFormat);
	Texture::ptr pointTexture=engine->getTextureManager()->createTexture(pointFormat,pointData);
	delete[] pointData;

	RenderState::ptr renderState=engine->getMaterialManager()->createRenderState();
	renderState->setBlendState(BlendState(BlendState::Operation_ONE_MINUS_SOURCE_ALPHA,BlendState::Operation_SOURCE_ALPHA));
	renderState->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
	renderState->setMaterialState(MaterialState(false));
	mMaterial=engine->getMaterialManager()->createSharedMaterial(engine->createDiffuseMaterial(pointTexture),renderState);

	VertexBuffer::ptr vertexBuffer=engine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,engine->getVertexFormats().POSITION_TEX_COORD,4);
	mVertexData=new VertexData(vertexBuffer);
	{
		VertexBufferAccessor vba(vertexBuffer,Buffer::Access_BIT_WRITE);

		vba.set3(0,0, -Math::ONE,-Math::ONE,0);
		vba.set2(0,1, 0,0);

		vba.set3(1,0, Math::ONE,-Math::ONE,0);
		vba.set2(1,1, Math::ONE,0);

		vba.set3(2,0, -Math::ONE,Math::ONE,0);
		vba.set2(2,1, 0,Math::ONE);

		vba.set3(3,0, Math::ONE,Math::ONE,0);
		vba.set2(3,1, Math::ONE,Math::ONE);

		vba.unlock();
	}
	mIndexData=new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,4);
}

DecalShadowRenderManager::~DecalShadowRenderManager(){
}

void DecalShadowRenderManager::interRenderRenderables(RenderableSet *set,RenderDevice *device,Camera *camera,bool useMaterials){
	LightComponent *light=NULL;
	if(set->getLightQueue().size()>0){
		light=set->getLightQueue()[0].light;
	}

	if(light==NULL){
		return;
	}

	LightState state;
	light->getLightState(state);

	if(camera->getDefaultState()!=NULL){
		device->setRenderState(camera->getDefaultState());
		mParams->setRenderState(camera->getDefaultState());
	}

	RenderPath *path=mMaterial->getBestPath();
	int numPasses=path->getNumPasses();
	int pi,ni;
	for(pi=0;pi<numPasses;++pi){
		RenderPass *pass=path->getPass(pi);
		setupPass(pass,mDevice);

		for(ni=0;ni<set->getNodeQueue().size();++ni){
			Node *node=set->getNodeQueue().at(ni);

			if((node->getScope()&mShadowScope)!=0){
				Matrix4x4 matrix;
				Matrix3x3 rotate;

				float mDistance=10;
				float mOffset=0.01;

				Segment segment;
				segment.origin.set(node->getWorldTranslate());
				Math::mul(segment.direction,Math::NEG_Z_UNIT_VECTOR3,mDistance);
				PhysicsCollision result;
				mScene->traceSegment(result,segment,-1,node);

				if(result.time<Math::ONE){
					Math::madd(result.point,result.normal,mOffset,result.point);
	
					Vector3 right,forward,up;
					Math::mul(forward,node->getWorldRotate(),Math::Y_UNIT_VECTOR3);
					up.set(result.normal);
					Math::cross(right,forward,up);
					Math::normalize(right);
					Math::cross(forward,up,right);
					Math::setMatrix3x3FromAxes(rotate,right,forward,up);
				}

				scalar scale=Math::ONE-result.time;
				Math::setMatrix4x4FromRotateScale(matrix,rotate,Vector3(scale,scale,scale));
				Math::setMatrix4x4FromTranslate(matrix,result.point);

				mDevice->setMatrix(RenderDevice::MatrixType_MODEL,matrix);
				mParams->setMatrix(RenderDevice::MatrixType_MODEL,matrix);

				setupVariableBuffers(pass,Material::Scope_RENDERABLE,mDevice);

				mDevice->renderPrimitive(mVertexData,mIndexData);
			}
		}

		if(camera->getDefaultState()!=NULL){
			mDevice->setRenderState(camera->getDefaultState());
			mParams->setRenderState(camera->getDefaultState());
			mLastRenderState=NULL;
		}
	}
}

tbyte *DecalShadowRenderManager::createPoint(TextureFormat *format){
	int width=format->getWidth(),height=format->getHeight();
	tbyte *data=new tbyte[format->getDataSize()];

	int x=0,y=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			float v=1.0;
			v=v*(Math::length(Vector2(x-width/2,y-height/2))/(width/2));
			if(v<0) v=0;
			if(v>1) v=1;
			v=pow(v,1.25f);

			data[y*width+x]=255*v;
		}
	}

	return data;
}

}
}
