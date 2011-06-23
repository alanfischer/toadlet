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

#ifndef TOADLET_PEEPER_BACKABLEBUFFER_H
#define TOADLET_PEEPER_BACKABLEBUFFER_H

#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/peeper/PixelBuffer.h>
#include <toadlet/peeper/VariableBuffer.h>

namespace toadlet{
namespace peeper{

class RenderDevice;

class TOADLET_API BackableBuffer:public IndexBuffer,public VertexBuffer,public PixelBuffer,public VariableBuffer{
public:
	TOADLET_SHARED_POINTERS(BackableBuffer);

	BackableBuffer();
	virtual ~BackableBuffer();

	virtual IndexBuffer *getRootIndexBuffer(){return (mIndexFormat!=(IndexFormat)0)?(IndexBuffer*)mBack.get():NULL;}
	virtual VertexBuffer *getRootVertexBuffer(){return (mVertexFormat!=NULL)?(VertexBuffer*)mBack.get():NULL;}
	virtual PixelBuffer *getRootPixelBuffer(){return (mPixelFormat!=0)?(PixelBuffer*)mBack.get():NULL;}
	virtual VariableBuffer *getRootVariableBuffer(){return (mVariableFormat!=0)?(VariableBuffer*)mBack.get():NULL;}

	virtual void setBufferDestroyedListener(BufferDestroyedListener *listener){mListener=listener;}

	virtual bool create(int usage,int access,IndexFormat indexFormat,int size);
	virtual bool create(int usage,int access,VertexFormat::ptr vertexFormat,int size);
	virtual bool create(int usage,int access,int pixelFormat,int width,int height,int depth);
	virtual bool create(int usage,int access,VariableBufferFormat::ptr format);
	virtual void destroy();

	virtual void resetCreate();
	virtual void resetDestroy();

	virtual int getUsage() const{return mUsage;}
	virtual int getAccess() const{return mAccess;}
	virtual int getDataSize() const{return mDataSize;}
	virtual int getSize() const{return mSize;}
	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}
	virtual int getDepth() const{return mDepth;}

	virtual IndexFormat getIndexFormat() const{return mIndexFormat;}
	virtual VertexFormat::ptr getVertexFormat() const{return mVertexFormat;}
	virtual int getPixelFormat() const{return mPixelFormat;}
	virtual VariableBufferFormat::ptr getVariableBufferFormat() const{return mVariableFormat;}

	virtual tbyte *lock(int lockAccess);
	virtual bool unlock();

	virtual bool update(tbyte *data,int start,int size);

	virtual void setBack(IndexBuffer::ptr back);
	virtual void setBack(VertexBuffer::ptr back);
	virtual void setBack(PixelBuffer::ptr back,RenderDevice *renderDevice);
	virtual void setBack(VariableBuffer::ptr back);
	virtual Buffer::ptr getBack(){return mBack;}

	static void transposeVariables(VariableBufferFormat *variableFormat,tbyte *data);
	static void transposeVariable(VariableBufferFormat *variableFormat,tbyte *data,int i);

protected:
	void readBack();
	void writeBack();
	void updateBack(int start,int size);

	BufferDestroyedListener *mListener;
	int mUsage;
	int mAccess;
	int mDataSize;
	IndexFormat mIndexFormat;
	VertexFormat::ptr mVertexFormat;
	int mPixelFormat;
	VariableBufferFormat::ptr mVariableFormat;
	int mSize;
	int mWidth,mHeight,mDepth;

	bool mHasData;
	uint8 *mData;
	Buffer::ptr mBack;
};

}
}

#endif
