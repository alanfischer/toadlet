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
#include <toadlet/peeper/FogState.h>
#include <toadlet/peeper/MaterialState.h>
#include <toadlet/peeper/BlendState.h>
#include <toadlet/peeper/DepthState.h>
#include <toadlet/peeper/PointState.h>
#include <toadlet/peeper/RasterizerState.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/peeper/Renderer.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Material:public egg::BaseResource{
public:
	TOADLET_SHARED_POINTERS(Material);

	enum States{
		State_MATERIAL=		1<<0,
		State_RASTERIZER=	1<<1,
		State_FOG=			1<<2,
		State_ALPHATEST=	1<<3,
		State_BLEND=		1<<4,
		State_DEPTHSORT=	1<<5,
		State_DEPTH=		1<<6,
		State_POINT=		1<<7,
	};
	
	Material();
	virtual ~Material();

	void destroy();
	Material::ptr clone();

	void setMaterialState(const peeper::MaterialState &state){mStates|=State_MATERIAL;mMaterialState.set(state);}
	inline const peeper::MaterialState &getMaterialState() const{return mMaterialState;}

	void setFogState(const peeper::FogState &state){mStates|=State_FOG;mFogState.set(state);}
	inline const peeper::FogState &getFogState() const{return mFogState;}

	void setAlphaTest(peeper::AlphaTest alphaTest,scalar cutoff){
		mStates|=State_ALPHATEST;mAlphaTest=alphaTest;mAlphaTestCutoff=cutoff;
	}
	inline const peeper::AlphaTest &getAlphaTest() const{return mAlphaTest;}
	inline scalar getAlphaTestCutoff() const{return mAlphaTestCutoff;}

	void setBlendState(const peeper::BlendState &state){mStates|=State_BLEND;mBlendState.set(state);}
	inline const peeper::BlendState &getBlendState() const{return mBlendState;}

	void setDepthSorted(bool sorted){mStates|=State_DEPTHSORT;mDepthSorted=sorted;}
	inline bool getDepthSorted() const{return mDepthSorted;}

	void setDepthState(const peeper::DepthState &state){mStates|=State_DEPTH;mDepthState.set(state);}
	inline const peeper::DepthState &getDepthState() const{return mDepthState;}

	void setPointState(const peeper::PointState &state){mStates|=State_POINT;mPointState.set(state);}
	inline const peeper::PointState &getPointSprite() const{return mPointState;}

	void setRasterizerState(const peeper::RasterizerState &state){mStates|=State_RASTERIZER;mRasterizerState.set(state);}
	inline const peeper::RasterizerState &getRasterizerState() const{return mRasterizerState;}

	inline int getNumTextureStages() const{return mTextureStages.size();}
	bool setTextureStage(int stage,const peeper::TextureStage::ptr &textureStage);
	inline const peeper::TextureStage::ptr &getTextureStage(int stage) const{return mTextureStages[stage];}

	void setLayer(int layer){mLayer=layer;}
	int getLayer() const{return mLayer;}

	int getStates(){return mStates;}
	void modifyWith(Material::ptr material);
	bool getManaged(){return getUniqueHandle()!=0;}

	void setupRenderer(peeper::Renderer *renderer,Material *previousMaterial=NULL);
	
protected:
	int mStates;
	peeper::MaterialState mMaterialState;
	peeper::FogState mFogState;
	peeper::AlphaTest mAlphaTest;
	scalar mAlphaTestCutoff;
	peeper::BlendState mBlendState;
	bool mDepthSorted;
	peeper::DepthState mDepthState;
	peeper::PointState mPointState;
	peeper::RasterizerState mRasterizerState;
	egg::Collection<peeper::TextureStage::ptr> mTextureStages;
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