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

#include <toadlet/egg/Logger.h>
#include <toadlet/peeper/BackableRenderStateSet.h>
#include <toadlet/peeper/CapabilityState.h>
#include <toadlet/tadpole/Material.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

Material::Material(RenderStateSet::ptr renderStateSet):BaseResource(),
	//mFogState,
	//mBlendState,
	mDepthSorted(false),
	//mDepthState,
	//mPointState,
	//mRasterizerState
	mLayer(0)
{
	if(renderStateSet==NULL){
		renderStateSet=RenderStateSet::ptr(new BackableRenderStateSet());
	}

	mRenderStateSet=renderStateSet;
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

void Material::modifyWith(Material *material){
	RenderStateSet::ptr src=material->mRenderStateSet;
	RenderStateSet::ptr dst=mRenderStateSet;

	BlendState blendState;
	if(src->getBlendState(blendState)) dst->setBlendState(blendState);

	DepthState depthState;
	if(src->getDepthState(depthState)) dst->setDepthState(depthState);

	RasterizerState rasterizerState;
	if(src->getRasterizerState(rasterizerState)) dst->setRasterizerState(rasterizerState);

	FogState fogState;
	if(src->getFogState(fogState)) dst->setFogState(fogState);

	PointState pointState;
	if(src->getPointState(pointState)) dst->setPointState(pointState);

	MaterialState materialState;
	if(src->getMaterialState(materialState)) dst->setMaterialState(materialState);

	int i;
	for(i=0;i<material->getNumTextureStages();++i){
		setTextureStage(i,material->getTextureStage(i)->clone());
	}
}

bool Material::equals(Material *material){
	RenderStateSet::ptr src=material->mRenderStateSet;
	RenderStateSet::ptr dst=mRenderStateSet;

	BlendState srcBlendState,dstBlendState;
	src->getBlendState(srcBlendState);dst->getBlendState(dstBlendState);
	if(srcBlendState!=dstBlendState) return false;

	DepthState srcDepthState,dstDepthState;
	src->getDepthState(srcDepthState);dst->getDepthState(dstDepthState);
	if(srcDepthState!=dstDepthState) return false;

	RasterizerState srcRasterizerState,dstRasterizerState;
	src->getRasterizerState(srcRasterizerState);dst->getRasterizerState(dstRasterizerState);
	if(srcRasterizerState!=dstRasterizerState) return false;

	FogState srcFogState,dstFogState;
	src->getFogState(srcFogState);dst->getFogState(dstFogState);
	if(srcFogState!=dstFogState) return false;

	PointState srcPointState,dstPointState;
	src->getPointState(srcPointState);dst->getPointState(dstPointState);
	if(srcPointState!=dstPointState) return false;

	MaterialState srcMaterialState,dstMaterialState;
	src->getMaterialState(srcMaterialState);dst->getMaterialState(dstMaterialState);
	if(srcMaterialState!=dstMaterialState) return false;

	if(material->getNumTextureStages()!=getNumTextureStages()) return false;

	int i;
	for(i=0;i<getNumTextureStages();++i){
		if(getTextureStage(i)->getTextureName()!=material->getTextureStage(i)->getTextureName()) return false;
	}

	return true;
}

/// @todo: Optimize this so we're not resetting a ton of texture states
void Material::setupRenderer(Renderer *renderer){
	renderer->setRenderStateSet(mRenderStateSet);

	int i;
	for(i=0;i<getNumTextureStages();++i){
		renderer->setTextureStage(i,getTextureStage(i));
	}
	for(;i<renderer->getCapabilityState().maxTextureStages;++i){
		renderer->setTextureStage(i,NULL);
	}
}

}
}
