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

#ifndef TOADLET_TADPOLE_MATERIAL_H
#define TOADLET_TADPOLE_MATERIAL_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/RenderStateSet.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Material:public egg::BaseResource{
public:
	TOADLET_SHARED_POINTERS(Material);

	Material(peeper::RenderStateSet::ptr renderStateSet);
	virtual ~Material();

	void destroy();

	virtual void setBlendState(const peeper::BlendState &state){mRenderStateSet->setBlendState(state);}
	virtual bool getBlendState(peeper::BlendState &state) const{return mRenderStateSet->getBlendState(state);}

	virtual void setDepthState(const peeper::DepthState &state){mRenderStateSet->setDepthState(state);}
	virtual bool getDepthState(peeper::DepthState &state) const{return mRenderStateSet->getDepthState(state);}

	virtual void setRasterizerState(const peeper::RasterizerState &state){mRenderStateSet->setRasterizerState(state);}
	virtual bool getRasterizerState(peeper::RasterizerState &state) const{return mRenderStateSet->getRasterizerState(state);}

	virtual void setFogState(const peeper::FogState &state){mRenderStateSet->setFogState(state);}
	virtual bool getFogState(peeper::FogState &state) const{return mRenderStateSet->getFogState(state);}

	virtual void setPointState(const peeper::PointState &state){mRenderStateSet->setPointState(state);}
	virtual bool getPointState(peeper::PointState &state) const{return mRenderStateSet->getPointState(state);}

	virtual void setMaterialState(const peeper::MaterialState &state){mRenderStateSet->setMaterialState(state);}
	virtual bool getMaterialState(peeper::MaterialState &state) const{return mRenderStateSet->getMaterialState(state);}

	inline int getNumTextureStages() const{return mTextureStages.size();}
	bool setTextureStage(int stage,const peeper::TextureStage::ptr &textureStage);
	inline const peeper::TextureStage::ptr &getTextureStage(int stage) const{return mTextureStages[stage];}

	void setDepthSorted(bool sorted){mDepthSorted=sorted;}
	inline bool getDepthSorted() const{return mDepthSorted;}

	void setLayer(int layer){mLayer=layer;}
	int getLayer() const{return mLayer;}

	void modifyWith(Material *material);
	bool getManaged(){return getUniqueHandle()!=0;}

	inline peeper::RenderStateSet::ptr getRenderStateSet(){return mRenderStateSet;}
	
protected:
	peeper::RenderStateSet::ptr mRenderStateSet;
	egg::Collection<peeper::TextureStage::ptr> mTextureStages;
	bool mDepthSorted;
	int mLayer;
};

}
}

#endif

/*
#include <toadlet/egg/BaseResource.h>
#include <toadlet/tadpole/RenderPass.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Material:public egg::BaseResource{
public:
	TOADLET_SHARED_POINTERS(Material);

	Material();
	virtual ~Material();

	void destroy();
	Material::ptr clone();

	inline void getNumRenderPasses(){return mRenderPasses.size();}
	inline RenderPass::ptr getRenderPass(int i){return mRenderPasses[i];}
	RenderPass::ptr addRenderPass();
	void removeRenderPass(RenderPass *pass);

protected:
	egg::Collection<RenderPass::ptr> mRenderPasses;
};

}
}
*/