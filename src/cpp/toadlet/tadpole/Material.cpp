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

#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

Material::Material():BaseResource(),
	mLighting(false),
	mFaceCulling(Renderer::FaceCulling_BACK),
	mAlphaTest(Renderer::AlphaTest_NONE),
	mAlphaTestCutoff(0),
	mDepthWrite(true),
	mDepthTest(Renderer::DepthTest_LEQUAL),
	mLayer(0),
	mSaveLocally(false)
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

Material::ptr Material::clone() const{
	Material::ptr material(new Material());

	material->mLightEffect.set(mLightEffect);
	material->mLighting=mLighting;
	material->mFaceCulling=mFaceCulling;
	material->mAlphaTest=mAlphaTest;
	material->mAlphaTestCutoff=mAlphaTestCutoff;
	material->mBlend.set(mBlend);
	material->mDepthWrite=mDepthWrite;
	material->mDepthTest=mDepthTest;

	int i;
	for(i=0;i<mTextureStages.size();++i){
		material->setTextureStage(i,mTextureStages[i]->clone());
	}

	return material;
}

void Material::setAlphaTest(Renderer::AlphaTest alphaTest,scalar cutoff){
	mAlphaTest=alphaTest;
	mAlphaTestCutoff=cutoff;
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
	if(previousMaterial==NULL){
		renderer->setAlphaTest(mAlphaTest,mAlphaTestCutoff);
		renderer->setBlend(mBlend);
		renderer->setDepthTest(mDepthTest);
		renderer->setDepthWrite(mDepthWrite);
		renderer->setFaceCulling(mFaceCulling);
		renderer->setLighting(mLighting);
		if(mLighting){
			renderer->setLightEffect(mLightEffect);
		}

		int numTextureStages=mTextureStages.size();
		int i;
		for(i=0;i<numTextureStages;++i){
			renderer->setTextureStage(i,mTextureStages[i]);
		}
	}
	else{
		if(previousMaterial->mFaceCulling!=mFaceCulling){
			renderer->setFaceCulling(mFaceCulling);
		}
		if(previousMaterial->mLighting!=mLighting){
			renderer->setLighting(mLighting);
		}
		if(mLighting){
			renderer->setLightEffect(mLightEffect);
		}
		if(previousMaterial->mAlphaTest!=mAlphaTest || previousMaterial->mAlphaTestCutoff!=mAlphaTestCutoff){
			renderer->setAlphaTest(mAlphaTest,mAlphaTestCutoff);
		}
		if(previousMaterial->mBlend!=mBlend){
			renderer->setBlend(mBlend);
		}
		if(previousMaterial->mDepthWrite!=mDepthWrite){
			renderer->setDepthWrite(mDepthWrite);
		}
		if(previousMaterial->mDepthTest!=mDepthTest){
			renderer->setDepthTest(mDepthTest);
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

}
}
