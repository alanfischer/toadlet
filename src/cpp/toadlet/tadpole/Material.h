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
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Renderer.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Material:public egg::BaseResource{
public:
	TOADLET_SHARED_POINTERS(Material);

	Material(peeper::RenderStateSet::ptr renderStateSet=peeper::RenderStateSet::ptr());
	virtual ~Material();

	void destroy();

	void setBlendState(const peeper::BlendState &state){mRenderStateSet->setBlendState(state);}
	bool getBlendState(peeper::BlendState &state) const{return mRenderStateSet->getBlendState(state);}
	bool hasBlendState() const{peeper::BlendState state;return mRenderStateSet->getBlendState(state);}

	void setDepthState(const peeper::DepthState &state){mRenderStateSet->setDepthState(state);}
	bool getDepthState(peeper::DepthState &state) const{return mRenderStateSet->getDepthState(state);}
	bool hasDepthState() const{peeper::DepthState state;return mRenderStateSet->getDepthState(state);}

	void setRasterizerState(const peeper::RasterizerState &state){mRenderStateSet->setRasterizerState(state);}
	bool getRasterizerState(peeper::RasterizerState &state) const{return mRenderStateSet->getRasterizerState(state);}
	bool hasRasterizerState() const{peeper::RasterizerState state;return mRenderStateSet->getRasterizerState(state);}

	void setFogState(const peeper::FogState &state){mRenderStateSet->setFogState(state);}
	bool getFogState(peeper::FogState &state) const{return mRenderStateSet->getFogState(state);}
	bool hasFogState() const{peeper::FogState state;return mRenderStateSet->getFogState(state);}

	void setPointState(const peeper::PointState &state){mRenderStateSet->setPointState(state);}
	bool getPointState(peeper::PointState &state) const{return mRenderStateSet->getPointState(state);}
	bool hasPointState() const{peeper::PointState state;return mRenderStateSet->getPointState(state);}

	void setMaterialState(const peeper::MaterialState &state){mRenderStateSet->setMaterialState(state);}
	bool getMaterialState(peeper::MaterialState &state) const{return mRenderStateSet->getMaterialState(state);}
	bool hasMaterialState() const{peeper::MaterialState state;return mRenderStateSet->getMaterialState(state);}

	int getNumSamplerStates() const{return mRenderStateSet->getNumSamplerStates();}
	void setSamplerState(int i,const peeper::SamplerState &state){return mRenderStateSet->setSamplerState(i,state);}
	bool getSamplerState(int i,peeper::SamplerState &state) const{return mRenderStateSet->getSamplerState(i,state);}

	int getNumTextureStates() const{return mRenderStateSet->getNumTextureStates();}
	void setTextureState(int i,const peeper::TextureState &state){return mRenderStateSet->setTextureState(i,state);}
	bool getTextureState(int i,peeper::TextureState &state) const{return mRenderStateSet->getTextureState(i,state);}

	int getNumTextures() const{return mTextures.size();}
	void setTexture(peeper::Texture::ptr texture){setTexture(0,texture);}
	void setTexture(int i,peeper::Texture::ptr texture);
	peeper::Texture::ptr getTexture(int i=0) const{return i<mTextures.size()?mTextures[i]:NULL;}
	void setTextureName(const egg::String &name){setTextureName(0,name);}
	void setTextureName(int i,const egg::String &name){if(i>=mTextureNames.size()){mTextureNames.resize(i+1);}mTextureNames[i]=name;}
	egg::String getTextureName(int i=0) const{return i<mTextureNames.size()?mTextureNames[i]:(char*)NULL;}

	void setDepthSorted(bool sorted){mDepthSorted=sorted;}
	inline bool getDepthSorted() const{return mDepthSorted;}

	void setLayer(int layer){mLayer=layer;}
	int getLayer() const{return mLayer;}

	void modifyWith(Material *material);
	bool getManaged(){return getUniqueHandle()!=0;}

	void setupRenderer(peeper::Renderer *renderer);
	
protected:
	peeper::RenderStateSet::ptr mRenderStateSet;
	egg::Collection<peeper::Texture::ptr> mTextures;
	egg::Collection<egg::String> mTextureNames;
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