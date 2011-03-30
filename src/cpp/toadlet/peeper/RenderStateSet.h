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

#ifndef TOADLET_PEEPER_RENDERSTATESET_H
#define TOADLET_PEEPER_RENDERSTATESET_H

#include <toadlet/egg/Resource.h>
#include <toadlet/peeper/Types.h>
#include <toadlet/peeper/BlendState.h>
#include <toadlet/peeper/DepthState.h>

namespace toadlet{
namespace peeper{

class TOADLET_API RenderStateSet:public egg::Resource{
public:
	TOADLET_SHARED_POINTERS(RenderStateSet);

	virtual ~RenderStateSet(){}

	// Rasterizer states
	virtual void setFaceCulling(const FaceCulling &faceCulling)=0;
	virtual const FaceCulling &getFaceCulling() const=0;
	virtual void setFill(const Fill &fill)=0;
	virtual const Fill &getFill()=0;
//	virtual void setDepthBiasState(const DepthBias &depthBias)=0;
//	virtual const DepthBiasState &getDepthBiasState() const=0;
	virtual void setDithering(bool dithering)=0;
	virtual bool getDithering() const=0;

	// Depth-Stencil states
	virtual void setDepthState(const DepthState &state)=0;
	virtual const DepthState &getDepthState(const DepthState &state)=0;

	// Blend state
	virtual void setBlendState(const BlendState &state)=0;
	virtual const BlendState &getBlendState() const=0;

	// Sampler states
//	virtual void setSamplerState(Stage stage,int number,const SamplerState &state)=0;
//	virtual const SamplerState &getSamplerState(Stage stage,int number)=0;
	
	// Pixel fixed states
	virtual void setAlphaTest(const AlphaTest &alphaTest,scalar cutoff)=0;
	virtual void setFogState(const FogState &state)=0;
	virtual void setShading(const Shading &shading)=0;
	virtual void setLighting(bool lighting)=0;
	virtual void setLightEffect(const LightEffect &lightEffect)=0;

	// Vertex fixed states
	virtual void setPointState(const PointState &state)=0;
	virtual void setNormalize(const Normalize &normalize)=0;
};

};

}
}

#endif
