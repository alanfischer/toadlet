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

#ifndef TOADLET_TADPOLE_BUFFERMANAGER_H
#define TOADLET_TADPOLE_BUFFERMANAGER_H

#include <toadlet/peeper/BackableBuffer.h>
#include <toadlet/peeper/BackableVertexFormat.h>
#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TOADLET_API BufferManager:public peeper::BufferDestroyedListener,public peeper::VertexFormatDestroyedListener{
public:
	BufferManager(Engine *engine,bool backable);
	virtual ~BufferManager();

	void destroy();

	peeper::VertexFormat::ptr createVertexFormat();
	peeper::IndexBuffer::ptr createIndexBuffer(int usage,int access,peeper::IndexBuffer::IndexFormat indexFormat,int size);
	peeper::VertexBuffer::ptr createVertexBuffer(int usage,int access,peeper::VertexFormat::ptr vertexFormat,int size);
	peeper::PixelBuffer::ptr createPixelBuffer(int usage,int access,int pixelFormat,int width,int height,int depth);
	peeper::ConstantBuffer::ptr createConstantBuffer(int usage,int access,int size);
	peeper::IndexBuffer::ptr cloneIndexBuffer(peeper::IndexBuffer::ptr oldIndexBuffer,int usage,int access,peeper::IndexBuffer::IndexFormat indexFormat,int size);
	peeper::VertexBuffer::ptr cloneVertexBuffer(peeper::VertexBuffer::ptr oldVertexBuffer,int usage,int access,peeper::VertexFormat::ptr vertexFormat,int size);
	/// @todo: clonePixelBuffer,cloneConstantBuffer

	void contextActivate(peeper::RenderDevice *renderDevice);
	void contextDeactivate(peeper::RenderDevice *renderDevice);
	void preContextReset(peeper::RenderDevice *renderDevice);
	void postContextReset(peeper::RenderDevice *renderDevice);

	void bufferDestroyed(peeper::Buffer *buffer);
	void vertexFormatDestroyed(peeper::VertexFormat *vertexFormat);

	bool useTriFan();

protected:
	egg::Collection<peeper::VertexFormat::ptr> mVertexFormats;
	egg::Collection<peeper::VertexBuffer::ptr> mVertexBuffers;
	egg::Collection<peeper::IndexBuffer::ptr> mIndexBuffers;
	egg::Collection<peeper::PixelBuffer::ptr> mPixelBuffers;
	egg::Collection<peeper::ConstantBuffer::ptr> mConstantBuffers;

	Engine *mEngine;
	bool mBackable;
};

}
}

#endif
