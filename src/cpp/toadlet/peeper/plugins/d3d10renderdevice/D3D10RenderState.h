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
#include <toadlet/peeper/RenderState.h>

namespace toadlet{
namespace peeper{

class D3D10RenderDevice;

class TOADLET_API D3D10RenderState:public RenderState{
public:
	D3D10RenderState(D3D10RenderDevice *renderDevice);
	virtual ~D3D10RenderState();

	RenderState *getRootRenderState(){return this;}

	void setRenderStateDestroyedListener(RenderStateDestroyedListener *listener){mListener=listener;}

	bool create(){return true;}
	void destroy();

	void setBlendState(const BlendState &state);
	bool getBlendState(BlendState &state) const;

	void setDepthState(const DepthState &state);
	bool getDepthState(DepthState &state) const;

	void setRasterizerState(const RasterizerState &state);
	bool getRasterizerState(RasterizerState &state) const;

	void setFogState(const FogState &state){}
	bool getFogState(FogState &state) const{return false;}

	void setPointState(const PointState &state){}
	bool getPointState(PointState &state) const{return false;}

	void setMaterialState(const MaterialState &state){}
	bool getMaterialState(MaterialState &state) const{return false;}

	int getNumSamplerStates() const{return mSamplerStates.size();}
	void setSamplerState(int i,const SamplerState &state);
	bool getSamplerState(int i,SamplerState &state) const;

	int getNumTextureStates() const{return mTextureStates.size();}
	void setTextureState(int i,const TextureState &state);
	bool getTextureState(int i,TextureState &state) const;

protected:
	D3D10RenderDevice *mDevice;
	ID3D10Device *mD3DDevice;

	RenderStateDestroyedListener *mListener;
	BlendState *mBlendState;
	ID3D10BlendState *mD3DBlendState;
	DepthState *mDepthState;
	ID3D10DepthStencilState *mD3DDepthStencilState;
	RasterizerState *mRasterizerState;
	ID3D10RasterizerState *mD3DRasterizerState;
	Collection<SamplerState*> mSamplerStates;
	Collection<ID3D10SamplerState*> mD3DSamplerStates;

	Collection<TextureState*> mTextureStates;
	MaterialState *mMaterialState;

	friend class D3D10RenderDevice;
};

}
}

#endif
