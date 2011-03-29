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
#include <toadlet/peeper/PointState.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/peeper/Renderer.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Material:public egg::BaseResource{
public:
	TOADLET_SHARED_POINTERS(Material);

	enum States{
		State_LIGHTING=		1<<0,
		State_MATERIAL=		1<<1,
		State_FACECULLING=	1<<2,
		State_FILL=			1<<3,
		State_FOG=			1<<4,
		State_ALPHATEST=	1<<5,
		State_BLEND=		1<<6,
		State_DEPTHSORT=	1<<7,
		State_DEPTHWRITE=	1<<8,
		State_DEPTHTEST=	1<<9,
		State_SHADING=		1<<10,
		State_POINT=		1<<11,
	};
	
	Material();
	virtual ~Material();

	void destroy();
	Material::ptr clone();

	void setLighting(bool lighting){mStates|=State_LIGHTING;mLighting=lighting;}
	inline bool getLighting() const{return mLighting;}

	void setMaterialState(const peeper::MaterialState &state){mStates|=State_MATERIAL;mMaterialState.set(state);}
	inline const peeper::MaterialState &getMaterialState() const{return mMaterialState;}

	void setFaceCulling(const peeper::Renderer::FaceCulling &faceCulling){mStates|=State_FACECULLING;mFaceCulling=faceCulling;}
	inline const peeper::Renderer::FaceCulling &getFaceCulling() const{return mFaceCulling;}

	void setFill(const peeper::Renderer::Fill &fill){mStates|=State_FILL;mFill=fill;}
	inline const peeper::Renderer::Fill &getFill() const{return mFill;}

	void setShading(const peeper::Renderer::Shading &shading){mStates|=State_SHADING;mShading=shading;}
	inline const peeper::Renderer::Shading &getShading() const{return mShading;}

	void setFogState(const peeper::FogState &state){mStates|=State_FOG;mFogState.set(state);}
	inline const peeper::FogState &getFogState() const{return mFogState;}

	void setAlphaTest(peeper::Renderer::AlphaTest alphaTest,scalar cutoff){
		mStates|=State_ALPHATEST;mAlphaTest=alphaTest;mAlphaTestCutoff=cutoff;
	}
	inline const peeper::Renderer::AlphaTest &getAlphaTest() const{return mAlphaTest;}
	inline scalar getAlphaTestCutoff() const{return mAlphaTestCutoff;}

	void setBlendState(const peeper::BlendState &state){mStates|=State_BLEND;mBlendState.set(state);}
	inline const peeper::BlendState &getBlendState() const{return mBlendState;}

	void setDepthSorted(bool sorted){mStates|=State_DEPTHSORT;mDepthSorted=sorted;}
	inline bool getDepthSorted() const{return mDepthSorted;}

	void setDepthWrite(bool depthWrite){mStates|=State_DEPTHWRITE;mDepthWrite=depthWrite;}
	inline bool getDepthWrite() const{return mDepthWrite;}

	void setDepthTest(peeper::Renderer::DepthTest depthTest){mStates|=State_DEPTHTEST;mDepthTest=depthTest;}
	inline peeper::Renderer::DepthTest getDepthTest() const{return mDepthTest;}

	void setPointState(const peeper::PointState &state){mStates|=State_POINT;mPointState.set(state);}
	inline const peeper::PointState &getPointSprite() const{return mPointState;}

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
	bool mLighting;
	peeper::Renderer::FaceCulling mFaceCulling;
	peeper::Renderer::Fill mFill;
	peeper::Renderer::Shading mShading;
	peeper::FogState mFogState;
	peeper::Renderer::AlphaTest mAlphaTest;
	scalar mAlphaTestCutoff;
	peeper::BlendState mBlendState;
	bool mDepthSorted;
	bool mDepthWrite;
	peeper::Renderer::DepthTest mDepthTest;
	peeper::PointState mPointState;
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