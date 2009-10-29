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

#ifndef TOADLET_PEEPER_RENDERER_H
#define TOADLET_PEEPER_RENDERER_H

#include <toadlet/egg/Collection.h>
#include <toadlet/peeper/Types.h>
#include <toadlet/peeper/Color.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>

namespace toadlet{
namespace peeper{

class Blend;
class CapabilitySet;
class Buffer;
class BufferPeer;
class Light;
class LightEffect;
class Program;
class ProgramPeer;
class RenderTarget;
class SurfaceRenderTarget;
class Shader;
class ShaderPeer;
class StatisticsSet;
class Texture;
class TexturePeer;
class TextureStage;
class Viewport;

class Renderer{
public:
	enum RendererStatus{
		RendererStatus_OK,
		RendererStatus_UNAVAILABLE,
		RendererStatus_NEEDSRESET,
	};

	enum ClearFlag{
		ClearFlag_COLOR=1,
		ClearFlag_DEPTH=2,
		ClearFlag_STENCIL=4,
	};

	enum FaceCulling{
		FaceCulling_NONE,
		FaceCulling_FRONT,
		FaceCulling_BACK,
	};

	enum DepthTest{
		DepthTest_NONE,
		DepthTest_NEVER,
		DepthTest_LESS,
		DepthTest_EQUAL,
		DepthTest_LEQUAL,
		DepthTest_GREATER,
		DepthTest_NOTEQUAL,
		DepthTest_GEQUAL,
		DepthTest_ALWAYS,
	};

	enum AlphaTest{
		AlphaTest_NONE,
		AlphaTest_LESS,
		AlphaTest_EQUAL,
		AlphaTest_LEQUAL,
		AlphaTest_GREATER,
		AlphaTest_NOTEQUAL,
		AlphaTest_GEQUAL,
		AlphaTest_ALWAYS,
	};

	enum TexCoordGen{
		TexCoordGen_DISABLED,
		TexCoordGen_OBJECTSPACE,
		TexCoordGen_CAMERASPACE,
	};

	enum Fog{
		Fog_NONE,
		Fog_LINEAR,
	};

	enum Fill{
		Fill_POINT,
		Fill_LINE,
		Fill_SOLID,
	};

	enum Shading{
		Shading_FLAT,
		Shading_SMOOTH,
	};

	enum Normalize{
		Normalize_NONE,
		Normalize_RESCALE,
		Normalize_NORMALIZE,
	};

	virtual ~Renderer(){}

	// Creation/Destruction
	virtual bool create(RenderTarget *target,int *options)=0;
	virtual bool destroy()=0;
	virtual RendererStatus getStatus()=0;
	virtual bool reset()=0;

	// Resource operations
	virtual Texture *createTexture()=0;
	virtual SurfaceRenderTarget *createSurfaceRenderTarget()=0;
	virtual BufferPeer *createBufferPeer(Buffer *buffer)=0;
	virtual ProgramPeer *createProgramPeer(Program *program)=0;
	virtual ShaderPeer *createShaderPeer(Shader *shader)=0;

	// Matrix operations
	virtual void setModelMatrix(const Matrix4x4 &matrix)=0;
	virtual void setViewMatrix(const Matrix4x4 &matrix)=0;
	virtual void setProjectionMatrix(const Matrix4x4 &matrix)=0;

	// Rendering operations
	virtual RenderTarget *getPrimaryRenderTarget()=0;
	virtual bool setRenderTarget(RenderTarget *target)=0;
	virtual RenderTarget *getRenderTarget()=0;
	virtual void setViewport(const Viewport &viewport)=0;
	virtual void clear(int clearFlags,const Color &clearColor)=0;
	virtual void swap()=0;
	virtual void beginScene()=0;
	virtual void endScene()=0;
	virtual void renderPrimitive(const VertexData::ptr &vertexData,const IndexData::ptr &indexData)=0;

	// Render state operations
	virtual void setDefaultStates()=0;
	virtual void setAlphaTest(const AlphaTest &alphaTest,scalar cutoff)=0;
	virtual void setBlend(const Blend &blend)=0;
	virtual void setFaceCulling(const FaceCulling &faceCulling)=0;
	virtual void setDepthTest(const DepthTest &depthTest)=0;
	virtual void setDepthWrite(bool depthWrite)=0;
	virtual void setDithering(bool dithering)=0;
	virtual void setFogParameters(const Fog &fog,scalar nearDistance,scalar farDistance,const Color &color)=0;
	virtual void setLightEffect(const LightEffect &lightEffect)=0;
	virtual void setFill(const Fill &fill)=0;
	virtual void setLighting(bool lighting)=0;
	virtual void setShading(const Shading &shading)=0;
	virtual void setColorWrite(bool color)=0;
	virtual void setNormalize(const Normalize &normalize)=0;
	virtual void setDepthBias(scalar constant,scalar slope)=0;
	virtual void setTexturePerspective(bool texturePerspective)=0;
	virtual void setPointParameters(bool sprite,scalar size,bool attenuated,scalar constant,scalar linear,scalar quadratic,scalar minSize,scalar maxSize)=0;
	virtual void setTextureStage(int stage,TextureStage *textureStage)=0;
	virtual void setProgram(const Program *program)=0;
	virtual void setTexCoordGen(int stage,const TexCoordGen &texCoordGen,const Matrix4x4 &matrix)=0;
	virtual void getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result)=0;
	 // A workaround here, since OpenGL & Direct3D have different shadow comparison methods
	virtual void setShadowComparisonMethod(bool enabled)=0;

	// Light operations
	virtual void setLight(int i,Light *light)=0;
	virtual void setLightEnabled(int i,bool enable)=0;
	virtual void setAmbientColor(const Color &ambient)=0;

	// Misc operations
	virtual void setMirrorY(bool mirrorY)=0;
	virtual void copyFrameBufferToTexture(Texture *texture)=0;

	virtual const StatisticsSet &getStatisticsSet()=0;
	virtual const CapabilitySet &getCapabilitySet()=0;
};

}
}

#endif
