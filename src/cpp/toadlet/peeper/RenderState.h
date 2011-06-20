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

#ifndef TOADLET_PEEPER_RENDERSTATE_H
#define TOADLET_PEEPER_RENDERSTATE_H

#include <toadlet/peeper/RenderStateDestroyedListener.h>
#include <toadlet/peeper/BlendState.h>
#include <toadlet/peeper/DepthState.h>
#include <toadlet/peeper/RasterizerState.h>
#include <toadlet/peeper/FogState.h>
#include <toadlet/peeper/PointState.h>
#include <toadlet/peeper/MaterialState.h>
#include <toadlet/peeper/SamplerState.h>
#include <toadlet/peeper/TextureState.h>

namespace toadlet{
namespace peeper{

class TOADLET_API RenderState{
public:
	TOADLET_SHARED_POINTERS(RenderState);

	virtual ~RenderState(){}

	virtual RenderState *getRootRenderState()=0;

	virtual void setRenderStateDestroyedListener(RenderStateDestroyedListener *listener)=0;

	virtual bool create()=0;
	virtual void destroy()=0;

	virtual void setBlendState(const BlendState &state)=0;
	virtual bool getBlendState(BlendState &state) const=0;

	virtual void setDepthState(const DepthState &state)=0;
	virtual bool getDepthState(DepthState &state) const=0;

	virtual void setRasterizerState(const RasterizerState &state)=0;
	virtual bool getRasterizerState(RasterizerState &state) const=0;

	virtual void setFogState(const FogState &state)=0;
	virtual bool getFogState(FogState &state) const=0;

	virtual void setPointState(const PointState &state)=0;
	virtual bool getPointState(PointState &state) const=0;

	virtual void setMaterialState(const MaterialState &state)=0;
	virtual bool getMaterialState(MaterialState &state) const=0;

	virtual void setSamplerState(int i,const SamplerState &state)=0;
	virtual bool getSamplerState(int i,SamplerState &state) const=0;
	virtual int getNumSamplerStates() const=0;

	virtual void setTextureState(int i,const TextureState &state)=0;
	virtual bool getTextureState(int i,TextureState &state) const=0;
	virtual int getNumTextureStates() const=0;
};

}
}

#endif
