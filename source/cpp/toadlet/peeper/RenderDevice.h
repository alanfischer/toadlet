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

#ifndef TOADLET_PEEPER_RENDERDEVICE_H
#define TOADLET_PEEPER_RENDERDEVICE_H

#include <toadlet/egg/Collection.h>
#include <toadlet/peeper/Types.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Shader.h>

namespace toadlet{
namespace peeper{

class BlendState;
class DepthState;
class FogState;
class IndexBuffer;
class LightState;
class MaterialState;
class PixelBufferRenderTarget;
class PixelBuffer;
class PointState;
class Query;
class RenderTarget;
class RasterizerState;
class RenderCaps;
class RenderState;
class ShaderState;
class Texture;
class VariableBuffer;
class VertexBuffer;
class VertexLayout;
class VertexFormat;
class Viewport;

class RenderDevice{
public:
	enum ClearType{
		ClearType_BIT_COLOR=	1<<0,
		ClearType_BIT_DEPTH=	1<<1,
		ClearType_BIT_STENCIL=	1<<2,
	};

	enum DeviceStatus{
		DeviceStatus_OK,
		DeviceStatus_UNAVAILABLE,
		DeviceStatus_NEEDSRESET,
	};

	enum MatrixType{
		MatrixType_PROJECTION,
		MatrixType_VIEW,
		MatrixType_MODEL,
	};

	virtual ~RenderDevice(){}

	// Creation/Destruction
	virtual bool create(RenderTarget *target,int *options)=0;
	virtual void destroy()=0;
	virtual DeviceStatus activate()=0;
	virtual bool reset()=0;
	virtual bool activateAdditionalContext()=0;

	// Resource operations
	/// @todo: Merge all the create*Buffer into just a createBuffer where I'd pass in the type?
	virtual Texture *createTexture()=0;
	virtual PixelBufferRenderTarget *createPixelBufferRenderTarget()=0;
	virtual PixelBuffer *createPixelBuffer()=0;
	virtual VertexFormat *createVertexFormat()=0;
	virtual VertexBuffer *createVertexBuffer()=0;
	virtual IndexBuffer *createIndexBuffer()=0;
	virtual VariableBuffer *createVariableBuffer()=0;
	virtual Shader *createShader()=0;
	virtual Query *createQuery()=0;
	virtual RenderState *createRenderState()=0;
	virtual ShaderState *createShaderState()=0;

	// Rendering operations
	virtual RenderTarget *getPrimaryRenderTarget()=0;
	virtual bool setRenderTarget(RenderTarget *target)=0;
	virtual RenderTarget *getRenderTarget()=0;
	virtual void setViewport(const Viewport &viewport)=0;
	virtual void clear(int clearFlags,const Vector4 &clearColor)=0;
	virtual void swap()=0;
	virtual void beginScene()=0;
	virtual void endScene()=0;
	virtual void renderPrimitive(VertexData *vertexData,IndexData *indexData)=0;
	virtual bool copyFrameBufferToPixelBuffer(PixelBuffer *dst)=0;
	virtual bool copyPixelBuffer(PixelBuffer *dst,PixelBuffer *src)=0;

	// State operations
	virtual void setDefaultState()=0;
	virtual bool setRenderState(RenderState *renderState)=0;
	virtual bool setShaderState(ShaderState *shaderState)=0;
	virtual void setBuffer(Shader::ShaderType type,int i,VariableBuffer *buffer)=0;
	virtual void setTexture(Shader::ShaderType type,int i,Texture *texture)=0;

	// Old fixed states
	virtual void setMatrix(MatrixType type,const Matrix4x4 &matrix)=0;
	virtual void setAmbientColor(const Vector4 &ambient)=0;
	virtual void setLightState(int i,const LightState &light)=0;
	virtual void setLightEnabled(int i,bool enable)=0;

	// Misc operations
	virtual int getClosePixelFormat(int pixelFormat,int usage)=0;
	virtual bool getShaderProfileSupported(const String &profile)=0;
	virtual void getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result)=0;
	virtual bool getRenderCaps(RenderCaps &caps)=0;
};

}
}

#endif
