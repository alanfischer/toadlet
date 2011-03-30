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

#include <toadlet/peeper/CapabilityState.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

Material::Material():BaseResource(),
	mStates(0),
	mLighting(false),
	mFaceCulling(FaceCulling_BACK),
	mFill(Fill_SOLID),
	mShading(Shading_SMOOTH),
	//mFogState,
	mAlphaTest(AlphaTest_NONE),
	mAlphaTestCutoff(0),
	//mBlendState,
	mDepthSorted(false),
	//mDepthState,
	//mPointState
	mLayer(0)
{
}

Material::~Material(){
	destroy();
}

void Material::destroy(){
	int i;
	for(i=0;i<mTextureStages.size();++i){
		mTextureStages[i]->destroy();
	}
	mTextureStages.clear();
}

Material::ptr Material::clone(){
	Material::ptr material(new Material());

	// Here we retain & release ourselves to handle the case
	//  Of an unused material being cloned, so the unused original will get cleaned up
	retain();

	material->mStates=mStates;
	material->mMaterialState.set(mMaterialState);
	material->mLighting=mLighting;
	material->mFaceCulling=mFaceCulling;
	material->mFogState.set(mFogState);
	material->mFill=mFill;
	material->mShading=mShading;
	material->mAlphaTest=mAlphaTest;
	material->mAlphaTestCutoff=mAlphaTestCutoff;
	material->mBlendState.set(mBlendState);
	material->mDepthSorted=mDepthSorted;
	material->mDepthState.set(mDepthState);
	material->mPointState.set(mPointState);
	material->mLayer=mLayer;

	int i;
	for(i=0;i<mTextureStages.size();++i){
		material->setTextureStage(i,mTextureStages[i]->clone());
	}

	release();

	return material;
}

bool Material::setTextureStage(int stage,const TextureStage::ptr &textureStage){
	if(mTextureStages.size()<=stage){
		mTextureStages.resize(stage+1);
	}

	if(mTextureStages[stage]!=NULL){
		mTextureStages[stage]->destroy();
	}

	mTextureStages[stage]=textureStage;

	return true;
}

void Material::setupRenderer(Renderer *renderer,Material *previousMaterial){
	int states=mStates;
	if(previousMaterial==NULL){
		if((states&State_ALPHATEST)>0){
			renderer->setAlphaTest(mAlphaTest,mAlphaTestCutoff);
		}
		if((states&State_BLEND)>0){
			renderer->setBlendState(mBlendState);
		}
		if((states&State_DEPTH)>0){
			renderer->setDepthState(mDepthState);
		}
		if((states&State_FACECULLING)>0){
			renderer->setFaceCulling(mFaceCulling);
		}
		if((states&State_FILL)>0){
			renderer->setFill(mFill);
		}
		if((states&State_SHADING)>0){
			renderer->setShading(mShading);
		}
		if((states&State_FOG)>0){
			renderer->setFogState(mFogState);
		}
		if((states&State_LIGHTING)>0){
			renderer->setLighting(mLighting);
		}
		if((states&State_MATERIAL)>0){
			renderer->setMaterialState(mMaterialState); // We set this even if lighting isnt enabled, since it includes color tracking
		}
		if((states&State_POINT)>0){
			renderer->setPointState(mPointState);
		}

		int numTextureStages=mTextureStages.size();
		int i;
		for(i=0;i<numTextureStages;++i){
			renderer->setTextureStage(i,mTextureStages[i]);
		}
		int maxTextureStages=renderer->getCapabilityState().maxTextureStages;
		for(;i<maxTextureStages;++i){
			renderer->setTextureStage(i,NULL);
		}
	}
	else{
		int pstates=previousMaterial->mStates;
		if((states&State_ALPHATEST)>0 && ((pstates&State_ALPHATEST)==0 || previousMaterial->mAlphaTest!=mAlphaTest || previousMaterial->mAlphaTestCutoff!=mAlphaTestCutoff)){
			renderer->setAlphaTest(mAlphaTest,mAlphaTestCutoff);
		}
		if((states&State_BLEND)>0 && ((pstates&State_BLEND)==0 || previousMaterial->mBlendState!=mBlendState)){
			renderer->setBlendState(mBlendState);
		}
		if((states&State_DEPTH)>0 && ((pstates&State_DEPTH)==0 || previousMaterial->mDepthState!=mDepthState)){
			renderer->setDepthState(mDepthState);
		}
		if((states&State_FACECULLING)>0 && ((pstates&State_FACECULLING)==0 || previousMaterial->mFaceCulling!=mFaceCulling)){
			renderer->setFaceCulling(mFaceCulling);
		}
		if((states&State_FILL)>0 && ((pstates&State_FILL)==0 || previousMaterial->mFill!=mFill)){
			renderer->setFill(mFill);
		}
		if((states&State_SHADING)>0 && ((pstates&State_SHADING)==0 || previousMaterial->mShading!=mShading)){
			renderer->setShading(mShading);
		}
		if((states&State_FOG)>0){
			renderer->setFogState(mFogState);
		}
		if((states&State_LIGHTING)>0 && ((pstates&State_LIGHTING)==0 || previousMaterial->mLighting!=mLighting)){
			renderer->setLighting(mLighting);
		}
		if((states&State_MATERIAL)>0){
			renderer->setMaterialState(mMaterialState);
		}
		if((states&State_POINT)>0){
			renderer->setPointState(mPointState);
		}

		int numTextureStages=mTextureStages.size();
		int numPreviousTextureStages=previousMaterial->mTextureStages.size();
		int i;
		for(i=0;i<numTextureStages;++i){
			if(i>=numPreviousTextureStages || previousMaterial->mTextureStages[i]!=mTextureStages[i]){
				renderer->setTextureStage(i,mTextureStages[i]);
			}
		}
		for(;i<numPreviousTextureStages;++i){
			renderer->setTextureStage(i,NULL);
		}
	}
}

void Material::modifyWith(Material::ptr material){
	int states=material->mStates;
	if((states&State_ALPHATEST)>0){
		setAlphaTest(material->mAlphaTest,material->mAlphaTestCutoff);
	}
	if((states&State_BLEND)>0){
		setBlendState(material->mBlendState);
	}
	if((states&State_DEPTH)>0){
		setDepthState(material->mDepthState);
	}
	if((states&State_FACECULLING)>0){
		setFaceCulling(material->mFaceCulling);
	}
	if((states&State_FILL)>0){
		setFill(material->mFill);
	}
	if((states&State_SHADING)>0){
		setShading(material->mShading);
	}
	if((states&State_FOG)>0){
		setFogState(material->mFogState);
	}
	if((states&State_LIGHTING)>0){
		setLighting(material->mLighting);
	}
	if((states&State_MATERIAL)>0){
		setMaterialState(material->mMaterialState);
	}
	if((states&State_POINT)>0){
		setPointState(material->mPointState);
	}
}

}
}
