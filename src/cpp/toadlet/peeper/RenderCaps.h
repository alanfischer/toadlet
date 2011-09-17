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
#include <toadlet/peeper/Shader.h>

namespace toadlet{
namespace peeper{

class RenderCaps{
public:
	RenderCaps():
		resetOnResize(false),
		maxTextureStages(0),
		maxTextureSize(0),
		renderToTexture(false),
		renderToTextureNonPowerOf2Restricted(false),
		textureDot3(false),
		textureNonPowerOf2Restricted(false),
		textureNonPowerOf2(false),
		textureAutogenMipMaps(false),
		cubeMap(false),
		idealVertexFormatType(VertexFormat::Format_TYPE_FLOAT_32),
		triangleFan(false),

		maxLights(0),
		texturePerspective(false),
		pointSprites(false)
	{
		memset(hasShader,0,sizeof(hasShader));
		memset(hasFixed,0,sizeof(hasFixed));
	}

	RenderCaps &set(const RenderCaps &caps){
		resetOnResize=caps.resetOnResize;
		maxTextureStages=caps.maxTextureStages;
		maxTextureSize=caps.maxTextureSize;
		renderToTexture=caps.renderToTexture;
		renderToDepthTexture=caps.renderToDepthTexture;
		renderToTextureNonPowerOf2Restricted=caps.renderToTextureNonPowerOf2Restricted;
		textureDot3=caps.textureDot3;
		textureNonPowerOf2Restricted=caps.textureNonPowerOf2Restricted;
		textureNonPowerOf2=caps.textureNonPowerOf2;
		textureAutogenMipMaps=caps.textureAutogenMipMaps;
		cubeMap=caps.cubeMap;
		idealVertexFormatType=caps.idealVertexFormatType;
		triangleFan=caps.triangleFan;

		memcpy(hasShader,caps.hasShader,sizeof(hasShader));
		memcpy(hasFixed,caps.hasFixed,sizeof(hasFixed));

		maxLights=caps.maxLights;
		texturePerspective=caps.texturePerspective;
		pointSprites=caps.pointSprites;

		return *this;
	}

	// Resource parameters
	bool resetOnResize;
	int maxTextureStages;
	int maxTextureSize;
	bool renderToTexture;
	bool renderToDepthTexture;
	bool renderToTextureNonPowerOf2Restricted;
	bool textureDot3;
	bool textureNonPowerOf2Restricted;
	bool textureNonPowerOf2;
	bool textureAutogenMipMaps;
	bool cubeMap;
	int idealVertexFormatType;
	bool triangleFan;

	// Shader parameters
	bool hasShader[Shader::ShaderType_MAX];
	bool hasFixed[Shader::ShaderType_MAX];

	// Fixed Vertex
	int maxLights;
	// Fixed Fragment
	bool texturePerspective;
	// Fixed Geometry
	bool pointSprites;
};

}
}

#endif
