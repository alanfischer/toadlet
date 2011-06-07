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

namespace toadlet{
namespace peeper{

class BlendState;
class DepthState;
class IndexBuffer;
class LightState;
class MaterialState;
class FogState;
class Program;
class Query;
class RenderTarget;
class PixelBufferRenderTarget;
class PixelBuffer;
class PointState;
class RasterizerState;
class RenderCaps;
class RenderState;
class Shader;
class Texture;
class VertexBuffer;
class VertexFormat;
class Viewport;

class RenderDevice{
public:
	enum DeviceStatus{
		DeviceStatus_OK,
		DeviceStatus_UNAVAILABLE,
		DeviceStatus_NEEDSRESET,
	};

	virtual ~RenderDevice(){}

	// Creation/Destruction
	virtual bool create(RenderTarget *target,int *options)=0;
	virtual void destroy()=0;
	virtual DeviceStatus getStatus()=0;
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
	virtual Program *createProgram()=0;
	virtual Shader *createShader()=0;
	virtual Query *createQuery()=0;
	virtual RenderState *createRenderState()=0;

	// Matrix operations
	virtual void setModelMatrix(const Matrix4x4 &matrix)=0;
	virtual void setViewMatrix(const Matrix4x4 &matrix)=0;
	virtual void setProjectionMatrix(const Matrix4x4 &matrix)=0;

	// Rendering operations
	virtual RenderTarget *getPrimaryRenderTarget()=0;
	virtual bool setRenderTarget(RenderTarget *target)=0;
	virtual RenderTarget *getRenderTarget()=0;
	virtual void setViewport(const Viewport &viewport)=0;
	virtual void clear(int clearFlags,const Vector4 &clearColor)=0;
	virtual void swap()=0;
	virtual void beginScene()=0;
	virtual void endScene()=0;
	virtual void renderPrimitive(const VertexData::ptr &vertexData,const IndexData::ptr &indexData)=0;
	virtual bool copyFrameBufferToPixelBuffer(PixelBuffer *dst)=0;
	virtual bool copyPixelBuffer(PixelBuffer *dst,PixelBuffer *src)=0;
	virtual void setDefaultState()=0;
	virtual bool setRenderState(RenderState *renderState)=0;
	virtual void setTexture(int i,Texture *texture)=0;

	// Old fixed states
	virtual void setNormalize(const Normalize &normalize)=0;
	virtual void setAmbientColor(const Vector4 &ambient)=0;
	virtual void setLightState(int i,const LightState &light)=0;
	virtual void setLightEnabled(int i,bool enable)=0;

	// Misc operations
	virtual int getCloseTextureFormat(int format,int usage)=0;
	virtual void getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result)=0;
	virtual bool getRenderCaps(RenderCaps &caps)=0;
};

}
}

#endif
