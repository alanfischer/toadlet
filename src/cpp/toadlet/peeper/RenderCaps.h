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

#ifndef TOADLET_PEEPER_RENDERCAPS_H
#define TOADLET_PEEPER_RENDERCAPS_H

#include <toadlet/peeper/Types.h>
#include <toadlet/peeper/VertexFormat.h>

namespace toadlet{
namespace peeper{

class RenderCaps{
public:
	RenderCaps():
		resetOnResize(false),
		maxLights(0),
		maxTextureStages(0),
		maxTextureSize(0),
		pointSprites(false),
		vertexShaders(false),
		maxVertexShaderLocalParameters(0),
		fragmentShaders(false),
		maxFragmentShaderLocalParameters(0),
		renderToTexture(false),
		renderToTextureNonPowerOf2Restricted(false),
		textureDot3(false),
		textureNonPowerOf2Restricted(false),
		textureNonPowerOf2(false),
		textureAutogenMipMaps(false),
		idealVertexFormatBit(VertexFormat::Format_BIT_FLOAT_32),
		triangleFan(false),
		fill(false),
		texturePerspective(false),
		cubeMap(false)
	{}

	RenderCaps &set(const RenderCaps &caps){
		resetOnResize=caps.resetOnResize;
		maxLights=caps.maxLights;
		maxTextureStages=caps.maxTextureStages;
		maxTextureSize=caps.maxTextureSize;
		pointSprites=caps.pointSprites;
		vertexShaders=caps.vertexShaders;
		maxVertexShaderLocalParameters=caps.maxVertexShaderLocalParameters;
		fragmentShaders=caps.fragmentShaders;
		maxFragmentShaderLocalParameters=caps.maxFragmentShaderLocalParameters;
		renderToTexture=caps.renderToTexture;
		renderToDepthTexture=caps.renderToDepthTexture;
		renderToTextureNonPowerOf2Restricted=caps.renderToTextureNonPowerOf2Restricted;
		textureDot3=caps.textureDot3;
		textureNonPowerOf2Restricted=caps.textureNonPowerOf2Restricted;
		textureNonPowerOf2=caps.textureNonPowerOf2;
		textureAutogenMipMaps=caps.textureAutogenMipMaps;
		idealVertexFormatBit=caps.idealVertexFormatBit;
		triangleFan=caps.triangleFan;
		fill=caps.fill;
		texturePerspective=caps.texturePerspective;
		cubeMap=caps.cubeMap;

		return *this;
	}

	bool resetOnResize;
	int maxLights;
	int maxTextureStages;
	int maxTextureSize;
	bool pointSprites;
	bool vertexShaders;
	int maxVertexShaderLocalParameters;
	bool fragmentShaders;
	int maxFragmentShaderLocalParameters;
	bool renderToTexture;
	bool renderToDepthTexture;
	bool renderToTextureNonPowerOf2Restricted;
	bool textureDot3;
	bool textureNonPowerOf2Restricted;
	bool textureNonPowerOf2;
	bool textureAutogenMipMaps;
	int idealVertexFormatBit;
	bool triangleFan;
	bool fill;
	bool texturePerspective;
	bool cubeMap;
};

}
}

#endif
