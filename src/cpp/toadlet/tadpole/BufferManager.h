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

#include <toadlet/peeper/BackableIndexBuffer.h>
#include <toadlet/peeper/BackableVertexBuffer.h>
#include <toadlet/peeper/Renderer.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TOADLET_API BufferManager:public peeper::BufferDestroyedListener{
public:
	BufferManager(Engine *engine);
	virtual ~BufferManager();

	virtual void destroy();

	virtual peeper::IndexBuffer::ptr createIndexBuffer(int usageFlags,peeper::Buffer::AccessType accessType,peeper::IndexBuffer::IndexFormat indexFormat,int size);
	virtual peeper::VertexBuffer::ptr createVertexBuffer(int usageFlags,peeper::Buffer::AccessType accessType,peeper::VertexFormat::ptr vertexFormat,int size);
	virtual peeper::IndexBuffer::ptr cloneIndexBuffer(peeper::IndexBuffer::ptr oldIndexBuffer,int usageFlags,peeper::Buffer::AccessType accessType,peeper::IndexBuffer::IndexFormat indexFormat,int size);
	virtual peeper::VertexBuffer::ptr cloneVertexBuffer(peeper::VertexBuffer::ptr oldVertexBuffer,int usageFlags,peeper::Buffer::AccessType accessType,peeper::VertexFormat::ptr vertexFormat,int size);

	virtual void contextActivate(peeper::Renderer *renderer);
	virtual void contextDeactivate(peeper::Renderer *renderer);
	virtual void preContextReset(peeper::Renderer *renderer);
	virtual void postContextReset(peeper::Renderer *renderer);

	virtual void bufferDestroyed(peeper::Buffer *buffer);

protected:
	egg::Collection<peeper::IndexBuffer::ptr> mIndexBuffers;
	egg::Collection<peeper::VertexBuffer::ptr> mVertexBuffers;

	Engine *mEngine;
};

}
}

#endif
