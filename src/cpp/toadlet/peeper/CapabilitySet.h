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

#ifndef TOADLET_PEEPER_CAPABILITYSET_H
#define TOADLET_PEEPER_CAPABILITYSET_H

#include <toadlet/peeper/Types.h>
#include <toadlet/peeper/VertexFormat.h>

namespace toadlet{
namespace peeper{

class CapabilitySet{
public:
	CapabilitySet():
		resetOnResize(false),
		hardwareTextures(false),
		hardwareVertexBuffers(false),
		hardwareIndexBuffers(false),
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

	bool resetOnResize;
	bool hardwareTextures;
	bool hardwareVertexBuffers;
	bool hardwareIndexBuffers;
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
