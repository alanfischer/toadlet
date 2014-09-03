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

#include <toadlet/egg/Mutex.h>
#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/peeper/VariableBuffer.h>
#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/Track.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TOADLET_API BufferManager:public Object,public ResourceDestroyedListener{
public:
	TOADLET_OBJECT(BufferManager);

	BufferManager(Engine *engine);
	virtual ~BufferManager();

	void destroy();

	VertexFormat::ptr createVertexFormat();
	IndexBuffer::ptr createIndexBuffer(int usage,int access,IndexBuffer::IndexFormat indexFormat,int size);
	VertexBuffer::ptr createVertexBuffer(int usage,int access,VertexFormat::ptr vertexFormat,int size);
	PixelBuffer::ptr createPixelBuffer(int usage,int access,int pixelFormat,int width,int height,int depth);
	VariableBuffer::ptr createVariableBuffer(int usage,int access,VariableBufferFormat::ptr variableFormat);
	IndexBuffer::ptr cloneIndexBuffer(IndexBuffer::ptr oldIndexBuffer,int usage,int access,IndexBuffer::IndexFormat indexFormat,int size);
	VertexBuffer::ptr cloneVertexBuffer(VertexBuffer::ptr oldVertexBuffer,int usage,int access,VertexFormat::ptr vertexFormat,int size);

	Track *createColorTrack();

	void contextActivate(RenderDevice *renderDevice);
	void contextDeactivate(RenderDevice *renderDevice);
	void preContextReset(RenderDevice *renderDevice);
	void postContextReset(RenderDevice *renderDevice);

	void resourceDestroyed(Resource *buffer);

	void outputVariableBufferFormat(VariableBufferFormat::ptr format);
	void outputVariable(VariableBufferFormat::Variable *variable,const String &tabs);

protected:
	Engine *mEngine;

	Collection<VertexFormat::ptr> mVertexFormats;
	Collection<VertexBuffer::ptr> mVertexBuffers;
	Collection<IndexBuffer::ptr> mIndexBuffers;
	Collection<PixelBuffer::ptr> mPixelBuffers;
	Collection<VariableBuffer::ptr> mVariableBuffers;

	#if defined(TOADLET_THREADSAFE)
		Mutex mMutex;
	#endif
};

}
}

#endif
