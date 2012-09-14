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

#ifndef TOADLET_PEEPER_D3D10BUFFER_H
#define TOADLET_PEEPER_D3D10BUFFER_H

#include "D3D10Includes.h"
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/peeper/VariableBuffer.h>

namespace toadlet{
namespace peeper{

class D3D10RenderDevice;

class TOADLET_API D3D10Buffer:public BaseResource,public IndexBuffer,public VertexBuffer,public VariableBuffer{
public:
	TOADLET_RESOURCE(D3D10Buffer,BaseResource);

	D3D10Buffer(D3D10RenderDevice *renderDevice);

	IndexBuffer *getRootIndexBuffer(){return this;}
	VertexBuffer *getRootVertexBuffer(){return this;}
	VariableBuffer *getRootVariableBuffer(){return this;}

	bool create(int usage,int access,IndexFormat indexFormat,int size);
	bool create(int usage,int access,VertexFormat::ptr vertexFormat,int size);
	bool create(int usage,int access,VariableBufferFormat::ptr variableFormat);
	void destroy();

	void resetCreate(){}
	void resetDestroy(){}

	int getUsage() const{return mUsage;}
	int getAccess() const{return mAccess;}
	int getDataSize() const{return mDataSize;}
	int getSize() const{return mSize;}

	IndexFormat getIndexFormat() const{return mIndexFormat;}
	VertexFormat::ptr getVertexFormat() const{return mVertexFormat;}
	VariableBufferFormat::ptr getVariableBufferFormat() const{return mVariableFormat;}

	tbyte *lock(int lockAccess);
	bool unlock();
	bool update(tbyte *data,int start,int size);
	/// @todo: This isn't used.  It will be removed, but I may move it to a ResourceBuffer class.
	bool updateTexture(int i,Texture::ptr resource,const SamplerState &state);

protected:
	bool createContext();
	bool destroyContext();

	D3D10RenderDevice *mDevice;

	int mUsage;
	int mAccess;
	int mDataSize;
	int mSize;

	IndexFormat mIndexFormat;
	VertexFormat::ptr mVertexFormat;
	VariableBufferFormat::ptr mVariableFormat;
	bool mHasTranspose;

	int mBindFlags;
	ID3D10Buffer *mBuffer;
	int mLockAccess;
	bool mMapping;
	bool mBacking;
	uint8 *mData;

	class TextureData{
	public:
		TextureData():
			mD3DState(NULL){}

		~TextureData(){
			if(mD3DState!=NULL){
				mD3DState->Release();
				mD3DState=NULL;
			}
		}

		void update(D3D10RenderDevice *device,Texture::ptr texture,const SamplerState &state);

		Texture::ptr mTexture;
		SamplerState mState;
		ID3D10SamplerState *mD3DState;
	};
	Collection<TextureData*> mTextureData;

	friend D3D10RenderDevice;
};

}
}

#endif
