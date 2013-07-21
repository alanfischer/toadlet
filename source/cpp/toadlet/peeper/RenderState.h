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

#include <toadlet/egg/Resource.h>
#include <toadlet/peeper/BlendState.h>
#include <toadlet/peeper/DepthState.h>
#include <toadlet/peeper/RasterizerState.h>
#include <toadlet/peeper/FogState.h>
#include <toadlet/peeper/PointState.h>
#include <toadlet/peeper/MaterialState.h>
#include <toadlet/peeper/SamplerState.h>
#include <toadlet/peeper/TextureState.h>
#include <toadlet/peeper/Shader.h>

namespace toadlet{
namespace peeper{

class TOADLET_API RenderState:public Resource{
public:
	TOADLET_INTERFACE(RenderState);

	enum StateType{
		StateType_BLEND=	0,
		StateType_DEPTH=	1,
		StateType_RASTERIZER=2,
		StateType_FOG=		3,
		StateType_POINT=	4,
		StateType_MATERIAL=	5,

		StateType_BIT_BLEND=	1<<StateType_BLEND,
		StateType_BIT_DEPTH=	1<<StateType_DEPTH,
		StateType_BIT_RASTERIZER=1<<StateType_RASTERIZER,
		StateType_BIT_FOG=		1<<StateType_FOG,
		StateType_BIT_POINT=	1<<StateType_POINT,
		StateType_BIT_MATERIAL=	1<<StateType_MATERIAL,
	};

	virtual ~RenderState(){}

	virtual RenderState *getRootRenderState()=0;

	virtual bool create()=0;
	virtual void destroy()=0;

	virtual int getSetStates() const=0;

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

	virtual void setSamplerState(Shader::ShaderType type,int i,const SamplerState &state)=0;
	virtual bool getSamplerState(Shader::ShaderType type,int i,SamplerState &state) const=0;
	virtual int getNumSamplerStates(Shader::ShaderType type) const=0;

	virtual void setTextureState(Shader::ShaderType type,int i,const TextureState &state)=0;
	virtual bool getTextureState(Shader::ShaderType type,int i,TextureState &state) const=0;
	virtual int getNumTextureStates(Shader::ShaderType type) const=0;
};

}
}

#endif
