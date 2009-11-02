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

#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

BufferManager::BufferManager(Engine *engine):ResourceManager(engine){
	mEngine=engine;
}

IndexBuffer::ptr BufferManager::createIndexBuffer(int usageFlags,Buffer::AccessType accessType,IndexBuffer::IndexFormat indexFormat,int size){
	IndexBuffer::ptr indexBuffer(mEngine->getRenderer()->createIndexBuffer());
	indexBuffer->create(usageFlags,accessType,indexFormat,size);
	return indexBuffer;
}

VertexBuffer::ptr BufferManager::createVertexBuffer(int usageFlags,Buffer::AccessType accessType,VertexFormat::ptr vertexFormat,int size){
	VertexBuffer::ptr vertexBuffer(mEngine->getRenderer()->createVertexBuffer());
	vertexBuffer->create(usageFlags,accessType,vertexFormat,size);
	return vertexBuffer;
}

}
}

