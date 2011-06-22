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

#include "D3D9VariableBuffer.h"
#include "D3D9RenderDevice.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D9VariableBuffer::D3D9VariableBuffer(D3D9RenderDevice *renderDevice):
	mDevice(NULL),

	mListener(NULL),
	mUsage(0),
	mAccess(0),
	mDataSize(0),

	mData(NULL)
{
	mDevice=renderDevice;
}

D3D9VariableBuffer::~D3D9VariableBuffer(){
	destroy();
}

bool D3D9VariableBuffer::create(int usage,int access,VariableBufferFormat::ptr variableFormat){
	mUsage=usage;
	mAccess=access;
	mVariableFormat=variableFormat;
	mDataSize=variableFormat->getSize();

	return true;
}

void D3D9VariableBuffer::destroy(){
	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}

	if(mListener!=NULL){
		mListener->bufferDestroyed(this);
		mListener=NULL;
	}
}

bool D3D9VariableBuffer::update(tbyte *data,int start,int size){

	return true;
}

}
}
