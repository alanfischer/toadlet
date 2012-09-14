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

#ifndef TOADLET_PEEPER_D3D9VARIABLEBUFFER_H
#define TOADLET_PEEPER_D3D9VARIABLEBUFFER_H

#include "D3D9Includes.h"
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/VariableBuffer.h>
#include <toadlet/peeper/Shader.h>

namespace toadlet{
namespace peeper{

class D3D9RenderDevice;

class TOADLET_API D3D9VariableBuffer:public BaseResource,public VariableBuffer{
public:
	TOADLET_RESOURCE(D3D9VariableBuffer,VariableBuffer);

	D3D9VariableBuffer(D3D9RenderDevice *renderDevice);

	VariableBuffer *getRootVariableBuffer(){return this;}

	bool create(int usage,int access,VariableBufferFormat::ptr variableFormat);
	void destroy();

	void resetCreate(){}
	void resetDestroy(){}

	int getUsage() const{return mUsage;}
	int getAccess() const{return mAccess;}
	VariableBufferFormat::ptr getVariableBufferFormat() const{return mVariableFormat;}
	int getDataSize() const{return mDataSize;}
	int getSize() const{return mVariableFormat->getSize();}

	tbyte *lock(int lockAccess);
	bool unlock();
	bool update(tbyte *data,int start,int size);

	void activateConstants(Shader::ShaderType type);

protected:
	D3D9RenderDevice *mDevice;
	int mUsage;
	int mAccess;
	VariableBufferFormat::ptr mVariableFormat;
	int mDataSize;
	bool mHasTranspose;
	
	tbyte *mData;

	friend D3D9RenderDevice;
};

}
}

#endif
