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
#include <toadlet/peeper/LightEffect.h>
#include <toadlet/peeper/Blend.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/peeper/Renderer.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Material:public egg::BaseResource{
public:
	TOADLET_SHARED_POINTERS(Material);

	// Min/Max Layers
	const static int MIN_LAYER=-63;
	const static int MAX_LAYER=63;

	enum States{
		State_LIGHTING=	1<<0,
		State_LIGHTEFFECT=1<<1,
		State_FACECULLING=1<<2,
		State_FILL=		1<<3,
		State_ALPHATEST=1<<4,
		State_BLEND=	1<<5,
		State_DEPTHSORT=1<<6,
		State_DEPTHWRITE=1<<7,
		State_DEPTHTEST=1<<8,
	};
	
	Material();
	virtual ~Material();

	void destroy();
	Material::ptr clone();

	void setLighting(bool lighting){
		mStates|=State_LIGHTING;
		mLighting=lighting;
	}
	inline bool getLighting() const{return mLighting;}

	void setLightEffect(const peeper::LightEffect &lightEffect){
		mStates|=State_LIGHTEFFECT;
		mLightEffect.set(lightEffect);
	}
	inline const peeper::LightEffect &getLightEffect() const{return mLightEffect;}

	void setFaceCulling(const peeper::Renderer::FaceCulling &faceCulling){
		mStates|=State_FACECULLING;
		mFaceCulling=faceCulling;
	}
	inline const peeper::Renderer::FaceCulling &getFaceCulling() const{return mFaceCulling;}

	void setFill(const peeper::Renderer::Fill &fill){
		mStates|=State_FILL;
		mFill=fill;
	}
	inline const peeper::Renderer::Fill &getFill() const{return mFill;}

	void setAlphaTest(peeper::Renderer::AlphaTest alphaTest,scalar cutoff){
		mStates|=State_ALPHATEST;
		mAlphaTest=alphaTest;
		mAlphaTestCutoff=cutoff;
	}
	inline const peeper::Renderer::AlphaTest &getAlphaTest() const{return mAlphaTest;}
	inline scalar getAlphaTestCutoff() const{return mAlphaTestCutoff;}

	void setBlend(const peeper::Blend &blend){
		mStates|=State_BLEND;
		mBlend=blend;
	}
	inline const peeper::Blend &getBlend() const{return mBlend;}

	void setDepthSorted(bool sorted){
		mStates|=State_DEPTHSORT;
		mDepthSorted=sorted;
	}
	inline bool getDepthSorted() const{return mDepthSorted;}

	void setDepthWrite(bool depthWrite){
		mStates|=State_DEPTHWRITE;
		mDepthWrite=depthWrite;
	}
	inline bool getDepthWrite() const{return mDepthWrite;}

	void setDepthTest(peeper::Renderer::DepthTest depthTest){
		mStates|=State_DEPTHTEST;
		mDepthTest=depthTest;
	}
	inline peeper::Renderer::DepthTest getDepthTest() const{return mDepthTest;}

	void setLayer(int layer){mLayer=layer;}
	inline int getLayer() const{return mLayer;}

	inline int getNumTextureStages() const{return mTextureStages.size();}
	bool setTextureStage(int stage,const peeper::TextureStage::ptr &textureStage);
	inline const peeper::TextureStage::ptr &getTextureStage(int stage) const{return mTextureStages[stage];}

	void setSaveLocally(bool local){mSaveLocally=local;}
	bool getSaveLocally() const{return mSaveLocally;}

	void setupRenderer(peeper::Renderer *renderer,Material *previousMaterial=NULL);
	
protected:
	int mStates;
	peeper::LightEffect mLightEffect;
	bool mLighting;
	peeper::Renderer::FaceCulling mFaceCulling;
	peeper::Renderer::Fill mFill;
	peeper::Renderer::AlphaTest mAlphaTest;
	scalar mAlphaTestCutoff;
	peeper::Blend mBlend;
	bool mDepthSorted;
	bool mDepthWrite;
	peeper::Renderer::DepthTest mDepthTest;
	int mLayer;
	egg::Collection<peeper::TextureStage::ptr> mTextureStages;
	bool mSaveLocally;
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

	// Min/Max Layers
	const static int MIN_LAYER=-63;
	const static int MAX_LAYER=63;

	Material();
	virtual ~Material();

	void destroy();
	Material::ptr clone();

	void setLayer(int layer){mLayer=layer;}
	inline int getLayer() const{return mLayer;}

	void setSaveLocally(bool local){mSaveLocally=local;}
	inline bool getSaveLocally() const{return mSaveLocally;}

	inline void getNumRenderPasses(){return mRenderPasses.size();}
	inline RenderPass::ptr getRenderPass(int i){return mRenderPasses[i];}
	RenderPass::ptr addRenderPass();
	void removeRenderPass(RenderPass *pass);

protected:
	int mLayer;
	bool mSaveLocally;
	egg::Collection<RenderPass::ptr> mRenderPasses;
};

}
}
*/