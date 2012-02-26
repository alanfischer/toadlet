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

#ifndef TOADLET_PEEPER_D3D10RENDERSTATE_H
#define TOADLET_PEEPER_D3D10RENDERSTATE_H

#include "D3D10Includes.h"
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/RenderState.h>

namespace toadlet{
namespace peeper{

class D3D10RenderDevice;

class TOADLET_API D3D10RenderState:public BaseResource,public RenderState{
public:
	TOADLET_RESOURCE(D3D10RenderState,RenderState);

	D3D10RenderState(D3D10RenderDevice *renderDevice);
	virtual ~D3D10RenderState();

	RenderState *getRootRenderState(){return this;}

	bool create(){return true;}
	void destroy();

	void setBlendState(const BlendState &state);
	bool getBlendState(BlendState &state) const;

	void setDepthState(const DepthState &state);
	bool getDepthState(DepthState &state) const;

	void setRasterizerState(const RasterizerState &state);
	bool getRasterizerState(RasterizerState &state) const;

	void setFogState(const FogState &state);
	bool getFogState(FogState &state) const;

	void setPointState(const PointState &state);
	bool getPointState(PointState &state) const;

	void setMaterialState(const MaterialState &state);
	bool getMaterialState(MaterialState &state) const;

	int getNumSamplerStates(Shader::ShaderType type) const{return mSamplerStates[type].size();}
	void setSamplerState(Shader::ShaderType type,int i,const SamplerState &state);
	bool getSamplerState(Shader::ShaderType type,int i,SamplerState &state) const;

	int getNumTextureStates(Shader::ShaderType type) const{return mTextureStates[type].size();}
	void setTextureState(Shader::ShaderType type,int i,const TextureState &state);
	bool getTextureState(Shader::ShaderType type,int i,TextureState &state) const;

protected:
	D3D10RenderDevice *mDevice;
	ID3D10Device *mD3DDevice;
	BlendState *mBlendState;
	DepthState *mDepthState;
	RasterizerState *mRasterizerState;
	FogState *mFogState;
	PointState *mPointState;
	MaterialState *mMaterialState;
	Collection<SamplerState*> mSamplerStates[Shader::ShaderType_MAX];
	Collection<TextureState*> mTextureStates[Shader::ShaderType_MAX];

	ID3D10BlendState *mD3DBlendState;
	ID3D10DepthStencilState *mD3DDepthStencilState;
	ID3D10RasterizerState *mD3DRasterizerState;
	Collection<ID3D10SamplerState*> mD3DSamplerStates[Shader::ShaderType_MAX];

	friend class D3D10RenderDevice;
};

}
}

#endif
